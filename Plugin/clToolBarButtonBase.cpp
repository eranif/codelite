#include "clToolBarButtonBase.h"
#include <wx/settings.h>

#ifdef __WXGTK20__
// We need this ugly hack to workaround a gtk2-wxGTK name-clash^M
// See http://trac.wxwidgets.org/ticket/10883^M
#define GSocket GlibGSocket
#include <gtk/gtk.h>
#undef GSocket
#endif

//----------------------------------------------------------
// Helper methods
//----------------------------------------------------------
static void RGBtoHSB(int r, int g, int b, float* h, float* s, float* br)
{
    float hue, saturation, brightness;
    int cmax = (r > g) ? r : g;
    if(b > cmax) cmax = b;
    int cmin = (r < g) ? r : g;
    if(b < cmin) cmin = b;

    brightness = ((float)cmax) / 255.0f;
    if(cmax != 0)
        saturation = ((float)(cmax - cmin)) / ((float)cmax);
    else
        saturation = 0;
    if(saturation == 0)
        hue = 0;
    else {
        float redc = ((float)(cmax - r)) / ((float)(cmax - cmin));
        float greenc = ((float)(cmax - g)) / ((float)(cmax - cmin));
        float bluec = ((float)(cmax - b)) / ((float)(cmax - cmin));
        if(r == cmax)
            hue = bluec - greenc;
        else if(g == cmax)
            hue = 2.0f + redc - bluec;
        else
            hue = 4.0f + greenc - redc;
        hue = hue / 6.0f;
        if(hue < 0) hue = hue + 1.0f;
    }
    (*h) = hue;
    (*s) = saturation;
    (*br) = brightness;
}

static bool IsDark(const wxColour& color)
{
    float h, s, b;
    RGBtoHSB(color.Red(), color.Green(), color.Blue(), &h, &s, &b);
    return (b < 0.5);
}

static wxColour GetMenuBarBgColour()
{
#if defined(__WXGTK__) && !defined(__WXGTK3__)
    static bool intitialized(false);
    // initialise default colour
    static wxColour bgColour(wxSystemSettings::GetColour(wxSYS_COLOUR_MENUBAR));

    if(!intitialized) {
        // try to get the background colour from a menu
        GtkWidget* menuBar = gtk_menu_bar_new();
        bgColour = GtkGetBgColourFromWidget(menuBar, bgColour);
        intitialized = true;
    }
    return bgColour;
#elif defined(__WXOSX__)
    return wxColour("rgb(209, 209, 209)");
#else
    return wxSystemSettings::GetColour(wxSYS_COLOUR_MENUBAR);
#endif
}

#ifdef __WXGTK__
static wxColour GtkGetBgColourFromWidget(GtkWidget* widget, const wxColour& defaultColour)
{
    wxColour bgColour = defaultColour;
    GtkStyle* def = gtk_rc_get_style(widget);
    if(!def) { def = gtk_widget_get_default_style(); }

    if(def) {
        GdkColor col = def->bg[GTK_STATE_NORMAL];
        bgColour = wxColour(col);
    }
    gtk_widget_destroy(widget);
    return bgColour;
}

static wxColour GtkGetTextColourFromWidget(GtkWidget* widget, const wxColour& defaultColour)
{
    wxColour textColour = defaultColour;
    GtkStyle* def = gtk_rc_get_style(widget);
    if(!def) { def = gtk_widget_get_default_style(); }

    if(def) {
        GdkColor col = def->fg[GTK_STATE_NORMAL];
        textColour = wxColour(col);
    }
    gtk_widget_destroy(widget);
    return textColour;
}
#endif

#ifdef __WXOSX__
double wxOSXGetMainScreenContentScaleFactor();
#endif

static wxBitmap CreateGrayBitmap(const wxBitmap& bmp)
{
    wxImage img = bmp.ConvertToImage();
    img = img.ConvertToGreyscale();
#ifdef __WXOSX__
    double scale = 1.0;
    if(wxOSXGetMainScreenContentScaleFactor() > 1.9) {
        scale = 2.0;
    }
    wxBitmap greyBmp(img, -1, scale);
#else
    wxBitmap greyBmp(img);
#endif
    return greyBmp;
}

static wxBitmap MakeDisabledBitmap(const wxBitmap& bmp)
{
#ifdef __WXOSX__
    return CreateGrayBitmap(bmp);
#else
    bool bDarkBG = IsDark(GetMenuBarBgColour());
    wxImage img = bmp.ConvertToImage();
    img = img.ConvertToGreyscale();
    wxBitmap greyBmp(img);
    if(bDarkBG) {
        return greyBmp.ConvertToDisabled(20);

    } else {
        return greyBmp.ConvertToDisabled(255);
    }
#endif
}

// -----------------------------------------------
// Button base
// -----------------------------------------------
clToolBarButtonBase::clToolBarButtonBase(clToolBar* parent, wxWindowID id, const wxBitmap& bmp, const wxString& label,
                                         size_t flags)
    : m_toolbar(parent)
    , m_id(id)
    , m_bmp(bmp)
    , m_label(label)
    , m_flags(flags)
    , m_renderFlags(0)
{
}

clToolBarButtonBase::~clToolBarButtonBase() {}

void clToolBarButtonBase::FillMenuBarBgColour(wxDC& dc, const wxRect& rect)
{
#ifdef __WXOSX__
    wxColour startColour("rgb(231, 229, 231)");
    wxColour endColour("rgb(180, 180, 180)");
    dc.GradientFillLinear(rect, startColour, endColour, wxSOUTH);
#else
    dc.SetPen(GetMenuBarBgColour());
    dc.SetBrush(GetMenuBarBgColour());
    dc.DrawRectangle(rect);
#endif
}

void clToolBarButtonBase::Render(wxDC& dc, const wxRect& rect)
{
    m_dropDownArrowRect = wxRect();
    m_buttonRect = rect;

    wxColour penColour;
    wxColour bgColour;
    wxColour textColour = wxSystemSettings::GetColour(wxSYS_COLOUR_MENUTEXT);

#if defined(__WXMSW__) || defined(__WXOSX__)
    wxColour bgHighlightColour("rgb(153, 209, 255)");
    penColour = bgHighlightColour;
#else
    wxColour bgHighlightColour(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT));
#endif
    FillMenuBarBgColour(dc, rect);

    if(IsEnabled() && (IsHover() || IsPressed() || IsChecked())) {
        penColour = bgHighlightColour;
        if(IsHover() || IsChecked()) {
            bgColour = bgHighlightColour;
        } else {
            bgColour = penColour;
        }
        dc.SetBrush(bgColour);
        dc.SetPen(penColour);
        dc.DrawRectangle(rect);
        textColour = wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT);
    } else if(!IsEnabled()) {
        // A disabled button
        textColour = wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT);
    }
    wxCoord xx = rect.GetX();
    wxCoord yy = 0;
    xx += CL_TOOL_BAR_X_MARGIN;

    if(m_bmp.IsOk()) {
        wxBitmap bmp(m_bmp);
        if(!IsEnabled()) {
            bmp = MakeDisabledBitmap(m_bmp);
        }
        yy = (rect.GetHeight() - bmp.GetScaledHeight()) / 2 + rect.GetY();
        dc.DrawBitmap(bmp, wxPoint(xx, yy));
        xx += bmp.GetScaledWidth();
        xx += CL_TOOL_BAR_X_MARGIN;
    }

    if(!m_label.IsEmpty() && m_toolbar->IsShowLabels()) {
        dc.SetTextForeground(textColour);
        wxSize sz = dc.GetTextExtent(m_label);
        yy = (rect.GetHeight() - sz.GetHeight()) / 2 + rect.GetY();
        dc.DrawText(m_label, wxPoint(xx, yy));
        xx += sz.GetWidth();
        xx += CL_TOOL_BAR_X_MARGIN;
    }

    // Do we need to draw a drop down arrow?
    if(HasMenu()) {
        // draw a drop down menu
        m_dropDownArrowRect =
            wxRect(xx, rect.GetY(), (2 * CL_TOOL_BAR_X_MARGIN) + CL_TOOL_BAR_DROPDOWN_ARROW_SIZE, rect.GetHeight());
        if(IsPressed() || IsHover()) {
            dc.DrawLine(wxPoint(xx, rect.GetY()), wxPoint(xx, rect.GetY() + rect.GetHeight()));
        }
        xx += CL_TOOL_BAR_X_MARGIN;

        wxPoint points[3];
        points[0].x = xx;
        points[0].y = (rect.GetHeight() - CL_TOOL_BAR_DROPDOWN_ARROW_SIZE) / 2 + rect.GetY();

        points[1].x = xx + CL_TOOL_BAR_DROPDOWN_ARROW_SIZE;
        points[1].y = points[0].y;

        points[2].x = xx + (CL_TOOL_BAR_DROPDOWN_ARROW_SIZE / 2);
        points[2].y = points[0].y + CL_TOOL_BAR_DROPDOWN_ARROW_SIZE;
        dc.SetPen(textColour);
        dc.SetBrush(textColour);
        dc.DrawPolygon(3, points);

        xx += CL_TOOL_BAR_DROPDOWN_ARROW_SIZE;
        xx += CL_TOOL_BAR_X_MARGIN;
    }
}
