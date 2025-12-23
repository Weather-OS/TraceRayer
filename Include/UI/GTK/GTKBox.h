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

#ifndef TRACERAYER_GTKBOX_H
#define TRACERAYER_GTKBOX_H

#include <gtk/gtk.h>

#include <Object.h>
#include <Types.h>

#include <UI/GTK/GTKWidget.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _GTKBoxObject GTKBoxObject;

typedef struct _GTKBoxInterface
{
    BEGIN_INTERFACE

    IMPLEMENTS_UNKNOWNOBJECT( GTKBoxObject )

    /**
     * @Method: void GTKBoxObject::AppendWidget( GTKWidgetObject *widget )
     * @Description: Appends a child GTKWidgetObject to this GTKBoxObject.
     *               Ownership of the widget is NOT transferred.
     * @Status: Always returns T_SUCCESS.
     */
    TR_STATUS (*AppendWidget)(
        IN GTKBoxObject *This,
        IN GTKWidgetObject *widget);

    END_INTERFACE
} GTKBoxInterface;

interface _GTKBoxObject
{
    CONST_VTBL GTKBoxInterface *lpVtbl;
};

/**
 * @Object: GTKBoxObject
 * @Description: A GTK Box object that can represent multiple widgets at once.
 * @Implements:
 *      GTKWidgetObject
 */
struct gtk_box_object
{
    // --- Public Members --- //
    GTKBoxObject GTKBoxObject_iface;

    // --- Subclasses --- //
    implements( GTKWidgetObject )

    // --- Private Members --- //
    TRLong ref;
};

// b2c82c2a-c717-403e-9dba-c7bc0b8ee17e
DEFINE_GUID( GTKBoxObject, 0xb2c82c2a, 0xc717, 0x403e, 0x9d, 0xba, 0xc7, 0xbc, 0x0b, 0x8e, 0xe1, 0x7e );

// Constructors
TR_STATUS TR_API new_gtk_box_object_override_orientation_and_spacing( IN GtkOrientation orientation, IN TRInt spacing, OUT GTKBoxObject **out );

#ifdef __cplusplus
} // extern "C"

namespace TR
{
    namespace UI
    {
        class GTKBoxObject : public UnknownObject<_GTKBoxObject>
        {
        public:
            using UnknownObject::UnknownObject;
            static constexpr const TRUUID &classId = IID_GTKBoxObject;

            explicit GTKBoxObject( GtkOrientation orientation, TRInt spacing )
            {
                check_tr_( new_gtk_box_object_override_orientation_and_spacing( orientation, spacing, put() ) );
            }

            // Implements a GTKWidgetObject
            operator GTKWidgetObject() const
            {
                return QueryInterface<GTKWidgetObject>();
            }

            void AppendWidget( const GTKWidgetObject &widget ) const
            {
                check_tr_( get()->lpVtbl->AppendWidget( get(), widget.get() ) );
            }
        };
    }
}

#endif

#endif