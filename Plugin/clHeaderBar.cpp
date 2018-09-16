#include "clHeaderBar.h"
#include "clScrolledPanel.h"
#include <wx/dcmemory.h>
#include <wx/font.h>
#include <wx/renderer.h>
#include <wx/settings.h>

#ifdef __WXMSW__
#define PEN_STYLE wxPENSTYLE_SHORT_DASH
#else
#define PEN_STYLE wxPENSTYLE_DOT
#endif

clHeaderBar::clHeaderBar(wxWindow* parent)
    : m_parent(parent)
{
}

clHeaderBar::~clHeaderBar() {}

size_t clHeaderBar::GetHeight() const
{
    if(m_hideHeaders || m_columns.empty()) { return 0; }
    return m_columns[0].GetRect().GetHeight();
}

void clHeaderBar::push_back(const clHeaderItem& item)
{
    m_columns.push_back(item);
    DoUpdateSize();
}

void clHeaderBar::DoUpdateSize()
{
    wxSize fixedText = GetTextSize("Tp");
    int xx = m_firstColumn;
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

void clHeaderBar::Render(wxDC& dc, const wxRect& rect, const clColours& colours)
{
    wxColour barBgColour = colours.GetBgColour().ChangeLightness(140);
    clColours _colours = colours;
    _colours.SetBgColour(_colours.GetHeaderBgColour());

    bool useNativeHeader = m_flags & kHeaderNative;
    if(useNativeHeader) {
        wxRendererNative::Get().DrawHeaderButton(m_parent, dc, rect, wxCONTROL_NONE);
    } else {
        dc.SetPen(_colours.GetBgColour());
        dc.SetBrush(_colours.GetBgColour());
        dc.DrawRectangle(rect);
    }
    for(size_t i = 0; i < size(); ++i) {
        bool is_last = (i == (size() - 1));
        Item(i).Render(dc, _colours, m_flags);
        if(!is_last && !useNativeHeader) {
            dc.SetPen(wxPen(_colours.GetHeaderVBorderColour(), 1, PEN_STYLE));
            dc.DrawLine(Item(i).GetRect().GetTopRight(), Item(i).GetRect().GetBottomRight());
        }
    }

    if(!useNativeHeader) {
        // The horizontal header line should be _all_ visible
        // incase we got h-scrolling we need to adjust the rect to cover the visibile area
        wxRect fixedRect = rect;
        wxPoint deviceOrigin = dc.GetDeviceOrigin();
        fixedRect.SetX(-deviceOrigin.x);
        dc.SetPen(_colours.GetHeaderHBorderColour());
        dc.DrawLine(fixedRect.GetBottomLeft(), fixedRect.GetBottomRight());
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

void clHeaderBar::SetHideHeaders(bool b) { m_hideHeaders = b; }

size_t clHeaderBar::GetWidth() const
{
    size_t w = 0;
    for(size_t i = 0; i < m_columns.size(); ++i) {
        w += m_columns[i].GetWidth();
    }
    return w;
}

void clHeaderBar::ScrollToColumn(int firstColumn)
{
    m_firstColumn = firstColumn;
    DoUpdateSize();
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
