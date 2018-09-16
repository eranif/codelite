#include "clCellValue.h"
#include "clHeaderItem.h"
#include "clRowEntry.h"
#include "clTreeCtrl.h"
#include <algorithm>
#include <functional>
#include <wx/dataview.h>
#include <wx/dc.h>
#include <wx/settings.h>

#ifdef __WXMSW__
#define PEN_STYLE wxPENSTYLE_SHORT_DASH
#else
#define PEN_STYLE wxPENSTYLE_DOT
#endif

clRowEntry::clRowEntry(clTreeCtrl* tree, const wxString& label, int bitmapIndex, int bitmapSelectedIndex)
    : m_tree(tree)
    , m_model(tree ? &tree->GetModel() : nullptr)
{
    // Fill the verctor with items constructed using the _non_ default constructor
    // to makes sure that IsOk() returns TRUE
    m_cells.resize(m_tree->GetHeader().empty() ? 1 : m_tree->GetHeader().size(),
                   clCellValue("", -1, -1)); // at least one column
    clCellValue cv(label, bitmapIndex, bitmapSelectedIndex);
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

void clRowEntry::GetNextItems(int count, clRowEntry::Vec_t& items)
{
    items.reserve(count);
    if(!this->IsHidden()) { items.push_back(this); }
    clRowEntry* next = GetNext();
    while(next) {
        if(next->IsVisible() && !next->IsHidden()) { items.push_back(next); }
        if((int)items.size() == count) { return; }
        next = next->GetNext();
    }
}

void clRowEntry::GetPrevItems(int count, clRowEntry::Vec_t& items)
{
    items.reserve(count);
    if(!this->IsHidden()) { items.insert(items.begin(), this); }
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

void clRowEntry::Render(wxWindow* win, wxDC& dc, const clColours& c, int row_index)
{
    wxRect rowRect = GetItemRect();
    bool zebraColouring = (m_tree->HasStyle(wxTR_ROW_LINES) || m_tree->HasStyle(wxDV_ROW_LINES));
    bool even_row = ((row_index % 2) == 0);

    // Not cell related
    clColours colours = c;
    if(zebraColouring) {
        // Set Zebra colouring, only if no user colour was provided for the given line
        colours.SetItemBgColour(even_row ? c.GetAlternateColourEven() : c.GetAlternateColourOdd());
    }

    // Override default item bg colour with the user's one
    if(GetBgColour().IsOk()) { colours.SetItemBgColour(GetBgColour()); }
    wxRect selectionRect = rowRect;
    wxPoint deviceOrigin = dc.GetDeviceOrigin();
    selectionRect.SetX(-deviceOrigin.x);
    if(IsSelected()) {
        dc.SetPen(win->HasFocus() ? colours.GetSelItemBgColour() : colours.GetSelItemBgColourNoFocus());
        dc.SetBrush(win->HasFocus() ? colours.GetSelItemBgColour() : colours.GetSelItemBgColourNoFocus());
        dc.DrawRectangle(selectionRect);
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
        wxSize textSize = dc.GetTextExtent(cell.GetText());
        int textY = rowRect.GetY() + (m_tree->GetLineHeight() - textSize.GetHeight()) / 2;
        // Draw the button
        wxRect cellRect = m_tree->GetHeader().empty() ? rowRect : m_tree->GetHeader().Item(i).GetRect();
        cellRect.SetY(rowRect.GetY());
        int textXOffset = cellRect.GetX();
        if(i == 0) {
            // The expand button is only make sense for the first cell
            if(HasChildren()) {
                wxPoint pts[3];
                wxRect buttonRect = GetButtonRect();
                textXOffset += buttonRect.GetWidth();
                buttonRect.Deflate((buttonRect.GetWidth() / 3), (buttonRect.GetHeight() / 3));
                if(IsExpanded()) {
                    pts[0] = buttonRect.GetTopRight();
                    pts[1] = buttonRect.GetBottomRight();
                    pts[2] = buttonRect.GetBottomLeft();
                    dc.SetBrush(buttonColour);
                    dc.SetPen(buttonColour);
                    dc.DrawPolygon(3, pts);
                } else {
                    pts[0] = buttonRect.GetTopLeft();
                    pts[1] = buttonRect.GetBottomLeft();
                    pts[2].x = buttonRect.GetRight();
                    pts[2].y = (buttonRect.GetY() + (buttonRect.GetHeight() / 2));
                    dc.SetBrush(*wxTRANSPARENT_BRUSH);
                    dc.SetPen(buttonColour);
                    dc.DrawPolygon(3, pts);
                }
            } else {
                textXOffset += rowRect.GetHeight();
            }
        }
        int itemIndent = (GetIndentsCount() * m_tree->GetIndent());
        int bitmapIndex = cell.GetBitmapIndex();
        if(IsExpanded() && HasChildren() && cell.GetBitmapSelectedIndex() != wxNOT_FOUND) {
            bitmapIndex = cell.GetBitmapSelectedIndex();
        }

        if(bitmapIndex != wxNOT_FOUND) {
            const wxBitmap& bmp = m_tree->GetBitmap(bitmapIndex);
            if(bmp.IsOk()) {
                textXOffset += X_SPACER;
                int bitmapY = rowRect.GetY() + ((rowRect.GetHeight() - bmp.GetScaledHeight()) / 2);
                dc.DrawBitmap(bmp, itemIndent + textXOffset, bitmapY);
                textXOffset += bmp.GetScaledWidth();
                textXOffset += X_SPACER;
            }
        }
        dc.SetTextForeground(IsSelected() ? colours.GetSelItemTextColour() : colours.GetItemTextColour());

        // Draw the indentation only for the first cell
        dc.DrawText(cell.GetText(), (i == 0 ? itemIndent : clHeaderItem::X_SPACER) + textXOffset, textY);

        if(!last_cell) {
            dc.SetPen(wxPen(colours.GetHeaderVBorderColour(), 1, PEN_STYLE));
            dc.DrawLine(cellRect.GetTopRight(), cellRect.GetBottomRight());
        }
    }
}

size_t clRowEntry::GetChildrenCount(bool recurse) const
{
    if(!recurse) {
        return m_children.size();
    } else {
        size_t count = m_children.size();
        for(size_t i = 0; i < count; ++i) {
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

    wxSize textSize = dc.GetTextExtent(cell.GetText());
    int item_width = 5;
    // always make room for the twist button
    item_width += rowHeight;

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
    if(col == 0) {
        int itemIndent = (GetIndentsCount() * m_tree->GetIndent());
        item_width += itemIndent;
    }
    item_width += textSize.GetWidth();
    item_width += (clHeaderItem::X_SPACER * 2);
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
    cell.SetText(label);
}

const wxString& clRowEntry::GetLabel(size_t col) const
{
    const clCellValue& cell = GetColumn(col);
    if(!cell.IsOk()) {
        static wxString empty_string;
        return empty_string;
    }
    return cell.GetText();
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
