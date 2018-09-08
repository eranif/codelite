#include "clHeaderItem.h"
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
    wxSize textSize = dc.GetTextExtent(GetLabel());
    int textY = m_rect.GetY() + (m_rect.GetHeight() - textSize.GetHeight()) / 2;

    dc.SetBrush(colours.GetBgColour());
    dc.SetPen(colours.GetBgColour());
    dc.SetTextForeground(colours.GetItemTextColour());
    dc.DrawRectangle(m_rect);
    dc.DrawText(GetLabel(), wxPoint(m_rect.GetX() + X_SPACER, textY));
}
