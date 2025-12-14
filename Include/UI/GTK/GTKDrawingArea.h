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

#ifndef TRACERAYER_GTKDRAWINGAREA_H
#define TRACERAYER_GTKDRAWINGAREA_H

#include <gtk/gtk.h>

#include <Object.h>
#include <Types.h>

#include <UI/GTK/GTKWidget.h>

typedef struct _GTKDrawingAreaObject GTKDrawingAreaObject;

typedef struct _GTKDrawingAreaInterface
{
    BEGIN_INTERFACE

    IMPLEMENTS_UNKNOWNOBJECT( GTKDrawingAreaObject )

    END_INTERFACE
} GTKDrawingAreaInterface;

interface _GTKDrawingAreaObject
{
    CONST_VTBL GTKDrawingAreaInterface *lpVtbl;
};

/**
 * @Object: GTKDrawingAreaObject
 * @Description: A GTK Drawing area objects that can be used to
 *               represent frame buffers, such as a Vulkan swapchain.
 * @Implements:
 *      GTKWidgetObject
 */
struct gtk_drawing_area_object
{
    // --- Public Members --- //
    GTKDrawingAreaObject GTKDrawingAreaObject_iface;

    // --- Subclasses --- //
    implements( GTKWidgetObject )

    // --- Private Members --- //
    TRLong ref;
};

// fa685c6a-dc44-4e0e-a4fb-4d5457d859a1
DEFINE_GUID( GTKDrawingAreaObject, 0xfa685c6a, 0xdc44, 0x4e0e, 0xa4, 0xfb, 0x4d, 0x54, 0x57, 0xd8, 0x59, 0xa1 );

// Constructors
TR_STATUS new_gtk_drawing_area_object( OUT GTKDrawingAreaObject **out );

#endif