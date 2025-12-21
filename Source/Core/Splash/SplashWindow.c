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

#include <Statics.h>

#include <Core/Splash/SplashWindow.h>
#include <UI/GTK/GTKPicture.h>
#include <UI/GTK/GTKWindowHandle.h>
#include <IO/FetchResources.h>

static void
CloseEvent(
    IN UnknownObject *invoker,
    IN void *user_data
) {
    TR_STATUS status;
    TRULong receivedToken = *(TRULong *)user_data;
    GTKWindowObject *gtk_window_object = (GTKWindowObject *)invoker;

    free( user_data );

    gtk_window_object->lpVtbl->eventremove_OnDelete( gtk_window_object, receivedToken );
    gtk_window_object->lpVtbl->Release( gtk_window_object ); // <-- Release window on close
}

TR_STATUS
SplashWindow(
    IN GTKObject *inGtk
) {
    TR_STATUS status = T_SUCCESS;

    TRPath *splashPicturePath;
    TRULong *receivedToken;
    GdkRectangle splashPictureRect = { .width = 600, .height = 450 };
    GTKWidgetObject *currentWidget = nullptr;
    GTKWindowObject *splashWindow = nullptr;
    GTKPictureObject *splashPicture = nullptr;
    GTKWindowHandleObject *splashWindowHandle = nullptr;

    TRACE( "inGtk %p\n", inGtk );

    CHECK_TR( FetchResource( "launch.png", &splashPicturePath ) )

    receivedToken = (TRULong *)malloc(1 * sizeof( TRULong ) );

    /* Object Initialization */
    TRACE("resource path is %s\n", splashPicturePath->Location);
    CHECK_TR( new_gtk_picture_object_override_path( splashPicturePath, &splashPicture ) );
    CHECK_TR( new_gtk_window_handle_object( &splashWindowHandle ) );

    //CHECK_TR( splashPicture->lpVtbl->GetPictureRect( splashPicture, &splashPictureRect ) );

    // Splash Window
    CHECK_TR( inGtk->lpVtbl->CreateWindow( inGtk, &splashWindow ) );
    CHECK_TR( splashWindow->lpVtbl->SetResizable( splashWindow, false ) );
    CHECK_TR( splashWindow->lpVtbl->set_WindowRect( splashWindow, splashPictureRect ) );
    CHECK_TR( splashWindow->lpVtbl->SetWindowTitle( splashWindow, APPNAME ) );
    CHECK_TR( splashWindow->lpVtbl->eventadd_OnDelete( splashWindow, CloseEvent, (void *)receivedToken, receivedToken) );

    // Splash Window Handle
    CHECK_TR( splashPicture->lpVtbl->QueryInterface( splashPicture, IID_GTKWidgetObject, (void **)&currentWidget ) );
    CHECK_TR( splashWindowHandle->lpVtbl->set_ChildWidget( splashWindowHandle, currentWidget ) );
    currentWidget->lpVtbl->Release( currentWidget ); // <-- Hand ownership to splashWindowHandle

    CHECK_TR( splashWindowHandle->lpVtbl->QueryInterface( splashWindowHandle, IID_GTKWidgetObject, (void **)&currentWidget ) );
    CHECK_TR( splashWindow->lpVtbl->set_ChildWidget( splashWindow, currentWidget ) );
    currentWidget->lpVtbl->Release( currentWidget ); // <-- Hand ownership to splashWindow

    CHECK_TR( splashWindow->lpVtbl->Show( splashWindow ) );

_CLEANUP:
    if ( FAILED( status ) )
    {
        if ( splashWindow )
            splashWindow->lpVtbl->Release( splashWindow );
    }
    if ( splashPicture )
        splashPicture->lpVtbl->Release( splashPicture );
    if ( splashWindowHandle )
        splashWindowHandle->lpVtbl->Release( splashWindowHandle );

    return status;
}
