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

#ifndef TRACERAYER_ASYNCOPERATIONCOMPLETEDHANDLERDEFAULT_H
#define TRACERAYER_ASYNCOPERATIONCOMPLETEDHANDLERDEFAULT_H

#include <Core/Async/AsyncOperation.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef TR_STATUS (*async_operation_completed_handler_callback)( AsyncOperationObject *invoker, void *param, AsyncStatus status );

struct async_operation_completed_handler_default_object
{
    // --- Public Members --- //
    AsyncOperationCompletedHandlerObject AsyncOperationCompletedHandlerObject_iface;

    // --- Private Members --- //
    async_operation_completed_handler_callback callback;
    void *param;
    ATOMIC(TRLong) ref;
};

// Constructors
TR_STATUS TR_API async_operation_completed_handler_default_object_override_callback( IN async_operation_completed_handler_callback callback, IN void *param, OUT AsyncOperationCompletedHandlerObject **out );

#ifdef __cplusplus
} // extern "C"

namespace TR
{
    namespace Core::Async
    {
        using AsyncOperationCompletedHandlerCallbackSafe = void (*)( const AsyncOperationObject &invoker, void *param, AsyncStatus status );

        struct AsyncOperationCompletedHandlerCallbackSafeObj
        {
            AsyncOperationCompletedHandlerCallbackSafe callback;
            void *param;
        };

        inline TR_STATUS
        AsyncOperationCompletedHandlerCallbackHandler(
            IN _AsyncOperationObject *invoker,
            IN void *param,
            AsyncStatus status
        ) {
            const auto *safeCallback = static_cast<AsyncOperationCompletedHandlerCallbackSafeObj *>( param );

            try
            {
                safeCallback->callback( AsyncOperationObject( invoker ), safeCallback->param, status );
            } catch ( const TRException &e )
            {
                return e.status;
            }

            // Mirroring internal behavior.
            if ( status > AsyncStatus_Started )
                delete safeCallback;
            return T_SUCCESS;
        }

        class AsyncOperationCompletedHandlerDefaultObject : public UnknownObject<_AsyncOperationCompletedHandlerObject>
        {
        public:
            using UnknownObject::UnknownObject;
            static constexpr const TRUUID &classId = IID_AsyncOperationCompletedHandlerObject;

            explicit AsyncOperationCompletedHandlerDefaultObject( AsyncOperationCompletedHandlerCallbackSafe callback, void* param )
            {
                AsyncOperationCompletedHandlerCallbackSafeObj* callbackObj = new AsyncOperationCompletedHandlerCallbackSafeObj{ callback, param };
                check_tr_( async_operation_completed_handler_default_object_override_callback( AsyncOperationCompletedHandlerCallbackHandler, callbackObj, put() ) );
            }

            void Invoke( const AsyncOperationObject &info, AsyncStatus status ) const
            {
                check_tr_( get()->lpVtbl->Invoke( get(), info.get(), status ) );
            }

            // Forward compatible with AsyncOperationCompletedHandlerObject
            operator AsyncOperationCompletedHandlerObject() const
            {
                return AsyncOperationCompletedHandlerObject( get() );
            }
        };
    }
}
#endif

#endif