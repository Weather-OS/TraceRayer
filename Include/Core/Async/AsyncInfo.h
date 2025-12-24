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

#ifndef TRACERAYER_ASYNCINFO_H
#define TRACERAYER_ASYNCINFO_H

#include <Object.h>
#include <Types.h>

#include <Core/Async/AsyncState.h>

typedef struct _AsyncInfoObject AsyncInfoObject;

typedef struct _AsyncInfoInterface
{
    BEGIN_INTERFACE

    IMPLEMENTS_UNKNOWNOBJECT( AsyncInfoObject )

    TR_STATUS (*get_Status)( IN AsyncInfoObject *This, OUT AsyncStatus *out ); // getter
    TR_STATUS (*get_ErrorCode)( IN AsyncInfoObject *This, OUT TR_STATUS *out ); // getter
    TR_STATUS (*Cancel)( IN AsyncInfoObject *This );
    TR_STATUS (*Close)( IN AsyncInfoObject *This );

    END_INTERFACE
} AsyncInfoInterface;

interface _AsyncInfoObject
{
    CONST_VTBL AsyncInfoInterface *lpVtbl;
};

struct async_info_object
{
    // --- Public Members --- //
    AsyncInfoObject AsyncInfoObject_iface;

    // --- Base Interfaces --- //
    implements( AsyncStateObject );

    // --- Private Members --- //
    ATOMIC(TRLong) ref;
};

// 00000036-0000-0000-c000-000000000046
DEFINE_GUID( AsyncInfoObject, 0x00000036, 0x0000, 0x0000, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46 );

// Constructors
TR_STATUS new_async_info_object_override_callback_and_outer( IN UnknownObject *invoker, IN void *param, IN async_operation_callback callback, IN UnknownObject *outer, OUT AsyncInfoObject **out );

#endif