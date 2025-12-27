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
 *  Module: AsyncOperation.c
 *  Description: Threading & Asynchronous operations throughout the project.
 */

#include <Core/Async/AsyncOperation.h>

static struct async_operation_object *impl_from_AsyncOperationObject( AsyncOperationObject *iface )
{
    return CONTAINING_RECORD( iface, struct async_operation_object, AsyncOperationObject_iface );
}

static TR_STATUS async_operation_object_QueryInterface( AsyncOperationObject *iface, const TRUUID uuid, void **out )
{
    const struct async_operation_object *impl = impl_from_AsyncOperationObject( iface );

    TRACE( "iface %p, uuid %s, out %p\n", iface, debugstr_uuid( uuid ), out );

    if ( !uuid_compare( uuid, IID_UnknownObject ) || !uuid_compare( uuid, IID_AsyncOperationObject ) )
    {
        iface->lpVtbl->AddRef( iface );
        *out = iface;
        return T_SUCCESS;
    }

    if ( !uuid_compare( uuid, IID_AsyncInfoObject ) )
    {
        if ( !impl->AsyncInfoObject_impl )
        {
            ERROR( "Subclass AsyncInfoObject for AsyncOperationObject %p is not initialized yet!\n", iface );
            return T_NOINIT;
        }
        impl->AsyncInfoObject_impl->lpVtbl->AddRef( impl->AsyncInfoObject_impl );
        *out = impl->AsyncInfoObject_impl;
        return T_SUCCESS;
    }

    // Nested subclass AsyncStateObject:
    if ( !uuid_compare( uuid, IID_AsyncStateObject ) )
    {
        if ( !impl->AsyncInfoObject_impl )
        {
            ERROR( "Subclass AsyncStateObject for AsyncOperationObject %p is not initialized yet!\n", iface );
            return T_NOINIT;
        }
        impl->AsyncInfoObject_impl->lpVtbl->QueryInterface( impl->AsyncInfoObject_impl, IID_AsyncStateObject, out );
        return T_SUCCESS;
    }

    ERROR( "uuid %s is not implemented! returning T_NOTIMPL\n", debugstr_uuid( uuid ) );
    return T_NOTIMPL;
}

static TRLong async_operation_object_AddRef( AsyncOperationObject *iface )
{
    struct async_operation_object *impl = impl_from_AsyncOperationObject( iface );
    const TRLong added = atomic_fetch_add( &impl->ref, 1 ) + 1;
    TRACE( "iface %p increasing ref count to %ld\n", iface, added );
    return added;
}

static TRLong async_operation_object_Release( AsyncOperationObject *iface )
{
    struct async_operation_object *impl = impl_from_AsyncOperationObject( iface );
    const ATOMIC(TRLong) removed = atomic_fetch_sub( &impl->ref, 1 );
    TRACE( "iface %p decreasing ref count to %ld\n", iface, removed - 1 );
    if ( !(removed - 1) )
    {
        if ( impl->AsyncInfoObject_impl )
            impl->AsyncInfoObject_impl->lpVtbl->Release( impl->AsyncInfoObject_impl );
        free( impl );
    }
    return removed;
}

static TR_STATUS async_operation_object_get_Completed( AsyncOperationObject *iface, AsyncOperationCompletedHandlerObject **out )
{
    TR_STATUS status;
    AsyncStateObject *state;

    TRACE( "iface %p, out %p\n", iface, out );

    status = iface->lpVtbl->QueryInterface( iface, IID_AsyncStateObject, (void **)&state );
    if ( FAILED( status ) ) return status;

    status = state->lpVtbl->get_Completed( state, (AsyncStateCompletedHandlerObject **)out );
    state->lpVtbl->Release( state );
    return status;
}

static TR_STATUS async_operation_object_set_Completed( AsyncOperationObject *iface, AsyncOperationCompletedHandlerObject *completed )
{
    TR_STATUS status;
    AsyncStateObject *state;

    TRACE( "iface %p, completed %p\n", iface, completed );

    status = iface->lpVtbl->QueryInterface( iface, IID_AsyncStateObject, (void **)&state );
    if ( FAILED( status ) ) return status;

    status = state->lpVtbl->set_Completed( state, (AsyncStateCompletedHandlerObject *)completed );
    state->lpVtbl->Release( state );
    return status;
}

static TR_STATUS async_operation_object_GetResults( AsyncOperationObject *iface, PropVariant **out )
{
    TR_STATUS status;
    AsyncStateObject *state;

    TRACE( "iface %p, out %p\n", iface, out );

    status = iface->lpVtbl->QueryInterface( iface, IID_AsyncStateObject, (void **)&state );
    if ( FAILED( status ) ) return status;

    status = state->lpVtbl->Result( state, out );
    state->lpVtbl->Release( state );
    return status;
}

static AsyncOperationInterface async_operation_interface =
{
    /* UnknownObject Methods */
    async_operation_object_QueryInterface,
    async_operation_object_AddRef,
    async_operation_object_Release,
    /* AsyncOperationObject Methods */
    async_operation_object_get_Completed,
    async_operation_object_set_Completed,
    async_operation_object_GetResults
};

TR_STATUS TR_API new_async_operation_object_override_callback( IN UnknownObject *invoker, IN void *param, IN async_operation_callback callback, OUT AsyncOperationObject **out )
{
    TR_STATUS status;
    AsyncStateObject *state;

    struct async_operation_object *impl;

    TRACE( "invoker %p, param %p, callback %p, out %p\n", invoker, param, callback, out );

    if ( !out ) throw_NullPtrException();

    // Freed in Release();
    if (!(impl = calloc( 1, sizeof(*impl) ))) return T_OUTOFMEMORY;
    impl->AsyncOperationObject_iface.lpVtbl = &async_operation_interface;
    impl->ref = 1;

    status = new_async_info_object_override_callback_and_outer( invoker, param, callback, (UnknownObject *)&impl->AsyncOperationObject_iface, &impl->AsyncInfoObject_impl );
    if ( FAILED( status ) )
    {
        free( impl );
        return status;
    }

    status = impl->AsyncInfoObject_impl->lpVtbl->QueryInterface( impl->AsyncInfoObject_impl, IID_AsyncStateObject, (void **)&state );
    if ( FAILED( status ) )
    {
        impl->AsyncInfoObject_impl->lpVtbl->Release( impl->AsyncInfoObject_impl );
        free( impl );
        return status;
    }

    status = state->lpVtbl->Start( state );
    if ( FAILED( status ) )
    {
        impl->AsyncInfoObject_impl->lpVtbl->Release( impl->AsyncInfoObject_impl );
        state->lpVtbl->Release( state );
        free( impl );
        return status;
    }

    state->lpVtbl->Release( state );

    *out = &impl->AsyncOperationObject_iface;
    TRACE( "created AsyncOperationObject %p\n", *out );
    return status;
}