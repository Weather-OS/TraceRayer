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
 *  Module: GTKBox.c
 *  Description: A GTK Box object that represents a box.
 */

#include <UI/GTK/GTKBox.h>

static struct gtk_box_object *impl_from_GTKBoxObject( GTKBoxObject *iface )
{
    return CONTAINING_RECORD( iface, struct gtk_box_object, GTKBoxObject_iface );
}

static TR_STATUS gtk_box_object_QueryInterface( GTKBoxObject *iface, const TRUUID uuid, void **out )
{
    const struct gtk_box_object *impl = impl_from_GTKBoxObject( iface );

    TRACE( "iface %p, uuid %s, out %p\n", iface, debugstr_uuid( uuid ), out );

    if ( !uuid_compare( uuid, IID_UnknownObject ) || !uuid_compare( uuid, IID_GTKBoxObject ) )
    {
        iface->lpVtbl->AddRef( iface );
        *out = iface;
        return T_SUCCESS;
    }

    if ( !uuid_compare( uuid, IID_GTKWidgetObject ) )
    {
        if ( !impl->GTKWidgetObject_impl )
        {
            ERROR( "Subclass GTKWidgetObject for GTKBoxObject %p is not initialized yet!\n", iface );
            return T_NOINIT;
        }
        impl->GTKWidgetObject_impl->lpVtbl->AddRef( impl->GTKWidgetObject_impl );
        *out = impl->GTKWidgetObject_impl;
        return T_SUCCESS;
    }

    ERROR( "uuid %s is not implemented! returning T_NOTIMPL\n", debugstr_uuid( uuid ) );
    return T_NOTIMPL;
}

static TRLong gtk_box_object_AddRef( GTKBoxObject *iface )
{
    struct gtk_box_object *impl = impl_from_GTKBoxObject( iface );
    const TRLong added = atomic_fetch_add( &impl->ref, 1 ) + 1;
    TRACE( "iface %p increasing ref count to %ld\n", iface, added );
    return added;
}

static TRLong gtk_box_object_Release( GTKBoxObject *iface )
{
    struct gtk_box_object *impl = impl_from_GTKBoxObject( iface );
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

static TR_STATUS gtk_box_object_AppendWidget( GTKBoxObject *iface, GTKWidgetObject *widget )
{
    TR_STATUS status;
    GTKWidgetObject *own;
    GtkWidget *ownWidget;
    GtkWidget *appendWidget;

    status = iface->lpVtbl->QueryInterface( iface, IID_GTKWidgetObject, (void**)&own );
    if ( FAILED( status ) ) return status;

    status = widget->lpVtbl->get_Widget( own, &ownWidget );
    own->lpVtbl->Release( own );
    if ( FAILED( status ) ) return status;

    //Non ownership taking.
    status = widget->lpVtbl->get_Widget( widget, &appendWidget );
    if ( FAILED( status ) ) return status;

    gtk_box_append( GTK_BOX( ownWidget ), appendWidget );

    return status;
}

static GTKBoxInterface gtk_box_interface =
{
    /* UnknownObject Methods */
    gtk_box_object_QueryInterface,
    gtk_box_object_AddRef,
    gtk_box_object_Release,
    /* GTKBoxObject Methods */
    gtk_box_object_AppendWidget
};

TR_STATUS TR_API new_gtk_box_object_override_orientation_and_spacing( IN GtkOrientation orientation, IN TRInt spacing, OUT GTKBoxObject **out )
{
    TR_STATUS status;
    GtkWidget *box;
    struct gtk_box_object *impl;

    TRACE( "out %p\n", out );

    if ( !out ) throw_NullPtrException();

    // Freed in Release();
    if (!(impl = calloc( 1, sizeof(*impl) ))) return T_OUTOFMEMORY;
    impl->GTKBoxObject_iface.lpVtbl = &gtk_box_interface;
    impl->ref = 1;

    box = gtk_box_new( orientation, spacing );

    status = new_gtk_widget_object_override_widget( box, &impl->GTKWidgetObject_impl );
    if ( FAILED( status ) ) return status;

    *out = &impl->GTKBoxObject_iface;

    TRACE( "created GTKBoxObject %p\n", *out );

    return T_SUCCESS;
}