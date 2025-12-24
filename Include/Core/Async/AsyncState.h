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

#ifndef TRACERAYER_AsyncState_H
#define TRACERAYER_AsyncState_H

#include <Object.h>
#include <Types.h>

#include <glib.h>

typedef struct _AsyncStateObject AsyncStateObject;
typedef struct _AsyncStateCompletedHandlerObject AsyncStateCompletedHandlerObject;

typedef TR_STATUS (*async_operation_callback)( UnknownObject *invoker, void *param, PropVariant *result );

typedef enum AsyncStatus {
    AsyncStatus_Started = 0,
    AsyncStatus_Completed = 1,
    AsyncStatus_Canceled = 2,
    AsyncStatus_Error = 3,
    AsyncStatus_Closed = 4
} AsyncStatus;

typedef struct _AsyncStateInterface
{
    BEGIN_INTERFACE

    IMPLEMENTS_UNKNOWNOBJECT( AsyncStateObject )

    TR_STATUS (*get_Completed)( IN AsyncStateObject *This, OUT AsyncStateCompletedHandlerObject **out ); // getter
    TR_STATUS (*set_Completed)( IN AsyncStateObject *This, IN AsyncStateCompletedHandlerObject *completed ); // setter
    TR_STATUS (*get_Status)( IN AsyncStateObject *This, OUT AsyncStatus *out ); // getter
    TR_STATUS (*get_ErrorCode)( IN AsyncStateObject *This, OUT TR_STATUS *out ); // getter
    TR_STATUS (*Result)( IN AsyncStateObject *This, OUT PropVariant **out );
    TR_STATUS (*Start)( IN AsyncStateObject *This );
    TR_STATUS (*Cancel)( IN AsyncStateObject *This );
    TR_STATUS (*Close)( IN AsyncStateObject *This );

    END_INTERFACE
} AsyncStateInterface;

// type-pruning version of AsyncOperationCompletedHandlerObject
typedef struct _AsyncStateCompletedHandlerInterface
{
    BEGIN_INTERFACE

    IMPLEMENTS_UNKNOWNOBJECT( AsyncStateCompletedHandlerObject )

    TR_STATUS (*Invoke)( AsyncStateCompletedHandlerObject *This, UnknownObject /* pruned from AsyncOperationObject */ *info, AsyncStatus status );

    END_INTERFACE
} AsyncStateCompletedHandlerInterface;

interface _AsyncStateObject
{
    CONST_VTBL AsyncStateInterface *lpVtbl;
};

interface _AsyncStateCompletedHandlerObject
{
    CONST_VTBL AsyncStateCompletedHandlerInterface *lpVtbl;
};

struct async_state_object
{
    // --- Public Members --- //
    AsyncStateObject AsyncStateObject_iface;
    AsyncStateCompletedHandlerObject *Completed;
    AsyncStatus Status;
    TR_STATUS ErrorCode;

    // --- Private Members --- //
    async_operation_callback callback;
    GThreadPool *pool;
    UnknownObject *invoker;
    UnknownObject *outer;
    void *param;
    pthread_mutex_t lock;
    PropVariant *result;
    ATOMIC(TRLong) ref;
};

delegate( async_state_completed_handler_object )

// d81ab70d-82e0-481c-983d-401225d98a2c
DEFINE_GUID( AsyncStateObject, 0xd81ab70d, 0x82e0, 0x481c, 0x98, 0x3d, 0x40, 0x12, 0x25, 0xd9, 0x8a, 0x2c );

// fcdcf02c-e5d8-4478-915a-4d90b74b83a5
DEFINE_GUID( AsyncStateCompletedHandlerObject, 0xfcdcf02c, 0xe5d8, 0x4478, 0x91, 0x5a, 0x4d, 0x90, 0xb7, 0x4b, 0x83, 0xa5 );

// Constructors
TR_STATUS new_async_state_object_override_callback_and_outer( IN UnknownObject *invoker, IN void *param, IN async_operation_callback callback, IN UnknownObject *outer, OUT AsyncStateObject **out );

#endif