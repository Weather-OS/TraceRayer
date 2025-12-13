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

#include <Core/ActivationLoop.h>

#include <IO/Arguments.h>
#include <IO/Logging.h>

#include <UI/GTK/GTK.h>

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

    TRULong activationToken;
    GTKObject *obj;
    AsyncOperationObject *operation;

    status = new_gtk_object( GTK_APPNAME, &obj );
    if ( FAILED( status ) ) return status;

    status = obj->lpVtbl->eventadd_OnActivation( obj, ActivationLoop, nullptr, &activationToken );
    if ( FAILED( status ) ) return status;

    status = new_async_operation_object_override_callback( (UnknownObject *)obj, nullptr, async_test, &operation );
    if ( FAILED( status ) ) return status;

    status = obj->lpVtbl->RunApplication( obj );
    if ( FAILED( status ) ) return status;

    status = obj->lpVtbl->eventremove_OnActivation( obj, activationToken );
    if ( FAILED( status ) ) return status;

    obj->lpVtbl->Release( obj );

    operation->lpVtbl->Release( operation );

    return status;
}
