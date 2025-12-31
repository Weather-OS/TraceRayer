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
 *  Module: AsyncInfo.c
 *  Description: Abstraction relating to Async Operations and their information.
 */

#include <Core/Async/AsyncInfo.h>

static struct async_info_object *impl_from_AsyncInfoObject( AsyncInfoObject *iface )
{
    return CONTAINING_RECORD( iface, struct async_info_object, AsyncInfoObject_iface );
}

static TR_STATUS async_info_object_QueryInterface( AsyncInfoObject *iface, const TRUUID uuid, void **out )
{
    const struct async_info_object *impl = impl_from_AsyncInfoObject( iface );

    TRACE( "iface %p, uuid %s, out %p\n", iface, debugstr_uuid( uuid ), out );

    if ( !uuid_compare( uuid, IID_UnknownObject ) || !uuid_compare( uuid, IID_AsyncInfoObject ) )
    {
        iface->lpVtbl->AddRef( iface );
        *out = iface;
        return T_SUCCESS;
    }

    if ( !uuid_compare( uuid, IID_AsyncStateObject ) )
    {
        if ( !impl->AsyncStateObject_impl )
        {
            ERROR( "Subclass AsyncStateObject for AsyncOperationObject %p is not initialized yet!\n", iface );
            return T_NOINIT;
        }
        impl->AsyncStateObject_impl->lpVtbl->AddRef( impl->AsyncStateObject_impl );
        *out = impl->AsyncStateObject_impl;
        return T_SUCCESS;
    }

    ERROR( "uuid %s is not implemented! returning T_NOTIMPL\n", debugstr_uuid( uuid ) );
    return T_NOTIMPL;
}

static TRLong async_info_object_AddRef( AsyncInfoObject *iface )
{
    struct async_info_object *impl = impl_from_AsyncInfoObject( iface );
    const TRLong added = atomic_fetch_add( &impl->ref, 1 ) + 1;
    TRACE( "iface %p increasing ref count to %ld\n", iface, added );
    return added;
}

static TRLong async_info_object_Release( AsyncInfoObject *iface )
{
    struct async_info_object *impl = impl_from_AsyncInfoObject( iface );
    const ATOMIC(TRLong) removed = atomic_fetch_sub( &impl->ref, 1 );
    TRACE( "iface %p decreasing ref count to %ld\n", iface, removed - 1 );
    if ( !(removed - 1) )
    {
        if ( impl->AsyncStateObject_impl )
            impl->AsyncStateObject_impl->lpVtbl->Release( impl->AsyncStateObject_impl );
        free( impl );
    }
    return removed;
}

static TR_STATUS async_info_object_get_CurrentStatus( AsyncInfoObject *iface, AsyncStatus *out )
{
    TR_STATUS status;
    AsyncStateObject *state;

    TRACE( "iface %p, out %p\n", iface, out );

    status = iface->lpVtbl->QueryInterface( iface, IID_AsyncStateObject, (void **)&state );
    if ( FAILED( status ) ) return status;

    status = state->lpVtbl->get_CurrentStatus( state, out );
    state->lpVtbl->Release( state );
    return status;
}

static TR_STATUS async_info_object_get_ErrorCode( AsyncInfoObject *iface, TR_STATUS *out )
{
    TR_STATUS status;
    AsyncStateObject *state;

    TRACE( "iface %p, out %p\n", iface, out );

    status = iface->lpVtbl->QueryInterface( iface, IID_AsyncStateObject, (void **)&state );
    if ( FAILED( status ) ) return status;

    status = state->lpVtbl->get_ErrorCode( state, out );
    state->lpVtbl->Release( state );
    return status;
}

static TR_STATUS async_info_object_Cancel( AsyncInfoObject *iface )
{
    TR_STATUS status;
    AsyncStateObject *state;

    TRACE( "iface %p\n", iface );

    status = iface->lpVtbl->QueryInterface( iface, IID_AsyncStateObject, (void **)&state );
    if ( FAILED( status ) ) return status;

    status = state->lpVtbl->Cancel( state );
    state->lpVtbl->Release( state );
    return status;
}

static TR_STATUS async_info_object_Close( AsyncInfoObject *iface )
{
    TR_STATUS status;
    AsyncStateObject *state;

    TRACE( "iface %p\n", iface );

    status = iface->lpVtbl->QueryInterface( iface, IID_AsyncStateObject, (void **)&state );
    if ( FAILED( status ) ) return status;

    status = state->lpVtbl->Close( state );
    state->lpVtbl->Release( state );
    return status;
}

static AsyncInfoInterface async_info_interface =
{
    /* UnknownObject Methods */
    async_info_object_QueryInterface,
    async_info_object_AddRef,
    async_info_object_Release,
    /* AsyncInfoObject Methods */
    async_info_object_get_CurrentStatus,
    async_info_object_get_ErrorCode,
    async_info_object_Cancel,
    async_info_object_Close
};

TR_STATUS TR_API new_async_info_object_override_callback_and_outer( IN UnknownObject *invoker, IN void *param, IN async_operation_callback callback, IN UnknownObject *outer, OUT AsyncInfoObject **out )
{
    struct async_info_object *impl;

    TRACE( "invoker %p, param %p, callback %p, outer %p, out %p\n", invoker, param, callback, outer, out );

    if ( !out ) throw_NullPtrException();

    // Freed in Release();
    if (!(impl = calloc( 1, sizeof(*impl) ))) return T_OUTOFMEMORY;
    impl->AsyncInfoObject_iface.lpVtbl = &async_info_interface;
    impl->ref = 1;

    *out = &impl->AsyncInfoObject_iface;

    TRACE( "created AsyncInfoObject %p\n", *out );
    return new_async_state_object_override_callback_and_outer( invoker, param, callback, outer, &impl->AsyncStateObject_impl );
}