#include "clToolBarButtonBase.h"
#include "drawingutils.h"

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
    , m_menu(nullptr)
{
}

clToolBarButtonBase::~clToolBarButtonBase() { wxDELETE(m_menu); }

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
    if(DrawingUtils::IsDark(bgHighlightColour)) { bgHighlightColour = bgHighlightColour.ChangeLightness(140); }
#endif
    if(IsEnabled() && (IsHover() || IsPressed() || IsChecked())) {
        wxRect highlightRect = rect;
        highlightRect.Deflate(0, 1);
        penColour = bgHighlightColour;
        if(IsHover() || IsChecked()) {
            bgColour = bgHighlightColour;
        } else if(IsPressed()) {
            bgColour = bgHighlightColour.ChangeLightness(90);
            penColour = bgColour;
        } else {
            bgColour = penColour;
        }
        dc.SetBrush(bgColour);
        dc.SetPen(penColour);
        dc.DrawRectangle(highlightRect);
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
        if(!IsEnabled()) { bmp = DrawingUtils::CreateDisabledBitmap(m_bmp); }
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
    if(IsMenuButton()) {
        // draw a drop down menu
        m_dropDownArrowRect =
            wxRect(xx, rect.GetY(), (2 * CL_TOOL_BAR_X_MARGIN) + CL_TOOL_BAR_DROPDOWN_ARROW_SIZE, rect.GetHeight());
        if((IsPressed() || IsHover()) && IsEnabled()) {
            dc.DrawLine(wxPoint(xx, rect.GetY() + 2), wxPoint(xx, rect.GetY() + rect.GetHeight() - 2));
        }
        xx += CL_TOOL_BAR_X_MARGIN;
        DrawingUtils::DrawDropDownArrow(nullptr, dc, m_dropDownArrowRect, textColour);
        xx += CL_TOOL_BAR_DROPDOWN_ARROW_SIZE;
        xx += CL_TOOL_BAR_X_MARGIN;
    }
}

void clToolBarButtonBase::SetMenu(wxMenu* menu)
{
    wxDELETE(m_menu);
    m_menu = menu;
}
