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

#ifndef TRACERAYER_GTKWIDGET_H
#define TRACERAYER_GTKWIDGET_H

#include <gtk/gtk.h>

#include <Object.h>
#include <Types.h>

typedef struct _GTKWidgetObject GTKWidgetObject;

typedef struct _GTKWidgetInterface
{
    BEGIN_INTERFACE

    IMPLEMENTS_UNKNOWNOBJECT( GTKWidgetObject )

    TR_STATUS (*get_Widget)( IN GTKWidgetObject *This, OUT GtkWidget **out );
    void      (*setVisibility)( IN GTKWidgetObject *This, TRBool visibility );
    
    END_INTERFACE
} GTKWidgetInterface;

interface _GTKWidgetObject
{
    CONST_VTBL GTKWidgetInterface *lpVtbl;
};

struct gtk_widget_object
{
    // --- Public Members --- //
    GTKWidgetObject GTKWidgetObject_iface;
    GtkWidget *Widget;

    // --- Private Members --- //
    TRLong ref;
};

// cfe1afb8-34c3-4ba0-9512-b02ef6ada3ef
DEFINE_GUID( GTKWidgetObject, 0xcfe1afb8, 0x34c3, 0x4ba0, 0x95, 0x12, 0xb0, 0x2e, 0xf6, 0xad, 0xa3, 0xef );

// Constructors
TR_STATUS new_gtk_widget_object_override_widget( IN GtkWidget *widget, OUT GTKWidgetObject **out );

#endif