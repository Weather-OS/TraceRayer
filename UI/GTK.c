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

#include <UI/GTK.h>

static struct gtk_object *impl_from_GTKObject( GTKObject *iface )
{
    return CONTAINING_RECORD( iface, struct gtk_object, GTKObject_iface );
}

DEFINE_SHALLOW_UNKNOWNOBJECT( GTKObject, gtk_object )

static TR_STATUS gtk_object_CreateWindow( GTKObject *iface, WindowLoopCallback callback, GTKWindowObject **out )
{
    TR_STATUS status;

    struct gtk_object *impl = impl_from_GTKObject( iface );

    TRACE( "iface %p, out %p\n", iface, out );

    if ( !out || !callback ) throw_NullPtrException();

    status = new_gtk_window_object( impl->app, callback, out );

    return status;
}

static TR_STATUS gtk_object_RunApplication( GTKObject *iface )
{
    const struct gtk_object *impl = impl_from_GTKObject( iface );

    TRACE( "iface %p\n", iface );

    return g_application_run( G_APPLICATION( impl->app ), 0, nullptr );
}

static GTKInterface gtk_interface =
{
    /* UnknownObject Methods */
    gtk_object_QueryInterface,
    gtk_object_AddRef,
    gtk_object_Release,
    /* GTKObject Methods */
    gtk_object_CreateWindow,
    gtk_object_RunApplication
};

TR_STATUS new_gtk_object( IN TRString appName, OUT GTKObject **out )
{
    struct gtk_object *impl;

    TRACE( "appName %s, out %p\n", appName, out );

    if ( !out ) throw_NullPtrException();

    // Freed in Release();
    if (!(impl = calloc( 1, sizeof(*impl) ))) return T_OUTOFMEMORY;

    impl->GTKObject_iface.lpVtbl = &gtk_interface;
    impl->app = gtk_application_new( appName, G_APPLICATION_DEFAULT_FLAGS );
    atomic_init( &impl->ref, 1 );

    *out = &impl->GTKObject_iface;
    TRACE( "created GTKObject %p\n", *out );

    return T_SUCCESS;
}