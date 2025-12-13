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
 *  Module: GTKWindow.c
 *  Description: Creating a GTK Window Object
 */

#include <UI/GTK/GTKWindowHandle.h>

static struct gtk_window_handle_object *impl_from_GTKWindowHandleObject( GTKWindowHandleObject *iface )
{
    return CONTAINING_RECORD( iface, struct gtk_window_handle_object, GTKWindowHandleObject_iface );
}

static TR_STATUS gtk_window_handle_object_QueryInterface( GTKWindowHandleObject *iface, const TRUUID uuid, void **out )
{
    const struct gtk_window_handle_object *impl = impl_from_GTKWindowHandleObject( iface );

    TRACE( "iface %p, uuid %s, out %p\n", iface, debugstr_uuid( uuid ), out );

    if ( !uuid_compare( uuid, IID_UnknownObject ) || !uuid_compare( uuid, IID_GTKWindowHandleObject ) )
    {
        iface->lpVtbl->AddRef( iface );
        *out = iface;
        return T_SUCCESS;
    }

    if ( !uuid_compare( uuid, IID_GTKWidgetObject ) )
    {
        if ( !impl->GTKWidgetObject_impl )
        {
            ERROR( "Subclass GTKWidgetObject for GTKWindowObject %p is not initialized yet!\n", iface );
            return T_NOINIT;
        }
        impl->GTKWidgetObject_impl->lpVtbl->AddRef( impl->GTKWidgetObject_impl );
        *out = impl->GTKWidgetObject_impl;
        return T_SUCCESS;
    }

    ERROR( "uuid %s is not implemented! returning T_NOTIMPL\n", debugstr_uuid( uuid ) );
    return T_NOTIMPL;
}

static TRLong gtk_window_handle_object_AddRef( GTKWindowHandleObject *iface )
{
    struct gtk_window_handle_object *impl = impl_from_GTKWindowHandleObject( iface );
    const TRLong added = atomic_fetch_add( &impl->ref, 1 ) + 1;
    TRACE( "iface %p increasing ref count to %ld\n", iface, added );
    return added;
}

static TRLong gtk_window_handle_object_Release( GTKWindowHandleObject *iface )
{
    struct gtk_window_handle_object *impl = impl_from_GTKWindowHandleObject( iface );
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

static GTKWindowHandleInterface gtk_drawing_area_interface =
{
    /* UnknownObject Methods */
    gtk_window_handle_object_QueryInterface,
    gtk_window_handle_object_AddRef,
    gtk_window_handle_object_Release
};

TR_STATUS new_gtk_window_handle_object( OUT GTKWindowHandleObject **out )
{
    TR_STATUS status;
    GtkWidget *window_handle;
    struct gtk_window_handle_object *impl;

    TRACE( "out %p\n", out );

    if ( !out ) throw_NullPtrException();

    // Freed in Release();
    if (!(impl = calloc( 1, sizeof(*impl) ))) return T_OUTOFMEMORY;
    impl->GTKWindowHandleObject_iface.lpVtbl = &gtk_drawing_area_interface;
    impl->ref = 1;

    window_handle = gtk_window_handle_new();
    status = new_gtk_widget_object_override_widget( window_handle, &impl->GTKWidgetObject_impl );
    if ( FAILED( status ) ) return status;

    *out = &impl->GTKWindowHandleObject_iface;

    TRACE( "created GTKWindowHandleObject %p\n", *out );

    return T_SUCCESS;
}