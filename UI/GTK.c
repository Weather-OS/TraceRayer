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

#define INITGUID

#include <UI/GTK.h>

static struct gtk_object *impl_from_GTKObject( IN GTKObject *iface )
{
    return CONTAINING_RECORD( iface, struct gtk_object, GTKObject_iface );
}

DEFINE_SHALLOW_UNKNOWNOBJECT( GTKObject, gtk_object )

static TR_STATUS gtk_object_CreateWindow( GTKObject *iface, GTKWindowObject *out )
{
    TRACE( "iface %p\n", iface );
    return T_SUCCESS;
}

static GTKObjectInterface gtk_object_interface =
{
    /* UnknownObject Methods */
    gtk_object_QueryInterface,
    gtk_object_AddRef,
    gtk_object_Release,
    /* GTKObject Methods */
    gtk_object_CreateWindow
};

TR_STATUS new_gtk_object( GTKObject **out )
{
    struct gtk_object *impl;

    TRACE( "out %p\n", out );

    if (!(impl = calloc( 1, sizeof(*impl) ))) return T_OUTOFMEMORY;

    impl->GTKObject_iface.lpVtbl = &gtk_object_interface;
    impl->ref = 1;

    *out = &impl->GTKObject_iface;

    return T_SUCCESS;
}