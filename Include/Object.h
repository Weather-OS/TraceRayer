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

#ifndef TRACERAYER_OBJECT_H
#define TRACERAYER_OBJECT_H

#include <stdlib.h>

#include <Types.h>
#include <IO/Logging.h>
#include <stdatomic.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BEGIN_INTERFACE
#define END_INTERFACE

#define interface struct
#define CONST_VTBL
#define implements(obj) obj *obj##_impl;
#define delegate(strct)

typedef struct _UnknownObject UnknownObject;

#define IMPLEMENTS_UNKNOWNOBJECT( type_name ) \
    TR_STATUS (*QueryInterface)( IN type_name *iface, IN const TRUUID uuid, OUT void **out ); \
    TRLong    (*AddRef)( IN type_name *iface ); \
    TRLong    (*Release)( IN type_name *iface );

#define CONTAINING_RECORD( address, type, field ) \
    ( (type *)( (TRChar *)(address) - offsetof( type, field ) ) )

typedef struct _UnknownObjectInterface
{
    BEGIN_INTERFACE
    IMPLEMENTS_UNKNOWNOBJECT( UnknownObject )
    END_INTERFACE
} UnknownObjectInterface;

interface _UnknownObject
{
    CONST_VTBL UnknownObjectInterface *lpVtbl;
};

// 00000000-0000-0000-C000-000000000046
DEFINE_GUID( UnknownObject, 0x00000000, 0x0000, 0x0000, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46 );

#define DEFINE_SHALLOW_UNKNOWNOBJECT( type_name, impl ) \
    TR_STATUS impl##_QueryInterface( type_name *iface, const TRUUID uuid, void **out )              \
    {                                                                                               \
        struct impl *root = impl_from_##type_name( iface );                                         \
        TRACE( "iface %p, uuid %s, out %p\n", iface, debugstr_uuid( uuid ), out );                  \
        if ( !out ) throw_NullPtrException();                                                       \
        if ( !uuid_compare( uuid, IID_UnknownObject ) || !uuid_compare( uuid, IID_##type_name ) )   \
        {                                                                                           \
            iface->lpVtbl->AddRef( iface );                                                         \
            *out = iface;                                                                           \
            return T_SUCCESS;                                                                       \
        }                                                                                           \
        ERROR( "uuid %s is not implemented! returning T_NOTIMPL\n", debugstr_uuid( uuid ) );        \
        return T_NOTIMPL;                                                                           \
    }                                                                                               \
    \
    TRLong impl##_AddRef( type_name *iface )                                                        \
    {                                                                                               \
        struct impl *root = impl_from_##type_name( iface );                                         \
        const TRLong added = atomic_fetch_add( &root->ref, 1 ) + 1;                                 \
        TRACE( "iface %p increasing ref count to %ld\n", iface, added );                            \
        return added;                                                                               \
    }                                                                                               \
    \
    TRLong impl##_Release( type_name *iface )                                                       \
    {                                                                                               \
        struct impl *root = impl_from_##type_name( iface );                                         \
        const TRLong removed = atomic_fetch_sub( &root->ref, 1 );                                   \
        TRACE( "iface %p decreasing ref count to %ld\n", iface, removed - 1 );                      \
        if ( !(removed - 1) )                                                                       \
            free( root );                                                                           \
        return removed;                                                                             \
    }


#ifdef __cplusplus

namespace TR
{
    class UnknownObject
    {
    public:
        _UnknownObject *unknwn = nullptr;

        ~UnknownObject()
        {
            if (unknwn) unknwn->lpVtbl->Release( unknwn );
        }

        void QueryInterface( IN const TRUUID uuid, OUT void **out )
        {
            TR_STATUS ts;
            ts = unknwn->lpVtbl->QueryInterface( unknwn, uuid, out );
            if ( FAILED( ts ) ) throw TRException( ts );
        }

        TRLong AddRef()
        {
            return unknwn->lpVtbl->AddRef( unknwn );
        }

        TRLong Release()
        {
            return unknwn->lpVtbl->Release( unknwn );
        }
    };
}

} // extern "C"
#endif

#endif
