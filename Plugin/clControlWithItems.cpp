#include "clControlWithItems.h"

#include "clTreeCtrl.h"
#include "file_logger.h"

#include <cmath>
#include <wx/minifram.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/textctrl.h>

#if defined(__WXGTK__) || defined(__WXOSX__)
#define USE_PANEL_PARENT 1
#else
#define USE_PANEL_PARENT 0
#endif

#if USE_PANEL_PARENT
#include <wx/panel.h>
#endif

wxDEFINE_EVENT(wxEVT_TREE_SEARCH_TEXT, wxTreeEvent);
wxDEFINE_EVENT(wxEVT_TREE_CLEAR_SEARCH, wxTreeEvent);

//===------------------------
// Helper class
//===------------------------
class clSearchControl :
#if USE_PANEL_PARENT
    public wxPanel
#else
    public wxMiniFrame
#endif
{
    wxTextCtrl* m_textCtrl = nullptr;

private:
    void DoSelect(bool next)
    {
        clTreeCtrl* tree = dynamic_cast<clTreeCtrl*>(GetParent());
        if(!tree || m_textCtrl->IsEmpty()) {
            return;
        }
        wxTreeItemId where = next ? tree->FindNext(tree->GetSelection(), m_textCtrl->GetValue(), 0,
                                                   wxTR_SEARCH_DEFAULT & ~wxTR_SEARCH_INCLUDE_CURRENT_ITEM)
                                  : tree->FindPrev(tree->GetSelection(), m_textCtrl->GetValue(), 0,
                                                   wxTR_SEARCH_DEFAULT & ~wxTR_SEARCH_INCLUDE_CURRENT_ITEM);
        if(where.IsOk()) {
            clRowEntry* row = reinterpret_cast<clRowEntry*>(where.GetID());
            clMatchResult res = row->GetHighlightInfo();

            // This will remove all the matched info, including the last call to FindNext/Prev
            tree->ClearAllHighlights();

            // Set back the match info
            row->SetHighlightInfo(res);

            // Select the item
            tree->SelectItem(where);

            // Make sure its visible
            tree->EnsureVisible(where);

            // Highlight the result
            tree->HighlightText(where, true);
        }
    }

public:
    clSearchControl(clControlWithItems* parent)
#if USE_PANEL_PARENT
        : wxPanel(parent)
#else
        : wxMiniFrame(parent, wxID_ANY, "Find", wxDefaultPosition, wxDefaultSize,
                      wxFRAME_FLOAT_ON_PARENT | wxBORDER_SIMPLE)
#endif
    {
        SetSizer(new wxBoxSizer(wxVERTICAL));
        wxPanel* mainPanel = new wxPanel(this);
        GetSizer()->Add(mainPanel, 1, wxEXPAND);
        mainPanel->SetSizer(new wxBoxSizer(wxVERTICAL));
        int scrollBarWidth = wxSystemSettings::GetMetric(wxSYS_VSCROLL_X, parent);
        wxSize searchControlSize(GetParent()->GetSize().GetWidth() / 2 - scrollBarWidth, -1);
        m_textCtrl = new wxTextCtrl(mainPanel, wxID_ANY, "", wxDefaultPosition, searchControlSize,
                                    wxTE_RICH | wxTE_PROCESS_ENTER);
        mainPanel->GetSizer()->Add(m_textCtrl, 0, wxEXPAND);
        m_textCtrl->CallAfter(&wxTextCtrl::SetFocus);
        m_textCtrl->Bind(wxEVT_TEXT, &clSearchControl::OnTextUpdated, this);
        m_textCtrl->Bind(wxEVT_KEY_DOWN, &clSearchControl::OnKeyDown, this);
        GetSizer()->Fit(this);
        Hide();
    }

    virtual ~clSearchControl()
    {
        m_textCtrl->Unbind(wxEVT_TEXT, &clSearchControl::OnTextUpdated, this);
        m_textCtrl->Unbind(wxEVT_KEY_DOWN, &clSearchControl::OnKeyDown, this);

        // Let the parent know that we were dismissed
        clControlWithItems* parent = dynamic_cast<clControlWithItems*>(GetParent());
        parent->SearchControlDismissed();
    }

    void PositionControl()
    {
#if USE_PANEL_PARENT
        int scrollBarHeight = wxSystemSettings::GetMetric(wxSYS_HSCROLL_Y, GetParent());
        int x = GetParent()->GetSize().GetWidth() / 2;
        int y = GetParent()->GetSize().GetHeight() - GetSize().GetHeight() - scrollBarHeight;
        SetPosition(wxPoint(x, y));
#else
        wxPoint parentPt = GetParent()->GetScreenPosition();
        CenterOnParent();
        SetPosition(wxPoint(GetPosition().x, parentPt.y - m_textCtrl->GetSize().GetHeight()));
#endif
    }
    void DoSelectNone() { m_textCtrl->SelectNone(); }

    void InitSearch(const wxChar& ch)
    {
        m_textCtrl->SetFocus();
        m_textCtrl->ChangeValue(wxString() << ch);
        m_textCtrl->SetInsertionPointEnd();
        CallAfter(&clSearchControl::DoSelectNone);
    }

    void ShowControl(const wxChar& ch)
    {
        Show();
        m_textCtrl->ChangeValue("");
        PositionControl();
        CallAfter(&clSearchControl::InitSearch, ch);
    }

    void SelectNext() { DoSelect(true); }

    void SelectPrev() { DoSelect(false); }

    void Dismiss()
    {
        GetParent()->CallAfter(&wxWindow::SetFocus);
        // Clear the search
        wxTreeEvent e(wxEVT_TREE_CLEAR_SEARCH);
        e.SetEventObject(GetParent());
        GetParent()->GetEventHandler()->QueueEvent(e.Clone());
        Hide();
    }

    void OnTextUpdated(wxCommandEvent& event)
    {
        event.Skip();
        wxTreeEvent e(wxEVT_TREE_SEARCH_TEXT);
        e.SetString(m_textCtrl->GetValue());
        e.SetEventObject(GetParent());
        GetParent()->GetEventHandler()->QueueEvent(e.Clone());
    }

    void OnKeyDown(wxKeyEvent& event)
    {
        if(event.GetKeyCode() == WXK_ESCAPE) {
            Dismiss();
            return;
        } else if(event.GetKeyCode() == WXK_DOWN) {
            SelectNext();
        } else if(event.GetKeyCode() == WXK_UP) {
            SelectPrev();
        } else if(event.GetKeyCode() == WXK_RETURN || event.GetKeyCode() == WXK_NUMPAD_ENTER) {
            // Activate the item
            clTreeCtrl* tree = dynamic_cast<clTreeCtrl*>(GetParent());
            wxTreeEvent evt(wxEVT_TREE_ITEM_ACTIVATED);
            evt.SetEventObject(tree);
            evt.SetItem(tree->GetSelection());
            tree->GetEventHandler()->AddPendingEvent(evt);
            Dismiss();
        } else {
            event.Skip();
        }
    }
};

clControlWithItems::clControlWithItems(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size,
                                       long style)
    : clScrolledPanel(parent, id, pos, size, style)
{
    DoInitialize();
}

clControlWithItems::clControlWithItems() {}

bool clControlWithItems::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
{
    if(!clScrolledPanel::Create(parent, id, pos, size, style)) {
        return false;
    }
    DoInitialize();
    return true;
}

void clControlWithItems::DoInitialize()
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    m_viewHeader = new clHeaderBar(this, m_colours);
    m_viewHeader->SetHeaderFont(GetDefaultFont());

    Bind(wxEVT_MOUSEWHEEL, &clControlWithItems::OnMouseScroll, this);
    Bind(wxEVT_SET_FOCUS, [&](wxFocusEvent& e) {
        e.Skip();
        if(m_searchControl && m_searchControl->IsShown()) {
            m_searchControl->Dismiss();
        }
    });
    wxSize textSize = GetTextSize("Tp");
    SetLineHeight(clRowEntry::Y_SPACER + textSize.GetHeight() + clRowEntry::Y_SPACER);
    SetIndent(0);
    SetBackgroundColour(GetColours().GetBgColour());
}

clControlWithItems::~clControlWithItems()
{
    m_searchControl = nullptr;
    Unbind(wxEVT_MOUSEWHEEL, &clControlWithItems::OnMouseScroll, this);
    wxDELETE(m_bitmapsInternal);
}

void clControlWithItems::SetShowHeader(bool b)
{
    if(GetHeader()) {
        GetHeader()->Show(b);
        DoPositionVScrollbar(); // Adjust the vertical scrollbar if needed
        Refresh();
    }
}

bool clControlWithItems::IsHeaderVisible() const { return GetHeader() && GetHeader()->IsShown(); }

wxRect clControlWithItems::GetItemsRect() const
{
    // Return the rectangle taking header into consideration
    int yOffset = 0;
    if(m_viewHeader && m_viewHeader->IsShown()) {
        yOffset = m_viewHeader->GetHeight();
    }
    wxRect clientRect = GetClientArea();
    clientRect.SetY(yOffset);
    clientRect.SetHeight(clientRect.GetHeight() - yOffset);
    return clientRect;
}

void clControlWithItems::RenderItems(wxDC& dc, const clRowEntry::Vec_t& items)
{
    AssignRects(items);
    vector<size_t> max_widths;

    // calculate the width of the cells
    for(size_t i = 0; i < items.size(); ++i) {
        clRowEntry* curitem = items[i];
        auto v_width = curitem->GetColumnWidths(this, dc);
        if(max_widths.empty()) {
            max_widths.swap(v_width);
        } else {
            for(size_t index = 0; index < v_width.size(); ++index) {
                max_widths[index] = wxMax(max_widths[index], v_width[index]);
            }
        }
    }

    if(GetHeader()) {
        GetHeader()->SetColumnsWidth(max_widths);
    }

    for(size_t i = 0; i < items.size(); ++i) {
        clRowEntry* curitem = items[i];
        if(curitem->IsHidden()) {
            continue;
        }
        if(m_customRenderer) {
            m_customRenderer->Render(this, dc, m_colours, i, curitem);
        } else {
            curitem->Render(this, dc, m_colours, i, &GetSearch());
        }
    }
}

int clControlWithItems::GetNumLineCanFitOnScreen(bool fully_fit) const
{
    wxRect clientRect = GetItemsRect();
    int max_lines_on_screen = 0;
    if(fully_fit) {
        max_lines_on_screen = std::floor((double)((double)clientRect.GetHeight() / (double)m_lineHeight));
    } else {
        max_lines_on_screen = std::ceil((double)((double)clientRect.GetHeight() / (double)m_lineHeight));
    }
    return max_lines_on_screen;
}

clRowEntry* clControlWithItems::GetFirstItemOnScreen() { return m_firstItemOnScreen; }

void clControlWithItems::SetFirstItemOnScreen(clRowEntry* item) { m_firstItemOnScreen = item; }

void clControlWithItems::UpdateScrollBar()
{
    {
        // V-scrollbar
        // wxRect rect = GetItemsRect();
        int thumbSize = GetNumLineCanFitOnScreen(); // Number of lines can be drawn
        int pageSize = (thumbSize);
        int rangeSize = GetRange();
        int position = GetFirstItemPosition();
        UpdateVScrollBar(position, thumbSize, rangeSize, pageSize);
    }
    {
        // H-scrollbar
        int thumbSize = GetClientArea().GetWidth();
        int rangeSize = IsEmpty() ? 0 : m_viewHeader->GetWidth();
        if((m_firstColumn + thumbSize) > rangeSize) {
            m_firstColumn = (rangeSize - thumbSize);
        }
        int pageSize = (thumbSize - 1);
        int position = m_firstColumn;

        int pixels_after = rangeSize - m_firstColumn - thumbSize;
        if((pixels_after < 0) && (rangeSize > thumbSize)) {
            // m_firstColumn += pixels_after; // reduce it from the left side
            clHeaderItem& column = GetHeader()->Item(GetHeader()->size() - 1);
            column.UpdateWidth(column.GetWidth() - pixels_after);
        }
        if(m_firstColumn < 0) {
            m_firstColumn = 0;
        }
        position = m_firstColumn;
        UpdateHScrollBar(position, thumbSize, rangeSize, pageSize);
    }
}

void clControlWithItems::Render(wxDC& dc)
{
    // draw the background on the entire client area
    dc.SetPen(GetColours().GetBgColour());
    dc.SetBrush(GetColours().GetBgColour());
    dc.DrawRectangle(GetClientRect());

    // draw the background on the entire client area
    dc.SetPen(GetColours().GetBgColour());
    dc.SetBrush(GetColours().GetBgColour());
    dc.DrawRectangle(GetClientArea());

    // Set the device origin to the X-offset
    dc.SetDeviceOrigin(-m_firstColumn, 0);
}

void clControlWithItems::OnSize(wxSizeEvent& event)
{
    event.Skip();
    clScrolledPanel::OnSize(event);
    m_firstColumn = 0;
    // since the control size was resized, we turn the "m_maxList" flag to ON
    // and in turn, in the OnPaint() we will try to maximize the list displayed
    // to fit
    m_maxList = true;
    UpdateScrollBar();
    Refresh();
}

void clControlWithItems::ScollToColumn(int firstColumn)
{
    m_firstColumn = firstColumn;
    UpdateScrollBar();
    Refresh();
}

void clControlWithItems::ScrollColumns(int steps, wxDirection direction)
{
    if((steps == 0) && (direction == wxLEFT)) {
        m_firstColumn = 0;
    } else if((steps == 0) && (direction == wxRIGHT)) {
        m_firstColumn = GetHeader()->GetWidth();
    } else {
        int max_width = GetHeader()->GetWidth();
        int firstColumn = m_firstColumn + ((direction == wxRIGHT) ? steps : -steps);
        if(firstColumn < 0) {
            firstColumn = 0;
        }
        int pageSize = GetClientArea().GetWidth();
        if((firstColumn + pageSize) > max_width) {
            firstColumn = max_width - pageSize;
        }
        m_firstColumn = firstColumn;
    }
    Refresh();
}

void clControlWithItems::DoUpdateHeader(clRowEntry* row)
{
    // do we have header?
    if(GetHeader()->empty()) {
        return;
    }

    if(row && row->IsHidden()) {
        return;
    }

    wxDC& dc = GetTempDC();
    dc.SetFont(GetDefaultFont());

    // Null row means: set the header bar to fit the column's label
    bool forceUpdate = (row == nullptr);

    // Use bold font, to get the maximum width needed
    for(size_t i = 0; i < GetHeader()->size(); ++i) {
        int row_width = 0;
        if(row) {
            row_width = row->CalcItemWidth(dc, m_lineHeight, i);
        } else {
            int colWidth = dc.GetTextExtent(GetHeader()->Item(i).GetLabel()).GetWidth();
            colWidth += 3 * clRowEntry::X_SPACER;
            row_width = colWidth;
        }
        if(forceUpdate || GetHeader()->Item(i).IsAutoResize()) {
            GetHeader()->UpdateColWidthIfNeeded(i, row_width, forceUpdate);
        }
    }
}

wxSize clControlWithItems::GetTextSize(const wxString& label) const
{
    wxDC& dc = GetTempDC();
    wxFont font = GetDefaultFont();
    dc.SetFont(font);
    wxSize textSize = dc.GetTextExtent(label);
    return textSize;
}

const wxBitmap& clControlWithItems::GetBitmap(size_t index) const
{
    if(!GetBitmaps() || (index >= GetBitmaps()->size())) {
        static wxBitmap emptyBitmap;
        return emptyBitmap;
    }
    return GetBitmaps()->at(index);
}

void clControlWithItems::OnMouseScroll(wxMouseEvent& event)
{
    event.Skip();
    DoMouseScroll(event);
}

bool clControlWithItems::DoKeyDown(const wxKeyEvent& event)
{
    if(m_searchControl && m_searchControl->IsShown()) {
        return true;
    }
    if(m_search.IsEnabled() && wxIsprint(event.GetUnicodeKey()) &&
       (event.GetModifiers() == wxMOD_NONE || event.GetModifiers() == wxMOD_SHIFT)) {
        if(!m_searchControl) {
            m_searchControl = new clSearchControl(this);
        }
        m_searchControl->ShowControl(event.GetUnicodeKey());
        return true;
    }
    return false;
}

void clControlWithItems::SearchControlDismissed() {}

void clControlWithItems::AssignRects(const clRowEntry::Vec_t& items)
{
    wxRect clientRect = GetItemsRect();
    int y = clientRect.GetY();
    int header_bar_width = m_viewHeader ? m_viewHeader->GetWidth() : wxNOT_FOUND;
    int width = wxMax(clientRect.GetWidth(), header_bar_width);

    for(size_t i = 0; i < items.size(); ++i) {
        clRowEntry* curitem = items[i];
        if(curitem->IsHidden()) {
            // Set the item's rects into something non visible
            curitem->SetRects(wxRect(-100, -100, 0, 0), wxRect(-100, -100, 0, 0));
            continue;
        }
        wxRect itemRect = wxRect(0, y, width, m_lineHeight);
        wxRect buttonRect;
        if(curitem->HasChildren()) {
            buttonRect = wxRect((curitem->GetIndentsCount() * GetIndent()), y, m_lineHeight, m_lineHeight);
        }
        curitem->SetRects(itemRect, buttonRect);
        y += m_lineHeight;
    }
}

void clControlWithItems::DoMouseScroll(const wxMouseEvent& event)
{
    int range = GetRange();
    bool going_up = (event.GetWheelRotation() > 0);
    int new_row = GetFirstItemPosition() + (going_up ? -GetScrollTick() : GetScrollTick());
    if(new_row < 0) {
        new_row = 0;
    }
    if(new_row >= range) {
        new_row = range - 1;
    }
    ScrollToRow(new_row);
}

clHeaderBar* clControlWithItems::GetHeader() const { return m_viewHeader; }

void clControlWithItems::DoPositionVScrollbar()
{
    if(IsHeaderVisible()) {
        // When the heaer is visible place the vertical scrollbar under it
        wxRect clientRect = GetClientRect();
        wxSize vsbSize = GetVScrollBar()->GetSize();

        int height = clientRect.GetHeight();
        if(GetHScrollBar() && GetHScrollBar()->IsShown()) {
            height -= GetHScrollBar()->GetSize().GetHeight();
        }
        int width = vsbSize.GetWidth();
        int x = clientRect.GetWidth() - vsbSize.GetWidth();
        int y = (GetHeader() ? GetHeader()->GetHeight() : 0);
        height -= (GetHeader() ? GetHeader()->GetHeight() : 0);
        if(height < 0) {
            height = 0;
        }
        GetVScrollBar()->SetSize(width, height);
        GetVScrollBar()->Move(x, y);

    } else {
        clScrolledPanel::DoPositionVScrollbar();
    }
}

void clControlWithItems::DoPositionHScrollbar() { clScrolledPanel::DoPositionHScrollbar(); }

void clControlWithItems::SetColumnWidth(size_t col, int width)
{
    if(col >= GetHeader()->size()) {
        return;
    }
    // Handle special values
    if(width == wxCOL_WIDTH_AUTOSIZE || width == wxCOL_WIDTH_DEFAULT) {
        GetHeader()->Item(col).SetWidthValue(width);
        GetHeader()->UpdateColWidthIfNeeded(col, width, true);
    } else if(width >= 0) {
        GetHeader()->Item(col).SetWidthValue(width);
        GetHeader()->UpdateColWidthIfNeeded(col, width, true);
    }
    UpdateScrollBar();
    GetHeader()->Refresh();
    Refresh();
}

void clControlWithItems::SetNativeTheme(bool nativeTheme)
{
    GetHeader()->SetNative(nativeTheme);
    m_nativeTheme = nativeTheme;
    Refresh();
}

void clControlWithItems::SetImageList(wxImageList* images)
{
    wxDELETE(m_bitmapsInternal);
    if(!images || images->GetImageCount() <= 0) {
        return;
    }

    m_bitmapsInternal = new BitmapVec_t();
    m_bitmapsInternal->reserve(images->GetImageCount());
    for(size_t i = 0; i < (size_t)images->GetImageCount(); ++i) {
        m_bitmapsInternal->push_back(images->GetBitmap(i));
    }
    SetBitmaps(m_bitmapsInternal);
}

void clControlWithItems::SetColours(const clColours& colours)
{
    this->m_colours = colours;
    GetVScrollBar()->SetColours(m_colours);
    GetHScrollBar()->SetColours(m_colours);
    SetBackgroundColour(GetColours().GetBgColour());
    Refresh();
}

void clControlWithItems::SetCustomRenderer(clControlWithItemsRowRenderer* renderer)
{
    m_customRenderer.reset(renderer);
}

void clControlWithItems::SetDefaultFont(const wxFont& font)
{
    m_defaultFont = font;
    // update the temp DC with the default font
    GetTempDC().SetFont(font);
    if(m_viewHeader) {
        m_viewHeader->SetHeaderFont(GetDefaultFont());
    }

    // update the line height
    wxSize textSize = GetTextSize("Tp");
    SetLineHeight(clRowEntry::Y_SPACER + textSize.GetHeight() + clRowEntry::Y_SPACER);
}

wxFont clControlWithItems::GetDefaultFont() const
{
    if(m_defaultFont.IsOk()) {
        return m_defaultFont;
    }
    return clScrolledPanel::GetDefaultFont();
}

//===---------------------------------------------------
// clSearchText
//===---------------------------------------------------
clSearchText::clSearchText() {}

clSearchText::~clSearchText() {}

bool clSearchText::Matches(const wxString& findWhat, size_t col, const wxString& text, size_t searchFlags,
                           clMatchResult* matches)
{
    wxString haystack = searchFlags & wxTR_SEARCH_ICASE ? text.Lower() : text;
    wxString needle = searchFlags & wxTR_SEARCH_ICASE ? findWhat.Lower() : findWhat;
    if(!matches) {
        if(searchFlags & wxTR_SEARCH_METHOD_CONTAINS) {
            return haystack.Contains(needle);
        } else {
            return (haystack == needle);
        }
    } else {
        if(searchFlags & wxTR_SEARCH_METHOD_CONTAINS) {
            int where = haystack.Find(needle);
            if(where == wxNOT_FOUND) {
                return false;
            }
            Str3Arr_t arr;
            arr[0] = text.Mid(0, where);
            arr[1] = text.Mid(where, needle.length());
            arr[2] = text.Mid(where + needle.length());
            matches->Add(col, arr);
            return true;
        } else {
            if(haystack == needle) {
                Str3Arr_t arr;
                arr[0] = "";
                arr[1] = text;
                arr[2] = "";
                matches->Add(col, arr);
                return true;
            } else {
                return false;
            }
        }
    }
    return false;
}
