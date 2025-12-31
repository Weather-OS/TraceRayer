/*
 * Async operation implementation
 *
 * Copyright (C) 2024 Weather
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 * This code is based on concepts and implementation patterns from Wine
 * (https://www.winehq.org/), which is licensed under LGPL-2.1-or-later.
 */

/**
 *  Module: AsyncState.c
 *  Description: GLib Threadpool backed async handling.
 */

#include <glib.h>

#include <Core/Async/AsyncState.h>

#define HANDLER_NOT_SET (AsyncStateCompletedHandlerObject *)((void *)~(TRULong)0)

static struct async_state_object *impl_from_AsyncStateObject( AsyncStateObject *iface )
{
    return CONTAINING_RECORD( iface, struct async_state_object, AsyncStateObject_iface );
}

static TR_STATUS async_state_object_QueryInterface( AsyncStateObject *iface, const TRUUID uuid, void **out )
{
    TRACE( "iface %p, uuid %s, out %p\n", iface, debugstr_uuid( uuid ), out );

    if ( !uuid_compare( uuid, IID_UnknownObject ) || !uuid_compare( uuid, IID_AsyncStateObject ) )
    {
        iface->lpVtbl->AddRef( iface );
        *out = iface;
        return T_SUCCESS;
    }

    ERROR( "uuid %s is not implemented! returning T_NOTIMPL\n", debugstr_uuid( uuid ) );
    return T_NOTIMPL;
}

static TRLong async_state_object_AddRef( AsyncStateObject *iface )
{
    struct async_state_object *impl = impl_from_AsyncStateObject( iface );
    const TRLong added = atomic_fetch_add( &impl->ref, 1 ) + 1;
    TRACE( "iface %p increasing ref count to %ld\n", iface, added );
    return added;
}

static TRLong async_state_object_Release( AsyncStateObject *iface )
{
    struct async_state_object *impl = impl_from_AsyncStateObject( iface );
    const ATOMIC(TRLong) removed = atomic_fetch_sub( &impl->ref, 1 );
    TRACE( "iface %p decreasing ref count to %ld\n", iface, removed - 1 );
    if ( !(removed - 1) )
    {
        if ( impl->Completed && impl->Completed != HANDLER_NOT_SET )
            impl->Completed->lpVtbl->Release( impl->Completed );
        iface->lpVtbl->Close( iface );
        if ( impl->invoker )
            impl->invoker->lpVtbl->Release( impl->invoker );
        pthread_mutex_destroy( &impl->lock );
        free( impl );
    }
    return removed;
}

static TR_STATUS async_state_object_get_Completed( AsyncStateObject *iface, AsyncStateCompletedHandlerObject **out )
{
    TR_STATUS status = T_SUCCESS;

    struct async_state_object *impl = impl_from_AsyncStateObject( iface );

    TRACE( "iface %p out %p\n", iface, out );

    if ( !out ) throw_NullPtrException();

    pthread_mutex_lock( &impl->lock );
    if ( impl->CurrentStatus == AsyncStatus_Closed )
        status = T_ILLEGAL_METHOD_CALL;

    if ( impl->Completed == NULL || impl->Completed == HANDLER_NOT_SET )
        *out = nullptr;
    else
    {
        impl->Completed->lpVtbl->AddRef( impl->Completed );
        *out = impl->Completed;
    }
    pthread_mutex_unlock( &impl->lock );

    return status;
}

static TR_STATUS async_state_object_set_Completed( AsyncStateObject *iface, AsyncStateCompletedHandlerObject *completed )
{
    TR_STATUS status = T_SUCCESS;

    struct async_state_object *impl = impl_from_AsyncStateObject( iface );

    TRACE( "iface %p completed %p\n", iface, completed );

    pthread_mutex_lock( &impl->lock );
    if ( impl->CurrentStatus == AsyncStatus_Closed )
        status = T_ILLEGAL_METHOD_CALL;
    else if ( impl->Completed != HANDLER_NOT_SET )
        status = T_ILLEGAL_DELEGATE_ASSIGNMENT;
    else if ( completed != nullptr )
    {
        completed->lpVtbl->AddRef( completed );
        impl->Completed = completed;

        if ( impl->CurrentStatus > AsyncStatus_Started )
        {
            impl->Completed = nullptr; // Prevent concurrent invoke
            pthread_mutex_unlock( &impl->lock );

            completed->lpVtbl->Invoke( completed, impl->outer, impl->CurrentStatus );
            completed->lpVtbl->Release( completed );

            return T_SUCCESS;
        }
    }
    pthread_mutex_unlock( &impl->lock );

    return status;
}

static TR_STATUS async_state_object_get_CurrentStatus( AsyncStateObject *iface, AsyncStatus *out )
{
    TR_STATUS status = T_SUCCESS;

    struct async_state_object *impl = impl_from_AsyncStateObject( iface );

    TRACE( "iface %p out %p\n", iface, out );

    if ( !out ) throw_NullPtrException();

    pthread_mutex_lock( &impl->lock );
    if ( impl->CurrentStatus == AsyncStatus_Closed )
        status = T_ILLEGAL_METHOD_CALL;
    *out = impl->CurrentStatus;
    pthread_mutex_unlock( &impl->lock );

    return status;
}

static TR_STATUS async_state_object_get_ErrorCode( AsyncStateObject *iface, TR_STATUS *out )
{
    TR_STATUS status = T_SUCCESS;

    struct async_state_object *impl = impl_from_AsyncStateObject( iface );

    TRACE( "iface %p out %p\n", iface, out );

    if ( !out ) throw_NullPtrException();

    pthread_mutex_lock( &impl->lock );
    if ( impl->CurrentStatus == AsyncStatus_Closed )
        *out = status = T_ILLEGAL_METHOD_CALL;
    else
        *out = impl->ErrorCode;
    pthread_mutex_unlock( &impl->lock );

    return status;
}

static TR_STATUS async_state_object_Result( AsyncStateObject *iface, PropVariant **out )
{
    TR_STATUS status = T_SUCCESS;

    struct async_state_object *impl = impl_from_AsyncStateObject( iface );

    TRACE( "iface %p out %p\n", iface, out );

    if ( !out ) throw_NullPtrException();

    pthread_mutex_lock( &impl->lock );
    if ( impl->CurrentStatus == AsyncStatus_Completed || impl->CurrentStatus == AsyncStatus_Error )
    {
        *out = impl->result;
        status = impl->ErrorCode;
    }
    pthread_mutex_unlock( &impl->lock );

    return status;
}

static TR_STATUS async_state_object_Start( AsyncStateObject *iface )
{
    struct async_state_object *impl = impl_from_AsyncStateObject( iface );

    TRACE( "iface %p\n", iface );

    impl->outer->lpVtbl->AddRef( impl->outer ); // keep the async alive in the callback
    g_thread_pool_push( impl->pool, (void *)&impl->AsyncStateObject_iface, nullptr );

    return T_SUCCESS;
}

static TR_STATUS async_state_object_Cancel( AsyncStateObject *iface )
{
    TR_STATUS status = T_SUCCESS;

    struct async_state_object *impl = impl_from_AsyncStateObject( iface );

    TRACE( "iface %p\n", iface );

    pthread_mutex_lock( &impl->lock );
    if ( impl->CurrentStatus == AsyncStatus_Closed )
        status = T_ILLEGAL_METHOD_CALL;
    else if ( impl->CurrentStatus == AsyncStatus_Started )
        impl->CurrentStatus = AsyncStatus_Closed;
    pthread_mutex_unlock( &impl->lock );

    return status;
}

static TR_STATUS async_state_object_Close( AsyncStateObject *iface )
{
    TR_STATUS status = T_SUCCESS;

    struct async_state_object *impl = impl_from_AsyncStateObject( iface );

    TRACE( "iface %p\n", iface );

    pthread_mutex_lock( &impl->lock );
    if ( impl->CurrentStatus == AsyncStatus_Started )
        status = T_ILLEGAL_STATE_CHANGE;
    else if ( impl->CurrentStatus != AsyncStatus_Closed )
    {
        g_thread_pool_free( impl->pool, false, false );
        impl->pool = nullptr;
        impl->CurrentStatus = AsyncStatus_Closed;
    }
    pthread_mutex_unlock( &impl->lock );

    return status;
}

static AsyncStateInterface async_state_interface =
{
    /* UnknownObject Methods */
    async_state_object_QueryInterface,
    async_state_object_AddRef,
    async_state_object_Release,
    /* AsyncStateObject Methods */
    async_state_object_get_Completed,
    async_state_object_set_Completed,
    async_state_object_get_CurrentStatus,
    async_state_object_get_ErrorCode,
    async_state_object_Result,
    async_state_object_Start,
    async_state_object_Cancel,
    async_state_object_Close
};

static void async_state_object_callback( void *iface, void *user_data )
{
    TR_STATUS status = T_SUCCESS;
    PropVariant result;

    struct async_state_object *impl = impl_from_AsyncStateObject( (AsyncStateObject *)iface );

    TRACE( "iface %p, user_data %p\n", iface, user_data );

    PropVariantInit( &result );

    status = impl->callback( impl->invoker, impl->param, &result );

    pthread_mutex_lock( &impl->lock );
    if ( impl->CurrentStatus != AsyncStatus_Closed )
        impl->CurrentStatus = FAILED( status ) ? AsyncStatus_Error : AsyncStatus_Completed;
    impl->result = &result;
    impl->ErrorCode = status;

    if ( impl->Completed && impl->Completed != HANDLER_NOT_SET )
    {
        AsyncStateCompletedHandlerObject *completed = impl->Completed;
        impl->Completed = nullptr; // Prevent concurrent invoke
        pthread_mutex_unlock( &impl->lock );

        completed->lpVtbl->Invoke( completed, impl->outer, impl->CurrentStatus );
        completed->lpVtbl->Release( completed );
    }
    else
        pthread_mutex_unlock( &impl->lock );

    impl->outer->lpVtbl->Release( impl->outer );
}

TR_STATUS TR_API new_async_state_object_override_callback_and_outer( IN UnknownObject *invoker, IN void *param, IN async_operation_callback callback, IN UnknownObject *outer, OUT AsyncStateObject **out )
{
    GError *error = nullptr;

    struct async_state_object *impl;

    TRACE( "invoker %p, param %p, callback %p, outer %p, out %p\n", invoker, param, callback, outer, out );

    if ( !out || !callback ) throw_NullPtrException();

    // Freed in Release();
    if (!(impl = calloc( 1, sizeof(*impl) ))) return T_OUTOFMEMORY;
    impl->AsyncStateObject_iface.lpVtbl = &async_state_interface;
    impl->ref = 1;

    impl->callback = callback;
    impl->Completed = HANDLER_NOT_SET;
    impl->CurrentStatus = AsyncStatus_Started;
    impl->outer = outer;

    impl->pool = g_thread_pool_new( async_state_object_callback, nullptr, (gint)g_get_num_processors(), true, &error );

    if ( !impl->pool )
    {
        ERROR( "Error while creating thread pool for object %p. Error was %s\n", impl, error->message );
        free( impl );
        return error->code;
    }

    impl->invoker = invoker;
    if ( invoker )
        invoker->lpVtbl->AddRef( invoker );

    impl->param = param;

    pthread_mutex_init( &impl->lock, nullptr );

    *out = &impl->AsyncStateObject_iface;

    TRACE( "created AsyncStateObject %p\n", *out );
    return T_SUCCESS;
}