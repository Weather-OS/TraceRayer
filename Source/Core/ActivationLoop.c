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
#include <IO/Arguments.h>
#include <IO/FetchResources.h>
#include <UI/GTK/GTKWindowHandle.h>
#include <UI/GTK/GTKPicture.h>

#include <Core/ActivationLoop.h>

static void CloseEvent( IN UnknownObject *invoker, IN void *user_data )
{
    TR_STATUS status;
    TRULong receivedToken = *(TRULong *)user_data;
    GTKWindowObject *gtk_window_object = (GTKWindowObject *)invoker;

    free( user_data );

    gtk_window_object->lpVtbl->eventremove_OnDelete( gtk_window_object, receivedToken );
    gtk_window_object->lpVtbl->Release( gtk_window_object ); // <-- Release window on close
}

void ActivationLoop( IN UnknownObject *invoker, IN void *user_data )
{
    TRULong token;
    TRPath *resourcesPath;
    TRPath *launcherImage;
    TRULong *passedToken = malloc( sizeof(TRULong) );
    TR_STATUS status;

    GTKObject *gtk_object = (GTKObject *)invoker;
    GTKPictureObject *picture_object;
    GTKWidgetObject *childWidget;
    GTKWidgetObject *pictureChildWidget;
    GTKWindowObject *window;
    GTKWindowHandleObject *window_handle;
    GdkRectangle rect = { 0, 0, 600, 450 };

    TRACE("Reached Here! GPUName is %s\n", GlobalArgumentsDefault.GPUName);

    status = FetchResources( &resourcesPath );
    if ( FAILED( status ) )
    {
        ERROR( "FetchResources failed with %ld\n", status );
        return;
    } else
        INFO( "Using resource path %s\n", resourcesPath->Location );

    status = FetchSubpath( resourcesPath, "launch.png", false, T_READ, &launcherImage );
    if ( FAILED( status ) )
    {
        ERROR( "Failed to locate resource %s from path %s.\n", "launch.png", resourcesPath->Location );
        return;
    }

    status = new_gtk_picture_object_override_path( launcherImage, &picture_object );
    if ( FAILED( status ) ) return;

    status = gtk_object->lpVtbl->CreateWindow( gtk_object, &window );
    if ( FAILED( status ) ) return;

    status = window->lpVtbl->set_WindowRect( window, rect );
    if ( FAILED( status ) ) return;

    status = window->lpVtbl->SetWindowTitle( window, APPNAME );
    if ( FAILED( status ) ) return;

    window->lpVtbl->Show( window );

    status = new_gtk_window_handle_object( &window_handle );
    if ( FAILED( status ) ) return;

    status = picture_object->lpVtbl->QueryInterface( picture_object, IID_GTKWidgetObject, (void **)&pictureChildWidget );
    if ( FAILED( status ) ) return;

    picture_object->lpVtbl->Release( picture_object );

    status = window_handle->lpVtbl->set_ChildWidget( window_handle, pictureChildWidget );
    if ( FAILED( status ) ) return;

    status = window_handle->lpVtbl->QueryInterface( window_handle, IID_GTKWidgetObject, (void **)&childWidget );
    if ( FAILED( status ) ) return;

    window_handle->lpVtbl->Release( window_handle ); // TODO: Do not release this here. Do something with the area.

    status = window->lpVtbl->set_ChildWidget( window, childWidget );
    if ( FAILED( status ) ) return;

    status = window->lpVtbl->eventadd_OnDelete( window, CloseEvent, passedToken, &token );
    if ( FAILED( status ) ) return;

    *passedToken = token;
}
