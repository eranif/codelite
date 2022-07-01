#include "clHeaderBar.h"

#include "clControlWithItems.h"
#include "clScrolledPanel.h"
#include "file_logger.h"

#include <wx/cursor.h>
#include <wx/dcbuffer.h>
#include <wx/dcclient.h>
#include <wx/dcgraph.h>
#include <wx/dcmemory.h>
#include <wx/event.h>
#include <wx/font.h>
#include <wx/log.h>
#include <wx/renderer.h>
#include <wx/settings.h>
#include <wx/window.h>

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
    if(!wxPanel::Create(parent)) {
        return;
    }
    Bind(wxEVT_PAINT, &clHeaderBar::OnPaint, this);
    Bind(wxEVT_LEFT_DOWN, &clHeaderBar::OnMouseLeftDown, this);
    Bind(wxEVT_MOTION, &clHeaderBar::OnMotion, this);
    Bind(wxEVT_LEFT_UP, &clHeaderBar::OnMouseLeftUp, this);
    GetParent()->Bind(wxEVT_SIZE, &clHeaderBar::OnSize, this);
    Bind(wxEVT_ERASE_BACKGROUND, [](wxEraseEvent& event) { wxUnusedVar(event); });
}

clHeaderBar::~clHeaderBar() {}

size_t clHeaderBar::GetHeight() const
{
    if(m_columns.empty()) {
        return 0;
    }
    return m_columns[0].GetRect().GetHeight();
}

void clHeaderBar::OnSize(wxSizeEvent& event)
{
    event.Skip();
    if(!GetParent()) {
        return;
    }
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
    wxClientDC dc(const_cast<clHeaderBar*>(this));
    wxFont font = GetHeaderFont().IsOk() ? GetHeaderFont() : clScrolledPanel::GetDefaultFont();
    dc.SetFont(font);
    wxSize textSize = dc.GetTextExtent(label);
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
    if(useNativeHeader) {
        wxRendererNative::Get().DrawHeaderButton(this, dc, rect, 0);
    }

    // Set the DC origin to reflect the h-scrollbar
    clControlWithItems* parent = dynamic_cast<clControlWithItems*>(GetParent());
    dc.SetDeviceOrigin(-parent->GetFirstColumn(), 0);
    if(parent->IsDisabled()) {
        _colours.SetItemTextColour(_colours.GetGrayText());
        _colours.SetSelItemTextColour(_colours.GetGrayText());
    }

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

void clHeaderBar::UpdateColWidthIfNeeded(size_t col, int width, bool force)
{
    if(col >= m_columns.size()) {
        return;
    }
    clHeaderItem& column = m_columns[col];
    column.UpdateWidth(force ? width : wxMax(column.GetWidth(), width));

    // Update the offsets
    int xx = 0;
    for(size_t i = 0; i < m_columns.size(); ++i) {
        clHeaderItem& column = m_columns[i];
        column.SetX(xx);
        xx += column.GetWidth();
    }
}

void clHeaderBar::OnMouseLeftDown(wxMouseEvent& event)
{
    event.Skip();

    clControlWithItems* parent = dynamic_cast<clControlWithItems*>(GetParent());
    int x = event.GetX() + parent->GetFirstColumn();
    m_draggedCol = HitBorder(x);
    if(m_draggedCol > wxNOT_FOUND) {
        // Get ready to drag
        m_previousCursor = GetCursor();
        SetCursor(wxCursor(wxCURSOR_SIZEWE));
        m_isDragging = true;
        CaptureMouse();
    }
}

void clHeaderBar::OnMotion(wxMouseEvent& event)
{
    event.Skip();
    clControlWithItems* parent = dynamic_cast<clControlWithItems*>(GetParent());
    int x = event.GetX() + parent->GetFirstColumn();
    if(m_isDragging) {
        wxCHECK_RET(m_draggedCol > -1 && m_draggedCol < (int)m_columns.size(), "Dragging but the column is invalid");
        int delta = x - Item(m_draggedCol).GetRect().GetRight();
        // Compare with COL_MIN_SIZE as we don't want to shrink the col to nothing (or beyond!)
        const static int COL_MIN_SIZE = 7;
        if((int)(Item(m_draggedCol).GetWidth()) + delta > COL_MIN_SIZE) {
            parent->SetColumnWidth(m_draggedCol, Item(m_draggedCol).GetWidth() + delta);
        }
    }
}

void clHeaderBar::OnMouseLeftUp(wxMouseEvent& event)
{
    event.Skip();
    DoCancelDrag();
}

int clHeaderBar::HitBorder(int x) const // Returns the column whose *right*-hand edge contains the cursor
{
    int xpos(0);
    for(size_t i = 0; i < size(); ++i) {
        xpos += Item(i).GetWidth();
        if(abs(x - xpos) < 5) {
            return i;
        }
    }

    return wxNOT_FOUND;
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
    if(!GetParent()) {
        return false;
    }
    SetSize(GetParent()->GetSize().GetWidth(), GetHeight());
    return wxPanel::Show(show);
}

void clHeaderBar::DoCancelDrag()
{
    m_isDragging = false;
    m_draggedCol = -1;
    SetCursor(m_previousCursor);
    m_previousCursor = wxCursor();
    if(HasCapture()) {
        ReleaseMouse();
    }
}

void clHeaderBar::SetColumnsWidth(const std::vector<size_t>& v_width)
{
    if(v_width.size() != m_columns.size()) {
        return;
    }

    size_t x = 0;
    for(size_t i = 0; i < m_columns.size(); ++i) {
        auto& column = m_columns[i];
        column.SetX(x);
        column.SetWidthValue(v_width[i]);
        x += v_width[i];
    }
}
