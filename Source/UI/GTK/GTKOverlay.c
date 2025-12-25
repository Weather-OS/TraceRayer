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
 *  Module: GTKOverlay.c
 *  Description: GTK Drawing area helpers, mainly meant for Vulkan swapchains.
 */

#include <UI/GTK/GTKOverlay.h>

static struct gtk_overlay_object *impl_from_GTKOverlayObject( GTKOverlayObject *iface )
{
    return CONTAINING_RECORD( iface, struct gtk_overlay_object, GTKOverlayObject_iface );
}

static TR_STATUS gtk_overlay_object_QueryInterface( GTKOverlayObject *iface, const TRUUID uuid, void **out )
{
    const struct gtk_overlay_object *impl = impl_from_GTKOverlayObject( iface );

    TRACE( "iface %p, uuid %s, out %p\n", iface, debugstr_uuid( uuid ), out );

    if ( !uuid_compare( uuid, IID_UnknownObject ) || !uuid_compare( uuid, IID_GTKOverlayObject ) )
    {
        iface->lpVtbl->AddRef( iface );
        *out = iface;
        return T_SUCCESS;
    }

    if ( !uuid_compare( uuid, IID_GTKWidgetObject ) )
    {
        if ( !impl->GTKWidgetObject_impl )
        {
            ERROR( "Subclass GTKWidgetObject for GTKOverlayObject %p is not initialized yet!\n", iface );
            return T_NOINIT;
        }
        impl->GTKWidgetObject_impl->lpVtbl->AddRef( impl->GTKWidgetObject_impl );
        *out = impl->GTKWidgetObject_impl;
        return T_SUCCESS;
    }

    ERROR( "uuid %s is not implemented! returning T_NOTIMPL\n", debugstr_uuid( uuid ) );
    return T_NOTIMPL;
}

static TRLong gtk_overlay_object_AddRef( GTKOverlayObject *iface )
{
    struct gtk_overlay_object *impl = impl_from_GTKOverlayObject( iface );
    const TRLong added = atomic_fetch_add( &impl->ref, 1 ) + 1;
    TRACE( "iface %p increasing ref count to %ld\n", iface, added );
    return added;
}

static TRLong gtk_overlay_object_Release( GTKOverlayObject *iface )
{
    struct gtk_overlay_object *impl = impl_from_GTKOverlayObject( iface );
    const ATOMIC(TRLong) removed = atomic_fetch_sub(&impl->ref, 1);
    TRACE( "iface %p decreasing ref count to %ld\n", iface, removed - 1 );
    if ( !(removed - 1) )
    {
        if ( impl->GTKWidgetObject_impl )
            impl->GTKWidgetObject_impl->lpVtbl->Release( impl->GTKWidgetObject_impl );
        if ( impl->ChildWidget )
            impl->ChildWidget->lpVtbl->Release( impl->ChildWidget );
        free( impl );
    }
    return removed;
}

static TR_STATUS gtk_overlay_object_get_ChildWidget( GTKOverlayObject *iface, GTKWidgetObject **out )
{
    const struct gtk_overlay_object *impl = impl_from_GTKOverlayObject( iface );
    TRACE( "iface %p, out %p\n", iface, out );
    if ( !out ) throw_NullPtrException();
    if ( impl->ChildWidget )
    {
        impl->ChildWidget->lpVtbl->AddRef( impl->ChildWidget );
        *out = impl->ChildWidget;
        return T_SUCCESS;
    }
    return T_NOINIT;
}

static TR_STATUS gtk_overlay_object_set_ChildWidget( GTKOverlayObject *iface, GTKWidgetObject *widget )
{
    TR_STATUS status;
    GtkWidget *overlay;
    GtkWidget *childWidget;
    GTKWidgetObject *widgetObject;

    struct gtk_overlay_object *impl = impl_from_GTKOverlayObject( iface );

    if ( !widget ) throw_NullPtrException();

    TRACE( "iface %p, widget %p\n", iface, widget );

    if (impl->ChildWidget)
        // prevent dangling pointers
        impl->ChildWidget->lpVtbl->Release(impl->ChildWidget);

    status = iface->lpVtbl->QueryInterface( iface, IID_GTKWidgetObject, (void**)&widgetObject );
    if ( FAILED( status ) ) return status;

    status = widgetObject->lpVtbl->get_Widget( widgetObject, &overlay );
    if ( FAILED( status ) ) return status;

    status = widget->lpVtbl->get_Widget( widget, &childWidget );
    if ( FAILED( status ) ) return status;

    gtk_overlay_set_child( GTK_OVERLAY( overlay ), childWidget );

    widget->lpVtbl->AddRef( widget );
    impl->ChildWidget = widget;

    widgetObject->lpVtbl->Release( widgetObject );

    return T_SUCCESS;
}

static TR_STATUS gtk_overlay_object_AddWidget( GTKOverlayObject *iface, GTKWidgetObject *widget )
{
    TR_STATUS status;
    GtkWidget *overlay;
    GtkWidget *childWidget;
    GTKWidgetObject *widgetObject;

    struct gtk_overlay_object *impl = impl_from_GTKOverlayObject( iface );

    if ( !widget ) throw_NullPtrException();

    TRACE( "iface %p, widget %p\n", iface, widget );

    if (impl->ChildWidget)
        // prevent dangling pointers
            impl->ChildWidget->lpVtbl->Release(impl->ChildWidget);

    status = iface->lpVtbl->QueryInterface( iface, IID_GTKWidgetObject, (void**)&widgetObject );
    if ( FAILED( status ) ) return status;

    status = widgetObject->lpVtbl->get_Widget( widgetObject, &overlay );
    if ( FAILED( status ) ) return status;

    status = widget->lpVtbl->get_Widget( widget, &childWidget );
    if ( FAILED( status ) ) return status;

    gtk_overlay_add_overlay( GTK_OVERLAY( overlay ), childWidget );

    widget->lpVtbl->AddRef( widget );
    impl->ChildWidget = widget;

    widgetObject->lpVtbl->Release( widgetObject );

    return T_SUCCESS;
}

static GTKOverlayInterface gtk_overlay_interface =
{
    /* UnknownObject Methods */
    gtk_overlay_object_QueryInterface,
    gtk_overlay_object_AddRef,
    gtk_overlay_object_Release,
    /* GTKOverlayObject Methods */
    gtk_overlay_object_get_ChildWidget,
    gtk_overlay_object_set_ChildWidget,
    gtk_overlay_object_AddWidget
};

TR_STATUS TR_API new_gtk_overlay_object( OUT GTKOverlayObject **out )
{
    TR_STATUS status;
    GtkWidget *overlay;
    struct gtk_overlay_object *impl;

    TRACE( "out %p\n", out );

    if ( !out ) throw_NullPtrException();

    // Freed in Release();
    if (!(impl = calloc( 1, sizeof(*impl) ))) return T_OUTOFMEMORY;
    impl->GTKOverlayObject_iface.lpVtbl = &gtk_overlay_interface;
    impl->ref = 1;

    overlay = gtk_overlay_new();
    status = new_gtk_widget_object_override_widget( overlay, &impl->GTKWidgetObject_impl );
    if ( FAILED( status ) ) return status;

    *out = &impl->GTKOverlayObject_iface;

    TRACE( "created GTKOverlayObject %p\n", *out );

    return T_SUCCESS;
}