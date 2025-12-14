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

#ifndef TRACERAYER_GTKWINDOW_H
#define TRACERAYER_GTKWINDOW_H

#include <gdk/gdk.h>

#include <Object.h>
#include <Signal.h>
#include <Types.h>

#include <UI/GTK/GTKWidget.h>

typedef struct _GTKWindowObject GTKWindowObject;

typedef void (*WindowLoopCallback)( IN GTKWindowObject *This );

typedef struct _GTKWindowInterface
{
    BEGIN_INTERFACE

    IMPLEMENTS_UNKNOWNOBJECT( GTKWindowObject )

    /**
     * @Method: GdkRectangle GTKWindowObject::WindowRect()
     * @Description: Retrieves the current window rectangle.
     *               The x and y positions are 0 on Wayland.
     * @Returns: The window rectangle.
     * @Status: Returns T_NOINIT if no child widget is initialized; otherwise
     *          returns T_SUCCESS.
     */
    TR_STATUS (*get_WindowRect)(
        IN GTKWindowObject *This,
        OUT GdkRectangle   *out);

    /**
     * @Method: void GTKWindowObject::WindowRect( GdkRectangle rect )
     * @Description: Set the current rectangle and the initial window rectangle.
     *               The x and y positions are ignored on Wayland.
     * @Status: Always returns T_SUCCESS.
     */
    TR_STATUS (*set_WindowRect)(
        IN GTKWindowObject *This,
        IN GdkRectangle     rect);

    /**
     * @Method: GTKWidgetObject* GTKWindowObject::ChildWidget()
     * @Description: Retrieves the child GTKWidgetObject associated with this
     *               GTKWindowObject.
     * @Returns: The child GTKWidgetObject, or nullptr if no child is set.
     * @Status: Returns T_NOINIT if no child widget is initialized; otherwise
     *          returns T_SUCCESS.
     */
    TR_STATUS (*get_ChildWidget)(
        IN  GTKWindowObject *This,
        OUT GTKWidgetObject **out);

    /**
     * @Method: void GTKWindowObject::ChildWidget( GTKWidgetObject *widget )
     * @Description: Assigns a child GTKWidgetObject to this GTKWindowObject.
     *               Ownership of the widget is transferred to the window handle,
     *               which becomes responsible for its lifetime.
     * @Status: Always returns T_SUCCESS.
     */
    TR_STATUS (*set_ChildWidget)(
        IN GTKWindowObject *This,
        IN GTKWidgetObject *widget);

    /**
     * @Method: void GTKWindowObject::SetWindowTitle( TRString title )
     * @Description: Set the current title of the window.
     * @Status: Returns T_SUCCESS unless QueryInterface from GTKWidgetObject fails.
     */
    TR_STATUS (*SetWindowTitle)(
        IN GTKWindowObject *This,
        IN TRString         title);

    /**
     * @Method: void GTKWindowObject::SetResizable( TRBool resizable )
     * @Description: Determines if the window should be resizable or not.
     * @Status: Returns T_SUCCESS unless QueryInterface from GTKWidgetObject fails.
     */
    TR_STATUS (*SetResizable)(
        IN GTKWindowObject *This,
        IN TRBool          resizable);

    /**
     * @Method: void GTKWindowObject::Show()
     * @Description: Shows the current window.
     * @Status: Returns T_SUCCESS unless QueryInterface from GTKWidgetObject fails.
     */
    TR_STATUS (*Show)(
        IN GTKWindowObject *This);

    /**
     * @Event: GTKWindowObject::OnDelete
     * @Description: Fired when the window receives a delete request
     *               (for example, from the window manager close button).
     *               Handlers may perform cleanup before the window is destroyed.
     */
    IMPLEMENTS_EVENT( GTKWindowObject, OnDelete )

    END_INTERFACE
} GTKWindowInterface;

interface _GTKWindowObject
{
    CONST_VTBL GTKWindowInterface *lpVtbl;
};

/**
 * @Object: GTKWindowObject
 * @Description: A GTK Window Object.
 *               Used to create a GTK 4 libadwaita window.
 * @Implements:
 *      GTKWidgetObject
 */
struct gtk_window_object
{
    // --- Public Members --- //
    GTKWindowObject GTKWindowObject_iface;
    volatile GdkRectangle WindowRect; // <-- It can be modified outside of the object context
    GTKWidgetObject *ChildWidget;

    // --- Base Interfaces --- //
    implements( GTKWidgetObject );

    // --- Private Members --- //
    WindowLoopCallback callback;
    implements_glib_eventlist( OnDelete )
    TRLong ref;
};

// 1b731a66-153d-4e54-898c-6d4de5c47e08
DEFINE_GUID( GTKWindowObject, 0x1b731a66, 0x153d, 0x4e54, 0x89, 0x8c, 0x6d, 0x4d, 0xe5, 0xc4, 0x7e, 0x08 );

// Constructors
/**
 * @Note: This constructor must not be called outside an "Activation Context".
 *        Refer to GTKObject on how to register "Activation Context" events.
 */
TR_STATUS new_gtk_window_object( IN GtkApplication *app, OUT GTKWindowObject **out );

#endif