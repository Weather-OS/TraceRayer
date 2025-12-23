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

#ifndef TRACERAYER_SIGNAL_H
#define TRACERAYER_SIGNAL_H

#include <Object.h>

typedef void (*SignalCallback)( UnknownObject *invoker, void *user_data );

typedef struct _SignalHandler
{
    TRULong id;
    SignalCallback callback;
    void *user_data;
} SignalHandler;

#define IMPLEMENTS_EVENT( type_name, name ) \
    TR_STATUS (*eventadd_##name)( IN type_name *This, IN SignalCallback callback, IN void *context, OUT TRULong *token ); \
    TR_STATUS (*eventremove_##name)( IN type_name *This, IN TRULong token );

#ifdef  __cplusplus

#include <unordered_map>
#include <memory>
#include <mutex>

template <typename From>
using SignalCallbackSafe = void (*)( From* invoker, void* user_data );

template <typename From>
struct SignalCallbackSafeObj
{
    From *invoker;
    SignalCallbackSafe<From> callback;
    void *user_data;
};

inline void
SignalCallbackHandler(
    IN _UnknownObject *invoker,
    IN void *user_data
) {
    const auto *safeCallback = static_cast<SignalCallbackSafeObj<TR::UnknownObject<UnknownObject>> *>( user_data );

    safeCallback->callback( safeCallback->invoker, safeCallback->user_data );
}

#define implements_event( name, from ) \
    std::unordered_map<TRULong, std::unique_ptr<void, void(*)(void*)>> name##_callback_store_;                      \
    std::mutex name##_callback_store_mutex_;                                                                        \
    TRULong name( SignalCallbackSafe<from> callback, void *context )                                                \
    {                                                                                                               \
        TRULong out;                                                                                                \
        SignalCallbackSafeObj<from>* callbackObj = new SignalCallbackSafeObj<from>{ this, callback, context };      \
        {                                                                                                           \
            std::scoped_lock lock( name##_callback_store_mutex_ );                                                  \
            name##_callback_store_.emplace(                                                                         \
                    out,                                                                                            \
                    std::unique_ptr<void, void(*)(void*)>( callbackObj, [](void* p)                                 \
                        {                                                                                           \
                            delete static_cast<SignalCallbackSafeObj<from>*>( p );                                  \
                        } )                                                                                         \
            );                                                                                                      \
        }                                                                                                           \
        check_tr_( get()->lpVtbl->eventadd_##name( get(), SignalCallbackHandler, callbackObj, &out ) );             \
        return out;                                                                                                 \
    }                                                                                                               \
    void name( TRULong token )                                                                                      \
    {                                                                                                               \
        check_tr_( get()->lpVtbl->eventremove_##name( get(), token ) );                                             \
        std::scoped_lock lock( name##_callback_store_mutex_ );                                                      \
        name##_callback_store_.erase( token );                                                                      \
    }

#endif

#define implements_glib_eventlist( name ) \
    GSList *name##_events;  \
    TRULong name##_next;    \
    GMutex name##_mutex;    \

#endif
