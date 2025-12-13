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
 *  Module: Path.c
 *  Description: File and folder path handling
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>

#ifdef _WIN32
#include <WinDef.h>
#else
#include <limits.h>
#endif

#include <IO/Path.h>

TR_STATUS
FetchPath(
    IN TRString path,
    IN TRBool create,
    IN AccessType accessType,
    OUT TRPath **pathObject
) {
    TRPath *newPath;
    TRString name;
    TRInt acs = 0;

    if ( !path || *path == '\0' )
        return T_INVALIDARG;

    *pathObject = nullptr;

    newPath = (TRPath *)calloc( 1, sizeof( *newPath ) );
    if ( !newPath ) return T_OUTOFMEMORY;

#ifdef _WIN32
    if ( strlen( path ) >= MAX_PATH )
        return T_INVALIDARG;

    for ( TRString iter = path; iter; iter++ )
        if ( strchr( "<>:\"|?*", *iter ) )
            return T_INVALIDARG;

    TRSize len = strlen( path );
    if ( path[len - 1] == ' ' || path[len - 1] == '.' )
        return T_INVALIDARG;
    return T_OK;
#else
    if ( strlen( path ) >= PATH_MAX )
        return T_INVALIDARG;

    for ( TRString iter = path; *iter != '\0'; iter++ )
    {
        if ( *iter == '\n' || *iter == '\r' )
            return T_INVALIDARG;
    }

    switch ( accessType )
    {
        case T_READ:
            acs = R_OK;
            break;

        case T_WRITE:
            acs = W_OK;
            break;

        case T_READWRITE:
            acs = R_OK | W_OK;
            break;

        case T_EXECUTE:
            acs = X_OK;
            break;
    }

    if ( !create )
    {
        struct stat statInfo;

        if ( FAILED( access( path, F_OK ) ) )
            return T_FILE_NOT_FOUND;

        if ( FAILED( access( path, F_OK | acs ) ) )
            return T_ACCESSDENIED;

        if ( FAILED( stat( path, &statInfo ) ) )
            return T_ERROR;

        newPath->IsDirectory = S_ISDIR( statInfo.st_mode );
    }
    else
    {
        TRString rootPath;
        TRString lastSlash;

        // The root should still be accessible
        rootPath = (TRString)malloc( (strlen( path ) + 1) * sizeof( TRChar ) );
        if ( !rootPath ) return T_OUTOFMEMORY;
        strcpy( rootPath, path );

        lastSlash = strrchr( rootPath, '/' );
        if ( lastSlash )
            *(lastSlash + 1) = '\0';

        if ( FAILED( access( rootPath, F_OK ) ) )
        {
            free ( rootPath );
            return T_FILE_NOT_FOUND;
        }

        // Write access is necessary because we'll need to create a file here.
        if ( FAILED( access( rootPath, F_OK | W_OK | R_OK ) ) )
        {
            free ( rootPath );
            return T_ACCESSDENIED;
        }

        free ( rootPath );
    }

#endif

    newPath->Location = (TRString)malloc( PATH_MAX * sizeof( TRChar ) );
    realpath( path, newPath->Location );
    name = strrchr( path, '/' ) + 1;
    newPath->Name = (TRString)malloc( (strlen( name ) + 1) * sizeof( TRString ) );
    strcpy( newPath->Name, name );
    newPath->AccessType = accessType;

    switch ( accessType )
    {
        case T_READ:
        case T_EXECUTE:
            if ( create )
                newPath->FileHandle = fopen( newPath->Location, "r" );
            else
                newPath->FileHandle = fopen( newPath->Location, "r+" );
            break;

        case T_WRITE:
            if ( create )
                newPath->FileHandle = fopen( newPath->Location, "w" );
            else
                newPath->FileHandle = fopen( newPath->Location, "w+" );
            break;

        case T_READWRITE:
            if ( create )
                newPath->FileHandle = fopen( newPath->Location, "a" );
            else
                newPath->FileHandle = fopen( newPath->Location, "a+" );
            break;
    }

    *pathObject = newPath;

    return T_SUCCESS;
}