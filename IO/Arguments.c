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
 *  Description: Command line arguments handling
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include <Arguments.h>

GlobalArguments GlobalArgumentsDefault =
{
    .GPUName = nullptr,
    .LogLevel = 0,
    .LogFile = nullptr,
    .ColoredTerminalOutput = true,
};

static TR_STATUS
InitGlobalArguments()
{
    // --gpu-name
    Available_Arguments[2].Value = &GlobalArgumentsDefault.GPUName;
    // --log-level
    Available_Arguments[3].Value = &GlobalArgumentsDefault.LogLevel;
    // --log-file
    Available_Arguments[4].Value = &GlobalArgumentsDefault.LogFile;
    // --colored-terminal-output
    Available_Arguments[5].Value = &GlobalArgumentsDefault.ColoredTerminalOutput;

    return T_SUCCESS;
}

TR_STATUS
ParseCommandLineArguments(
    IN const int argc,
    IN char **arguments
) {
    TR_STATUS status = T_SUCCESS;

    TRInt iterator;
    TRInt secondIterator;
    TRInt stringIterator;
    TRSize valSize = 0;
    TRLong longVal;
    TRBool boolVal;
    TRString val = nullptr;

    status = InitGlobalArguments();
    if ( FAILED( status ) ) return status;

    for ( iterator = 1; iterator < argc; iterator++ )
    {
        if ( strncmp( arguments[iterator], "--", 2 ) == 0 )
        {
            for ( secondIterator = 0; secondIterator < sizeof(Available_Arguments) / sizeof(Argument); secondIterator++ )
            {
                if ( strncmp( arguments[iterator] + 2, Available_Arguments[secondIterator].Name, strlen( Available_Arguments[secondIterator].Name ) ) == 0 )
                {
                    /**
                     *  There are 2 situations here:
                     *      --arg=val
                     *  OR
                     *      --arg val
                     */
                    // Some thing to keep in mind: Values wrapped in "" are automatically unwrapped for us.

                    if ( strlen( arguments[iterator] + 2 ) > strlen( Available_Arguments[secondIterator].Name ) )
                    {
                        if ( (arguments[iterator] + 2)[ strlen( Available_Arguments[secondIterator].Name ) ] == '=' )
                        {
                            val = strdup( arguments[iterator] + strlen(Available_Arguments[secondIterator].Name) + 3 );
                            valSize = strlen( val );
                        } else
                        {
                            //invalid argument trap
                            return T_ERROR;
                        }
                    } else if ( argc > iterator + 1 && arguments[iterator + 1][0] != '-' )
                    {
                        val = arguments[iterator + 1];
                        valSize = strlen( val );
                    }

                    switch ( Available_Arguments[secondIterator].ValueType )
                    {
                        case TYPE_STRING:
                        {
                            if ( val == nullptr )
                            {
                                return T_ERROR;
                            }
                            const auto target = (TRString *)Available_Arguments[secondIterator].Value;
                            *target = strdup(val);
                            break;
                        }

                        case TYPE_LONG:
                        {
                            if ( val == nullptr )
                            {
                                return T_ERROR;
                            }
                            for ( stringIterator = 0; stringIterator < valSize; stringIterator++ )
                            {
                                if ( !isdigit( val[stringIterator] ) )
                                {
                                    //TODO: Handle errors
                                    return T_ERROR;
                                }
                            }
                            longVal = strtol( val, nullptr, 10 );
                            const auto target = (TRLong *)Available_Arguments[secondIterator].Value;
                            *target = longVal;
                            break;
                        }

                        case TYPE_BOOL:
                        {
                            // Assume true if no value is provided
                            if ( val == nullptr )
                                boolVal = true;
                            else
                            {
                                if ( val[0] == 't' || val[0] == 'T' || val[0] == 'y' || val[0] == 'Y' || val[0] == '1' )
                                    boolVal = true;
                                else if ( val[0] == 'n' || val[0] == 'N' || val[0] == 'f' || val[0] == 'F' || val[0] == '0' )
                                    boolVal = false;
                                else
                                {
                                    //TODO: Handle errors
                                    exit( 1 );
                                }
                            }
                            const auto target = (TRBool *)Available_Arguments[secondIterator].Value;
                            *target = boolVal;
                            break;
                        }

                        case TYPE_EMPTY:
                        {
                            if ( val != nullptr )
                            {
                                //TODO: Handle errors
                                exit( 1 );
                            }
                            if ( Available_Arguments[secondIterator].Callback != nullptr )
                            {
                                Available_Arguments[secondIterator].Callback();
                            }
                            break;
                        }

                        case TYPE_PATH:
                        {
                            if ( val == nullptr )
                            {
                                //TODO: Handle errors
                                exit( 1 );
                            }
                            auto target = (TRPath *)Available_Arguments[secondIterator].Value;

                            status = FetchPath( val, true, T_READWRITE, &target );
                            if ( FAILED( status ) ) return status;

                            if ( Available_Arguments[secondIterator].Callback != nullptr )
                            {
                                Available_Arguments[secondIterator].Callback();
                            }
                            break;
                        }
                    }
                }
            }
        }
    }
    return T_SUCCESS;
}
