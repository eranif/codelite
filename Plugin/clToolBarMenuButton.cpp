#include "clToolBarMenuButton.h"

clToolBarMenuButton::clToolBarMenuButton(clToolBar* parent, wxWindowID winid, size_t bmpId, const wxString& label)
    : clToolBarButtonBase(parent, winid, bmpId, label, kHasMenu)
{
}

clToolBarMenuButton::~clToolBarMenuButton() {}

wxSize clToolBarMenuButton::CalculateSize(wxDC& dc) const
{
    wxSize sz;
    sz.x += m_toolbar->GetXSpacer();
    sz.y = 2 * m_toolbar->GetYSpacer();

    const wxBitmap& m_bmp = GetBitmap();
    if(m_bmp.IsOk()) {
        sz.x += m_bmp.GetScaledWidth();
        sz.x += m_toolbar->GetXSpacer();

        int height = m_bmp.GetScaledHeight() + (2 * m_toolbar->GetYSpacer());
        sz.y = wxMax(sz.GetHeight(), height);
    }

    if(!m_label.IsEmpty() && m_toolbar->IsShowLabels()) {
        wxSize textSize = dc.GetTextExtent(m_label);
        sz.x += textSize.GetWidth();
        sz.x += m_toolbar->GetXSpacer();

        int height = textSize.GetHeight() + (2 * m_toolbar->GetYSpacer());
        sz.y = wxMax(sz.GetHeight(), height);
    }

    sz.x += m_toolbar->GetXSpacer();
    sz.x += CL_TOOL_BAR_DROPDOWN_ARROW_SIZE;
    sz.x += m_toolbar->GetXSpacer();
    return sz;
}
