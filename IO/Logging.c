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
#include <stdio.h>
#include <time.h>

#include <Logging.h>
#include <Statics.h>

TR_STATUS
ParseMessage(
    IN TRCString format,
    OPTIONAL IN Log_Category logCategory,
    OPTIONAL IN pid_t threadId,
    OPTIONAL IN TRString module,
    OPTIONAL IN TRString function,
    OPTIONAL IN TRString message,
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
            if ( strlen(format + iterator) >= 5 && strncmp( format + iterator, "$DATE", 5 ) == 0 )
            {
                TRChar dateStr[11];
                time_t t;
                struct tm *time_info;

                fmtSize -= strlen( "$DATE" );

                time( &t );
                time_info = localtime( &t );
                // date format: YYYY-MM-DD (11 bytes)
                strftime( dateStr, sizeof( dateStr ), "%Y-%m-%d", time_info );
                fmtSize += strlen( dateStr );

                reallocBuffer = realloc( buffer, fmtSize );
                if ( !reallocBuffer )
                {
                    free( buffer );
                    return T_OUTOFMEMORY;
                }

                buffer = reallocBuffer;

                strncat( buffer, dateStr, 10 );
                currentPos += 10;
                iterator += 4;
                continue;
            }

            if ( strlen(format + iterator) >= 5 && strncmp( format + iterator, "$TIME", 5 ) == 0 )
            {
                TRChar timeStr[9];
                time_t t;
                struct tm *time_info;

                fmtSize -= strlen( "$TIME" );

                time( &t );
                time_info = localtime( &t );
                // time format: HH:MM:SS (9 bytes)
                strftime( timeStr, sizeof( timeStr ), "%H:%M:%S", time_info );
                fmtSize += strlen( timeStr );

                reallocBuffer = realloc( buffer, fmtSize );
                if ( !reallocBuffer )
                {
                    free( buffer );
                    return T_OUTOFMEMORY;
                }

                buffer = reallocBuffer;

                strncat( buffer, timeStr, 8 );
                currentPos += 8;
                iterator += 4;
                continue;
            }

            if ( strlen(format + iterator) >= 7 && strncmp( format + iterator, "$VERSION", 7 ) == 0 )
            {
                const auto versionString = TRACERAYER_VERSION;
                fmtSize -= strlen( "$VERSION" );
                fmtSize += strlen( versionString );

                reallocBuffer = realloc( buffer, fmtSize );
                if ( !reallocBuffer )
                {
                    free( buffer );
                    return T_OUTOFMEMORY;
                }

                buffer = reallocBuffer;

                strncat( buffer, versionString, strlen( versionString ) + 1 );
                currentPos += strlen( versionString );
                iterator += 7;
                continue;
            }
        }

        buffer[currentPos++] = format[iterator];
    }

    // ensure NULL termination.
    buffer[currentPos] = '\0';

    *formattedString = buffer;

    return T_SUCCESS;
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
    ParseMessage( LOGFILE_HEADER, 0, 0, nullptr, nullptr, nullptr, &parsedMessage );
    printf("parsedMessage: %s", parsedMessage);
    return T_SUCCESS;
}
