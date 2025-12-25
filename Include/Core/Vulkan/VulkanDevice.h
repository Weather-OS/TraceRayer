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

#ifndef TRACERAYER_VULKANDEVICE_H
#define TRACERAYER_VULKANDEVICE_H

#include <vulkan/vulkan.h>

#include <Object.h>
#include <Types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _VulkanDeviceObject VulkanDeviceObject;

typedef struct _VulkanDeviceInterface
{
    BEGIN_INTERFACE

    IMPLEMENTS_UNKNOWNOBJECT( VulkanDeviceObject )

    /**
     * @Method: TRBool VulkanObject::SupportsExtension( TRCString extension )
     * @Description: Checks if a device supports an extension. Returns true if so.
     * @Status: Returns T_ERROR if a vulkan call fails.
     */
    TR_STATUS (*SupportsExtension)(
        VulkanDeviceObject *iface,
        TRCString           extension,
        TRBool             *out);

    END_INTERFACE
} VulkanDeviceInterface;

interface _VulkanDeviceObject
{
    CONST_VTBL VulkanDeviceInterface *lpVtbl;
};

/**
 * @Object: VulkanDeviceObject
 * @Description: A Vulkan device that represents a physical GPU device.
 */
struct vulkan_device_object
{
    // --- Public Members --- //
    VulkanDeviceObject VulkanDeviceObject_iface;

    // --- Private Members --- //
    VkPhysicalDevice device;
    ATOMIC(TRLong) ref;
};

// b054c6a8-90a0-4383-bc88-4243fc91cbfe
DEFINE_GUID( VulkanDeviceObject, 0xb054c6a8, 0x90a0, 0x4383, 0xbc, 0x88, 0x42, 0x43, 0xfc, 0x91, 0xcb, 0xfe );

// Constructors
// NOTE: This should not be called outside of a VulkanObject.
TR_STATUS TR_API new_vulkan_device_object_override_device( IN VkPhysicalDevice device, OUT VulkanDeviceObject **out );

#ifdef __cplusplus
} // extern "C"

namespace TR
{
    namespace Core::Vulkan
    {
        class VulkanDeviceObject : public UnknownObject<_VulkanDeviceObject>
        {
        public:
            using UnknownObject::UnknownObject;
            static constexpr const TRUUID &classId = IID_VulkanDeviceObject;

            explicit VulkanDeviceObject( VkPhysicalDevice device )
            {
                check_tr_( new_vulkan_device_object_override_device( device, put() ) );
            }

            TRBool SupportsExtension( const std::string& extension ) const
            {
                TRBool out;
                check_tr_( get()->lpVtbl->SupportsExtension( get(), extension.c_str(), &out ) );
                return out;
            }
        };
    }
}
#endif

#endif