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

#include <IO/Arguments.h>
#include <Statics.h>

#include <UI/GTK/GTKDrawingArea.h>

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
    TRULong *passedToken = malloc( sizeof(TRULong) );
    TR_STATUS status;
    GTKObject *gtk_object = (GTKObject *)invoker;
    GTKWidgetObject *childWidget;
    GTKWindowObject *window;
    GTKDrawingAreaObject *drawing_area;
    GdkRectangle rect = { 0, 0, 500, 900 };

    TRACE("Reached Here! GPUName is %s\n", GlobalArgumentsDefault.GPUName);

    status = gtk_object->lpVtbl->CreateWindow( gtk_object, &window );
    if ( FAILED( status ) ) return;

    status = window->lpVtbl->set_WindowRect( window, rect );
    if ( FAILED( status ) ) return;

    status = window->lpVtbl->setWindowTitle( window, APPNAME );
    if ( FAILED( status ) ) return;

    window->lpVtbl->Show( window );

    status = new_gtk_drawing_area_object( &drawing_area );
    if ( FAILED( status ) ) return;

    status = drawing_area->lpVtbl->QueryInterface( drawing_area, IID_GTKWidgetObject, (void **)&childWidget );
    if ( FAILED( status ) ) return;

    drawing_area->lpVtbl->Release( drawing_area ); // TODO: Do not release this here. Do something with the area.

    status = window->lpVtbl->set_ChildWidget( window, childWidget );
    if ( FAILED( status ) ) return;

    childWidget->lpVtbl->Release( childWidget );

    status = window->lpVtbl->eventadd_OnDelete( window, CloseEvent, passedToken, &token );
    if ( FAILED( status ) ) return;

    *passedToken = token;
}
