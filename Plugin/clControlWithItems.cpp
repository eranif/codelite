#include "clControlWithItems.h"
#include <wx/settings.h>

wxDEFINE_EVENT(wxEVT_TREE_SEARCH_TEXT, wxTreeEvent);
wxDEFINE_EVENT(wxEVT_TREE_CLEAR_SEARCH, wxTreeEvent);

clControlWithItems::clControlWithItems(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size,
                                       long style)
    : clScrolledPanel(parent, id, pos, size, style)
    , m_viewHeader(this)
{
    DoInitialize();
}

clControlWithItems::clControlWithItems()
    : m_viewHeader(this)
{
}

bool clControlWithItems::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
{
    if(!clScrolledPanel::Create(parent, id, pos, size, style)) { return false; }
    DoInitialize();
    return true;
}

void clControlWithItems::DoInitialize()
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    Bind(wxEVT_SIZE, &clControlWithItems::OnSize, this);
    Bind(wxEVT_MOUSEWHEEL, &clControlWithItems::OnMouseScroll, this);
    wxSize textSize = GetTextSize("Tp");
    SetLineHeight(clRowEntry::Y_SPACER + textSize.GetHeight() + clRowEntry::Y_SPACER);
    SetIndent(0);
}

clControlWithItems::~clControlWithItems()
{
    Unbind(wxEVT_SIZE, &clControlWithItems::OnSize, this);
    Unbind(wxEVT_MOUSEWHEEL, &clControlWithItems::OnMouseScroll, this);
}

void clControlWithItems::SetHeader(const clHeaderBar& header)
{
    wxASSERT_MSG(IsEmpty(), "SetHeader can not be called on a non empty control");
    m_viewHeader = header;
    SetShowHeader(true);
}

void clControlWithItems::SetShowHeader(bool b)
{
    m_viewHeader.SetHideHeaders(!b);
    Refresh();
}

bool clControlWithItems::IsHeaderVisible() const { return !m_viewHeader.IsHideHeaders(); }

wxRect clControlWithItems::GetItemsRect() const
{
    // Return the rectangle taking header into consideration
    int yOffset = m_viewHeader.GetHeight();
    wxRect clientRect = GetClientArea();
    clientRect.SetY(yOffset);
    clientRect.SetHeight(clientRect.GetHeight() - yOffset);
    return clientRect;
}

void clControlWithItems::RenderHeader(wxDC& dc)
{
    if(IsHeaderVisible()) {
        wxRect headerRect = GetClientArea();
        headerRect.SetHeight(m_viewHeader.GetHeight());
        m_viewHeader.Render(dc, headerRect, m_colours);
    }
}

void clControlWithItems::RenderItems(wxDC& dc, const clRowEntry::Vec_t& items)
{
    wxRect clientRect = GetItemsRect();
    int y = clientRect.GetY();
    for(size_t i = 0; i < items.size(); ++i) {
        clRowEntry* curitem = items[i];
        if(curitem->IsHidden()) {
            // Set the item's rects into something non visible
            curitem->SetRects(wxRect(-100, -100, 0, 0), wxRect(-100, -100, 0, 0));
            continue;
        }
        wxRect itemRect = wxRect(0, y, clientRect.GetWidth(), m_lineHeight);
        wxRect buttonRect;
        if(curitem->HasChildren()) {
            buttonRect = wxRect((curitem->GetIndentsCount() * GetIndent()), y, m_lineHeight, m_lineHeight);
        }
        curitem->SetRects(itemRect, buttonRect);
        curitem->Render(this, dc, m_colours, i, &GetSearch());
        y += m_lineHeight;
    }
}

int clControlWithItems::GetNumLineCanFitOnScreen() const
{
    wxRect clientRect = GetItemsRect();
    int max_lines_on_screen = ceil(clientRect.GetHeight() / m_lineHeight);
    return max_lines_on_screen;
}

clRowEntry* clControlWithItems::GetFirstItemOnScreen() { return m_firstItemOnScreen; }

void clControlWithItems::SetFirstItemOnScreen(clRowEntry* item) { m_firstItemOnScreen = item; }

void clControlWithItems::UpdateScrollBar()
{
    {
        // V-scrollbar
        wxRect rect = GetItemsRect();
        int thumbSize = (rect.GetHeight() / m_lineHeight); // Number of lines can be drawn
        int pageSize = (thumbSize);
        int rangeSize = GetRange();
        int position = GetFirstItemPosition();
        UpdateVScrollBar(position, thumbSize, rangeSize, pageSize);
    }
    {
        // H-scrollbar
        int thumbSize = GetClientArea().GetWidth();
        int pageSize = (thumbSize - 1);
        int rangeSize = IsEmpty() ? 0 : m_viewHeader.GetWidth();
        int position = m_firstColumn;
        UpdateHScrollBar(position, thumbSize, rangeSize, pageSize);
    }
}

void clControlWithItems::Render(wxDC& dc)
{
    // draw the background on the entire client area
    dc.SetPen(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
    dc.SetBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
    dc.DrawRectangle(GetClientRect());

    // draw the background on the entire client area
    dc.SetPen(GetColours().GetBgColour());
    dc.SetBrush(GetColours().GetBgColour());
    dc.DrawRectangle(GetClientArea());

    // Set the device origin to the X-offset
    dc.SetDeviceOrigin(-m_firstColumn, 0);
    RenderHeader(dc);
}

void clControlWithItems::OnSize(wxSizeEvent& event)
{
    event.Skip();
    m_firstColumn = 0;
    UpdateScrollBar();
    Refresh();
}

void clControlWithItems::ScollToColumn(int firstColumn)
{
    m_firstColumn = firstColumn;
    Refresh();
}

void clControlWithItems::ScrollColumns(int steps, wxDirection direction)
{
    if((steps == 0) && (direction == wxLEFT)) {
        m_firstColumn = 0;
    } else if((steps == 0) && (direction == wxRIGHT)) {
        m_firstColumn = GetHeader().GetWidth();
    } else {
        int max_width = GetHeader().GetWidth();
        int firstColumn = m_firstColumn + ((direction == wxRIGHT) ? steps : -steps);
        if(firstColumn < 0) { firstColumn = 0; }
        int pageSize = GetClientArea().GetWidth();
        if((firstColumn + pageSize) > max_width) { firstColumn = max_width - pageSize; }
        m_firstColumn = firstColumn;
    }
    Refresh();
}

void clControlWithItems::DoUpdateHeader(clRowEntry* row)
{
    // do we have header?
    if(GetHeader().empty()) { return; }
    if(row && row->IsHidden()) { return; }
    wxDC& dc = GetTempDC();

    // Null row means: set the header bar to fit the column's label
    bool forceUpdate = (row == nullptr);

    // Use bold font, to get the maximum width needed
    for(size_t i = 0; i < GetHeader().size(); ++i) {

        int row_width = 0;
        if(row) {
            row_width = row->CalcItemWidth(dc, m_lineHeight, i);
        } else {
            int colWidth = dc.GetTextExtent(GetHeader().Item(i).GetLabel()).GetWidth();
            colWidth += 3 * clRowEntry::X_SPACER;
            row_width = colWidth;
        }
        GetHeader().UpdateColWidthIfNeeded(i, row_width, forceUpdate);
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
    if(index >= m_bitmaps.size()) {
        static wxBitmap emptyBitmap;
        return emptyBitmap;
    }
    return m_bitmaps[index];
}

void clControlWithItems::OnMouseScroll(wxMouseEvent& event)
{
    event.Skip();
    int range = GetRange();
    bool going_up = (event.GetWheelRotation() > 0);
    int new_row = GetFirstItemPosition() + (going_up ? -GetScrollTick() : GetScrollTick());
    if(new_row < 0) { new_row = 0; }
    if(new_row >= range) { new_row = range - 1; }
    ScrollToRow(new_row);
}

void clControlWithItems::SetNativeHeader(bool b)
{
    m_viewHeader.SetNative(b);
    Refresh();
}

bool clControlWithItems::DoKeyDown(const wxKeyEvent& event)
{
    m_search.OnKeyDown(event, this);
    return false; // continue processing
}

void clControlWithItems::SetFindWhat(const wxString& what) { GetSearch().SetFindWhat(what); }

const wxString& clControlWithItems::GetFindWhat() const { return GetSearch().GetFindWhat(); }

void clControlWithItems::ClearFindWhat() { GetSearch().Clear(); }

//===---------------------------------------------------
// clSearchText
//===---------------------------------------------------
clSearchText::clSearchText() {}

clSearchText::~clSearchText() {}

void clSearchText::OnKeyDown(const wxKeyEvent& event, clControlWithItems* control)
{
    if(!IsEnabled()) { return; }
    if((event.GetKeyCode() == WXK_ESCAPE) || (event.GetKeyCode() == WXK_BACK && event.ControlDown())) {
        Reset();
        // Notify about search clear
        control->Refresh();
        wxTreeEvent e(wxEVT_TREE_CLEAR_SEARCH);
        e.SetEventObject(control);
        control->GetEventHandler()->QueueEvent(e.Clone());
    } else if(event.GetKeyCode() == WXK_BACK) {
        m_findWhat.RemoveLast();
        control->Refresh();
        wxTreeEvent e(m_findWhat.IsEmpty() ? wxEVT_TREE_CLEAR_SEARCH : wxEVT_TREE_SEARCH_TEXT);
        e.SetEventObject(control);
        e.SetString(m_findWhat);
        control->GetEventHandler()->QueueEvent(e.Clone());

    } else if(wxIsprint(event.GetUnicodeKey())) {
        m_findWhat << event.GetUnicodeKey();
        control->Refresh();
        wxTreeEvent e(wxEVT_TREE_SEARCH_TEXT);
        e.SetEventObject(control);
        e.SetString(m_findWhat);
        control->GetEventHandler()->QueueEvent(e.Clone());
    }
}

void clSearchText::Reset() { m_findWhat.Clear(); }

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
            if(where == wxNOT_FOUND) { return false; }
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

void clSearchText::Clear() { m_findWhat.Clear(); }

const wxString& clSearchText::GetFindWhat() const { return m_findWhat; }
