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

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _GTKOverlayObject GTKOverlayObject;

typedef struct _GTKOverlayInterface
{
    BEGIN_INTERFACE

    IMPLEMENTS_UNKNOWNOBJECT( GTKOverlayObject )

    /**
     * @Method: GTKWidgetObject* GTKOverlayObject::ChildWidget()
     * @Description: Retrieves the child GTKWidgetObject associated with this
     *               GTKOverlayObject.
     * @Returns: The child GTKWidgetObject, or nullptr if no child is set.
     * @Status: Returns T_NOINIT if no child widget is initialized; otherwise
     *          returns T_SUCCESS.
     */
    TR_STATUS (*get_ChildWidget)(
        IN  GTKOverlayObject *This,
        OUT GTKWidgetObject **out);

    /**
     * @Method: void GTKOverlayObject::ChildWidget( GTKWidgetObject *widget )
     * @Description: Assigns a child GTKWidgetObject to this GTKOverlayObject.
     *               Ownership of the widget is transferred to the window handle,
     *               which becomes responsible for its lifetime.
     * @Status: Always returns T_SUCCESS.
     */
    TR_STATUS (*set_ChildWidget)(
        IN GTKOverlayObject *This,
        IN GTKWidgetObject *widget);

    /**
     * @Method: void GTKOverlayObject::AddWidget( GTKWidgetObject *widget )
     * @Description: Adds a child GTKWidgetObject to this GTKOverlayObject.
     *               Ownership of the widget is NOT transferred.
     * @Status: Always returns T_SUCCESS.
     */
    TR_STATUS (*AddWidget)(
        IN GTKOverlayObject *This,
        IN GTKWidgetObject *widget);

    END_INTERFACE
} GTKOverlayInterface;

com_interface _GTKOverlayObject
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
    GTKWidgetObject *ChildWidget;

    // --- Subclasses --- //
    implements( GTKWidgetObject )

    // --- Private Members --- //
    ATOMIC(TRLong) ref;
};

// fa685c6a-dc44-4e0e-a4fb-4d5457d859a1
DEFINE_GUID( GTKOverlayObject, 0xfa685c6a, 0xdc44, 0x4e0e, 0xa4, 0xfb, 0x4d, 0x54, 0x57, 0xd8, 0x59, 0xa1 );

// Constructors
TR_STATUS TR_API new_gtk_overlay_object( OUT GTKOverlayObject **out );

#ifdef __cplusplus
} // extern "C"

namespace TR
{
    namespace UI
    {
        class GTKOverlayObject : public UnknownObject<_GTKOverlayObject>
        {
        public:
            using UnknownObject::UnknownObject;
            static constexpr const TRUUID &classId = IID_GTKOverlayObject;

            explicit GTKOverlayObject()
            {
                check_tr_( new_gtk_overlay_object( put() ) );
            }

            // Implements a GTKWidgetObject
            operator GTKWidgetObject() const
            {
                return QueryInterface<GTKWidgetObject>();
            }

            [[nodiscard]]
            GTKWidgetObject ChildWidget() const
            {
                _GTKWidgetObject *ChildWidget;
                check_tr_( get()->lpVtbl->get_ChildWidget( get(), &ChildWidget ) );
                return GTKWidgetObject( ChildWidget );
            }

            void ChildWidget( const GTKWidgetObject& widget ) const
            {
                check_tr_( get()->lpVtbl->set_ChildWidget( get(), widget.get() ) );
            }

            void AddWidget( const GTKWidgetObject &widget ) const
            {
                check_tr_( get()->lpVtbl->AddWidget( get(), widget.get() ) );
            }
        };
    }
}

#endif

#endif