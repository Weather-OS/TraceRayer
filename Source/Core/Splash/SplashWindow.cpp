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

#include <Core/Splash/SplashWindow.hpp>

#include <UI/GTK/GTKPicture.h>
#include <UI/GTK/GTKSpinner.h>
#include <UI/GTK/GTKBox.h>
#include <UI/GTK/GTKOverlay.h>
#include <UI/GTK/GTKWindowHandle.h>
#include <IO/FetchResources.h>
#include <Statics.h>

using namespace TR;

void
SplashWindow(
    const UI::GTKObject &inGtk
) {
    TRPath *splashPicturePath;

    UI::GTKWindowObject window = inGtk.CreateWindow();
    UI::GTKPictureObject picture;
    UI::GTKWindowHandleObject windowHandle{};
    UI::GTKSpinnerObject spinner{};
    UI::GTKOverlayObject overlay{};
    UI::GTKBoxObject box( GTK_ORIENTATION_HORIZONTAL, 5 );

    FetchResource( "launch.png", &splashPicturePath );

    picture = UI::GTKPictureObject( splashPicturePath );
    free( splashPicturePath );

    spinner.Spinning( true );
    spinner.QueryInterface<UI::GTKWidgetObject>().Alignment( { .Horizontal = GTK_ALIGN_START, .Vertical = GTK_ALIGN_END } );
    spinner.QueryInterface<UI::GTKWidgetObject>().SetSizeRequest( 48, 48 );

    overlay.ChildWidget( picture );
    overlay.AddWidget( spinner );

    box.AppendWidget( overlay );

    window.ChildWidget( windowHandle );
    window.SetResizable( true );
    window.SetWindowTitle( APPNAME );
    window.WindowRect( picture.GetPictureRect() );

    windowHandle.ChildWidget( box );

    window.Show();
}
