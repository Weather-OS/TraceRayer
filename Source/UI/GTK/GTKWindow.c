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

#include <UI/GTK/GTKWindow.h>

#include <libadwaita-1/adwaita.h>

static struct gtk_window_object *impl_from_GTKWindowObject( GTKWindowObject *iface )
{
    return CONTAINING_RECORD( iface, struct gtk_window_object, GTKWindowObject_iface );
}

static gboolean DeleteCallback( GtkWidget *widget, void *user_data )
{
    GSList *snapshot = nullptr;
    GSList *handlerList;
    SignalHandler *handler;
    auto const window = (GTKWindowObject *)user_data;

    struct gtk_window_object *impl = impl_from_GTKWindowObject( window );

    TRACE( "widget %p, user_data %p\n", widget, user_data );

    g_mutex_lock( &impl->OnDelete_mutex );
    if ( impl->OnDelete_events )
        snapshot = g_slist_copy( impl->OnDelete_events );
    g_mutex_unlock( &impl->OnDelete_mutex );

    for ( handlerList = snapshot; handlerList; handlerList = g_slist_next( handlerList ) )
    {
        handler = (SignalHandler *)handlerList->data;
        // guard against being null
        if ( handler && handler->callback )
        {
            handler->callback( (UnknownObject *)window, handler->user_data );
        }
    }

    g_slist_free( snapshot );

    return FALSE;
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
    const TRLong added = atomic_fetch_add( &impl->ref, 1 ) + 1;
    TRACE( "iface %p increasing ref count to %ld\n", iface, added );
    return added;
}

static TRLong gtk_window_object_Release( GTKWindowObject *iface )
{
    struct gtk_window_object *impl = impl_from_GTKWindowObject( iface );
    const ATOMIC(TRLong) removed = atomic_fetch_sub(&impl->ref, 1);
    TRACE( "iface %p decreasing ref count to %ld\n", iface, removed - 1 );
    if ( !(removed - 1) )
    {
        if ( impl->GTKWidgetObject_impl )
            impl->GTKWidgetObject_impl->lpVtbl->Release( impl->GTKWidgetObject_impl );
        if ( impl->ChildWidget )
            impl->ChildWidget->lpVtbl->Release( impl->ChildWidget );
        free( impl );
    }
    return removed;
}

static TR_STATUS gtk_window_object_get_WindowRect( GTKWindowObject *iface, GdkRectangle *out )
{
    const struct gtk_window_object *impl = impl_from_GTKWindowObject( iface );
    TRACE( "iface %p, out %p\n", iface, out );
    if ( !out ) throw_NullPtrException();
    *out = impl->WindowRect;
    return T_SUCCESS;
}

static TR_STATUS gtk_window_object_set_WindowRect( GTKWindowObject *iface, GdkRectangle rect )
{
    TR_STATUS status;
    GtkWidget *window;
    GTKWidgetObject *widget;

    struct gtk_window_object *impl = impl_from_GTKWindowObject( iface );

    TRACE( "iface %p, rect %p\n", iface, &rect );

    status = iface->lpVtbl->QueryInterface( iface, IID_GTKWidgetObject, (void**)&widget );
    if ( FAILED( status ) ) return status;

    status = widget->lpVtbl->get_Widget( widget, &window );
    if ( FAILED( status ) ) return status;

    gtk_window_set_default_size( GTK_WINDOW( window ), rect.width, rect.height );

    impl->WindowRect = rect;

    widget->lpVtbl->Release( widget );
    return T_SUCCESS;
}

static TR_STATUS gtk_window_object_get_ChildWidget( GTKWindowObject *iface, GTKWidgetObject **out )
{
    const struct gtk_window_object *impl = impl_from_GTKWindowObject( iface );
    TRACE( "iface %p, out %p\n", iface, out );
    if ( !out ) throw_NullPtrException();
    if ( impl->ChildWidget )
    {
        impl->ChildWidget->lpVtbl->AddRef( impl->ChildWidget );
        *out = impl->ChildWidget;
        return T_SUCCESS;
    }
    return T_NOINIT;
}

static TR_STATUS gtk_window_object_set_ChildWidget( GTKWindowObject *iface, GTKWidgetObject *widget )
{
    TR_STATUS status;
    GtkWidget *window;
    GtkWidget *childWidget;
    GTKWidgetObject *widgetObject;

    struct gtk_window_object *impl = impl_from_GTKWindowObject( iface );

    if ( !widget ) throw_NullPtrException();

    TRACE( "iface %p, widget %p\n", iface, widget );

    if (impl->ChildWidget)
        // prevent dangling pointers
        impl->ChildWidget->lpVtbl->Release(impl->ChildWidget);

    status = iface->lpVtbl->QueryInterface( iface, IID_GTKWidgetObject, (void**)&widgetObject );
    if ( FAILED( status ) ) return status;

    status = widgetObject->lpVtbl->get_Widget( widgetObject, &window );
    if ( FAILED( status ) ) return status;

    status = widget->lpVtbl->get_Widget( widget, &childWidget );
    if ( FAILED( status ) ) return status;

    adw_window_set_content( ADW_WINDOW( window ), childWidget );

    widget->lpVtbl->AddRef( widget );
    impl->ChildWidget = widget;

    widgetObject->lpVtbl->Release( widgetObject );

    return T_SUCCESS;
}

static TR_STATUS gtk_window_object_get_Representation( GTKWindowObject *iface, WindowRepresentation *out )
{
    const struct gtk_window_object *impl = impl_from_GTKWindowObject( iface );
    TRACE( "iface %p, out %p\n", iface, out );
    if ( !out ) throw_NullPtrException();
    *out = impl->Representation;
    return T_SUCCESS;
}

static TR_STATUS gtk_window_object_SetWindowTitle( GTKWindowObject *iface, TRCString title )
{
    TR_STATUS status;
    GtkWidget *window;
    GTKWidgetObject *widget;

    TRACE( "iface %p, title %s\n", iface, title );

    status = iface->lpVtbl->QueryInterface( iface, IID_GTKWidgetObject, (void**)&widget );
    if ( FAILED( status ) ) return status;

    status = widget->lpVtbl->get_Widget( widget, &window );
    if ( FAILED( status ) ) return status;
    gtk_window_set_title( GTK_WINDOW( window ), title );

    widget->lpVtbl->Release( widget );
    return T_SUCCESS;
}

static TR_STATUS gtk_window_object_SetResizable( GTKWindowObject *iface, TRBool resizable )
{
    TR_STATUS status;
    GtkWidget *window;
    GTKWidgetObject *widget;

    TRACE( "iface %p, resizable %d\n", iface, resizable );

    status = iface->lpVtbl->QueryInterface( iface, IID_GTKWidgetObject, (void**)&widget );
    if ( FAILED( status ) ) return status;

    status = widget->lpVtbl->get_Widget( widget, &window );
    if ( FAILED( status ) ) return status;
    gtk_window_set_resizable( GTK_WINDOW( window ), resizable );

    widget->lpVtbl->Release( widget );
    return status;
}

static TR_STATUS gtk_window_object_Show( GTKWindowObject *iface )
{
    TR_STATUS status;
    GTKWidgetObject *widget;

    TRACE( "iface %p\n", iface );

    status = iface->lpVtbl->QueryInterface( iface, IID_GTKWidgetObject, (void**)&widget );
    if ( FAILED( status ) ) return status;

    widget->lpVtbl->setVisibility( widget, true );
    widget->lpVtbl->Release( widget );
    return status;
}

static TR_STATUS gtk_window_object_eventadd_OnDelete( GTKWindowObject *iface, SignalCallback callback, void *context, TRULong *token )
{
    TR_STATUS status;
    GtkWidget *window;
    GTKWidgetObject *widgetObject;
    SignalHandler *handler;

    struct gtk_window_object *impl = impl_from_GTKWindowObject( iface );

    TRACE( "iface %p, callback %p, context %p, token %p\n", iface, callback, context, token );

    status = iface->lpVtbl->QueryInterface( iface, IID_GTKWidgetObject, (void**)&widgetObject );
    if ( FAILED( status ) ) return status;

    status = widgetObject->lpVtbl->get_Widget( widgetObject, &window );
    if ( FAILED( status ) ) return status;

    g_mutex_lock( &impl->OnDelete_mutex );
    handler = g_new0( SignalHandler, 1 );
    if ( !handler )
    {
        g_mutex_unlock( &impl->OnDelete_mutex );
        return T_OUTOFMEMORY;
    }
    handler->callback = callback;
    handler->id = impl->OnDelete_next++;
    handler->user_data = context;
    impl->OnDelete_events = g_slist_prepend( impl->OnDelete_events, handler );
    g_signal_connect( window, "close-request", G_CALLBACK( DeleteCallback ), iface );

    if ( !impl->OnDelete_events )
    {
        g_mutex_unlock( &impl->OnDelete_mutex );
        g_free( handler );
        return T_OUTOFMEMORY;
    }
    g_mutex_unlock( &impl->OnDelete_mutex );

    *token = handler->id;

    return T_SUCCESS;
}

static TR_STATUS gtk_window_object_eventremove_OnDelete( GTKWindowObject *iface, TRULong token )
{
    GSList *handlerList;
    SignalHandler *found = nullptr;
    SignalHandler *current;

    struct gtk_window_object *impl = impl_from_GTKWindowObject( iface );

    TRACE( "iface %p, token %ld\n", iface, token );

    g_mutex_lock( &impl->OnDelete_mutex );
    for ( handlerList = impl->OnDelete_events; handlerList; handlerList = g_slist_next( handlerList ) )
    {
        current = (SignalHandler *)handlerList->data;
        if ( current->id == token )
        {
            found = current;
            break;
        }
    }
    if ( !found )
    {
        g_mutex_unlock( &impl->OnDelete_mutex );
        return T_NOINIT;
    }

    impl->OnDelete_events = g_slist_remove( impl->OnDelete_events, found );
    g_mutex_unlock( &impl->OnDelete_mutex );

    g_free( found );

    return T_SUCCESS;
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
    gtk_window_object_get_ChildWidget,
    gtk_window_object_set_ChildWidget,
    gtk_window_object_get_Representation,
    gtk_window_object_SetWindowTitle,
    gtk_window_object_SetResizable,
    gtk_window_object_Show,
    gtk_window_object_eventadd_OnDelete,
    gtk_window_object_eventremove_OnDelete
};

TR_STATUS TR_API new_gtk_window_object( IN GtkApplication *app, OUT GTKWindowObject **out )
{
    GtkWidget *window;
    GdkSurface *surface;
    GdkDisplay *display;
    struct gtk_window_object *impl;

    TRACE( "app %p, out %p\n", app, out );

    if ( !out || !app ) throw_NullPtrException();

    // Freed in Release();
    if (!(impl = calloc( 1, sizeof(*impl) ))) return T_OUTOFMEMORY;

    impl->GTKWindowObject_iface.lpVtbl = &gtk_window_interface;
    impl->OnDelete_events = nullptr; // <-- TODO: OnDelete_events gets mutated here. Find the cause.
    impl->Representation.windowType = WindowType_GTK;
    g_mutex_init( &impl->OnDelete_mutex );
    atomic_init( &impl->ref, 1 );

    window = adw_window_new();
    gtk_window_set_application( GTK_WINDOW(window), app );
    new_gtk_widget_object_override_widget( window, &impl->GTKWidgetObject_impl );

    surface = gtk_native_get_surface( GTK_NATIVE( window ) );
    display = gdk_surface_get_display( surface );

#ifdef PLATFORM_SUPPORTS_X11
    if ( GDK_IS_X11_DISPLAY( display ) )
    {
        impl->Representation.surfaceType = SurfaceType_X11;
        impl->Representation.x11_display = gdk_x11_display_get_xdisplay( display );
        impl->Representation.x11_window = gdk_x11_surface_get_xid( surface );
    }
#endif

#ifdef PLATFORM_SUPPORTS_WAYLAND
    if ( GDK_IS_WAYLAND_DISPLAY( display ) )
    {
        impl->Representation.surfaceType = SurfaceType_Wayland;
        impl->Representation.wayland_display = gdk_wayland_display_get_wl_display( display );
        impl->Representation.wayland_surface = gdk_wayland_surface_get_wl_surface( surface );
    }
#endif

#ifdef PLATFORM_SUPPORTS_WIN32
    if ( GDK_IS_WIN32_DISPLAY( display ) )
    {
        impl->Representation.surfaceType = SurfaceType_Win32;
        impl->Representation.win32_window = gdk_win32_window_get_handle( surface );
    }
#endif

#ifdef PLATFORM_SUPPORTS_MACOS
    if ( GDK_IS_QUARTZ_DISPLAY( display ) )
    {
        impl->Representation.surfaceType = SurfaceType_macOS;
        impl->Representation.metal_layer = CAMetalLayer_from_nsview( gdk_quartz_window_get_nsview( window ) );
    }
#endif

    *out = &impl->GTKWindowObject_iface;

    TRACE( "created GTKWindowObject %p\n", *out );

    return T_SUCCESS;
}