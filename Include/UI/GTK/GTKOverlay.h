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

#ifndef TRACERAYER_GTKOVERLAY_H
#define TRACERAYER_GTKOVERLAY_H

#include <gtk/gtk.h>

#include <Object.h>
#include <Types.h>

#include <UI/GTK/GTKWidget.h>

typedef struct _GTKOverlayObject GTKOverlayObject;

typedef struct _GTKOverlayInterface
{
    BEGIN_INTERFACE

    IMPLEMENTS_UNKNOWNOBJECT( GTKOverlayObject )

    END_INTERFACE
} GTKOverlayInterface;

interface _GTKOverlayObject
{
    CONST_VTBL GTKOverlayInterface *lpVtbl;
};

/**
 * @Object: GTKOverlayObject
 * @Description: A GTK Overlay object used to depict multiple widgets
 *               overlayed on top of eachother.
 * @Implements:
 *      GTKWidgetObject
 */
struct gtk_overlay_object
{
    // --- Public Members --- //
    GTKOverlayObject GTKOverlayObject_iface;

    // --- Subclasses --- //
    implements( GTKWidgetObject )

    // --- Private Members --- //
    TRLong ref;
};

// fa685c6a-dc44-4e0e-a4fb-4d5457d859a1
DEFINE_GUID( GTKOverlayObject, 0xfa685c6a, 0xdc44, 0x4e0e, 0xa4, 0xfb, 0x4d, 0x54, 0x57, 0xd8, 0x59, 0xa1 );

// Constructors
TR_STATUS new_gtk_overlay_object( OUT GTKOverlayObject **out );

#endif