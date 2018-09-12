#include "clHeaderItem.h"
#include "clScrolledPanel.h"
#include <wx/dc.h>

clHeaderItem::clHeaderItem(const wxString& label, const wxBitmap& bmp)
    : m_label(label)
    , m_bitmap(bmp)
{
}
clHeaderItem::clHeaderItem() {}

clHeaderItem::~clHeaderItem() {}

void clHeaderItem::Render(wxDC& dc, const clColours& colours)
{
    dc.SetFont(clScrolledPanel::GetDefaultFont());
    wxSize textSize = dc.GetTextExtent(GetLabel());
    int textY = m_rect.GetY() + (m_rect.GetHeight() - textSize.GetHeight()) / 2;

    dc.SetBrush(colours.GetHeaderBgColour());
    dc.SetPen(colours.GetHeaderBgColour());
    dc.DrawRectangle(m_rect);

    dc.SetTextForeground(colours.GetItemTextColour());
    dc.DrawText(GetLabel(), wxPoint(m_rect.GetX() + X_SPACER, textY));
}
