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

#ifndef TRACERAYER_GTK_H
#define TRACERAYER_GTK_H

#include <gdk/gdk.h>

#include <Object.h>
#include <Signal.h>
#include <Types.h>

#include <UI/GTK/GTKWindow.h>

typedef struct _GTKObject GTKObject;

typedef struct _GTKInterface
{
    BEGIN_INTERFACE

    IMPLEMENTS_UNKNOWNOBJECT( GTKObject )

    TR_STATUS (*CreateWindow)( IN GTKObject *This, OUT GTKWindowObject **out );
    TR_STATUS (*RunApplication)( IN GTKObject *This );

    IMPLEMENTS_EVENT( GTKObject, OnActivation )

    END_INTERFACE
} GTKInterface;

interface _GTKObject
{
    CONST_VTBL GTKInterface *lpVtbl;
};

struct gtk_object
{
    // --- Public Members --- //
    GTKObject GTKObject_iface;

    // --- Private Members --- //
    GtkApplication *app;
    TRBool isInActivationThread;
    implements_glib_eventlist( OnActivation )
    TRLong ref;
};

// 71e34ecd-fd1e-4e3c-94fa-d329c7301325
DEFINE_GUID( GTKObject, 0x71e34ecd, 0xfd1e, 0x4e3c, 0x94, 0xfa, 0xd3, 0x29, 0xc7, 0x30, 0x13, 0x25 );

// Constructors
TR_STATUS new_gtk_object( IN TRString appName, OUT GTKObject **out );

#endif