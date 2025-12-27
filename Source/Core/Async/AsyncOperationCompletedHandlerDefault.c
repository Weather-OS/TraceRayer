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
 *  Module: AsyncOperationCompletedHandlerDefault.c
 *  Description: A default completed handler.
 */


#include <Core/Async/AsyncOperationCompletedHandlerDefault.h>

static struct async_operation_completed_handler_default_object *impl_from_AsyncOperationCompletedHandlerObject( AsyncOperationCompletedHandlerObject *iface )
{
    return CONTAINING_RECORD( iface, struct async_operation_completed_handler_default_object, AsyncOperationCompletedHandlerObject_iface );
}

static TR_STATUS async_operation_completed_handler_default_object_QueryInterface( AsyncOperationCompletedHandlerObject *iface, const TRUUID uuid, void **out )
{
    TRACE( "iface %p, uuid %s, out %p\n", iface, debugstr_uuid( uuid ), out );

    if ( !uuid_compare( uuid, IID_UnknownObject ) || !uuid_compare( uuid, IID_AsyncOperationCompletedHandlerObject ) )
    {
        iface->lpVtbl->AddRef( iface );
        *out = iface;
        return T_SUCCESS;
    }

    ERROR( "uuid %s is not implemented! returning T_NOTIMPL\n", debugstr_uuid( uuid ) );
    return T_NOTIMPL;
}

static TRLong async_operation_completed_handler_default_object_AddRef( AsyncOperationCompletedHandlerObject *iface )
{
    struct async_operation_completed_handler_default_object *impl = impl_from_AsyncOperationCompletedHandlerObject( iface );
    const TRLong added = atomic_fetch_add( &impl->ref, 1 ) + 1;
    TRACE( "iface %p increasing ref count to %ld\n", iface, added );
    return added;
}

static TRLong async_operation_completed_handler_default_object_Release( AsyncOperationCompletedHandlerObject *iface )
{
    struct async_operation_completed_handler_default_object *impl = impl_from_AsyncOperationCompletedHandlerObject( iface );
    const ATOMIC(TRLong) removed = atomic_fetch_sub( &impl->ref, 1 );
    TRACE( "iface %p decreasing ref count to %ld\n", iface, removed - 1 );
    if ( !(removed - 1) )
    {
        free( impl );
    }
    return removed;
}

static TR_STATUS async_operation_completed_handler_default_object_Invoke( AsyncOperationCompletedHandlerObject *iface, AsyncOperationObject *info, AsyncStatus status )
{
    const struct async_operation_completed_handler_default_object *impl = impl_from_AsyncOperationCompletedHandlerObject( iface );
    TRACE( "iface %p, info %p, status %d\n", iface, info, status );
    info->lpVtbl->AddRef( info ); // Ownership is handed to invoker.
    return impl->callback( info, impl->param, status );
}

static AsyncOperationCompletedHandlerInterface async_operation_completed_handler_default_interface =
{
    /* UnknownObject Methods */
    async_operation_completed_handler_default_object_QueryInterface,
    async_operation_completed_handler_default_object_AddRef,
    async_operation_completed_handler_default_object_Release,
    /* AsyncOperationObject Methods */
    async_operation_completed_handler_default_object_Invoke
};

TR_STATUS TR_API async_operation_completed_handler_default_object_override_callback( IN async_operation_completed_handler_callback callback, IN void *param, OUT AsyncOperationCompletedHandlerObject **out )
{
    struct async_operation_completed_handler_default_object *impl;

    TRACE( "param %p, callback %p, out %p\n", param, callback, out );

    if ( !out ) throw_NullPtrException();

    // Freed in Release();
    if (!(impl = calloc( 1, sizeof(*impl) ))) return T_OUTOFMEMORY;
    impl->AsyncOperationCompletedHandlerObject_iface.lpVtbl = &async_operation_completed_handler_default_interface;
    impl->callback = callback;
    impl->param = param;
    impl->ref = 1;

    *out = &impl->AsyncOperationCompletedHandlerObject_iface;
    TRACE( "created AsyncOperationCompletedHandlerObject %p\n", *out );
    return T_SUCCESS;
}
