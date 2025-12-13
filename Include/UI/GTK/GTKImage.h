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

#ifndef TRACERAYER_GTKIMAGE_H
#define TRACERAYER_GTKIMAGE_H

#include <gdk/gdk.h>

#include <IO/Path.h>
#include <Object.h>
#include <Types.h>

#include <UI/GTK/GTKWidget.h>

typedef struct _GTKImageObject GTKImageObject;

typedef struct _GTKImageInterface
{
    BEGIN_INTERFACE

    IMPLEMENTS_UNKNOWNOBJECT( GTKImageObject )
    
    END_INTERFACE
} GTKImageInterface;

interface _GTKImageObject
{
    CONST_VTBL GTKImageInterface *lpVtbl;
};

struct gtk_image_object
{
    // --- Public Members --- //
    GTKImageObject GTKImageObject_iface;

    // --- Subclasses --- //
    implements( GTKWidgetObject );

    // --- Private Members --- //
    TRLong ref;
};

// af332f42-a0ec-4ee2-b882-d975926c15f0
DEFINE_GUID( GTKImageObject, 0xaf332f42, 0xa0ec, 0x4ee2, 0xb8, 0x82, 0xd9, 0x75, 0x92, 0x6c, 0x15, 0xf0 );

// Constructors
TR_STATUS new_gtk_image_object_override_path( IN TRPath *imagePath, OUT GTKImageObject **out );

#endif