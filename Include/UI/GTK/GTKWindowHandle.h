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

#ifndef TRACERAYER_GTKWINDOWHANDLE_H
#define TRACERAYER_GTKWINDOWHANDLE_H

#include <gdk/gdk.h>

#include <Object.h>
#include <Types.h>

#include <UI/GTK/GTKWidget.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _GTKWindowHandleObject GTKWindowHandleObject;

typedef struct _GTKWindowHandleInterface
{
    BEGIN_INTERFACE

    IMPLEMENTS_UNKNOWNOBJECT( GTKWindowHandleObject )

    /**
     * @Method: GTKWidgetObject* GTKWindowHandleObject::ChildWidget()
     * @Description: Retrieves the child GTKWidgetObject associated with this
     *               GTKWindowHandleObject.
     * @Returns: The child GTKWidgetObject, or nullptr if no child is set.
     * @Status: Returns T_NOINIT if no child widget is initialized; otherwise
     *          returns T_SUCCESS.
     */
    TR_STATUS (*get_ChildWidget)(
        IN  GTKWindowHandleObject *This,
        OUT GTKWidgetObject       **out);

    /**
     * @Method: void GTKWindowHandleObject::ChildWidget( GTKWidgetObject *widget )
     * @Description: Assigns a child GTKWidgetObject to this GTKWindowHandleObject.
     *               Ownership of the widget is transferred to the window handle,
     *               which becomes responsible for its lifetime.
     * @Status: Always returns T_SUCCESS.
     */
    TR_STATUS (*set_ChildWidget)(
        IN GTKWindowHandleObject *This,
        IN GTKWidgetObject       *widget);

    END_INTERFACE
} GTKWindowHandleInterface;

interface _GTKWindowHandleObject
{
    CONST_VTBL GTKWindowHandleInterface *lpVtbl;
};

/**
 * @Object: GTKWindowHandleObject
 * @Description: A GTK Window Handle Object mainly used for drag
 *               and click events on a GTKWindowObject surface.
 * @Implements:
 *      GTKWidgetObject
 */
struct gtk_window_handle_object
{
    // --- Public Members --- //
    GTKWindowHandleObject GTKWindowHandleObject_iface;
    GTKWidgetObject *ChildWidget;

    // --- Base Interfaces --- //
    implements( GTKWidgetObject );

    // --- Private Members --- //
    ATOMIC(TRLong) ref;
};

// 674b7bc3-ac8b-411e-b93a-d59319554acd
DEFINE_GUID( GTKWindowHandleObject, 0x674b7bc3, 0xac8b, 0x411e, 0xb9, 0x3a, 0xd5, 0x93, 0x19, 0x55, 0x4a, 0xcd );

// Constructors
TR_STATUS TR_API new_gtk_window_handle_object( OUT GTKWindowHandleObject **out );

#ifdef __cplusplus
} // extern "C"

namespace TR
{
    namespace UI
    {
        class GTKWindowHandleObject : public UnknownObject<_GTKWindowHandleObject>
        {
        public:
            using UnknownObject::UnknownObject;
            static constexpr const TRUUID &classId = IID_GTKWindowHandleObject;

            explicit GTKWindowHandleObject()
            {
                check_tr_( new_gtk_window_handle_object( put() ) );
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
                _GTKWidgetObject *ChildWidget = widget.get();
                check_tr_( get()->lpVtbl->set_ChildWidget( get(), widget.get() ) );
            }
        };
    }
}
#endif

#endif