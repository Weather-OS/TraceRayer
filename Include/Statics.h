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

#ifndef TRACERAYER_STATICS_H
#define TRACERAYER_STATICS_H

#define TRACERAYER_VERSION "0.0.1"

#define LOGFILE_HEADER "// --- ($DATE) ($TIME) Trace Rayer ($VERSION), A Ray Tracing Demo Written in GTK and Vulkan --- //\n"
#define LOG_FORMAT "[$THREAD] $MODULE::$FUNCTION ($LOG_CATEGORY) $MESSAGE"

// Terminal Colors
#define RESET_COLOR "\033[0m" // DO NOT CHANGE THIS!
#define DATE_COLOR "\033[1;37m" // White Bold
#define TIME_COLOR "\033[1;37m" // White Bold
#define VERSION_COLOR "\033[1;34m" // Cyan Bold
#define THREAD_COLOR "\033[0;37m" // White
#define MODULE_COLOR "\033[1;32m" // Green Bold
#define FUNCTION_COLOR "\033[1;37m" // White Bold

#endif //TRACERAYER_STATICS_H