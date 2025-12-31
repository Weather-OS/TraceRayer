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

#ifndef TRACERAYER_SURFACE_H
#define TRACERAYER_SURFACE_H

#include <gdk/gdk.h>

#ifdef GDK_WINDOWING_X11
#include <gdk/x11/gdkx.h>
#define PLATFORM_SUPPORTS_X11
#endif
#ifdef GDK_WINDOWING_WAYLAND
#include <gdk/wayland/gdkwayland.h>
#define PLATFORM_SUPPORTS_WAYLAND
#endif
#ifdef GDK_WINDOWING_WIN32
#include <gdk/win32/gdkwin32.h>
#define PLATFORM_SUPPORTS_WIN32
#endif
#ifdef GDK_WINDOWING_MACOS
#include <gdk/macos/gdkmacos.h>
#define PLATFORM_SUPPORTS_MACOS
#endif

typedef enum _WindowType
{
    WindowType_GTK,
    WindowType_Qt,
    WindowType_Cocoa,
    WindowType_Win32
} WindowType;

typedef enum _SurfaceType
{
    SurfaceType_X11,
    SurfaceType_Wayland,
    SurfaceType_macOS,
    SurfaceType_Win32
} SurfaceType;

typedef struct _WindowRepresentation
{
    WindowType windowType;
    SurfaceType surfaceType;

#ifdef GDK_WINDOWING_X11
    Display *x11_display;
    Window x11_window;
#endif
#ifdef GDK_WINDOWING_WAYLAND
    struct wl_display *wayland_display;
    struct wl_surface *wayland_surface;
#endif
#ifdef GDK_WINDOWING_WIN32
    HWND win32_window;
#endif
#ifdef GDK_MACOS
    CAMetalLayer *metal_layer;
#endif

} WindowRepresentation;

#endif