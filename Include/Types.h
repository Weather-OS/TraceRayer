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

#ifndef TRACERAYER_TYPES_H
#define TRACERAYER_TYPES_H

#include <wchar.h>
#include <stddef.h>
#include <stdbool.h>

#define IN
#define OUT
#define INOUT
#define OPTIONAL

typedef char TRChar;
typedef char *TRString;
typedef unsigned char TRBool;
typedef int TRInt;
typedef unsigned int TRUInt;
typedef long TRLong;
typedef unsigned long TRULong;
typedef size_t TRSize;
typedef double TRFloat;

typedef wchar_t TRWChar;
typedef wchar_t *TRWString;

typedef enum _TR_STATUS
{
    T_SUCCESS = 0,
    T_ERROR = 1,
    T_FILE_NOT_FOUND = 2,
    T_INVALIDARG = 3,
    T_ACCESSDENIED = 13,
    T_OUTOFMEMORY = 14,
} TR_STATUS;

#define FAILED( status ) \
    (status != T_SUCCESS)


#endif //TRACERAYER_TYPES_H