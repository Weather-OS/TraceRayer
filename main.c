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
 *  Project: Ray Tracing using GTK and Vulkan.
 *  Project Description: For my computer science class.
 */

#include <stdio.h>

#include <InitGUID.h>
#include <Statics.h>

#include <Core/WindowLoop.h>

#include <IO/Arguments.h>
#include <IO/Logging.h>

#include <UI/GTK.h>

TR_STATUS async_test( UnknownObject *invoker, void *param, PropVariant *out )
{
    const auto obj = (GTKObject *)invoker;
    TR_STATUS status;
    TRACE("Reached here!\n");
    return T_SUCCESS;
}

int main( const int argc, char **argv )
{
    TR_STATUS status;
    status = ParseCommandLineArguments( argc, argv );
    if ( FAILED( status ) ) return status;
    status = InitializeLogging();
    if ( FAILED( status ) ) return status;

    GTKObject *obj;
    GTKWindowObject *window;
    UnknownObject *unknown;
    AsyncOperationObject *operation;
    GdkRectangle rect = { 0, 0, 500, 900 };

    status = new_gtk_object( GTK_APPNAME, &obj );
    if ( FAILED( status ) ) return status;

    status = obj->lpVtbl->CreateWindow( obj, WindowCallbackProc, &window );
    if ( FAILED( status ) ) return status;

    status = obj->lpVtbl->QueryInterface( obj, IID_UnknownObject, (void **)&unknown );
    if ( FAILED( status ) ) return status;

    status = window->lpVtbl->set_WindowRect( window, rect );
    if ( FAILED( status ) ) return status;

    status = window->lpVtbl->setWindowTitle( window, APPNAME );
    if ( FAILED( status ) ) return status;

    window->lpVtbl->Show( window ); // <-- This must fail here.

    status = new_async_operation_object_override_callback( (UnknownObject *)obj, nullptr, async_test, &operation );
    if ( FAILED( status ) ) return status;

    status = obj->lpVtbl->RunApplication( obj );
    if ( FAILED( status ) ) return status;

    window->lpVtbl->Release( window );

    unknown->lpVtbl->Release( unknown );

    obj->lpVtbl->Release( obj );

    operation->lpVtbl->Release( operation );

    return status;
}
