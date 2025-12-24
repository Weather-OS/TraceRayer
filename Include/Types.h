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
#include <uuid/uuid.h>

#define TR_API __attribute__((visibility("default")))

#define IN
#define OUT
#define INOUT
#define OPTIONAL

typedef char TRChar;
typedef char *TRString;
typedef const char *TRCString;
typedef unsigned char TRBool;
typedef unsigned short TRUShort;
typedef short TRShort;
typedef int TRInt;
typedef unsigned int TRUInt;
typedef long TRLong;
typedef unsigned long TRULong;
typedef size_t TRSize;
typedef double TRFloat;
typedef uuid_t TRUUID;

#ifdef __cplusplus
#include <atomic>
#define ATOMIC(type) std::atomic<type>
#else
#include <stdatomic.h>
#define ATOMIC(type) _Atomic type
#endif

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
    T_HANDLE = 38,
    T_POINTER = 125,
    T_NOTIMPL = 213,
    T_NOINIT = 214,
    T_ILLEGAL_METHOD_CALL = 310,
    T_ILLEGAL_DELEGATE_ASSIGNMENT = 311,
    T_ILLEGAL_STATE_CHANGE = 332,
} TR_STATUS;

typedef enum _TR_VariantType
{
    VT_EMPTY,
    VT_CHAR,
    VT_STRING,
    VT_BOOL,
    VT_UI8,
    VT_UI16,
    VT_UI32,
    VT_UI64,
    VT_I8,
    VT_I16,
    VT_I32,
    VT_I64,
    VT_PUNKVAL,
    VT_PPUNKVAL
} VariantType;

typedef struct _TR_PropVariant
{
    VariantType type;
    union
    {
        TRChar charVal;
        TRString stringVal;
        TRBool boolVal;
        TRUShort ushortVal;
        TRShort shortVal;
        TRUInt uintVal;
        TRInt intVal;
        TRLong longVal;
        TRULong ulongVal;
        void *punkVal;
        void **ppunkVal;
    };
} PropVariant;

#define PropVariantInit(p) \
    memset((p), 0, sizeof(PropVariant))

#define FAILED( status ) \
    (status != T_SUCCESS)

/**
 * CHECK_TR requires a _CLEANUP label for cleanup operations,
 * and a TR_STATUS for status assignment
 */
#define CHECK_TR( cb ) \
        if ( FAILED(( status = cb )) ) goto _CLEANUP;

#define __FILENAME__ \
    (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : \
    strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : \
    __FILE__)

#ifdef INITGUID
#define DEFINE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
    __attribute__((visibility("default"))) const uuid_t IID_##name = { \
        (unsigned char)(((l) >> 24) & 0xff), (unsigned char)(((l) >> 16) & 0xff), \
        (unsigned char)(((l) >> 8) & 0xff),  (unsigned char)((l) & 0xff), \
        (unsigned char)(((w1) >> 8) & 0xff), (unsigned char)((w1) & 0xff), \
        (unsigned char)(((w2) >> 8) & 0xff), (unsigned char)((w2) & 0xff), \
        (unsigned char)(b1), (unsigned char)(b2), (unsigned char)(b3), (unsigned char)(b4), \
        (unsigned char)(b5), (unsigned char)(b6), (unsigned char)(b7), (unsigned char)(b8) }
#else
#define DEFINE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
    extern const TRUUID IID_##name
#endif

#endif //TRACERAYER_TYPES_H
