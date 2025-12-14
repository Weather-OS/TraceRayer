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

#include <stdio.h>
#include <stdlib.h>

#include <IO/FetchResources.h>
#include <IO/Logging.h>

TR_STATUS
FetchResource(
    IN TRString resourceName,
    OUT TRPath **outResourcePath
) {
    TR_STATUS status;
    TRString path;
    TRString resourcePath;
    TRSize resourcePathSize;

    // First pass: Binary root
    path = "./Resources";
    resourcePathSize = strlen( path ) + strlen( resourceName ) + 3;
    resourcePath = (TRString)malloc( resourcePathSize * sizeof( TRChar ) );
    snprintf( resourcePath, resourcePathSize, "%s/%s", path, resourceName );
    status = FetchPath( resourcePath, false, T_READ, outResourcePath );
    free( resourcePath );
    if ( !FAILED( status ) ) return T_SUCCESS;

    // Second pass: Source dir
    path = "../Resources";
    resourcePathSize = strlen( path ) + strlen( resourceName ) + 3;
    resourcePath = (TRString)malloc( resourcePathSize * sizeof( TRChar ) );
    snprintf( resourcePath, resourcePathSize, "%s/%s", path, resourceName );
    status = FetchPath( resourcePath, false, T_READ, outResourcePath );
    free( resourcePath );
    if ( !FAILED( status ) ) return T_SUCCESS;

    // Third pass: Install dir
    path = RESOURCE_DIR;
    resourcePathSize = strlen( path ) + strlen( resourceName ) + 3;
    resourcePath = (TRString)malloc( resourcePathSize * sizeof( TRChar ) );
    snprintf( resourcePath, resourcePathSize, "%s/%s", path, resourceName );
    status = FetchPath( resourcePath, false, T_READ, outResourcePath );

    if ( FAILED( status ) )
        ERROR( "Failed to fetch resources! Make sure %s exists!\n", resourcePath );
    free( resourcePath );

    return status;
}
