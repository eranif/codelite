#ifdef __WXGTK__
#include "WindowUtils.hpp"

#include <algorithm>
#include <gtk/gtk.h>
#include <wx/window.h>

#if GTK_CHECK_VERSION(3, 0, 0)
namespace
{
// The corner radius (in pixels) is stored on the window with this key. It also marks the window as styled
constexpr const char* kRadiusKey = "codelite-popup-radius";

void RoundedRectPath(cairo_t* cr, double x, double y, double w, double h, double r)
{
    r = std::min({r, w / 2.0, h / 2.0});
    cairo_new_sub_path(cr);
    cairo_arc(cr, x + w - r, y + r, r, -G_PI / 2.0, 0.0);
    cairo_arc(cr, x + w - r, y + h - r, r, 0.0, G_PI / 2.0);
    cairo_arc(cr, x + r, y + h - r, r, G_PI / 2.0, G_PI);
    cairo_arc(cr, x + r, y + r, r, G_PI, 3.0 * G_PI / 2.0);
    cairo_close_path(cr);
}

double GetRadius(GtkWidget* widget)
{
    const auto* radius = static_cast<const double*>(g_object_get_data(G_OBJECT(widget), kRadiusKey));
    return radius != nullptr ? *radius : 0.0;
}

// Runs before the default handler: everything that is drawn after it (the background and all the children) is
// clipped to the rounded rectangle. The pixels outside of it stay transparent.
gboolean OnDrawBefore(GtkWidget* widget, cairo_t* cr, gpointer)
{
    const int width = gtk_widget_get_allocated_width(widget);
    const int height = gtk_widget_get_allocated_height(widget);
    RoundedRectPath(cr, 0.0, 0.0, width, height, GetRadius(widget));
    cairo_clip(cr);

    // The window is "app paintable" (so it can be transparent): the default handler does not draw its background.
    // Draw it here, so the areas that no child paints (e.g. the sizer border) are filled and rounded
    gtk_render_background(gtk_widget_get_style_context(widget), cr, 0.0, 0.0, width, height);
    return FALSE;
}

// Runs after the default handler: draw a thin border over the edge (the clip is still active)
gboolean OnDrawAfter(GtkWidget* widget, cairo_t* cr, gpointer)
{
    GdkRGBA color;
    gtk_style_context_get_color(gtk_widget_get_style_context(widget), GTK_STATE_FLAG_NORMAL, &color);
    cairo_set_source_rgba(cr, color.red, color.green, color.blue, 0.3);
    cairo_set_line_width(cr, 1.0);
    RoundedRectPath(cr,
                    0.5,
                    0.5,
                    gtk_widget_get_allocated_width(widget) - 1.0,
                    gtk_widget_get_allocated_height(widget) - 1.0,
                    std::max(0.0, GetRadius(widget) - 0.5));
    cairo_stroke(cr);
    return FALSE;
}

void FreeRadius(gpointer data) { delete static_cast<double*>(data); }
} // namespace
#endif

void StylePopupWindow(wxWindow* popup, double radius)
{
#if GTK_CHECK_VERSION(3, 0, 0)
    if (popup == nullptr) {
        return;
    }

    GtkWidget* widget = popup->GetHandle();
    if (widget == nullptr || !GTK_IS_WINDOW(widget)) {
        return;
    }

    // The visual of a window cannot be changed after it is realized (shown), so on GTK this must be called before
    // the popup is shown. After that, we cannot make the window transparent: leave it as is.
    if (gtk_widget_get_realized(widget)) {
        return;
    }

    // Transparent corners need an alpha channel and a compositor (always the case on Wayland)
    GdkScreen* screen = gtk_widget_get_screen(widget);
    GdkVisual* visual =
        (screen != nullptr && gdk_screen_is_composited(screen)) ? gdk_screen_get_rgba_visual(screen) : nullptr;
    if (visual == nullptr) {
        return;
    }

    // Call it only once per popup
    if (g_object_get_data(G_OBJECT(widget), kRadiusKey) != nullptr) {
        return;
    }

    gtk_widget_set_visual(widget, visual);
    gtk_widget_set_app_paintable(widget, TRUE);

    // `radius` is in DIPs. It is stored on the widget (and freed with it), and the draw handlers read it from there
    g_object_set_data_full(G_OBJECT(widget),
                           kRadiusKey,
                           new double(static_cast<double>(popup->FromDIP(static_cast<int>(radius)))),
                           FreeRadius);
    g_signal_connect(widget, "draw", G_CALLBACK(OnDrawBefore), nullptr);
    g_signal_connect_after(widget, "draw", G_CALLBACK(OnDrawAfter), nullptr);
#else
    wxUnusedVar(popup);
    wxUnusedVar(radius);
#endif
}
#endif
