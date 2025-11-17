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

#ifndef TRACERAYER_PATH_H
#define TRACERAYER_PATH_H

#include <Types.h>

typedef enum _TR_AccessType
{
    T_READ = 0,
    T_WRITE = 1,
    T_READWRITE = 2,
    T_EXECUTE = 3,
} TR_AccessType;

typedef struct _TR_Path
{
#ifdef _WIN32
    TRChar RootLetter;
#endif
    TRString Name;
    TRString Location;
    TRBool IsDirectory;
} TRPath;

TR_STATUS FetchPath( TRString path, TRBool create, TR_AccessType accessType, TRPath **pathObject );

#endif //TRACERAYER_PATH_H