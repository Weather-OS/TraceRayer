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
 *  Module: VulkanDevice.c
 *  Description: Vulkan device representation.
 */

#include <Core/Vulkan/VulkanDevice.h>

static struct vulkan_device_object *impl_from_VulkanDeviceObject( VulkanDeviceObject *iface )
{
    return CONTAINING_RECORD( iface, struct vulkan_device_object, VulkanDeviceObject_iface );
}

static TR_STATUS vulkan_device_object_QueryInterface( VulkanDeviceObject *iface, const TRUUID uuid, void **out )
{
    TRACE( "iface %p, uuid %s, out %p\n", iface, debugstr_uuid( uuid ), out );

    if ( !uuid_compare( uuid, IID_UnknownObject ) || !uuid_compare( uuid, IID_VulkanDeviceObject ) )
    {
        iface->lpVtbl->AddRef( iface );
        *out = iface;
        return T_SUCCESS;
    }

    ERROR( "uuid %s is not implemented! returning T_NOTIMPL\n", debugstr_uuid( uuid ) );
    return T_NOTIMPL;
}

static TRLong vulkan_device_object_AddRef( VulkanDeviceObject *iface )
{
    struct vulkan_device_object *impl = impl_from_VulkanDeviceObject( iface );
    const TRLong added = atomic_fetch_add( &impl->ref, 1 ) + 1;
    TRACE( "iface %p increasing ref count to %ld\n", iface, added );
    return added;
}

static TRLong vulkan_device_object_Release( VulkanDeviceObject *iface )
{
    struct vulkan_device_object *impl = impl_from_VulkanDeviceObject( iface );
    const ATOMIC(TRLong) removed = atomic_fetch_sub(&impl->ref, 1);
    TRACE( "iface %p decreasing ref count to %ld\n", iface, removed - 1 );
    return removed;
}

static VulkanDeviceInterface vulkan_device_interface =
{
    /* UnknownObject Methods */
    vulkan_device_object_QueryInterface,
    vulkan_device_object_AddRef,
    vulkan_device_object_Release
};

TR_STATUS TR_API new_vulkan_device_object_override_device_name( IN VkPhysicalDevice device, OUT VulkanDeviceObject **out )
{
    struct vulkan_device_object *impl;

    TRACE( "out %p\n", out );

    if ( !out ) throw_NullPtrException();

    // Freed in Release();
    if (!(impl = calloc( 1, sizeof(*impl) ))) return T_OUTOFMEMORY;
    impl->VulkanDeviceObject_iface.lpVtbl = &vulkan_device_interface;
    impl->device = device;
    impl->ref = 1;

    *out = &impl->VulkanDeviceObject_iface;

    TRACE( "created VulkanDeviceObject %p\n", *out );

    return T_SUCCESS;
}