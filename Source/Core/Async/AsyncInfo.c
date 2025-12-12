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
    const TRLong added = atomic_load( &impl->ref ) + 1;
    TRACE( "iface %p increasing ref count to %ld\n", iface, added );
    atomic_fetch_add( &impl->ref, 1 );
    return added;
}

static TRLong async_info_object_Release( AsyncInfoObject *iface )
{
    struct async_info_object *impl = impl_from_AsyncInfoObject( iface );
    const TRLong removed = atomic_load( &impl->ref ) - 1;
    TRACE( "iface %p decreasing ref count to %ld\n", iface, removed );
    atomic_fetch_sub( &impl->ref, 1 );
    if ( !removed )
    {
        if ( impl->AsyncStateObject_impl )
            impl->AsyncStateObject_impl->lpVtbl->Release( impl->AsyncStateObject_impl );
        free( impl );
    }
    return removed;
}

static TR_STATUS async_info_object_get_Status( AsyncInfoObject *iface, AsyncStatus *out )
{
    TR_STATUS status;
    AsyncStateObject *state;

    TRACE( "iface %p, out %p\n", iface, out );

    status = iface->lpVtbl->QueryInterface( iface, IID_AsyncStateObject, (void **)&state );
    if ( FAILED( status ) ) return status;

    status = state->lpVtbl->get_Status( state, out );
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
    async_info_object_get_Status,
    async_info_object_get_ErrorCode,
    async_info_object_Cancel,
    async_info_object_Close
};

TR_STATUS new_async_info_object_override_callback_and_outer( IN UnknownObject *invoker, IN void *param, IN async_operation_callback callback, IN UnknownObject *outer, OUT AsyncInfoObject **out )
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