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

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _GTKWidgetObject GTKWidgetObject;

typedef struct _GTKWidgetInterface
{
    BEGIN_INTERFACE

    IMPLEMENTS_UNKNOWNOBJECT( GTKWidgetObject )

    /**
     * @Method: GtkWidget* GTKWidgetObject::Widget()
     * @Description: Gets the widget assigned during the constructor.
     * @Status: Always returns T_SUCCESS.
     */
    TR_STATUS (*get_Widget)(
        IN GTKWidgetObject *This,
        OUT GtkWidget      **out);

    /**
     * @Method: void GTKWidgetObject::setVisibility( TRBool visibility )
     * @Description: Determines the visibility of the current widget.
     */
    void      (*setVisibility)(
        IN GTKWidgetObject *This,
        TRBool              visibility);
    
    END_INTERFACE
} GTKWidgetInterface;

interface _GTKWidgetObject
{
    CONST_VTBL GTKWidgetInterface *lpVtbl;
};

/**
 * @Object: GTKWidgetObject
 * @Description: Inherited from many different GTK Objects.
 *               This object represents a Widget for different GTK Objects,
 *               to use internally
 */
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
TR_STATUS TR_API new_gtk_widget_object_override_widget( IN GtkWidget *widget, OUT GTKWidgetObject **out );

#ifdef __cplusplus
} // extern "C"

namespace TR
{
    namespace UI
    {
        class GTKWidgetObject : public UnknownObject<_GTKWidgetObject>
        {
        public:
            using UnknownObject::UnknownObject;
            static constexpr const TRUUID &classId = IID_GTKWidgetObject;

            explicit GTKWidgetObject( GtkWidget *widget )
            {
                check_tr_( new_gtk_widget_object_override_widget( widget, put() ) );
            }

            [[nodiscard]]
            GtkWidget *Widget() const noexcept
            {
                GtkWidget *out;
                get()->lpVtbl->get_Widget( get(), &out );
                return out;
            }

            void SetVisibility( TRBool visibility ) const noexcept
            {
                get()->lpVtbl->setVisibility( get() , visibility );
            }
        };
    }
}

#endif


#endif