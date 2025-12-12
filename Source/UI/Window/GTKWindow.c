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

#include <UI/Window/GTKWindow.h>

static struct gtk_window_object *impl_from_GTKWindowObject( GTKWindowObject *iface )
{
    return CONTAINING_RECORD( iface, struct gtk_window_object, GTKWindowObject_iface );
}

static void WindowBuilder( GtkApplication *app, void *user_data )
{
    GtkWidget *windowWidget;
    struct gtk_window_object *impl = impl_from_GTKWindowObject( (GTKWindowObject *)user_data );

    TRACE( "app %p, user_data %p\n", app, user_data );

    new_gtk_widget_object_override_widget( gtk_application_window_new( app ), &impl->GTKWidgetObject_impl );

    impl->GTKWidgetObject_impl->lpVtbl->get_Widget( impl->GTKWidgetObject_impl, &windowWidget );

    gtk_window_set_default_size( GTK_WINDOW( windowWidget ), impl->WindowRect.width, impl->WindowRect.height );
    gtk_window_set_title( GTK_WINDOW( windowWidget ), impl->windowTitle );

    impl->callback( user_data ); // <-- Callback proc
}

static TR_STATUS gtk_window_object_QueryInterface( GTKWindowObject *iface, const TRUUID uuid, void **out )
{
    const struct gtk_window_object *impl = impl_from_GTKWindowObject( iface );

    TRACE( "iface %p, uuid %s, out %p\n", iface, debugstr_uuid( uuid ), out );

    if ( !uuid_compare( uuid, IID_UnknownObject ) || !uuid_compare( uuid, IID_GTKWindowObject ) )
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

static TRLong gtk_window_object_AddRef( GTKWindowObject *iface )
{
    struct gtk_window_object *impl = impl_from_GTKWindowObject( iface );
    const TRLong added = atomic_load( &impl->ref ) + 1;
    TRACE( "iface %p increasing ref count to %ld\n", iface, added );
    atomic_fetch_add( &impl->ref, 1 );
    return added;
}

static TRLong gtk_window_object_Release( GTKWindowObject *iface )
{
    struct gtk_window_object *impl = impl_from_GTKWindowObject( iface );
    const TRLong removed = atomic_load( &impl->ref ) - 1;
    TRACE( "iface %p decreasing ref count to %ld\n", iface, removed );
    atomic_fetch_sub( &impl->ref, 1 );
    if ( !removed )
        free( impl );
    return removed;
}

static TR_STATUS gtk_window_object_get_WindowRect( GTKWindowObject *iface, GdkRectangle *out )
{
    const struct gtk_window_object *impl = impl_from_GTKWindowObject( iface );
    TRACE( "iface %p, GdkRectangle %p\n", iface, out );
    if ( !out ) throw_NullPtrException();
    *out = impl->WindowRect;
    return T_SUCCESS;
}

static TR_STATUS gtk_window_object_set_WindowRect( GTKWindowObject *iface, GdkRectangle rect )
{
    struct gtk_window_object *impl = impl_from_GTKWindowObject( iface );
    TRACE( "iface %p, GdkRectangle %p\n", iface, &rect );
    impl->WindowRect = rect;
    return T_SUCCESS;
}

static TR_STATUS gtk_window_object_setWindowTitle( GTKWindowObject *iface, TRString title )
{
    struct gtk_window_object *impl = impl_from_GTKWindowObject( iface );
    TRACE( "iface %p, title %s\n", iface, title );
    impl->windowTitle = title;
    return T_SUCCESS;
}

static void gtk_window_object_Show( GTKWindowObject *iface )
{
    TR_STATUS status;
    GTKWidgetObject *widget;

    TRACE( "iface %p\n", iface );

    status = iface->lpVtbl->QueryInterface( iface, IID_GTKWidgetObject, (void**)&widget );
    if ( FAILED( status ) ) return;

    widget->lpVtbl->setVisibility( widget, true );
}

static GTKWindowInterface gtk_window_interface =
{
    /* UnknownObject Methods */
    gtk_window_object_QueryInterface,
    gtk_window_object_AddRef,
    gtk_window_object_Release,
    /* GTKWindowObject Methods */
    gtk_window_object_get_WindowRect,
    gtk_window_object_set_WindowRect,
    gtk_window_object_setWindowTitle,
    gtk_window_object_Show
};

TR_STATUS new_gtk_window_object( IN GtkApplication *app, IN WindowLoopCallback callback, OUT GTKWindowObject **out )
{
    TR_STATUS status;
    struct gtk_window_object *impl;

    TRACE( "app %p, callback %p, out %p\n", app, callback, out );

    if ( !out || !app ) throw_NullPtrException();

    // Freed in Release();
    if (!(impl = calloc( 1, sizeof(*impl) ))) return T_OUTOFMEMORY;

    impl->GTKWindowObject_iface.lpVtbl = &gtk_window_interface;
    impl->callback = callback;
    atomic_init( &impl->ref, 1 );

    *out = &impl->GTKWindowObject_iface;

    g_signal_connect( app, "activate", G_CALLBACK( WindowBuilder ), (void *)*out );

    TRACE( "created GTKWindowObject %p\n", *out );

    return T_SUCCESS;
}