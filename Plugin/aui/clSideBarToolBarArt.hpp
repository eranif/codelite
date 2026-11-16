#pragma once

#include "codelite_exports.h"

#include <wx/aui/auibar.h>

/// The look of the tools of the toolbars of the side bars (copied from Kennel). The bar has the colour of the
/// window, a tool gets a round highlight on hover and while pressed, and a tool that is checked is marked with the
/// accent colour of the system. There are no borders and no gripper, and the bar is a soft gradient across its width.
///
/// A tool is a square that is sized from its icon, so the highlight circle is centered on the icon and has room
/// around its corners. A separator is only a gap.
///
/// All the colours are taken from the system colours and the appearance (dark or light) when something is drawn, so
/// a toolbar follows a change of the theme without a new art. Use it with SetArtProvider(), the toolbar takes the art.
class WXDLLIMPEXP_SDK clSideBarToolBarArt : public wxAuiGenericToolBarArt
{
public:
    enum class Style {
        Soft,      // A tint of the bar colour (lighter or darker)
        Accent,    // A fill with the accent colour, stronger when pressed
        Outline,   // A ring of the accent colour, with a soft fill when checked
        Indicator, // A tint, and an accent bar on the outer edge when checked
        Circle,    // A round tint, with a ring of the accent colour when checked
    };

    /// `large`: bigger tools (more room around the icons)
    explicit clSideBarToolBarArt(Style style = Style::Circle, bool large = false);

    wxAuiToolBarArt* Clone() override { return new clSideBarToolBarArt(*this); }

    void DrawBackground(wxDC& dc, wxWindow* wnd, const wxRect& rect) override;
    void DrawPlainBackground(wxDC& dc, wxWindow* wnd, const wxRect& rect) override;
    void DrawButton(wxDC& dc, wxWindow* wnd, const wxAuiToolBarItem& item, const wxRect& rect) override;
    /// The arrow of the menu is at the end of a horizontal bar, and under the icon in a vertical one
    void DrawDropDownButton(wxDC& dc, wxWindow* wnd, const wxAuiToolBarItem& item, const wxRect& rect) override;
    void DrawSeparator(wxDC& dc, wxWindow* wnd, const wxRect& rect) override;
    void DrawGripper(wxDC& dc, wxWindow* wnd, const wxRect& rect) override;
    /// A tool is a little bigger than its image: the highlight needs room around it
    wxSize GetToolSize(wxReadOnlyDC& dc, wxWindow* wnd, const wxAuiToolBarItem& item) override;

private:
    /// The highlight of a tool that has the mouse over it, is pressed or is checked, in `box`
    void DrawHighlight(wxDC& dc, wxWindow* wnd, const wxRect& box, bool hover, bool pressed, bool checked) const;
    /// The highlight of a tool in `rect`, when it needs one
    void DrawToolHighlight(wxDC& dc, wxWindow* wnd, const wxAuiToolBarItem& item, const wxRect& rect);

    Style m_style;
    bool m_large;
};
