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

#include <UI/Representation.h>

#ifdef PLATFORM_SUPPORTS_X11
#include <xcb/xcb.h>
#include <vulkan/vulkan_xcb.h>
#endif
#ifdef PLATFORM_SUPPORTS_WAYLAND
#include <vulkan/vulkan_wayland.h>
#endif
#ifdef PLATFORM_SUPPORTS_WIN32
#include <vulkan/vulkan_win32.h>
#endif
#ifdef PLATFORM_SUPPORTS_MACOS
#include <vulkan/vulkan_macos.h>
#endif


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

static TR_STATUS vulkan_object_CreateDeviceOverloadDeviceName( VulkanObject *iface, TRCString deviceName, VulkanDeviceObject **out )
{
    TR_STATUS status;
    VkResult result = VK_SUCCESS;
    VkPhysicalDevice *vkDevices = nullptr;
    VkPhysicalDevice vkDevice = VK_NULL_HANDLE;
    TRUInt deviceCount = 0;
    TRUInt iterator;

    struct vulkan_object *impl = impl_from_VulkanObject( iface );

    TRACE( "iface %p, deviceName %s, out %p\n", iface, deviceName, out );

    result = vkEnumeratePhysicalDevices( impl->instance, &deviceCount, nullptr );
    if ( result != VK_SUCCESS || !deviceCount )
    {
        ERROR( "Vulkan device enumeration failed with %d\n", result );
        return T_ERROR;
    }

    if (!(vkDevices = malloc( sizeof(VkPhysicalDevice) * deviceCount ))) return T_OUTOFMEMORY;

    result = vkEnumeratePhysicalDevices( impl->instance, &deviceCount, vkDevices );
    if ( result != VK_SUCCESS )
    {
        ERROR( "Vulkan device enumeration failed with %d\n", result );
        free( vkDevices );
        return T_ERROR;
    }

    for ( iterator = 0; iterator < deviceCount; iterator++ )
    {
        VkPhysicalDeviceProperties deviceProperties;

        vkGetPhysicalDeviceProperties( vkDevices[iterator], &deviceProperties );
        if ( !strcmp( deviceName, deviceProperties.deviceName ) )
        {
            vkDevice = vkDevices[iterator];
            break;
        }
        WARN( "device %s did not match\n", deviceProperties.deviceName );
    }

    if ( vkDevice == VK_NULL_HANDLE )
    {
        ERROR( "Could not find a vulkan device with name %s\n", deviceName );
        free( vkDevices );
        return T_NOINIT;
    }

    status = new_vulkan_device_object_override_device( vkDevice, out );

    free( vkDevices );

    return status;
}

static VulkanInterface vulkan_interface =
{
    /* UnknownObject Methods */
    vulkan_object_QueryInterface,
    vulkan_object_AddRef,
    vulkan_object_Release,
    /* VulkanObject Methods */
    vulkan_object_CreateDeviceOverloadDeviceName
};

TR_STATUS TR_API new_vulkan_object_override_app_name_and_version_and_platform( IN TRCString appName, IN FormattedVersion version, IN Platform platform, OUT VulkanObject **out )
{
    VkResult result = VK_SUCCESS;
    VkApplicationInfo appInfo = {0};
    VkInstanceCreateInfo createInfo = {0};
    FormattedVersion engineVer = TRACERAYER_FORMATTED_VERSION;
    TRCString extensions[2] = { VK_KHR_SURFACE_EXTENSION_NAME, nullptr };
    struct vulkan_object *impl;

    TRACE( "appName %s, version %p, out %p\n", appName, &version, out );

    if ( !out ) throw_NullPtrException();

    // Freed in Release();
    if (!(impl = calloc( 1, sizeof(*impl) ))) return T_OUTOFMEMORY;
    impl->VulkanObject_iface.lpVtbl = &vulkan_interface;
    impl->platform = platform;
    impl->ref = 1;

    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = appName;
    appInfo.applicationVersion = VK_MAKE_VERSION( version.Major, version.Minor, version.Patch );
    appInfo.pEngineName = ENGINE_NAME;
    appInfo.engineVersion = VK_MAKE_VERSION( engineVer.Major, engineVer.Minor, engineVer.Patch );
    appInfo.apiVersion = VK_API_VERSION_1_4;

    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    switch ( platform )
    {
        case Platform_Wayland:
        {
#ifdef PLATFORM_SUPPORTS_WAYLAND
            INFO( "Vulkan: Creating an Instance for a Wayland surface...\n" );
            extensions[1] = VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME;
#endif
            break;
        }
        case Platform_X11:
        {
#ifdef PLATFORM_SUPPORTS_X11
            INFO( "Vulkan: Creating an Instance for an X11 surface...\n" );
            extensions[1] = VK_KHR_XCB_SURFACE_EXTENSION_NAME;
#endif
            break;
        }
        case Platform_win32:
        {
#ifdef PLATFORM_SUPPORTS_WIN32
            INFO( "Vulkan: Creating an Instance for a win32 surface...\n" );
            extensions[1] = VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
#endif
            break;
        }
        case Platform_macOS:
        {
#ifdef PLATFORM_SUPPORTS_MACOS
            INFO( "Vulkan: Creating an Instance for a macOS surface...\n" );
            extensions[1] = VK_MVK_MACOS_SURFACE_EXTENSION_NAME;
#endif
            break;
        }
    }

    if ( !extensions[1] )
    {
        ERROR( "Vulkan: Unsupported platform!\n" );
        free( impl );
        return T_NOTIMPL;
    }

    createInfo.enabledExtensionCount = 2;
    createInfo.ppEnabledExtensionNames = extensions;

    result = vkCreateInstance( &createInfo, nullptr, &impl->instance );
    if ( result != VK_SUCCESS )
    {
        ERROR( "Vulkan Instance creation failed with %d\n", result );
        free( impl );
        return T_ERROR;
    }

    *out = &impl->VulkanObject_iface;

    TRACE( "created VulkanObject %p\n", *out );

    return T_SUCCESS;
}