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

#ifndef TRACERAYER_VULKAN_H
#define TRACERAYER_VULKAN_H

#include <vulkan/vulkan.h>

#include <Object.h>
#include <Types.h>

#include <Core/Vulkan/VulkanDevice.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _VulkanObject VulkanObject;

typedef struct _VulkanInterface
{
    BEGIN_INTERFACE

    IMPLEMENTS_UNKNOWNOBJECT( VulkanObject )

    /**
     * @Method: VulkanDeviceObject* VulkanObject::CreateDevice( TRCString deviceName )
     * @Description: Creates a Vulkan device from the given Name
     * @Status: Returns T_SUCCESS if the device was found, otherwise, T_NOINIT.
     *          Returns T_ERROR if a vulkan call fails.
     */
    TR_STATUS (*CreateDeviceOverloadDeviceName)(
        VulkanObject        *This,
        TRCString            deviceName,
        VulkanDeviceObject  **out );

    /**
     * @Method: VulkanDeviceObject* VulkanObject::CreateDevice( TRUInt index )
     * @Description: Creates a Vulkan device with the given device index.
     * @Status: Returns T_SUCCESS if the device was found, otherwise, T_NOINIT.
     *          Returns T_ERROR if a vulkan call fails.
     */
    TR_STATUS (*CreateDeviceOverloadIndex)(
        VulkanObject        *This,
        TRUInt               index,
        VulkanDeviceObject  **out );

    END_INTERFACE
} VulkanInterface;

interface _VulkanObject
{
    CONST_VTBL VulkanInterface *lpVtbl;
};

/**
 * @Object: VulkanObject
 * @Description: Root Vulkan Object, represents an Instance that can be used to create Vulkan devices.
 */
struct vulkan_object
{
    // --- Public Members --- //
    VulkanObject VulkanObject_iface;

    // --- Private Members --- //
    VkInstance instance;
    Platform platform;
    ATOMIC(TRLong) ref;
};

// 76e09a11-2a01-4127-8b64-586f25c181ab
DEFINE_GUID( VulkanObject, 0x76e09a11, 0x2a01, 0x4127, 0x8b, 0x64, 0x58, 0x6f, 0x25, 0xc1, 0x81, 0xab );

// Constructors
TR_STATUS TR_API new_vulkan_object_override_app_name_and_version_and_platform( IN TRCString appName, IN FormattedVersion version, IN Platform platform, OUT VulkanObject **out );

#ifdef __cplusplus
} // extern "C"

namespace TR
{
    namespace Core::Vulkan
    {
        class VulkanObject : public UnknownObject<_VulkanObject>
        {
        public:
            using UnknownObject::UnknownObject;
            static constexpr const TRUUID &classId = IID_VulkanObject;

            explicit VulkanObject( const std::string& appName, FormattedVersion version, Platform platform )
            {
                check_tr_( new_vulkan_object_override_app_name_and_version_and_platform( appName.c_str(), version, platform, put() ) );
            }

            [[nodiscard]]
            VulkanDeviceObject CreateDevice( const std::string& deviceName ) const
            {
                _VulkanDeviceObject *vkdevobj;
                check_tr_( get()->lpVtbl->CreateDeviceOverloadDeviceName( get(), deviceName.c_str(), &vkdevobj ) );
                return VulkanDeviceObject( vkdevobj );
            }

            VulkanDeviceObject CreateDevice( TRUInt index ) const
            {
                _VulkanDeviceObject *vkdevobj;
                check_tr_( get()->lpVtbl->CreateDeviceOverloadIndex( get(), index, &vkdevobj ) );
                return VulkanDeviceObject( vkdevobj );
            }
        };
    }
}
#endif

#endif