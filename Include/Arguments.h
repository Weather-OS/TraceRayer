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

#ifndef TRACERAYER_ARGUMENTS_H
#define TRACERAYER_ARGUMENTS_H

#include <Types.h>
#include <Path.h>

typedef struct _TR_GlobalArguments
{
    TRString GPUName;
    TRLong LogLevel;
    TRPath *LogFile;
    TRBool ColoredTerminalOutput;
} GlobalArguments;

extern GlobalArguments GlobalArgumentsDefault;

typedef enum _TR_Argument_ValueType
{
    TYPE_STRING,
    TYPE_BOOL,
    TYPE_EMPTY,
    TYPE_LONG,
    TYPE_PATH //Mainly for checking if the path is valid. We could use TYPE_STRING here.
} Argument_ValueType;

typedef struct _TR_Argument
{
    TRString Name;
    TRChar Abbreviation;
    Argument_ValueType ValueType;
    OPTIONAL void *Value;
    OPTIONAL TR_STATUS (*Callback)();
} Argument;

static Argument Available_Arguments[] =
{
    {
        .Name         = "help",
        .Abbreviation = 'h',
        .ValueType    = TYPE_EMPTY
    },
    {
        .Name         = "version",
        .Abbreviation = 'v',
        .ValueType    = TYPE_EMPTY
    },
    {
        .Name         = "gpu-name",
        .Abbreviation = 'g',
        .ValueType    = TYPE_STRING,
    },
    {
        .Name         = "log-level",
        .Abbreviation = 'l',
        .ValueType    = TYPE_LONG
    },
    {
        .Name         = "log-file",
        .Abbreviation = 'f',
        .ValueType    = TYPE_PATH
    },
    {
        .Name         = "colored-terminal-output",
        .Abbreviation = 'c',
        .ValueType   = TYPE_BOOL
    }
};

TR_STATUS ParseCommandLineArguments( IN int argc, IN char **arguments );

#endif //TRACERAYER_ARGUMENTS_H