#include "clControlWithItems.h"
#include "clHeaderBar.h"
#include "clScrolledPanel.h"
#include <wx/dcbuffer.h>
#include <wx/dcgraph.h>
#include <wx/dcmemory.h>
#include <wx/font.h>
#include <wx/log.h>
#include <wx/renderer.h>
#include <wx/settings.h>

#ifdef __WXMSW__
#define PEN_STYLE wxPENSTYLE_SHORT_DASH
#else
#define PEN_STYLE wxPENSTYLE_DOT
#endif

clHeaderBar::clHeaderBar(clControlWithItems* parent, const clColours& colours)
    : m_colours(colours)
{
    Hide();
    SetBackgroundStyle(wxBG_STYLE_CUSTOM);
    if(!wxPanel::Create(parent)) { return; }
    Bind(wxEVT_PAINT, &clHeaderBar::OnPaint, this);
    GetParent()->Bind(wxEVT_SIZE, &clHeaderBar::OnSize, this);
    Bind(wxEVT_ERASE_BACKGROUND, [](wxEraseEvent& event) { wxUnusedVar(event); });
}

clHeaderBar::~clHeaderBar() {}

size_t clHeaderBar::GetHeight() const
{
    if(m_columns.empty()) { return 0; }
    return m_columns[0].GetRect().GetHeight();
}

void clHeaderBar::OnSize(wxSizeEvent& event)
{
    event.Skip();
    if(!GetParent()) { return; }
    clControlWithItems* parent = dynamic_cast<clControlWithItems*>(GetParent());
    int width = parent->GetSize().GetWidth();
    SetSize(width, GetHeight());
    Move(0, 0);
    Refresh();
}

void clHeaderBar::push_back(const clHeaderItem& item)
{
    m_columns.push_back(item);
    DoUpdateSize();
}

void clHeaderBar::DoUpdateSize()
{
    wxSize fixedText = GetTextSize("Tp");
    int xx = 0;
    for(size_t i = 0; i < m_columns.size(); ++i) {
        clHeaderItem& item = m_columns[i];
        wxSize textSize = GetTextSize(item.GetLabel());
        wxRect headerRect(xx, 0, textSize.GetWidth() + (2 * clHeaderItem::X_SPACER),
                          fixedText.GetHeight() + (2 * clHeaderItem::Y_SPACER));
        item.SetRect(headerRect);
        xx += item.GetRect().GetWidth();
    }
}

wxSize clHeaderBar::GetTextSize(const wxString& label) const
{
    wxBitmap bmp(1, 1);
    wxMemoryDC memDC(bmp);
    wxGCDC gcdc(memDC);
    wxFont font = clScrolledPanel::GetDefaultFont();
    gcdc.SetFont(font);
    wxSize textSize = gcdc.GetTextExtent(label);
    return textSize;
}

void clHeaderBar::Render(wxDC& dc, const clColours& colours)
{
    const wxRect rect = GetClientRect();
    dc.SetPen(colours.GetHeaderBgColour());
    dc.SetBrush(colours.GetHeaderBgColour());
    dc.DrawRectangle(rect);

    clColours _colours = colours;
    _colours.SetBgColour(_colours.GetHeaderBgColour());

    bool useNativeHeader = (m_flags & kHeaderNative);
    if(useNativeHeader) { wxRendererNative::Get().DrawHeaderButton(m_parent, dc, rect, wxCONTROL_NONE); }

    // Set the DC origin to reflect the h-scrollbar
    clControlWithItems* parent = dynamic_cast<clControlWithItems*>(GetParent());
    dc.SetDeviceOrigin(-parent->GetFirstColumn(), 0);
    for(size_t i = 0; i < size(); ++i) {
        bool is_last = (i == (size() - 1));
        Item(i).Render(dc, _colours, m_flags);
        if(!is_last && !useNativeHeader) {
            dc.SetPen(wxPen(_colours.GetHeaderVBorderColour(), 1, PEN_STYLE));
            dc.DrawLine(Item(i).GetRect().GetTopRight(), Item(i).GetRect().GetBottomRight());
        }
    }

    // Restore the DC origin
    dc.SetDeviceOrigin(0, 0);
    if(!useNativeHeader) {
        dc.SetPen(_colours.GetHeaderHBorderColour());
        dc.DrawLine(rect.GetBottomLeft(), rect.GetBottomRight());
    }
}

void clHeaderBar::UpdateColWidthIfNeeded(size_t col, size_t width, bool force)
{
    if(col >= m_columns.size()) { return; }
    clHeaderItem& column = m_columns[col];
    column.SetWidth(force ? width : wxMax(column.GetWidth(), width));

    // Update the offsets
    int xx = 0;
    for(size_t i = 0; i < m_columns.size(); ++i) {
        clHeaderItem& column = m_columns[i];
        column.SetX(xx);
        xx += column.GetWidth();
    }
}

size_t clHeaderBar::GetWidth() const
{
    size_t w = 0;
    for(size_t i = 0; i < m_columns.size(); ++i) {
        w += m_columns[i].GetWidth();
    }
    return w;
}

const clHeaderItem& clHeaderBar::Last() const
{
    if(IsEmpty()) {
        static clHeaderItem emptyItem;
        return emptyItem;
    }
    return m_columns.back();
}

clHeaderItem& clHeaderBar::Last()
{
    if(IsEmpty()) {
        static clHeaderItem emptyItem;
        return emptyItem;
    }
    return m_columns.back();
}

void clHeaderBar::Clear() { m_columns.clear(); }

void clHeaderBar::OnPaint(wxPaintEvent& event)
{
    wxUnusedVar(event);
    wxAutoBufferedPaintDC dc(this);
    PrepareDC(dc);
    wxGCDC gcdc(dc);
    Render(gcdc, m_colours);
}

bool clHeaderBar::Show(bool show)
{
    if(!GetParent()) { return false; }
    SetSize(GetParent()->GetSize().GetWidth(), GetHeight());
    return wxPanel::Show(show);
}
