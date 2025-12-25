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
 *  Module: Vulkan.c
 *  Description: Vulkan Instance used to create individual devices.
 */

#include <Core/Vulkan/Vulkan.h>

#include <Statics.h>

static struct vulkan_object *impl_from_VulkanObject( VulkanObject *iface )
{
    return CONTAINING_RECORD( iface, struct vulkan_object, VulkanObject_iface );
}

static TR_STATUS vulkan_object_QueryInterface( VulkanObject *iface, const TRUUID uuid, void **out )
{
    TRACE( "iface %p, uuid %s, out %p\n", iface, debugstr_uuid( uuid ), out );

    if ( !uuid_compare( uuid, IID_UnknownObject ) || !uuid_compare( uuid, IID_VulkanObject ) )
    {
        iface->lpVtbl->AddRef( iface );
        *out = iface;
        return T_SUCCESS;
    }

    ERROR( "uuid %s is not implemented! returning T_NOTIMPL\n", debugstr_uuid( uuid ) );
    return T_NOTIMPL;
}

static TRLong vulkan_object_AddRef( VulkanObject *iface )
{
    struct vulkan_object *impl = impl_from_VulkanObject( iface );
    const TRLong added = atomic_fetch_add( &impl->ref, 1 ) + 1;
    TRACE( "iface %p increasing ref count to %ld\n", iface, added );
    return added;
}

static TRLong vulkan_object_Release( VulkanObject *iface )
{
    struct vulkan_object *impl = impl_from_VulkanObject( iface );
    const ATOMIC(TRLong) removed = atomic_fetch_sub(&impl->ref, 1);
    TRACE( "iface %p decreasing ref count to %ld\n", iface, removed - 1 );
    if ( !(removed - 1) )
    {
        vkDestroyInstance( impl->instance, nullptr );
        free( impl );
    }
    return removed;
}

static VulkanInterface vulkan_interface =
{
    /* UnknownObject Methods */
    vulkan_object_QueryInterface,
    vulkan_object_AddRef,
    vulkan_object_Release
};

TR_STATUS TR_API new_vulkan_object_override_app_name_and_version( IN TRCString appName, IN FormattedVersion version, OUT VulkanObject **out )
{
    VkResult result = VK_SUCCESS;
    VkApplicationInfo appInfo = {0};
    VkInstanceCreateInfo createInfo = {0};
    FormattedVersion engineVer = TRACERAYER_FORMATTED_VERSION;
    struct vulkan_object *impl;

    TRACE( "appName %s, version %p, out %p\n", appName, &version, out );

    if ( !out ) throw_NullPtrException();

    // Freed in Release();
    if (!(impl = calloc( 1, sizeof(*impl) ))) return T_OUTOFMEMORY;
    impl->VulkanObject_iface.lpVtbl = &vulkan_interface;
    impl->ref = 1;

    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = appName;
    appInfo.applicationVersion = VK_MAKE_VERSION( version.Major, version.Minor, version.Patch );
    appInfo.pEngineName = ENGINE_NAME;
    appInfo.engineVersion = VK_MAKE_VERSION( engineVer.Major, engineVer.Minor, engineVer.Patch );
    appInfo.apiVersion = VK_API_VERSION_1_4;

    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    result = vkCreateInstance( &createInfo, nullptr, &impl->instance );
    if ( result != VK_SUCCESS )
    {
        ERROR( "Vk Device creation failed with %d\n", result );
        return T_ERROR;
    }

    *out = &impl->VulkanObject_iface;

    TRACE( "created VulkanObject %p\n", *out );

    return T_SUCCESS;
}