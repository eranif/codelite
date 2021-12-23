#include "clHeaderItem.h"

#include "clScrolledPanel.h"

#include <wx/dc.h>
#include <wx/headercol.h>
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
        wxRendererNative::Get().DrawHeaderButton(m_parent, dc, m_rect, 0);

    } else {
        dc.SetBrush(colours.GetHeaderBgColour());
        dc.SetPen(colours.GetHeaderBgColour());
        dc.DrawRectangle(m_rect);
    }

    dc.SetTextForeground(flags & kHeaderNative ? wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT)
                                               : colours.GetItemTextColour());
    dc.DrawText(GetLabel(), wxPoint(m_rect.GetX() + X_SPACER, textY));
}

void clHeaderItem::SetWidthValue(int width)
{
    switch(width) {
    case wxCOL_WIDTH_AUTOSIZE:
        EnableFlag(kHeaderColWidthMask, false);
        EnableFlag(kHeaderColWidthFitData, true);
        return;
    case wxCOL_WIDTH_DEFAULT:
        EnableFlag(kHeaderColWidthMask, false);
        EnableFlag(kHeaderColWidthFitHeader, true);
        return;
    default:
        if(width < 0) {
            return;
        }
        EnableFlag(kHeaderColWidthMask, false);
        EnableFlag(kHeaderColWidthUser, true);
        break;
    }
    UpdateWidth(width);
}

void clHeaderItem::UpdateWidth(int width)
{
    if(width >= 0) {
        m_rect.SetWidth(width);
    }
}
