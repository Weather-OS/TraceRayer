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

#ifndef TRACERAYER_ASYNCOPERATION_H
#define TRACERAYER_ASYNCOPERATION_H

#include <Object.h>
#include <Types.h>

#include <Core/Async/AsyncInfo.h>

typedef struct _AsyncOperationObject AsyncOperationObject;
typedef struct _AsyncOperationCompletedHandlerObject AsyncOperationCompletedHandlerObject;

typedef struct _AsyncOperationInterface
{
    BEGIN_INTERFACE

    IMPLEMENTS_UNKNOWNOBJECT( AsyncOperationObject )

    TR_STATUS (*get_Completed)( IN AsyncOperationObject *This, OUT AsyncOperationCompletedHandlerObject **out ); // getter
    TR_STATUS (*set_Completed)( IN AsyncOperationObject *This, IN AsyncOperationCompletedHandlerObject *completed ); // setter
    TR_STATUS (*GetResults)( IN AsyncOperationObject *This, OUT PropVariant **out );

    END_INTERFACE
} AsyncOperationInterface;

typedef struct _AsyncOperationCompletedHandlerObjectInterface
{
    BEGIN_INTERFACE

    IMPLEMENTS_UNKNOWNOBJECT( AsyncOperationCompletedHandlerObject )

    TR_STATUS (*Invoke)( AsyncOperationCompletedHandlerObject *This, AsyncOperationObject *info, AsyncStatus status );

    END_INTERFACE
} AsyncOperationCompletedHandlerObjectInterface;

interface _AsyncOperationObject
{
    CONST_VTBL AsyncOperationInterface *lpVtbl;
};

interface _AsyncOperationCompletedHandlerObject
{
    CONST_VTBL AsyncOperationCompletedHandlerObjectInterface *lpVtbl;
};

struct async_operation_object
{
    // --- Public Members --- //
    AsyncOperationObject AsyncOperationObject_iface;

    // --- Base Interfaces --- //
    implements( AsyncInfoObject );

    // --- Private Members --- //
    ATOMIC(TRLong) ref;
};

delegate( async_operation_completed_handler_object ) // Implementation dependent

// 9fc2b0bb-e446-44e2-aa61-9cab8f636af2
DEFINE_GUID( AsyncOperationObject, 0x9fc2b0bb, 0xe446, 0x44e2, 0xaa, 0x61, 0xb0, 0x9c, 0xab, 0x63, 0x6a, 0xf2 );

// fcdcf02c-e5d8-4478-915a-4d90b74b83a5
DEFINE_GUID( AsyncOperationCompletedHandlerObject, 0xfcdcf02c, 0xe5d8, 0x4478, 0x91, 0x5a, 0x4d, 0x90, 0xb7, 0x4b, 0x83, 0xa5 );

// Constructors
TR_STATUS new_async_operation_object_override_callback( IN UnknownObject *invoker, IN void *param, IN async_operation_callback callback, OUT AsyncOperationObject **out );

#endif