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

#ifndef TRACERAYER_GTKSPINNER_H
#define TRACERAYER_GTKSPINNER_H

#include <gtk/gtk.h>

#include <Object.h>
#include <Types.h>

#include <UI/GTK/GTKWidget.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _GTKSpinnerObject GTKSpinnerObject;

typedef struct _GTKSpinnerInterface
{
    BEGIN_INTERFACE

    IMPLEMENTS_UNKNOWNOBJECT( GTKSpinnerObject )

    /**
     * @Method: TRBool GTKSpinnerObject::Spinning()
     * @Description: Returns true if the snipper is spinning, and false otherwise.
     * @Status: Always returns T_SUCCESS.
     */
    TR_STATUS (*get_Spinning)(
        IN GTKSpinnerObject *This,
        OUT TRBool          *out);

    /**
     * @Method: void GTKSpinnerObject::Spinning( TRBool spinning )
     * @Description: Sets the current spinning status.
     * @Status: Returns T_SUCCESS unless QueryInterface from GTKWidgetObject fails.
     */
    TR_STATUS (*set_Spinning)(
        IN GTKSpinnerObject *This,
        OUT TRBool           spinning);

    END_INTERFACE
} GTKSpinnerInterface;

interface _GTKSpinnerObject
{
    CONST_VTBL GTKSpinnerInterface *lpVtbl;
};

/**
 * @Object: GTKSpinnerObject
 * @Description: A GTK Spinner object used to signify loading.
 * @Implements:
 *      GTKWidgetObject
 */
struct gtk_spinner_object
{
    // --- Public Members --- //
    GTKSpinnerObject GTKSpinnerObject_iface;
    TRBool Spinning;

    // --- Subclasses --- //
    implements( GTKWidgetObject )

    // --- Private Members --- //
    ATOMIC(TRLong) ref;
};

// 68e7c87b-7e17-4b34-8ef5-6761caab417b
DEFINE_GUID( GTKSpinnerObject, 0x68e7c87b, 0x7e17, 0x4b34, 0x8e, 0xf5, 0x67, 0x61, 0xca, 0xab, 0x41, 0x7b );

// Constructors
TR_STATUS TR_API new_gtk_spinner_object( OUT GTKSpinnerObject **out );

#ifdef __cplusplus
} // extern "C"

namespace TR
{
    namespace UI
    {
        class GTKSpinnerObject : public UnknownObject<_GTKSpinnerObject>
        {
        public:
            using UnknownObject::UnknownObject;
            static constexpr const TRUUID &classId = IID_GTKSpinnerObject;

            explicit GTKSpinnerObject()
            {
                check_tr_( new_gtk_spinner_object( put() ) );
            }

            // Implements a GTKWidgetObject
            operator GTKWidgetObject() const
            {
                return QueryInterface<GTKWidgetObject>();
            }

            TRBool Spinning() const noexcept
            {
                TRBool spinning;
                get()->lpVtbl->get_Spinning( get(), &spinning );
                return spinning;
            }

            void Spinning( TRBool spinning ) const noexcept
            {
                get()->lpVtbl->set_Spinning( get(), spinning );
            }
        };
    }
}

#endif

#endif