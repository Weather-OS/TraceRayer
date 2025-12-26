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

#include <Application/Splash/SplashWindow.hpp>
#include <memory>

#include <Core/Vulkan/Vulkan.h>
#include <Core/Async/AsyncOperation.h>

#include <UI/UI.h>
#include <Statics.h>

#include <IO/Arguments.h>
#include <IO/FetchResources.h>

using namespace TR;

void
CustomAsync( UI::GTKWindowObject *window, void *param, PropVariant *result )
{
    sleep(2);
    window->SetResizable( true );
    throw TRException( T_OUTOFMEMORY );
}

void
SplashWindow(
    const UI::GTKObject &inGtk
) {
    TRPath *splashPicturePath;

    Core::Vulkan::VulkanObject vkInst;
    Core::Vulkan::VulkanDeviceObject device;
    UI::GTKPictureObject picture;
    UI::GTKWindowHandleObject windowHandle{};
    UI::GTKSpinnerObject spinner{};
    UI::GTKOverlayObject overlay{};
    UI::GTKWindowObject window = inGtk.CreateWindow();
    UI::GTKBoxObject box( GTK_ORIENTATION_HORIZONTAL, 5 );
    UI::GTKLabelObject label( "Loading..." );

    FetchResource( "launch.png", &splashPicturePath );

    picture = UI::GTKPictureObject( splashPicturePath );
    free( splashPicturePath );

    Core::Async::AsyncOperationObject asyncOperation( &window, nullptr, CustomAsync );

    vkInst = Core::Vulkan::VulkanObject( "Test", {1, 0, 0}, inGtk.CurrentPlatform() );

    device = vkInst.CreateDevice( GlobalArgumentsDefault.GPUName );

    if ( !device.SupportsExtension( "VK_KHR_ray_tracing_pipeline" ) )
        WARN( "This device does not support ray tracing!\n" );

    spinner.Spinning( true );
    spinner.QueryInterface<UI::GTKWidgetObject>().Alignment( { .Horizontal = GTK_ALIGN_START, .Vertical = GTK_ALIGN_END } );
    spinner.QueryInterface<UI::GTKWidgetObject>().SetSizeRequest( 48, 48 );

    label.QueryInterface<UI::GTKWidgetObject>().Alignment( { .Horizontal = GTK_ALIGN_CENTER, .Vertical = GTK_ALIGN_END } );
    label.QueryInterface<UI::GTKWidgetObject>().SetSizeRequest( 48, 48 );

    overlay.ChildWidget( picture );
    overlay.AddWidget( spinner );
    overlay.AddWidget( label );

    box.AppendWidget( overlay );

    window.ChildWidget( windowHandle );
    window.SetResizable( false );
    window.SetWindowTitle( APPNAME );
    window.WindowRect( picture.GetPictureRect() );

    windowHandle.ChildWidget( box );

    window.Show();
}
