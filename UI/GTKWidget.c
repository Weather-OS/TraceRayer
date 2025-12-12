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
 *  Module: GTKWidget.c
 *  Description: GTKWidget can be contained from different GTK classes.
 */

#include <UI/GTKWidget.h>

static struct gtk_widget_object *impl_from_GTKWidgetObject( GTKWidgetObject *iface )
{
    return CONTAINING_RECORD( iface, struct gtk_widget_object, GTKWidgetObject_iface );
}

DEFINE_SHALLOW_UNKNOWNOBJECT( GTKWidgetObject, gtk_widget_object );

static TR_STATUS gtk_widget_object_get_Widget( GTKWidgetObject *iface, GtkWidget **out )
{
    const struct gtk_widget_object *impl = impl_from_GTKWidgetObject( iface );
    TRACE( "iface %p, widget %p\n", iface, out );
    if ( !out ) throw_NullPtrException();
    *out = impl->Widget;
    return T_SUCCESS;
}

static void gtk_widget_object_setVisibility( GTKWidgetObject *iface, TRBool visibility )
{
    const struct gtk_widget_object *impl = impl_from_GTKWidgetObject( iface );
    TRACE( "iface %p\n", iface );

    gtk_widget_set_visible( impl->Widget, visibility );
}

static GTKWidgetObjectInterface gtk_widget_object_interface =
{
    /* UnknownObject Methods */
    gtk_widget_object_QueryInterface,
    gtk_widget_object_AddRef,
    gtk_widget_object_Release,
    /* GTKObject Methods */
    gtk_widget_object_get_Widget,
    gtk_widget_object_setVisibility
};

TR_STATUS new_gtk_widget_object_override_widget( IN GtkWidget *widget, OUT GTKWidgetObject **out )
{
    struct gtk_widget_object *impl;

    TRACE( "widget %p, out %p\n", widget, out );

    if ( !out ) throw_NullPtrException();

    // Freed in Release();
    if (!(impl = calloc( 1, sizeof(*impl) ))) return T_OUTOFMEMORY;

    impl->GTKWidgetObject_iface.lpVtbl = &gtk_widget_object_interface;
    impl->Widget = widget;
    atomic_init( &impl->ref, 1 );

    *out = &impl->GTKWidgetObject_iface;
    TRACE( "created GTKWidgetObject %p\n", *out );

    return T_SUCCESS;
}