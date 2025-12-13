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

#ifndef TRACERAYER_GTKWINDOW_H
#define TRACERAYER_GTKWINDOW_H

#include <gdk/gdk.h>

#include <Object.h>
#include <Signal.h>
#include <Types.h>

#include <UI/GTK/GTKWidget.h>

typedef struct _GTKWindowObject GTKWindowObject;

typedef void (*WindowLoopCallback)( IN GTKWindowObject *This );

typedef struct _GTKWindowInterface
{
    BEGIN_INTERFACE

    IMPLEMENTS_UNKNOWNOBJECT( GTKWindowObject )

    TR_STATUS (*get_WindowRect)( IN GTKWindowObject *This, OUT GdkRectangle *out ); // getter
    TR_STATUS (*set_WindowRect)( IN GTKWindowObject *This, IN GdkRectangle rect ); // setter
    TR_STATUS (*get_ChildWidget)( IN GTKWindowObject *This, OUT GTKWidgetObject **out ); // getter
    TR_STATUS (*set_ChildWidget)( IN GTKWindowObject *This, IN GTKWidgetObject *widget ); // setter
    TR_STATUS (*setWindowTitle)( IN GTKWindowObject *This, IN TRString title );
    void      (*Show)( IN GTKWindowObject *This );

    IMPLEMENTS_EVENT( GTKWindowObject, OnDelete )

    END_INTERFACE
} GTKWindowInterface;

interface _GTKWindowObject
{
    CONST_VTBL GTKWindowInterface *lpVtbl;
};

struct gtk_window_object
{
    // --- Public Members --- //
    GTKWindowObject GTKWindowObject_iface;
    volatile GdkRectangle WindowRect; // <-- It can be modified outside of the object context
    GTKWidgetObject *ChildWidget;

    // --- Subclasses --- //
    implements( GTKWidgetObject );

    // --- Private Members --- //
    WindowLoopCallback callback;
    implements_glib_eventlist( OnDelete )
    TRLong ref;
};

// 1b731a66-153d-4e54-898c-6d4de5c47e08
DEFINE_GUID( GTKWindowObject, 0x1b731a66, 0x153d, 0x4e54, 0x89, 0x8c, 0x6d, 0x4d, 0xe5, 0xc4, 0x7e, 0x08 );

// Constructors
TR_STATUS new_gtk_window_object( IN GtkApplication *app, OUT GTKWindowObject **out );

#endif