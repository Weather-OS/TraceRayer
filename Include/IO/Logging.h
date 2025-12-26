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

#ifndef TRACERAYER_LOGGING_H
#define TRACERAYER_LOGGING_H

#include <unistd.h>
#include <string.h>
#include <sys/syscall.h>

#include <Types.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef SYS_gettid
#define gettid() syscall(SYS_gettid)
#else
#error "SYS_gettid unavailable on this system"
#endif

typedef enum _TR_Log_Category
{
    LOG_CATEGORY_INFO = 0,
    LOG_CATEGORY_ERROR = 1,
    LOG_CATEGORY_WARNING = 2,
    LOG_CATEGORY_TRACE = 3,
} Log_Category;

typedef struct _TR_Log_Token
{
    TRString Module;
    Log_Category Category;
} Log_Token;

TR_STATUS TR_API InitializeLogging();
void TR_API TraceRayer_DEBUG( IN Log_Category category, IN pid_t threadId, IN TRCString module, IN TRCString function, IN TRCString fmt, ... );

#define INFO(message, ...) \
    TraceRayer_DEBUG( LOG_CATEGORY_INFO, gettid(), __FILENAME__, __FUNCTION__, message, ##__VA_ARGS__)

#define WARN(message, ...) \
    TraceRayer_DEBUG( LOG_CATEGORY_WARNING, gettid(), __FILENAME__, __FUNCTION__, message, ##__VA_ARGS__)

#define ERROR(message, ...) \
    TraceRayer_DEBUG( LOG_CATEGORY_ERROR, gettid(), __FILENAME__, __FUNCTION__, message, ##__VA_ARGS__)

#define TRACE(message, ...) \
    TraceRayer_DEBUG( LOG_CATEGORY_TRACE, gettid(), __FILENAME__, __FUNCTION__, message, ##__VA_ARGS__)

// Critical Exceptions
// TODO: Implement unified throw routine
#define throw_NullPtrException() \
    {                                                           \
        ERROR("A critical null pointer exception occured!\n");  \
        exit(125);                                              \
    }                                                           \

TRString TR_API debugstr_uuid( IN const uuid_t uuid );

#ifdef __cplusplus
} // extern "C"

#include <stdexcept>
#include <string>

struct TRException final : std::runtime_error
{
    TR_STATUS status;
    std::string msg;

    explicit TRException( TR_STATUS s ): std::runtime_error( "Unhandled Exception: " + std::to_string(s) ), status(s)
    {
        ERROR( "Exception %d within C++ code.\n", status );
    }

    explicit TRException( TR_STATUS s, const std::string &message ): std::runtime_error( "Unhandled Exception: " + std::to_string(s) + " with message " + message ), status(s), msg(message)
    {
        ERROR( "Exception %d within C++ code with message %s.\n", status, message.c_str() );
    }
};

#define check_tr_( tr ){ TR_STATUS st = tr; if ( FAILED( st ) ) throw TRException( st ); }

#endif

#endif