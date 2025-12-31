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

#ifndef TRACERAYER_ASYNCAWAITER_HPP
#define TRACERAYER_ASYNCAWAITER_HPP

#include <coroutine>
#include <future>
#include <Core/Async/AsyncOperation.h>
#include <Core/Async/AsyncOperationCompletedHandlerDefault.h>

struct deferred_completed_handler_object
{
    // --- Public Members --- //
    AsyncOperationCompletedHandlerObject AsyncOperationCompletedHandlerObject_iface;

    // --- Private Members --- //
    ATOMIC(TRLong) ref;
};

// co-routines
namespace TR
{
    namespace Core::Async
    {
        struct AsyncOperationCoAwaiter
        {
            struct promise_type
            {
                AsyncOperationCoAwaiter get_return_object() noexcept
                {
                    return AsyncOperationCoAwaiter{ std::coroutine_handle<promise_type>::from_promise( *this ) };
                }

                static std::suspend_always initial_suspend() noexcept { return {}; }
                static std::suspend_always final_suspend() noexcept { return {}; }
                static void return_void() noexcept {}
                static void unhandled_exception() { std::terminate(); }
            };

            AsyncOperationObject op;
            PropVariant* result = nullptr;
            std::exception_ptr eptr;
            std::coroutine_handle<promise_type> m_handle;
            std::coroutine_handle<> received_handle; // keepalive purposes.

            explicit AsyncOperationCoAwaiter( std::coroutine_handle<promise_type> h )
                : m_handle( h ) {}

            ~AsyncOperationCoAwaiter() { if ( m_handle ) m_handle.destroy(); }

            explicit AsyncOperationCoAwaiter( AsyncOperationObject o )
                : op( std::move( o ) ) {}

            // If operation already completed we can return immediately.
            bool await_ready()
            {
                AsyncStatus status = op.QueryInterface<AsyncInfoObject>().CurrentStatus();
                if ( status != AsyncStatus_Completed && status != AsyncStatus_Error )
                {
                    return false;
                }

                result = op.GetResults();
                return true;
            }

            // Called when coroutine should be suspended. attach Completed handler and resume later.
            void await_suspend( std::coroutine_handle<> h )
            {
                received_handle = h;
                auto handler = AsyncOperationCompletedHandlerDefaultObject( []( const AsyncOperationObject &invoker, void *param, AsyncStatus status ) -> void
                {
                    auto instance = static_cast<AsyncOperationCoAwaiter*>( param );

                    try
                    {
                        if (status == AsyncStatus_Completed)
                        {
                            instance->result = invoker.GetResults();
                        }
                        else if (status == AsyncStatus_Error)
                        {
                            instance->eptr = std::make_exception_ptr (
                                TRException( invoker.QueryInterface<AsyncInfoObject>().ErrorCode() )
                            );
                        }
                    }
                    catch ( ... )
                    {
                        instance->eptr = std::current_exception();
                    }

                    if ( instance->received_handle )
                        instance->received_handle.resume();

                }, this );

                // NOTE: CoAwaiter has it's own reference here.
                handler.get()->lpVtbl->AddRef( handler.get() );
                op.Completed( std::move( handler ) );
            }

            [[nodiscard]]
            PropVariant* await_resume() const
            {
                if ( eptr ) std::rethrow_exception( eptr );
                return result;
            }
        };

        inline AsyncOperationCoAwaiter operator co_await( AsyncOperationObject op )
        {
            // NOTE: CoAwaiter has it's own reference here.
            op.get()->lpVtbl->AddRef( op.get() );
            return AsyncOperationCoAwaiter( std::move( op ) );
        }
    }
}


namespace TR
{
    namespace Core::Async
    {
        class AwaitableAsyncOperationObject : public AsyncOperationObject
        {
        public:
            using AsyncOperationObject::AsyncOperationObject;

            PropVariant await()
            {
                auto promise = std::make_shared<std::promise<PropVariant>>();
                auto future = promise->get_future();

                auto starter = [this, promise]() -> AsyncOperationCoAwaiter
                {
                    try
                    {
                        PropVariant* result = co_await AsyncOperationObject( get() );
                        promise->set_value( *result );
                    } catch ( const TRException &e )
                    {
                        promise->set_exception( std::current_exception() );
                    }
                    co_return;
                };

                auto awaiter = starter();
                if ( awaiter.m_handle ) awaiter.m_handle.resume();
                return future.get();
            }
        };
    }
}

#endif