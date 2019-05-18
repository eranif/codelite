#include "clCellValue.h"
#include "clHeaderBar.h"
#include "clHeaderItem.h"
#include "clRowEntry.h"
#include "clTreeCtrl.h"
#include <algorithm>
#include <functional>
#include <wx/dataview.h>
#include <wx/dc.h>
#include <wx/renderer.h>
#include <wx/settings.h>
#include <wx/window.h>
#include <drawingutils.h>

#if !wxCHECK_VERSION(3, 1, 0)
#define wxCONTROL_NONE 0
#endif

#ifdef __WXMSW__
#define PEN_STYLE wxPENSTYLE_SHORT_DASH
#else
#define PEN_STYLE wxPENSTYLE_DOT
#endif

#ifdef __WXMSW__
int clRowEntry::X_SPACER = 4;
int clRowEntry::Y_SPACER = 2;
#else
int clRowEntry::X_SPACER = 5;
int clRowEntry::Y_SPACER = 3;
#endif

#ifdef __WXOSX__
#define IS_OSX 1
#else
#define IS_OSX 0
#endif

struct clClipperHelper {

    bool m_used = false;
    wxRect m_oldRect;
    wxDC& m_dc;

    clClipperHelper(wxDC& dc)
        : m_dc(dc)
    {
    }

    ~clClipperHelper() { Reset(); }

    void Clip(const wxRect& rect)
    {
        // Make sure we can call this method only once
        if(m_used) { return; }
        m_dc.GetClippingBox(m_oldRect);
        m_dc.SetClippingRegion(rect);
        m_used = true;
    }

    void Reset()
    {
        // Make sure we can call this method only once
        if(m_used) {
            m_dc.DestroyClippingRegion();
            if(!m_oldRect.IsEmpty()) { m_dc.SetClippingRegion(m_oldRect); }
            m_used = false;
        }
    }
};

void clRowEntry::DrawSimpleSelection(wxWindow* win, wxDC& dc, const wxRect& rect, const clColours& colours)
{
#ifdef __WXMSW__
    if(m_tree->IsNativeTheme()) {
        int flags = wxCONTROL_SELECTED;
        if(win->HasFocus()) { flags |= wxCONTROL_FOCUSED; }
        wxRendererNative::Get().DrawItemSelectionRect(win, dc, rect, flags);

    } else {
        dc.SetPen(win->HasFocus() ? colours.GetSelItemBgColour() : colours.GetSelItemBgColourNoFocus());
        dc.SetBrush(win->HasFocus() ? colours.GetSelItemBgColour() : colours.GetSelItemBgColourNoFocus());
        dc.DrawRectangle(rect);
    }
#else
    dc.SetPen(win->HasFocus() ? colours.GetSelItemBgColour() : colours.GetSelItemBgColourNoFocus());
    dc.SetBrush(win->HasFocus() ? colours.GetSelItemBgColour() : colours.GetSelItemBgColourNoFocus());
    dc.DrawRectangle(rect);
#endif
}

clRowEntry::clRowEntry(clTreeCtrl* tree, const wxString& label, int bitmapIndex, int bitmapSelectedIndex)
    : m_tree(tree)
    , m_model(tree ? &tree->GetModel() : nullptr)
{
    // Fill the verctor with items constructed using the _non_ default constructor
    // to makes sure that IsOk() returns TRUE
    m_cells.resize(m_tree->GetHeader()->empty() ? 1 : m_tree->GetHeader()->size(),
                   clCellValue("", -1, -1)); // at least one column
    clCellValue cv(label, bitmapIndex, bitmapSelectedIndex);
    m_cells[0] = cv;
}

clRowEntry::clRowEntry(clTreeCtrl* tree, bool checked, const wxString& label, int bitmapIndex, int bitmapSelectedIndex)
    : m_tree(tree)
    , m_model(tree ? &tree->GetModel() : nullptr)
{
    // Fill the verctor with items constructed using the _non_ default constructor
    // to makes sure that IsOk() returns TRUE
    m_cells.resize(m_tree->GetHeader()->empty() ? 1 : m_tree->GetHeader()->size(),
                   clCellValue("", -1, -1)); // at least one column
    clCellValue cv(checked, label, bitmapIndex, bitmapSelectedIndex);
    m_cells[0] = cv;
}

clRowEntry::~clRowEntry()
{
    // Delete all the node children
    DeleteAllChildren();
    wxDELETE(m_clientObject);

    // Notify the model that a selection is being deleted
    if(m_model) { m_model->NodeDeleted(this); }
}

void clRowEntry::ConnectNodes(clRowEntry* first, clRowEntry* second)
{
    if(first) { first->m_next = this; }
    this->m_prev = first;
    this->m_next = second;
    if(second) { second->m_prev = this; }
}

void clRowEntry::InsertChild(clRowEntry* child, clRowEntry* prev)
{
    child->SetParent(this);
    child->SetIndentsCount(GetIndentsCount() + 1);

    // We need the last item of this subtree (prev 'this' is the root)
    if(prev == nullptr) {
        // make it the first item
        m_children.insert(m_children.begin(), child);
    } else {
        // Insert the item in the parent children list
        clRowEntry::Vec_t::iterator iter = m_children.end();
        iter = std::find_if(m_children.begin(), m_children.end(), [&](clRowEntry* c) { return c == prev; });
        if(iter != m_children.end()) { ++iter; }
        // if iter is end(), than the is actually appending the item
        m_children.insert(iter, child);
    }

    // Connect the linked list for sequential iteration
    clRowEntry::Vec_t::iterator iterCur =
        std::find_if(m_children.begin(), m_children.end(), [&](clRowEntry* c) { return c == child; });

    clRowEntry* nodeBefore = nullptr;
    // Find the item before and after
    if(iterCur == m_children.begin()) {
        nodeBefore = child->GetParent(); // "this"
    } else {
        --iterCur;
        clRowEntry* prevSibling = (*iterCur);
        while(prevSibling && prevSibling->HasChildren()) {
            prevSibling = prevSibling->GetLastChild();
        }
        nodeBefore = prevSibling;
    }
    child->ConnectNodes(nodeBefore, nodeBefore->m_next);
}

void clRowEntry::AddChild(clRowEntry* child) { InsertChild(child, m_children.empty() ? nullptr : m_children.back()); }

void clRowEntry::SetParent(clRowEntry* parent)
{
    if(m_parent == parent) { return; }
    if(m_parent) { m_parent->DeleteChild(this); }
    m_parent = parent;
}

void clRowEntry::DeleteChild(clRowEntry* child)
{
    // first remove all of its children
    // do this in a while loop since 'child->RemoveChild(c);' will alter
    // the array and will invalidate all iterators
    while(!child->m_children.empty()) {
        clRowEntry* c = child->m_children[0];
        child->DeleteChild(c);
    }
    // Connect the list
    clRowEntry* prev = child->m_prev;
    clRowEntry* next = child->m_next;
    if(prev) { prev->m_next = next; }
    if(next) { next->m_prev = prev; }
    // Now disconnect this child from this node
    clRowEntry::Vec_t::iterator iter =
        std::find_if(m_children.begin(), m_children.end(), [&](clRowEntry* c) { return c == child; });
    if(iter != m_children.end()) { m_children.erase(iter); }
    wxDELETE(child);
}

int clRowEntry::GetExpandedLines() const
{
    clRowEntry* node = const_cast<clRowEntry*>(this);
    int counter = 0;
    while(node) {
        if(node->IsVisible()) { ++counter; }
        node = node->m_next;
    }
    return counter;
}

void clRowEntry::GetNextItems(int count, clRowEntry::Vec_t& items, bool selfIncluded)
{
    if(count <= 0) { return; }
    items.reserve(count);
    if(!this->IsHidden() && selfIncluded) { items.push_back(this); }
    clRowEntry* next = GetNext();
    while(next) {
        if(next->IsVisible() && !next->IsHidden()) { items.push_back(next); }
        if((int)items.size() == count) { return; }
        next = next->GetNext();
    }
}

void clRowEntry::GetPrevItems(int count, clRowEntry::Vec_t& items, bool selfIncluded)
{
    if(count <= 0) { return; }
    items.reserve(count);
    if(!this->IsHidden() && selfIncluded) { items.insert(items.begin(), this); }
    clRowEntry* prev = GetPrev();
    while(prev) {
        if(prev->IsVisible() && !prev->IsHidden()) { items.insert(items.begin(), prev); }
        if((int)items.size() == count) { return; }
        prev = prev->GetPrev();
    }
}

clRowEntry* clRowEntry::GetVisibleItem(int index)
{
    clRowEntry::Vec_t items;
    GetNextItems(index, items);
    if((int)items.size() != index) { return nullptr; }
    return items.back();
}

void clRowEntry::UnselectAll()
{
    clRowEntry* item = const_cast<clRowEntry*>(this);
    while(item) {
        item->SetSelected(false);
        item = item->GetNext();
    }
}

bool clRowEntry::SetExpanded(bool b)
{
    if(!m_model) { return false; }
    if(IsHidden() && !b) {
        // Hidden root can not be hidden
        return false;
    }

    if(IsHidden()) {
        // Hidden node do not fire events
        SetFlag(kNF_Expanded, b);
        return true;
    }

    // Already expanded?
    if(b && IsExpanded()) { return true; }

    // Already collapsed?
    if(!b && !IsExpanded()) { return true; }
    if(!m_model->NodeExpanding(this, b)) { return false; }

    SetFlag(kNF_Expanded, b);
    m_model->NodeExpanded(this, b);
    return true;
}

void clRowEntry::ClearRects()
{
    m_buttonRect = wxRect();
    m_rowRect = wxRect();
}

void clRowEntry::Render(wxWindow* win, wxDC& dc, const clColours& c, int row_index, clSearchText* searcher)
{
    wxUnusedVar(searcher);
    wxRect rowRect = GetItemRect();
    bool zebraColouring = (m_tree->HasStyle(wxTR_ROW_LINES) || m_tree->HasStyle(wxDV_ROW_LINES));
    bool even_row = ((row_index % 2) == 0);

    // Define the clipping region
    bool hasHeader = (m_tree->GetHeader() && !m_tree->GetHeader()->empty());

    // Not cell related
    clColours colours = c;
    if(zebraColouring) {
        // Set Zebra colouring, only if no user colour was provided for the given line
        colours.SetItemBgColour(even_row ? c.GetAlternateColour() : c.GetBgColour());
    }

    // Override default item bg colour with the user's one
    if(GetBgColour().IsOk()) { colours.SetItemBgColour(GetBgColour()); }
    wxRect selectionRect = rowRect;
    wxPoint deviceOrigin = dc.GetDeviceOrigin();
    selectionRect.SetX(-deviceOrigin.x);
    if(IsSelected()) {
        DrawSimpleSelection(win, dc, selectionRect, colours);
    } else if(IsHovered()) {
        dc.SetPen(colours.GetHoverBgColour());
        dc.SetBrush(colours.GetHoverBgColour());
        dc.DrawRectangle(selectionRect);
    } else if(colours.GetItemBgColour().IsOk()) {
        dc.SetBrush(colours.GetItemBgColour());
        dc.SetPen(colours.GetItemBgColour());
        dc.DrawRectangle(selectionRect);
    }

    // Per cell drawings
    for(size_t i = 0; i < m_cells.size(); ++i) {
        bool last_cell = (i == (m_cells.size() - 1));
        colours = c; // reset the colours
        wxFont f = clScrolledPanel::GetDefaultFont();
        clCellValue& cell = GetColumn(i);
        if(cell.GetFont().IsOk()) { f = cell.GetFont(); }
        if(cell.GetTextColour().IsOk()) { colours.SetItemTextColour(cell.GetTextColour()); }
        if(cell.GetBgColour().IsOk()) { colours.SetItemBgColour(cell.GetBgColour()); }
        dc.SetFont(f);
        wxColour buttonColour = IsSelected() ? colours.GetSelbuttonColour() : colours.GetButtonColour();
        wxRect cellRect = GetCellRect(i);

        // We use a helper class to clip the drawings this ensures that if we exit the scope
        // the clipping region is restored properly
        clClipperHelper clipper(dc);
        if(hasHeader) { clipper.Clip(cellRect); }

        int textXOffset = cellRect.GetX();
        if((i == 0) && !IsListItem()) {
            // The expand button is only make sense for the first cell
            if(HasChildren()) {
                wxRect buttonRect = GetButtonRect();
                buttonRect.Deflate(1);
                textXOffset += buttonRect.GetWidth();
                if(m_tree->IsNativeTheme() && !IS_OSX) {
                    int flags = wxCONTROL_CURRENT;
                    if(IsExpanded()) { flags |= wxCONTROL_EXPANDED; }
                    int button_width = wxSystemSettings::GetMetric(wxSYS_SMALLICON_X);
                    wxRect modButtonRect = buttonRect;
                    modButtonRect.SetWidth(button_width);
                    modButtonRect.SetHeight(button_width);
                    modButtonRect = modButtonRect.CenterIn(buttonRect);
                    wxRendererNative::Get().DrawTreeItemButton(win, dc, modButtonRect, flags);
                } else {
                    wxRect buttonRect = GetButtonRect();
                    if(textXOffset >= cellRect.GetWidth()) {
                        // if we cant draw the button (off screen etc)
                        SetRects(GetItemRect(), wxRect());
                        continue;
                    }
                    buttonRect.Deflate((buttonRect.GetWidth() / 3), (buttonRect.GetHeight() / 3));
                    wxRect tribtn = buttonRect;
                    dc.SetPen(wxPen(buttonColour, 2));
                    if(IsExpanded()) {
                        tribtn.SetHeight(tribtn.GetHeight() - tribtn.GetHeight() / 2);
                        tribtn = tribtn.CenterIn(buttonRect);
                        wxPoint middleLeft = wxPoint((tribtn.GetLeft() + tribtn.GetWidth() / 2), tribtn.GetBottom());
                        dc.DrawLine(tribtn.GetTopLeft(), middleLeft);
                        dc.DrawLine(tribtn.GetTopRight(), middleLeft);
                    } else {
                        tribtn.SetWidth(tribtn.GetWidth() - tribtn.GetWidth() / 2);
                        tribtn = tribtn.CenterIn(buttonRect);

                        wxPoint middleLeft = wxPoint(tribtn.GetRight(), (tribtn.GetY() + (tribtn.GetHeight() / 2)));
                        wxPoint p1 = tribtn.GetTopLeft();
                        wxPoint p2 = tribtn.GetBottomLeft();
                        dc.DrawLine(p1, middleLeft);
                        dc.DrawLine(middleLeft, p2);
                    }
                }

            } else {
                wxRect buttonRect(rowRect);
                buttonRect.SetWidth(rowRect.GetHeight());
                buttonRect.Deflate(1);
                textXOffset += buttonRect.GetWidth();
                if(textXOffset >= cellRect.GetWidth()) {
                    SetRects(GetItemRect(), wxRect());
                    continue;
                }
            }
        }
        int itemIndent = IsListItem() ? clHeaderItem::X_SPACER : (GetIndentsCount() * m_tree->GetIndent());
        int bitmapIndex = cell.GetBitmapIndex();
        if(IsExpanded() && HasChildren() && cell.GetBitmapSelectedIndex() != wxNOT_FOUND) {
            bitmapIndex = cell.GetBitmapSelectedIndex();
        }

        // Draw checkbox
        if(cell.IsBool()) {
            // Render the checkbox
            textXOffset += X_SPACER;
            int checkboxSize = GetCheckBoxWidth(win);
            wxRect checkboxRect = wxRect(textXOffset, rowRect.GetY(), checkboxSize, checkboxSize);
            checkboxRect = checkboxRect.CenterIn(rowRect, wxVERTICAL);
            dc.SetPen(colours.GetItemTextColour());
            RenderCheckBox(win, dc, colours, checkboxRect, cell.GetValueBool());
            cell.SetCheckboxRect(checkboxRect);
            textXOffset += checkboxRect.GetWidth();
            textXOffset += X_SPACER;
        } else {
            cell.SetCheckboxRect(wxRect()); // clear the checkbox rect
        }

        // Draw the bitmap
        if(bitmapIndex != wxNOT_FOUND) {
            const wxBitmap& bmp = m_tree->GetBitmap(bitmapIndex);
            if(bmp.IsOk()) {
                textXOffset += IsListItem() ? 0 : X_SPACER;
                int bitmapY = rowRect.GetY() + ((rowRect.GetHeight() - bmp.GetScaledHeight()) / 2);
                // if((textXOffset + bmp.GetScaledWidth()) >= cellRect.GetWidth()) { continue; }
                dc.DrawBitmap(bmp, itemIndent + textXOffset, bitmapY, true);
                textXOffset += bmp.GetScaledWidth();
                textXOffset += X_SPACER;
            }
        }

        // Draw the text
        wxRect textRect(dc.GetTextExtent(cell.GetValueString()));
        textRect = textRect.CenterIn(rowRect, wxVERTICAL);
        int textY = textRect.GetY();
        int textX = (i == 0 ? itemIndent : clHeaderItem::X_SPACER) + textXOffset;
        RenderText(win, dc, colours, cell.GetValueString(), textX, textY, i);
        textXOffset += textRect.GetWidth();
        textXOffset += X_SPACER;

        if(cell.IsChoice()) {
            // draw the drop down arrow. Make it aligned to the right
            wxRect dropDownRect(cellRect.GetTopRight().x - rowRect.GetHeight(), rowRect.GetY(), rowRect.GetHeight(),
                                rowRect.GetHeight());
            dropDownRect = dropDownRect.CenterIn(rowRect, wxVERTICAL);
            DrawingUtils::DrawDropDownArrow(win, dc, dropDownRect, wxNullColour);
            // Keep the rect to test clicks
            cell.SetDropDownRect(dropDownRect);
            textXOffset += dropDownRect.GetWidth();
            textXOffset += X_SPACER;
            
            // Draw a separator line between the drop down arrow and the rest of the cell content
            dropDownRect.Deflate(3);
            dropDownRect = dropDownRect.CenterIn(rowRect, wxVERTICAL);
            dc.SetPen(wxPen(colours.GetHeaderVBorderColour(), 1, PEN_STYLE));
            dc.DrawLine(dropDownRect.GetTopLeft(), dropDownRect.GetBottomLeft());
            
        } else {
            cell.SetDropDownRect(wxRect());
        }

        if(!last_cell) {
            cellRect.SetHeight(rowRect.GetHeight());
            dc.SetPen(wxPen(colours.GetHeaderVBorderColour(), 1, PEN_STYLE));
            dc.DrawLine(cellRect.GetTopRight(), cellRect.GetBottomRight());
        }
    }
}

void clRowEntry::RenderText(wxWindow* win, wxDC& dc, const clColours& colours, const wxString& text, int x, int y,
                            size_t col)
{
    if(IsHighlight()) {
        const clMatchResult& hi = GetHighlightInfo();
        Str3Arr_t arr;
        if(!hi.Get(col, arr)) {
            RenderTextSimple(win, dc, colours, text, x, y, col);
            return;
        }
#ifdef __WXMSW__
        const wxColour& defaultTextColour =
            m_tree->IsNativeTheme() ? colours.GetItemTextColour()
                                    : (IsSelected() ? colours.GetSelItemTextColour() : colours.GetItemTextColour());
#else
        const wxColour& defaultTextColour = IsSelected() ? colours.GetSelItemTextColour() : colours.GetItemTextColour();
#endif
        const wxColour& matchBgColour = colours.GetMatchedItemBgText();
        const wxColour& matchTextColour = colours.GetMatchedItemText();
        int xx = x;
        wxRect rowRect = GetItemRect();
        for(size_t i = 0; i < arr.size(); ++i) {
            wxString str = arr[i];
            bool is_match = (i == 1); // the middle entry is always the matched string
            wxSize sz = dc.GetTextExtent(str);
            rowRect.SetX(xx);
            rowRect.SetWidth(sz.GetWidth());
            if(is_match) {
                // draw a match rectangle
                dc.SetPen(matchBgColour);
                dc.SetBrush(matchBgColour);
                dc.SetTextForeground(matchTextColour);
                dc.DrawRoundedRectangle(rowRect, 3.0);
            } else {
                dc.SetTextForeground(defaultTextColour);
            }
            dc.DrawText(str, xx, y);
            xx += sz.GetWidth();
        }
    } else {
        // No match
        RenderTextSimple(win, dc, colours, text, x, y, col);
    }
}

void clRowEntry::RenderTextSimple(wxWindow* win, wxDC& dc, const clColours& colours, const wxString& text, int x, int y,
                                  size_t col)
{
    wxUnusedVar(win);
    wxUnusedVar(col);
#ifdef __WXMSW__
    if(m_tree->IsNativeTheme()) {
        dc.SetTextForeground(colours.GetItemTextColour());
        dc.DrawText(text, x, y);
    } else {
        dc.SetTextForeground(IsSelected() ? colours.GetSelItemTextColour() : colours.GetItemTextColour());
        dc.DrawText(text, x, y);
    }
#else
    dc.SetTextForeground(IsSelected() ? colours.GetSelItemTextColour() : colours.GetItemTextColour());
    dc.DrawText(text, x, y);
#endif
}

size_t clRowEntry::GetChildrenCount(bool recurse) const
{
    if(!recurse) {
        return m_children.size();
    } else {
        size_t count = m_children.size();
        // Loop over the children and add its children count
        for(size_t i = 0; i < m_children.size(); ++i) {
            count += m_children[i]->GetChildrenCount(recurse);
        }
        return count;
    }
}

bool clRowEntry::IsVisible() const
{
    if(IsHidden()) { return false; }
    clRowEntry* parent = GetParent();
    while(parent) {
        if(!parent->IsExpanded()) { return false; }
        parent = parent->GetParent();
    }
    return true;
}

void clRowEntry::DeleteAllChildren()
{
    while(!m_children.empty()) {
        clRowEntry* c = m_children[0];
        // DeleteChild will remove it from the array
        DeleteChild(c);
    }
}

clRowEntry* clRowEntry::GetLastChild() const
{
    if(m_children.empty()) { return nullptr; }
    return m_children.back();
}

clRowEntry* clRowEntry::GetFirstChild() const
{
    if(m_children.empty()) { return nullptr; }
    return m_children[0];
}

void clRowEntry::SetHidden(bool b)
{
    if(b && !IsRoot()) { return; }
    SetFlag(kNF_Hidden, b);
    if(b) {
        m_indentsCount = -1;
    } else {
        m_indentsCount = 0;
    }
}

int clRowEntry::CalcItemWidth(wxDC& dc, int rowHeight, size_t col)
{
    wxUnusedVar(col);
    if(col >= m_cells.size()) { return 0; }

    clCellValue& cell = GetColumn(col);
    wxFont f = clScrolledPanel::GetDefaultFont();
    if(cell.GetFont().IsOk()) { f = cell.GetFont(); }
    dc.SetFont(f);

    int item_width = X_SPACER;
    if(cell.IsBool()) {
        // add the checkbox size
        item_width += rowHeight;
        item_width += X_SPACER;
    } else if(cell.IsChoice()) {
        item_width += rowHeight;
        item_width += X_SPACER;
    }

    wxSize textSize = dc.GetTextExtent(cell.GetValueString());
    if((col == 0) && !IsListItem()) {
        // always make room for the twist button
        item_width += rowHeight;
    }
    int bitmapIndex = cell.GetBitmapIndex();
    if(IsExpanded() && HasChildren() && cell.GetBitmapSelectedIndex() != wxNOT_FOUND) {
        bitmapIndex = cell.GetBitmapSelectedIndex();
    }

    if(bitmapIndex != wxNOT_FOUND) {
        const wxBitmap& bmp = m_tree->GetBitmap(bitmapIndex);
        if(bmp.IsOk()) {
            item_width += X_SPACER;
            item_width += bmp.GetScaledWidth();
            item_width += X_SPACER;
        }
    }
    if((col == 0) && !IsListItem()) {
        int itemIndent = (GetIndentsCount() * m_tree->GetIndent());
        item_width += itemIndent;
    }
    item_width += textSize.GetWidth();
    item_width += clHeaderItem::X_SPACER;
    return item_width;
}

void clRowEntry::SetBitmapIndex(int bitmapIndex, size_t col)
{
    clCellValue& cell = GetColumn(col);
    if(!cell.IsOk()) { return; }
    cell.SetBitmapIndex(bitmapIndex);
}

int clRowEntry::GetBitmapIndex(size_t col) const
{
    const clCellValue& cell = GetColumn(col);
    if(!cell.IsOk()) { return wxNOT_FOUND; }
    return cell.GetBitmapIndex();
}

void clRowEntry::SetBitmapSelectedIndex(int bitmapIndex, size_t col)
{
    clCellValue& cell = GetColumn(col);
    if(!cell.IsOk()) { return; }
    cell.SetBitmapSelectedIndex(bitmapIndex);
}

int clRowEntry::GetBitmapSelectedIndex(size_t col) const
{
    const clCellValue& cell = GetColumn(col);
    if(!cell.IsOk()) { return wxNOT_FOUND; }
    return cell.GetBitmapSelectedIndex();
}

void clRowEntry::SetLabel(const wxString& label, size_t col)
{
    clCellValue& cell = GetColumn(col);
    if(!cell.IsOk()) { return; }
    cell.SetValue(label);
}

const wxString& clRowEntry::GetLabel(size_t col) const
{
    const clCellValue& cell = GetColumn(col);
    if(!cell.IsOk()) {
        static wxString empty_string;
        return empty_string;
    }
    return cell.GetValueString();
}

void clRowEntry::SetChecked(bool checked, int bitmapIndex, const wxString& label, size_t col)
{
    clCellValue& cell = GetColumn(col);
    if(!cell.IsOk()) { return; }
    cell.SetValue(checked);
    cell.SetValue(label);
    cell.SetBitmapIndex(bitmapIndex);
    // Mark this type as 'bool'
    cell.SetType(clCellValue::kTypeBool);
}

bool clRowEntry::IsChecked(size_t col) const
{
    const clCellValue& cell = GetColumn(col);
    if(!cell.IsOk()) { return false; }
    return cell.GetValueBool();
}

const clCellValue& clRowEntry::GetColumn(size_t col) const
{
    if(col >= m_cells.size()) {
        static clCellValue null_column;
        return null_column;
    }
    return m_cells[col];
}

clCellValue& clRowEntry::GetColumn(size_t col)
{
    if(col >= m_cells.size()) {
        static clCellValue null_column;
        return null_column;
    }
    return m_cells[col];
}

void clRowEntry::SetBgColour(const wxColour& bgColour, size_t col)
{
    clCellValue& cell = GetColumn(col);
    if(!cell.IsOk()) { return; }
    cell.SetBgColour(bgColour);
}

void clRowEntry::SetFont(const wxFont& font, size_t col)
{
    clCellValue& cell = GetColumn(col);
    if(!cell.IsOk()) { return; }
    cell.SetFont(font);
}

void clRowEntry::SetTextColour(const wxColour& textColour, size_t col)
{
    clCellValue& cell = GetColumn(col);
    if(!cell.IsOk()) { return; }
    cell.SetTextColour(textColour);
}

const wxColour& clRowEntry::GetBgColour(size_t col) const
{
    const clCellValue& cell = GetColumn(col);
    if(!cell.IsOk()) {
        static wxColour invalid_colour;
        return invalid_colour;
    }
    return cell.GetBgColour();
}

const wxFont& clRowEntry::GetFont(size_t col) const
{
    const clCellValue& cell = GetColumn(col);
    if(!cell.IsOk()) {
        static wxFont invalid_font;
        return invalid_font;
    }
    return cell.GetFont();
}

const wxColour& clRowEntry::GetTextColour(size_t col) const
{
    const clCellValue& cell = GetColumn(col);
    if(!cell.IsOk()) {
        static wxColour invalid_colour;
        return invalid_colour;
    }
    return cell.GetTextColour();
}

wxRect clRowEntry::GetCellRect(size_t col) const
{
    if(m_tree && m_tree->GetHeader() && (col < m_tree->GetHeader()->size())) {
        // Check which column was clicked
        wxRect cellRect = m_tree->GetHeader()->Item(col).GetRect();

        const wxRect& itemRect = GetItemRect();
        // Make sure that the cellRect has all the correct attributes of the row
        cellRect.SetY(itemRect.GetY());
        // If we got h-scrollbar, adjust the X coordinate
        // cellRect.SetX(cellRect.GetX() - m_tree->GetFirstColumn());
        cellRect.SetHeight(itemRect.GetHeight());
        return cellRect;
    } else {
        return GetItemRect();
    }
}

const wxRect& clRowEntry::GetCheckboxRect(size_t col) const
{
    const clCellValue& cell = GetColumn(col);
    if(!cell.IsOk()) {
        static wxRect emptyRect;
        return emptyRect;
    }
    return cell.GetCheckboxRect();
}

const wxRect& clRowEntry::GetChoiceRect(size_t col) const
{
    const clCellValue& cell = GetColumn(col);
    if(!cell.IsOk()) {
        static wxRect emptyRect;
        return emptyRect;
    }
    return cell.GetDropDownRect();
}

void clRowEntry::RenderCheckBox(wxWindow* win, wxDC& dc, const clColours& colours, const wxRect& rect, bool checked)
{
#if 1
    wxUnusedVar(win);
    wxUnusedVar(colours);
    wxRendererNative::Get().DrawCheckBox(win, dc, rect, checked ? wxCONTROL_CHECKED : wxCONTROL_NONE);
#else
    dc.SetPen(wxPen(colours.GetBorderColour(), 2));
    dc.SetBrush(checked ? colours.GetBorderColour() : *wxTRANSPARENT_BRUSH);
    dc.DrawRoundedRectangle(rect, 2.0);
    if(checked) {
        wxRect innerRect = rect;
        innerRect.Deflate(5);
        const wxColour& penColour = IsSelected() ? colours.GetSelItemTextColour() : colours.GetDarkBorderColour();
        dc.SetPen(wxPen(penColour, 3));

        wxPoint p1, p2, p3;
        p1.x = innerRect.GetTopLeft().x;
        p1.y = innerRect.GetTopLeft().y + (innerRect.GetHeight() / 2);

        p2.x = innerRect.GetBottomLeft().x + (innerRect.GetWidth() / 3);
        p2.y = innerRect.GetBottomLeft().y;

        p3 = innerRect.GetTopRight();
        dc.DrawLine(p1, p2);
        dc.DrawLine(p2, p3);
    }
#endif
}

int clRowEntry::GetCheckBoxWidth(wxWindow* win)
{
    static int width = wxNOT_FOUND;
    if(width == wxNOT_FOUND) {
        width = wxRendererNative::Get().GetCheckBoxSize(win).GetWidth();
        if(width <= 0) {
            // set default value
            width = 20;
        }
    }
    return width;
}

void clRowEntry::SetChoice(bool b, size_t col)
{
    clCellValue& cell = GetColumn(col);
    if(!cell.IsOk()) { return; }
    cell.SetType(clCellValue::kTypeChoice);
}

bool clRowEntry::IsChoice(size_t col) const
{
    const clCellValue& cell = GetColumn(col);
    if(!cell.IsOk()) { return false; }
    return cell.IsChoice();
}
