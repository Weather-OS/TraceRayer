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

#ifndef TRACERAYER_GTKLABEL_H
#define TRACERAYER_GTKLABEL_H

#include <gdk/gdk.h>

#include <IO/Path.h>
#include <Object.h>
#include <Types.h>

#include <UI/GTK/GTKWidget.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _GTKLabelObject GTKLabelObject;

typedef struct _GTKLabelInterface
{
    BEGIN_INTERFACE

    IMPLEMENTS_UNKNOWNOBJECT( GTKLabelObject )

    END_INTERFACE
} GTKLabelInterface;

com_interface _GTKLabelObject
{
    CONST_VTBL GTKLabelInterface *lpVtbl;
};

/**
 * @Object: GTKLabelObject
 * @Description: A GTK Label object that represents a text.
 * @Implements:
 *      GTKWidgetObject
 */
struct gtk_label_object
{
    // --- Public Members --- //
    GTKLabelObject GTKLabelObject_iface;

    // --- Subclasses --- //
    implements( GTKWidgetObject );

    // --- Private Members --- //
    ATOMIC(TRLong) ref;
};

// 799390ab-f9f0-4e7e-b737-848363218e72
DEFINE_GUID( GTKLabelObject, 0x799390ab, 0xf9f0, 0x4e7e, 0xb7, 0x37, 0x84, 0x83, 0x63, 0x21, 0x8e, 0x72 );

// Constructors
TR_STATUS TR_API new_gtk_label_object_override_text( IN TRCString text, OUT GTKLabelObject **out );

#ifdef __cplusplus
} // extern "C"

namespace TR
{
    namespace UI
    {
        class GTKLabelObject : public UnknownObject<_GTKLabelObject>
        {
        public:
            using UnknownObject::UnknownObject;
            static constexpr const TRUUID &classId = IID_GTKLabelObject;

            explicit GTKLabelObject( const std::string& text )
            {
                check_tr_( new_gtk_label_object_override_text( text.c_str(), put() ) );
            }

            // Implements a GTKWidgetObject
            operator GTKWidgetObject() const
            {
                return QueryInterface<GTKWidgetObject>();
            }
        };
    }
}

#endif

#endif