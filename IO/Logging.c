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
 *  Module: Arguments.c
 *  Description: Handling logging.
 */

#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <time.h>

#include <IO/Logging.h>
#include <IO/Arguments.h>
#include <Statics.h>

static TR_STATUS
ParseMessage(
    IN TRCString format,
    IN TRBool useOutputColoring,
    OPTIONAL IN Log_Category logCategory,
    OPTIONAL IN pid_t threadId,
    OPTIONAL IN TRCString module,
    OPTIONAL IN TRCString function,
    OPTIONAL IN TRCString message,
    OUT TRString *formattedString
) {
    TRString buffer;
    TRString reallocBuffer;
    TRSize fmtSize;
    TRSize currentPos = 0;
    TRSize iterator;

    if ( !format || !formattedString )
        return T_INVALIDARG;

    fmtSize = strlen(format) + 1;

    buffer = (TRString)malloc( fmtSize * sizeof( TRChar ) );
    if ( !buffer )
        return T_OUTOFMEMORY;
    buffer[0] = '\0';

    for ( iterator = 0; format[iterator] != '\0'; iterator++ )
    {
        if ( format[iterator] == '$' )
        {
            if ( strlen( format + iterator ) >= 5 && strncmp( format + iterator, "$DATE", 5 ) == 0 )
            {
                TRChar dateStr[11];
                time_t t;
                struct tm *time_info;

                fmtSize -= strlen( "$DATE" );

                time( &t );
                time_info = localtime( &t );
                // date format: YYYY-MM-DD (11 bytes)
                strftime( dateStr, sizeof( dateStr ), "%Y-%m-%d", time_info );
                if ( useOutputColoring )
                {
                    fmtSize += strlen( RESET_COLOR ) + strlen( DATE_COLOR );
                    currentPos += strlen( RESET_COLOR ) + strlen( DATE_COLOR );
                }

                fmtSize += strlen( dateStr );

                reallocBuffer = realloc( buffer, fmtSize );
                if ( !reallocBuffer )
                {
                    free( buffer );
                    return T_OUTOFMEMORY;
                }

                buffer = reallocBuffer;

                if ( useOutputColoring )
                    strcat( buffer, DATE_COLOR );
                strncat( buffer, dateStr, 10 );
                if ( useOutputColoring )
                    strcat( buffer, RESET_COLOR );

                currentPos += 10;
                iterator += 4;
                continue;
            }

            if ( strlen( format + iterator ) >= 5 && strncmp( format + iterator, "$TIME", 5 ) == 0 )
            {
                TRChar timeStr[9];
                time_t t;
                struct tm *time_info;

                fmtSize -= strlen( "$TIME" );

                time( &t );
                time_info = localtime( &t );
                // time format: HH:MM:SS (9 bytes)
                strftime( timeStr, sizeof( timeStr ), "%H:%M:%S", time_info );
                if ( useOutputColoring )
                {
                    fmtSize += strlen( RESET_COLOR ) + strlen( TIME_COLOR );
                    currentPos += strlen( RESET_COLOR ) + strlen( TIME_COLOR );
                }
                fmtSize += strlen( timeStr );

                reallocBuffer = realloc( buffer, fmtSize );
                if ( !reallocBuffer )
                {
                    free( buffer );
                    return T_OUTOFMEMORY;
                }

                buffer = reallocBuffer;

                if ( useOutputColoring )
                    strcat( buffer, TIME_COLOR );
                strncat( buffer, timeStr, 8 );
                if ( useOutputColoring )
                    strcat( buffer, RESET_COLOR );

                currentPos += 8;
                iterator += 4;
                continue;
            }

            if ( strlen( format + iterator ) >= 8 && strncmp( format + iterator, "$VERSION", 8 ) == 0 )
            {
                const auto versionString = TRACERAYER_VERSION;
                fmtSize -= strlen( "$VERSION" );
                if ( useOutputColoring )
                {
                    fmtSize += strlen( RESET_COLOR ) + strlen( VERSION_COLOR );
                    currentPos += strlen( RESET_COLOR ) + strlen( VERSION_COLOR );
                }
                fmtSize += strlen( versionString );

                reallocBuffer = realloc( buffer, fmtSize );
                if ( !reallocBuffer )
                {
                    free( buffer );
                    return T_OUTOFMEMORY;
                }

                buffer = reallocBuffer;

                if ( useOutputColoring )
                    strcat( buffer, VERSION_COLOR );
                strncat( buffer, versionString, strlen( versionString ) + 1 );
                if ( useOutputColoring )
                    strcat( buffer, RESET_COLOR );

                currentPos += strlen( versionString );
                iterator += 7;
                continue;
            }

            if ( strlen( format + iterator ) >= 13 && strncmp( format + iterator, "$LOG_CATEGORY", 13 ) == 0 )
            {
                TRChar colorStr[8];
                TRChar logCategoryStr[8];

                switch ( logCategory )
                {
                    case LOG_CATEGORY_INFO:
                        strcpy( logCategoryStr, "info" );
                        strcpy( colorStr, "\033[1;37m" );
                        break;

                    case LOG_CATEGORY_ERROR:
                        strcpy( logCategoryStr, "error" );
                        strcpy( colorStr, "\033[1;31m" );
                        break;

                    case LOG_CATEGORY_WARNING:
                        strcpy( logCategoryStr, "warning" );
                        strcpy( colorStr, "\033[1;33m" );
                        break;

                    case LOG_CATEGORY_TRACE:
                        strcpy( logCategoryStr, "trace" );
                        strcpy( colorStr, "\033[1;37m" );
                        break;
                }

                fmtSize -= strlen( "$LOG_CATEGORY" );
                if ( useOutputColoring )
                {
                    fmtSize += strlen( RESET_COLOR ) + strlen( colorStr );
                    currentPos += strlen( RESET_COLOR ) + strlen( colorStr );
                }
                fmtSize += strlen( logCategoryStr );

                reallocBuffer = realloc( buffer, fmtSize );
                if ( !reallocBuffer )
                {
                    free( buffer );
                    return T_OUTOFMEMORY;
                }

                buffer = reallocBuffer;

                if ( useOutputColoring )
                    strcat( buffer, colorStr );
                strcat( buffer, logCategoryStr );
                if ( useOutputColoring )
                    strcat( buffer, RESET_COLOR );

                currentPos += strlen( logCategoryStr );
                iterator += 12;
                continue;
            }

            if ( strlen( format + iterator ) >= 7 && strncmp( format + iterator, "$THREAD", 7 ) == 0 )
            {
                TRChar threadString[20];
                sprintf( threadString, "%d", threadId );
                fmtSize -= strlen( "$THREAD" );
                if ( useOutputColoring )
                {
                    fmtSize += strlen( RESET_COLOR ) + strlen( THREAD_COLOR );
                    currentPos += strlen( RESET_COLOR ) + strlen( THREAD_COLOR );
                }
                fmtSize += strlen( threadString );

                reallocBuffer = realloc( buffer, fmtSize );
                if ( !reallocBuffer )
                {
                    free( buffer );
                    return T_OUTOFMEMORY;
                }

                buffer = reallocBuffer;

                if ( useOutputColoring )
                    strcat( buffer, THREAD_COLOR );
                strcat( buffer, threadString );
                if ( useOutputColoring )
                    strcat( buffer, RESET_COLOR );

                currentPos += strlen( threadString );
                iterator += 6;
                continue;
            }

            if ( strlen( format + iterator ) >= 7 && strncmp( format + iterator, "$MODULE", 7 ) == 0 )
            {
                fmtSize -= strlen( "$MODULE" );
                if ( useOutputColoring )
                {
                    fmtSize += strlen( RESET_COLOR ) + strlen( MODULE_COLOR );
                    currentPos += strlen( RESET_COLOR ) + strlen( MODULE_COLOR );
                }
                fmtSize += strlen( module );

                reallocBuffer = realloc( buffer, fmtSize );
                if ( !reallocBuffer )
                {
                    free( buffer );
                    return T_OUTOFMEMORY;
                }

                buffer = reallocBuffer;

                if ( useOutputColoring )
                    strcat( buffer, MODULE_COLOR );
                strcat( buffer, module );
                if ( useOutputColoring )
                    strcat( buffer, RESET_COLOR );

                currentPos += strlen( module );
                iterator += 6;
                continue;
            }

            if ( strlen( format + iterator ) >= 9 && strncmp( format + iterator, "$FUNCTION", 9 ) == 0 )
            {
                fmtSize -= strlen( "$FUNCTION" );
                if ( useOutputColoring )
                {
                    fmtSize += strlen( RESET_COLOR ) + strlen( FUNCTION_COLOR );
                    currentPos += strlen( RESET_COLOR ) + strlen( FUNCTION_COLOR );
                }
                fmtSize += strlen( function );

                reallocBuffer = realloc( buffer, fmtSize );
                if ( !reallocBuffer )
                {
                    free( buffer );
                    return T_OUTOFMEMORY;
                }

                buffer = reallocBuffer;

                if ( useOutputColoring )
                    strcat( buffer, FUNCTION_COLOR );
                strcat( buffer, function );
                if ( useOutputColoring )
                    strcat( buffer, RESET_COLOR );

                currentPos += strlen( function );
                iterator += 8;
                continue;
            }

            if ( strlen( format + iterator ) >= 8 && strncmp( format + iterator, "$MESSAGE", 8 ) == 0 )
            {
                fmtSize -= strlen( "$MESSAGE" );
                fmtSize += strlen( message );

                reallocBuffer = realloc( buffer, fmtSize );
                if ( !reallocBuffer )
                {
                    free( buffer );
                    return T_OUTOFMEMORY;
                }

                buffer = reallocBuffer;

                strcat( buffer, message );

                currentPos += strlen( message );
                iterator += 7;
                continue;
            }
        }

        buffer[currentPos++] = format[iterator];
        buffer[currentPos] = '\0';
    }

    *formattedString = buffer;

    return T_SUCCESS;
}

void
TraceRayer_DEBUG(
    IN Log_Category category,
    IN pid_t threadId,
    IN TRCString module,
    IN TRCString function,
    IN TRCString fmt,
    ...
) {
    va_list ap = {};
    va_list ap_copy = {};

    TRString parsedMessage;
    TRString buffer;
    TRSize bufferSize;

    if ( GlobalArgumentsDefault.LogLevel < category )
        return;

    va_start( ap, fmt );
    va_copy( ap_copy, ap );
    bufferSize = vsnprintf( nullptr, 0, fmt, ap_copy );
    va_end( ap_copy );

    buffer = (TRString)malloc( bufferSize + 1 );
    if ( !buffer )
    {
        va_end( ap );
        return;
    }

    vsnprintf( buffer, bufferSize + 1, fmt, ap );
    va_end( ap );

    ParseMessage( LOG_FORMAT, GlobalArgumentsDefault.ColoredTerminalOutput, category, threadId, module, function, buffer, &parsedMessage );
    fprintf( stdout, "%s", parsedMessage );
    free(parsedMessage);

    if ( GlobalArgumentsDefault.LogFile )
    {
        ParseMessage( LOG_FORMAT, false, category, threadId, module, function, buffer, &parsedMessage );
        fprintf( GlobalArgumentsDefault.LogFile->FileHandle, "%s", parsedMessage );
        free(parsedMessage);
    }

    free( buffer );
}

TRChar *debugstr_uuid( IN const uuid_t uuid )
{
    static _Thread_local TRChar str[37];
    uuid_unparse( uuid, str );
    return str;
}

TR_STATUS
InitializeLogging()
{
    TRString parsedMessage;
    if ( GlobalArgumentsDefault.LogFile && GlobalArgumentsDefault.LogFile->Location )
    {
        if ( GlobalArgumentsDefault.LogFile->IsDirectory )
            return T_INVALIDARG;

        if ( !GlobalArgumentsDefault.LogFile->FileHandle )
            return T_HANDLE;
    }

    ParseMessage( LOGFILE_HEADER, GlobalArgumentsDefault.ColoredTerminalOutput, LOG_CATEGORY_INFO, 0, nullptr, nullptr, nullptr, &parsedMessage );
    printf("%s", parsedMessage);
    free(parsedMessage);
    return T_SUCCESS;
}