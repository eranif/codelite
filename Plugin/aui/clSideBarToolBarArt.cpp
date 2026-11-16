#include "clSideBarToolBarArt.hpp"

#include "clSystemSettings.h"

#include <algorithm>
#include <memory>
#include <wx/aui/framemanager.h>
#include <wx/dcgraph.h>
#include <wx/graphics.h>

#if wxCHECK_VERSION(3, 3, 0)

namespace
{
// In DIPs.
constexpr int kMargin = 4;        // Between the icon and the edge of the highlight circle
constexpr int kLargeMargin = 5;   // The same, for a large toolbar
constexpr int kSeparatorSize = 8; // The gap between two groups of tools

bool IsDark() { return clSystemSettings::IsDark(); }

/// The colour of the bar: the same as the bar of the tabs
wxColour BarColour() { return clSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE); }

/// The size of `bmp` as the DC draws it. wxMSW draws a bitmap with its size in pixels, whatever its scale factor, the
/// other ports draw it with its logical size
wxSize DrawnSize(const wxBitmap& bmp)
{
#ifdef __WXMSW__
    return bmp.GetSize();
#else
    return bmp.GetLogicalSize();
#endif
}

/// The side of the (square) tool of an icon of `iconSize`: the highlight is a circle in the tool. Icons rarely fill
/// the corners of their bounding box, so the circle is sized from the longer side of the icon (not its diagonal),
/// with `margin` around it
int ToolSide(const wxSize& iconSize, int margin) { return std::max(iconSize.x, iconSize.y) + 2 * margin; }

/// `bar` made lighter on a dark theme and darker on a light one
wxColour Shade(const wxColour& bar, int darkLightness, int lightLightness)
{
    return bar.ChangeLightness(IsDark() ? darkLightness : lightLightness);
}

/// `over` on `under`, `alpha` of 0 to 255 is how much of `over` there is
wxColour Blend(const wxColour& under, const wxColour& over, int alpha)
{
    const auto mix = [alpha](unsigned char below, unsigned char above) {
        return static_cast<unsigned char>((above * alpha + below * (255 - alpha)) / 255);
    };
    return wxColour(mix(under.Red(), over.Red()), mix(under.Green(), over.Green()), mix(under.Blue(), over.Blue()));
}

/// A rounded rectangle with smooth edges when the platform can draw them. The colours can have alpha (when the
/// platform can draw it). `ring` is the colour of the outline, or not ok for none
void DrawRounded(wxDC& dc,
                 const wxRect& rect,
                 double radius,
                 const wxColour& fill,
                 const wxColour& ring = wxColour(),
                 double ringWidth = 1.0)
{
    std::unique_ptr<wxGraphicsContext> gc{wxGraphicsContext::CreateFromUnknownDC(dc)};
    if (gc) {
        gc->SetPen(ring.IsOk() ? gc->CreatePen(wxGraphicsPenInfo(ring).Width(ringWidth)) : wxNullGraphicsPen);
        gc->SetBrush(fill.IsOk() ? gc->CreateBrush(wxBrush(fill)) : wxNullGraphicsBrush);
        // The line is centred on the edge: keep it inside the rectangle
        const double half = ring.IsOk() ? ringWidth / 2.0 : 0.0;
        gc->DrawRoundedRectangle(rect.x + half,
                                 rect.y + half,
                                 rect.width - ringWidth * (ring.IsOk() ? 1 : 0),
                                 rect.height - ringWidth * (ring.IsOk() ? 1 : 0),
                                 radius);
        return;
    }
    dc.SetPen(ring.IsOk() ? wxPen(ring, static_cast<int>(ringWidth)) : *wxTRANSPARENT_PEN);
    dc.SetBrush(fill.IsOk() ? wxBrush(fill) : *wxTRANSPARENT_BRUSH);
    dc.DrawRoundedRectangle(rect, radius);
}

/// A circle that looks pushed into the bar: its fill is darker than the bar, with a 1 pixel ring that is lighter
/// than the fill. `deeper` is for a button that is also being pressed
void DrawSunkCircle(wxDC& dc, const wxRect& rect, const wxColour& bar, bool deeper)
{
    const wxColour fill = bar.ChangeLightness(IsDark() ? (deeper ? 72 : 80) : (deeper ? 80 : 88));
    const wxColour ring = fill.ChangeLightness(125);
    DrawRounded(dc, rect, std::min(rect.width, rect.height) / 2.0, fill, ring, 1.0);
}

wxColour Accent() { return clSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT); }

/// `colour` with an alpha of 0 to 255
wxColour WithAlpha(const wxColour& colour, int alpha)
{
    return wxColour(colour.Red(), colour.Green(), colour.Blue(), static_cast<unsigned char>(alpha));
}

} // namespace

clSideBarToolBarArt::clSideBarToolBarArt(Style style, bool large)
    : m_style(style)
    , m_large(large)
{
    m_separatorSize = kSeparatorSize;
}

void clSideBarToolBarArt::DrawBackground(wxDC& dc, wxWindow* wnd, const wxRect& rect)
{
    wxUnusedVar(wnd);
    const wxColour startColour = BarColour();
    const wxColour endColour = startColour.ChangeLightness(IsDark() ? 109 : 95);

    // A gradient of the bar colour across the bar: from the left to the right in a vertical bar, from the top to
    // the bottom in a horizontal one. It starts a little lighter and ends a little darker
    const bool vertical = (m_flags & wxAUI_TB_VERTICAL) != 0;
    dc.GradientFillLinear(rect, endColour, startColour, vertical ? wxEAST : wxSOUTH);
    dc.SetPen(*wxTRANSPARENT_PEN);
}

void clSideBarToolBarArt::DrawPlainBackground(wxDC& dc, wxWindow* wnd, const wxRect& rect)
{
    DrawBackground(dc, wnd, rect);
}

void clSideBarToolBarArt::DrawHighlight(
    wxDC& dc, wxWindow* wnd, const wxRect& box, bool hover, bool pressed, bool checked) const
{
    const wxColour bar = BarColour();
    const wxColour accent = Accent();
    // Every style draws its highlight as a circle in the middle of the tool (which is the circle's bounding square)
    const int side = std::min(box.width, box.height);
    const wxRect circle(box.x + (box.width - side) / 2, box.y + (box.height - side) / 2, side, side);
    const double radius = side / 2.0;
    const double ringWidth = std::max(1.0, wnd->FromDIP(1) * 1.5);

    switch (m_style) {
    case Style::Soft: {
        wxColour fill;
        if (pressed) {
            fill = checked ? Blend(bar, accent, 110) : Shade(bar, 145, 82);
        } else if (checked) {
            fill = Blend(bar, accent, hover ? 85 : 60);
        } else {
            fill = Shade(bar, 125, 92);
        }
        DrawRounded(dc, circle, radius, fill);
        break;
    }
    case Style::Accent: {
        // The tool is the accent colour, pressed is the strongest
        const int alpha = pressed ? 150 : checked ? 115 : 75;
        DrawRounded(dc, circle, radius, Blend(bar, accent, alpha));
        break;
    }
    case Style::Outline: {
        const wxColour ring = pressed ? accent : WithAlpha(accent, hover ? 230 : 170);
        const wxColour fill =
            checked ? Blend(bar, accent, pressed ? 80 : 45) : (pressed ? Shade(bar, 125, 92) : wxColour());
        DrawRounded(dc, circle, radius, fill, ring, ringWidth);
        break;
    }
    case Style::Indicator: {
        // A tint, a checked tool has the accent bar on the outer edge of the tool (the left of a vertical bar, the
        // top of a horizontal one)
        wxColour fill = pressed ? Shade(bar, 145, 82) : Shade(bar, 125, 92);
        if (checked && !pressed) {
            fill = Blend(bar, accent, hover ? 55 : 35);
        }
        DrawRounded(dc, circle, radius, fill);
        if (checked) {
            const int thickness = std::max(2, wnd->FromDIP(3));
            dc.SetPen(*wxTRANSPARENT_PEN);
            dc.SetBrush(wxBrush(accent));
            if (m_flags & wxAUI_TB_VERTICAL) {
                dc.DrawRectangle(box.x, box.y, thickness, box.height);
            } else {
                dc.DrawRectangle(box.x, box.y, box.width, thickness);
            }
        }
        break;
    }
    case Style::Circle: {
        // A checked (or pressed) tool looks pushed in. Hover is a plain raised tint
        if (checked || pressed) {
            DrawSunkCircle(dc, circle, bar, checked && pressed);
        } else {
            DrawRounded(dc, circle, radius, Shade(bar, 115, 92));
        }
        break;
    }
    }
}

void clSideBarToolBarArt::DrawToolHighlight(wxDC& dc, wxWindow* wnd, const wxAuiToolBarItem& item, const wxRect& rect)
{
    const int state = item.GetState();
    if (state & wxAUI_BUTTON_STATE_DISABLED) {
        return;
    }
    const bool pressed = (state & wxAUI_BUTTON_STATE_PRESSED) != 0;
    const bool hover = (state & wxAUI_BUTTON_STATE_HOVER) != 0;
    const bool checked = (state & wxAUI_BUTTON_STATE_CHECKED) != 0 || item.IsSticky();
    if (!pressed && !hover && !checked) {
        return;
    }
    // The tool is sized for the highlight (see GetToolSize()): the circle is centered on the icon
    DrawHighlight(dc, wnd, rect, hover, pressed, checked);
}

void clSideBarToolBarArt::DrawButton(wxDC& dc, wxWindow* wnd, const wxAuiToolBarItem& item, const wxRect& rect)
{
    // A tool with a label is rare here: the generic art does the layout of the text
    if (m_flags & wxAUI_TB_TEXT) {
        wxAuiGenericToolBarArt::DrawButton(dc, wnd, item, rect);
        return;
    }

    DrawToolHighlight(dc, wnd, item, rect);

    const wxBitmap bmp = item.GetCurrentBitmapFor(wnd);
    if (bmp.IsOk()) {
        const wxSize size = DrawnSize(bmp);
        dc.DrawBitmap(bmp, rect.x + (rect.width - size.x) / 2, rect.y + (rect.height - size.y) / 2, true);
    }
}

void clSideBarToolBarArt::DrawDropDownButton(wxDC& dc, wxWindow* wnd, const wxAuiToolBarItem& item, const wxRect& rect)
{
    if (m_flags & wxAUI_TB_TEXT) {
        wxAuiGenericToolBarArt::DrawDropDownButton(dc, wnd, item, rect);
        return;
    }

    // The room of the arrow: at the end of a horizontal bar, under the icon in a vertical one. The rest is the
    // button, with the highlight and the icon
    const wxBitmap& arrow = (item.GetState() & wxAUI_BUTTON_STATE_DISABLED)
                                ? m_disabledButtonDropDownBmp.GetBitmapFor(wnd)
                                : m_buttonDropDownBmp.GetBitmapFor(wnd);
    const wxSize arrowSize = arrow.IsOk() ? DrawnSize(arrow) : wxSize();
    const bool vertical = (m_flags & wxAUI_TB_VERTICAL) != 0;
    const int room = (vertical ? arrowSize.y : arrowSize.x) + wnd->FromDIP(4);
    wxRect button = rect;
    wxRect arrowRect = rect;
    if (vertical) {
        button.height -= room;
        arrowRect.y = button.GetBottom() + 1;
        arrowRect.height = room;
    } else {
        button.width -= room;
        arrowRect.x = button.GetRight() + 1;
        arrowRect.width = room;
    }

    DrawToolHighlight(dc, wnd, item, button);

    const wxBitmap bmp = item.GetCurrentBitmapFor(wnd);
    if (bmp.IsOk()) {
        const wxSize size = DrawnSize(bmp);
        dc.DrawBitmap(bmp, button.x + (button.width - size.x) / 2, button.y + (button.height - size.y) / 2, true);
    }
    if (arrow.IsOk()) {
        dc.DrawBitmap(arrow,
                      arrowRect.x + (arrowRect.width - arrowSize.x) / 2,
                      arrowRect.y + (arrowRect.height - arrowSize.y) / 2,
                      true);
    }
}

void clSideBarToolBarArt::DrawSeparator(wxDC& WXUNUSED(dc), wxWindow* WXUNUSED(wnd), const wxRect& WXUNUSED(rect))
{
    // The separator is only a gap
}

void clSideBarToolBarArt::DrawGripper(wxDC& WXUNUSED(dc), wxWindow* WXUNUSED(wnd), const wxRect& WXUNUSED(rect))
{
    // The bar is fixed: no gripper
}

wxSize clSideBarToolBarArt::GetToolSize(wxReadOnlyDC& dc, wxWindow* wnd, const wxAuiToolBarItem& item)
{
    // A tool with a label is laid out by the generic art
    if (m_flags & wxAUI_TB_TEXT) {
        return wxAuiGenericToolBarArt::GetToolSize(dc, wnd, item);
    }

    // The tool is a square that holds the highlight circle, with room around the corners of the icon, so the
    // circle is centered on the icon and never cuts it
    const wxBitmap bmp = item.GetCurrentBitmapFor(wnd);
    if (!bmp.IsOk()) {
        return wxAuiGenericToolBarArt::GetToolSize(dc, wnd, item);
    }
    const int side = ToolSide(DrawnSize(bmp), wnd->FromDIP(m_large ? kLargeMargin : kMargin));
    wxSize size(side, side);

    if (item.HasDropDown()) {
        // The room of the arrow (see DrawDropDownButton()): at the end of a horizontal bar, under the icon in a
        // vertical one
        const wxBitmap& arrow = m_buttonDropDownBmp.GetBitmapFor(wnd);
        const wxSize arrowSize = arrow.IsOk() ? DrawnSize(arrow) : wxSize();
        if (m_flags & wxAUI_TB_VERTICAL) {
            size.y += arrowSize.y + wnd->FromDIP(4);
        } else {
            size.x += arrowSize.x + wnd->FromDIP(4);
        }
    }
    return size;
}

#endif // wxCHECK_VERSION(3, 3, 0)
