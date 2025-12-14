/*
 * Copyright (c) 2025 Weather
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/**
 *  Module: GTKPicture.c
 *  Description: A GTK Widget object that represents a picture.
 */

#include <UI/GTK/GTKPicture.h>

static struct gtk_picture_object *impl_from_GTKPictureObject( GTKPictureObject *iface )
{
    return CONTAINING_RECORD( iface, struct gtk_picture_object, GTKPictureObject_iface );
}

static TR_STATUS gtk_picture_object_QueryInterface( GTKPictureObject *iface, const TRUUID uuid, void **out )
{
    const struct gtk_picture_object *impl = impl_from_GTKPictureObject( iface );

    TRACE( "iface %p, uuid %s, out %p\n", iface, debugstr_uuid( uuid ), out );

    if ( !uuid_compare( uuid, IID_UnknownObject ) || !uuid_compare( uuid, IID_GTKPictureObject ) )
    {
        iface->lpVtbl->AddRef( iface );
        *out = iface;
        return T_SUCCESS;
    }

    if ( !uuid_compare( uuid, IID_GTKWidgetObject ) )
    {
        if ( !impl->GTKWidgetObject_impl )
        {
            ERROR( "Subclass GTKWidgetObject for GTKPictureObject %p is not initialized yet!\n", iface );
            return T_NOINIT;
        }
        impl->GTKWidgetObject_impl->lpVtbl->AddRef( impl->GTKWidgetObject_impl );
        *out = impl->GTKWidgetObject_impl;
        return T_SUCCESS;
    }

    ERROR( "uuid %s is not implemented! returning T_NOTIMPL\n", debugstr_uuid( uuid ) );
    return T_NOTIMPL;
}

static TRLong gtk_picture_object_AddRef( GTKPictureObject *iface )
{
    struct gtk_picture_object *impl = impl_from_GTKPictureObject( iface );
    const TRLong added = atomic_fetch_add( &impl->ref, 1 ) + 1;
    TRACE( "iface %p increasing ref count to %ld\n", iface, added );
    return added;
}

static TRLong gtk_picture_object_Release( GTKPictureObject *iface )
{
    struct gtk_picture_object *impl = impl_from_GTKPictureObject( iface );
    const TRLong removed = atomic_fetch_sub(&impl->ref, 1);
    TRACE( "iface %p decreasing ref count to %ld\n", iface, removed - 1 );
    if ( !(removed - 1) )
    {
        if ( impl->GTKWidgetObject_impl )
            impl->GTKWidgetObject_impl->lpVtbl->Release( impl->GTKWidgetObject_impl );
        free( impl );
    }
    return removed;
}

// FIXME: PictureRect is only assigned after notify::paintable
static TR_STATUS gtk_picture_object_GetPictureRect( GTKPictureObject *iface, GdkRectangle *out )
{
    TR_STATUS status;
    GtkWidget *pictureWidget;
    GdkPaintable *paintable;
    GTKWidgetObject *widget;

    TRACE( "iface %p, out %p\n", iface, out );

    if ( !out ) throw_NullPtrException();

    status = iface->lpVtbl->QueryInterface( iface, IID_GTKWidgetObject, (void **)&widget );
    if ( FAILED( status ) ) return status;

    status = widget->lpVtbl->get_Widget( widget, &pictureWidget );
    if ( FAILED( status ) ) return status;

    paintable = gtk_picture_get_paintable( GTK_PICTURE( pictureWidget ) );
    out->width = gdk_paintable_get_intrinsic_width( paintable );
    out->height = gdk_paintable_get_intrinsic_height( paintable );
    g_object_unref( paintable );

    widget->lpVtbl->Release( widget );

    return T_SUCCESS;
}

static GTKPictureInterface gtk_picture_interface =
{
    /* UnknownObject Methods */
    gtk_picture_object_QueryInterface,
    gtk_picture_object_AddRef,
    gtk_picture_object_Release,
    /* GTKPictureObject Methods */
    gtk_picture_object_GetPictureRect
};

TR_STATUS new_gtk_picture_object_override_path( IN TRPath *imagePath, OUT GTKPictureObject **out )
{
    TR_STATUS status;
    GtkWidget *picture;
    TRInt width, height;
    struct gtk_picture_object *impl;

    TRACE( "out %p\n", out );

    if ( !out ) throw_NullPtrException();

    // Freed in Release();
    if (!(impl = calloc( 1, sizeof(*impl) ))) return T_OUTOFMEMORY;
    impl->GTKPictureObject_iface.lpVtbl = &gtk_picture_interface;
    impl->ref = 1;

    picture = gtk_picture_new_for_filename( imagePath->Location );

    status = new_gtk_widget_object_override_widget( picture, &impl->GTKWidgetObject_impl );
    if ( FAILED( status ) ) return status;

    *out = &impl->GTKPictureObject_iface;

    TRACE( "created GTKPictureObject %p\n", *out );

    return T_SUCCESS;
}