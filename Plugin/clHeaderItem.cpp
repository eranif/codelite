#include "clHeaderItem.h"
#include "clScrolledPanel.h"
#include <wx/dc.h>
#include <wx/renderer.h>
#include <wx/settings.h>

clHeaderItem::clHeaderItem(wxWindow* parent, const wxString& label, const wxBitmap& bmp)
    : m_label(label)
    , m_bitmap(bmp)
    , m_parent(parent)
{
}
clHeaderItem::clHeaderItem() {}

clHeaderItem::~clHeaderItem() {}

void clHeaderItem::Render(wxDC& dc, const clColours& colours, int flags)
{
    dc.SetFont(clScrolledPanel::GetDefaultFont());
    wxSize textSize = dc.GetTextExtent(GetLabel());
    int textY = m_rect.GetY() + (m_rect.GetHeight() - textSize.GetHeight()) / 2;

    if(flags & kHeaderNative) {
        wxRendererNative::Get().DrawHeaderButton(m_parent, dc, m_rect, wxCONTROL_NONE);

    } else {
        dc.SetBrush(colours.GetHeaderBgColour());
        dc.SetPen(colours.GetHeaderBgColour());
        dc.DrawRectangle(m_rect);
    }

    dc.SetTextForeground(flags & kHeaderNative ? wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT)
                                               : colours.GetItemTextColour());
    dc.DrawText(GetLabel(), wxPoint(m_rect.GetX() + X_SPACER, textY));
}
