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
 *  Module: GTK.c
 *  Description: Root GTK object to create sub-objects.
 */

#include <gdk/gdk.h>

#include <UI/Representation.h>
#include <UI/GTK/GTK.h>

static struct gtk_object *impl_from_GTKObject( GTKObject *iface )
{
    return CONTAINING_RECORD( iface, struct gtk_object, GTKObject_iface );
}

static void ActivationCallback( GtkApplication *app, void *user_data )
{
    GSList *snapshot = nullptr;
    GSList *handlerList;
    SignalHandler *handler;

    struct gtk_object *impl = impl_from_GTKObject( (GTKObject *)user_data );

    TRACE( "app %p, user_data %p\n", app, user_data );

    g_mutex_lock( &impl->OnActivation_mutex );
    if ( impl->OnActivation_events )
        snapshot = g_slist_copy( impl->OnActivation_events );
    g_mutex_unlock( &impl->OnActivation_mutex );

    for ( handlerList = snapshot; handlerList; handlerList = g_slist_next( handlerList ) )
    {
        handler = (SignalHandler *)handlerList->data;
        // guard against being null
        if ( handler && handler->callback )
        {
            // The object's app is now in an activated context
            impl->isInActivationThread = true;
            handler->callback( (UnknownObject *)user_data, handler->user_data );
            // The object's app exited the context
            impl->isInActivationThread = false;
        }
    }

    g_slist_free( snapshot );
}

DEFINE_SHALLOW_UNKNOWNOBJECT( GTKObject, gtk_object )

TR_STATUS gtk_object_CreateWindow( GTKObject *iface, GTKWindowObject **out )
{
    TR_STATUS status;

    struct gtk_object *impl = impl_from_GTKObject( iface );

    TRACE( "iface %p, out %p\n", iface, out );

    if ( !out ) throw_NullPtrException();

    if ( !impl->isInActivationThread )
    {
        ERROR( "Application %p is not in an activated context!\n", impl->app );
        return T_ILLEGAL_METHOD_CALL;
    }

    status = new_gtk_window_object( impl->app, out );

    return status;
}

static TR_STATUS gtk_object_RunApplication( GTKObject *iface )
{
    const struct gtk_object *impl = impl_from_GTKObject( iface );

    TRACE( "iface %p\n", iface );

    return g_application_run( G_APPLICATION( impl->app ), 0, nullptr );
}

static TR_STATUS gtk_object_eventadd_OnActivation( GTKObject *iface, SignalCallback callback, void *context, TRULong *token )
{
    SignalHandler *handler;

    struct gtk_object *impl = impl_from_GTKObject( iface );

    TRACE( "iface %p, callback %p, context %p, token %p\n", iface, callback, context, token );

    g_mutex_lock( &impl->OnActivation_mutex );
    handler = g_new0( SignalHandler, 1 );
    if ( !handler )
    {
        g_mutex_unlock( &impl->OnActivation_mutex );
        return T_OUTOFMEMORY;
    }
    handler->callback = callback;
    handler->id = impl->OnActivation_next++;
    handler->user_data = context;
    impl->OnActivation_events = g_slist_prepend( impl->OnActivation_events, handler );
    if ( !impl->OnActivation_events )
    {
        g_mutex_unlock( &impl->OnActivation_mutex );
        g_free( handler );
        return T_OUTOFMEMORY;
    }
    g_mutex_unlock( &impl->OnActivation_mutex );

    *token = handler->id;

    return T_SUCCESS;
}

static TR_STATUS gtk_object_eventremove_OnActivation( GTKObject *iface, TRULong token )
{
    GSList *handlerList;
    SignalHandler *found = nullptr;
    SignalHandler *current;

    struct gtk_object *impl = impl_from_GTKObject( iface );

    TRACE( "iface %p, token %ld\n", iface, token );

    g_mutex_lock( &impl->OnActivation_mutex );
    for ( handlerList = impl->OnActivation_events; handlerList; handlerList = g_slist_next( handlerList ) )
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
        g_mutex_unlock( &impl->OnActivation_mutex );
        return T_NOINIT;
    }

    impl->OnActivation_events = g_slist_remove( impl->OnActivation_events, found );
    g_mutex_unlock( &impl->OnActivation_mutex );

    g_free( found );

    return T_SUCCESS;
}

static void gtk_object_CurrentPlatform( GTKObject *iface, Platform *out )
{
    GdkDisplay *display;

    TRACE( "iface %p, out %ld\n", iface, out );

    display = gdk_display_get_default();

#ifdef PLATFORM_SUPPORTS_X11
    if ( GDK_IS_X11_DISPLAY( display ) )
    {
        *out = Platform_X11;
        return;
    }
#endif

#ifdef PLATFORM_SUPPORTS_WAYLAND
    if ( GDK_IS_WAYLAND_DISPLAY( display ) )
    {
        *out = Platform_Wayland;
        return;
    }
#endif

#ifdef PLATFORM_SUPPORTS_WIN32
    if ( GDK_IS_WIN32_DISPLAY( display ) )
    {
        *out = Platform_win32;
        return;
    }
#endif

#ifdef PLATFORM_SUPPORTS_MACOS
    if ( GDK_IS_QUARTZ_DISPLAY( display ) )
    {
        *out = Platform_macOS;
        return;
    }
#endif
}

static GTKInterface gtk_interface =
{
    /* UnknownObject Methods */
    gtk_object_QueryInterface,
    gtk_object_AddRef,
    gtk_object_Release,
    /* GTKObject Methods */
    gtk_object_CreateWindow,
    gtk_object_RunApplication,
    gtk_object_CurrentPlatform,
    gtk_object_eventadd_OnActivation,
    gtk_object_eventremove_OnActivation
};

TR_STATUS TR_API new_gtk_object( IN TRCString appName, OUT GTKObject **out )
{
    struct gtk_object *impl;

    TRACE( "appName %s, out %p\n", appName, out );

    gtk_init();

    if ( !out ) throw_NullPtrException();

    // Freed in Release();
    if (!(impl = calloc( 1, sizeof(*impl) ))) return T_OUTOFMEMORY;

    impl->GTKObject_iface.lpVtbl = &gtk_interface;
    impl->app = gtk_application_new( appName, G_APPLICATION_DEFAULT_FLAGS );
    atomic_init( &impl->ref, 1 );
    g_mutex_init( &impl->OnActivation_mutex );
    impl->OnActivation_next = 0;

    *out = &impl->GTKObject_iface;

    g_signal_connect( impl->app, "activate", G_CALLBACK( ActivationCallback ), (void *)*out );

    TRACE( "created GTKObject %p\n", *out );

    return T_SUCCESS;
}