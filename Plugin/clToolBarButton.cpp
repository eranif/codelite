#include "clToolBarButton.h"

clToolBarButton::clToolBarButton(clToolBar* parent, wxWindowID winid, const wxBitmap& bmp, const wxString& label)
    : clToolBarButtonBase(parent, winid, bmp, label)
{
}

clToolBarButton::~clToolBarButton() {}

wxSize clToolBarButton::CalculateSize(wxDC& dc) const
{
    wxSize sz;
    sz.x += CL_TOOL_BAR_X_MARGIN;
    sz.y = 2 * CL_TOOL_BAR_Y_MARGIN;

    if(m_bmp.IsOk()) {
        sz.x += m_bmp.GetScaledWidth();
        sz.x += CL_TOOL_BAR_X_MARGIN;

        int height = m_bmp.GetScaledHeight() + (2 * CL_TOOL_BAR_Y_MARGIN);
        sz.y = wxMax(sz.GetHeight(), height);
    }

    if(!m_label.IsEmpty() && m_toolbar->IsShowLabels()) {
        wxSize textSize = dc.GetTextExtent(m_label);
        sz.x += textSize.GetWidth();
        sz.x += CL_TOOL_BAR_X_MARGIN;

        int height = textSize.GetHeight() + (2 * CL_TOOL_BAR_Y_MARGIN);
        sz.y = wxMax(sz.GetHeight(), height);
    }
    return sz;
}
