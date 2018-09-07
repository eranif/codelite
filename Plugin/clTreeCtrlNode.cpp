#include "clTreeCtrl.h"
#include "clTreeCtrlNode.h"
#include <functional>
#include <wx/dc.h>
#include <wx/settings.h>

clTreeCtrlNode::clTreeCtrlNode(clTreeCtrl* tree)
    : m_tree(tree)
    , m_model(tree ? &tree->GetModel() : nullptr)
{
}

clTreeCtrlNode::clTreeCtrlNode(clTreeCtrl* tree, const wxString& label, int bitmapIndex, int bitmapSelectedIndex)
    : m_tree(tree)
    , m_model(tree ? &tree->GetModel() : nullptr)
    , m_label(label)
    , m_bitmapIndex(bitmapIndex)
    , m_bitmapSelectedIndex(bitmapSelectedIndex)
{
}

clTreeCtrlNode::~clTreeCtrlNode()
{
    // Delete all the node children
    DeleteAllChildren();
    wxDELETE(m_clientData);

    // Notify the model that a selection is being deleted
    if(m_model) { m_model->NodeDeleted(this); }
}

void clTreeCtrlNode::ConnectNodes(clTreeCtrlNode* first, clTreeCtrlNode* second)
{
    if(first) { first->m_next = this; }
    this->m_prev = first;
    this->m_next = second;
    if(second) { second->m_prev = this; }
}

void clTreeCtrlNode::InsertChild(clTreeCtrlNode* child, clTreeCtrlNode* prev)
{
    child->SetParent(this);
    child->SetIndentsCount(GetIndentsCount() + 1);

    // We need the last item of this subtree (prev 'this' is the root)
    if(prev == nullptr) {
        // make it the first item
        m_children.insert(m_children.begin(), child);
    } else {
        // Insert the item in the parent children list
        clTreeCtrlNode::Vec_t::iterator iter = m_children.end();
        iter = std::find_if(m_children.begin(), m_children.end(), [&](clTreeCtrlNode* c) { return c == prev; });
        if(iter != m_children.end()) { ++iter; }
        // if iter is end(), than the is actually appending the item
        m_children.insert(iter, child);
    }

    // Connect the linked list for sequential iteration
    clTreeCtrlNode::Vec_t::iterator iterCur
        = std::find_if(m_children.begin(), m_children.end(), [&](clTreeCtrlNode* c) { return c == child; });

    clTreeCtrlNode* nodeBefore = nullptr;
    // Find the item before and after
    if(iterCur == m_children.begin()) {
        nodeBefore = child->GetParent(); // "this"
    } else {
        --iterCur;
        clTreeCtrlNode* prevSibling = (*iterCur);
        while(prevSibling && prevSibling->HasChildren()) { prevSibling = prevSibling->GetLastChild(); }
        nodeBefore = prevSibling;
    }
    child->ConnectNodes(nodeBefore, nodeBefore->m_next);
}

void clTreeCtrlNode::AddChild(clTreeCtrlNode* child)
{
    InsertChild(child, m_children.empty() ? nullptr : m_children.back());
}

void clTreeCtrlNode::SetParent(clTreeCtrlNode* parent)
{
    if(m_parent) { m_parent->DeleteChild(this); }
    m_parent = parent;
}

void clTreeCtrlNode::DeleteChild(clTreeCtrlNode* child)
{
    // first remove all of its children
    // do this in a while loop since 'child->RemoveChild(c);' will alter
    // the array and will invalidate all iterators
    while(!child->m_children.empty()) {
        clTreeCtrlNode* c = child->m_children[0];
        child->DeleteChild(c);
    }
    // Connect the list
    clTreeCtrlNode* prev = child->m_prev;
    clTreeCtrlNode* next = child->m_next;
    if(prev) { prev->m_next = next; }
    if(next) { next->m_prev = prev; }
    // Now disconnect this child from this node
    clTreeCtrlNode::Vec_t::iterator iter
        = std::find_if(m_children.begin(), m_children.end(), [&](clTreeCtrlNode* c) { return c == child; });
    if(iter != m_children.end()) { m_children.erase(iter); }
    wxDELETE(child);
}

int clTreeCtrlNode::GetExpandedLines() const
{
    clTreeCtrlNode* node = const_cast<clTreeCtrlNode*>(this);
    int counter = 0;
    while(node) {
        if(node->IsVisible()) { ++counter; }
        node = node->m_next;
    }
    return counter;
}

void clTreeCtrlNode::GetNextItems(int count, clTreeCtrlNode::Vec_t& items)
{
    items.reserve(count);
    items.push_back(this);
    clTreeCtrlNode* next = GetNext();
    while(next) {
        if(next->IsVisible()) { items.push_back(next); }
        if((int)items.size() == count) { return; }
        next = next->GetNext();
    }
}

void clTreeCtrlNode::GetPrevItems(int count, clTreeCtrlNode::Vec_t& items)
{
    items.reserve(count);
    items.insert(items.begin(), this);
    clTreeCtrlNode* prev = GetPrev();
    while(prev) {
        if(prev->IsVisible()) { items.insert(items.begin(), prev); }
        if((int)items.size() == count) { return; }
        prev = prev->GetPrev();
    }
}

clTreeCtrlNode* clTreeCtrlNode::GetVisibleItem(int index)
{
    clTreeCtrlNode::Vec_t items;
    GetNextItems(index, items);
    if((int)items.size() != index) { return nullptr; }
    return items.back();
}

void clTreeCtrlNode::UnselectAll()
{
    clTreeCtrlNode* item = const_cast<clTreeCtrlNode*>(this);
    while(item) {
        item->SetSelected(false);
        item = item->GetNext();
    }
}

bool clTreeCtrlNode::SetExpanded(bool b)
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

void clTreeCtrlNode::ClearRects()
{
    m_buttonRect = wxRect();
    m_itemRect = wxRect();
}

void clTreeCtrlNode::Render(wxDC& dc, const clColours& c, int visibileIndex)
{
    wxRect itemRect = GetItemRect();
    bool zebraColouring = (m_tree->GetTreeStyle() & wxTR_ROW_LINES);
    bool even_row = ((visibileIndex % 2) == 0);

    clColours colours = c;
    wxFont f = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
    if(GetFont().IsOk()) { f = GetFont(); }
    if(GetTextColour().IsOk()) { colours.SetItemTextColour(GetTextColour()); }
    if(GetBgColour().IsOk()) { colours.SetItemBgColour(GetBgColour()); }
    dc.SetFont(f);

    if(zebraColouring) {
        colours.SetItemBgColour(even_row ? colours.GetAlternateColourEven() : colours.GetAlternateColourOdd());
    }

    if(IsSelected() || IsHovered()) {
        dc.SetBrush(IsSelected() ? colours.GetSelItemBgColour() : colours.GetHoverBgColour());
        dc.SetPen(IsSelected() ? colours.GetSelItemBgColour() : colours.GetHoverBgColour());
        dc.DrawRoundedRectangle(itemRect, 1.5);
    } else if(colours.GetItemBgColour().IsOk()) {
        dc.SetBrush(colours.GetItemBgColour());
        dc.SetPen(colours.GetItemBgColour());
        dc.DrawRectangle(itemRect);
    }

    wxSize textSize = dc.GetTextExtent(GetLabel());
    int textY = itemRect.GetY() + (m_tree->GetLineHeight() - textSize.GetHeight()) / 2;
    // Draw the button
    int textXOffset = 0;
    if(HasChildren()) {
        wxPoint pts[3];
        wxRect buttonRect = GetButtonRect();
        textXOffset += buttonRect.GetWidth();
        buttonRect.Deflate((buttonRect.GetWidth() / 3), (buttonRect.GetHeight() / 3));
        if(IsExpanded()) {
            pts[0] = buttonRect.GetTopRight();
            pts[1] = buttonRect.GetBottomRight();
            pts[2] = buttonRect.GetBottomLeft();
            dc.SetBrush(colours.GetButtonColour());
            dc.SetPen(colours.GetButtonColour());
            dc.DrawPolygon(3, pts);
        } else {
            pts[0] = buttonRect.GetTopLeft();
            pts[1] = buttonRect.GetBottomLeft();
            pts[2].x = buttonRect.GetRight();
            pts[2].y = (buttonRect.GetY() + (buttonRect.GetHeight() / 2));
            dc.SetBrush(*wxTRANSPARENT_BRUSH);
            dc.SetPen(colours.GetButtonColour());
            dc.DrawPolygon(3, pts);
        }
    } else {
        textXOffset += itemRect.GetHeight();
    }

    int itemIndent = (GetIndentsCount() * m_tree->GetIndent());
    int bitmapIndex = GetBitmapIndex();
    if(IsExpanded() && HasChildren() && GetBitmapSelectedIndex() != wxNOT_FOUND) {
        bitmapIndex = GetBitmapSelectedIndex();
    }

    if(bitmapIndex != wxNOT_FOUND) {
        const wxBitmap& bmp = m_tree->GetBitmap(bitmapIndex);
        if(bmp.IsOk()) {
            textXOffset += X_SPACER;
            int bitmapY = itemRect.GetY() + ((itemRect.GetHeight() - bmp.GetScaledHeight()) / 2);
            dc.DrawBitmap(bmp, itemIndent + textXOffset, bitmapY);
            textXOffset += bmp.GetScaledWidth();
            textXOffset += X_SPACER;
        }
    }
    dc.SetTextForeground(IsSelected() ? colours.GetSelItemTextColour() : colours.GetItemTextColour());
    dc.DrawText(GetLabel(), itemIndent + textXOffset, textY);
}

size_t clTreeCtrlNode::GetChildrenCount(bool recurse) const
{
    if(!recurse) {
        return m_children.size();
    } else {
        size_t count = m_children.size();
        for(size_t i = 0; i < count; ++i) { count += m_children[i]->GetChildrenCount(recurse); }
        return count;
    }
}

bool clTreeCtrlNode::IsVisible() const
{
    clTreeCtrlNode* parent = GetParent();
    while(parent) {
        if(!parent->IsExpanded()) { return false; }
        parent = parent->GetParent();
    }
    return true;
}

void clTreeCtrlNode::DeleteAllChildren()
{
    while(!m_children.empty()) {
        clTreeCtrlNode* c = m_children[0];
        // DeleteChild will remove it from the array
        DeleteChild(c);
    }
}

clTreeCtrlNode* clTreeCtrlNode::GetLastChild() const
{
    if(m_children.empty()) { return nullptr; }
    return m_children.back();
}

clTreeCtrlNode* clTreeCtrlNode::GetFirstChild() const
{
    if(m_children.empty()) { return nullptr; }
    return m_children[0];
}

void clTreeCtrlNode::SetHidden(bool b)
{
    if(b && !IsRoot()) { return; }
    SetFlag(kNF_Hidden, b);
    if(b) {
        m_indentsCount = -1;
    } else {
        m_indentsCount = 0;
    }
}

void clColours::InitDefaults()
{
    itemTextColour = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);
    selItemTextColour = itemTextColour;
    selItemBgColour = wxColour("rgb(199,203,209)");
    buttonColour = wxSystemSettings::GetColour(wxSYS_COLOUR_3DSHADOW);
    hoverBgColour = wxColour("rgb(219,221,224)");
    bgColour = wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);
    scrolBarButton = wxSystemSettings::GetColour(wxSYS_COLOUR_3DSHADOW);
    scrollBarBgColour = bgColour.ChangeLightness(95); // A bit darker
    itemBgColour = bgColour;
    alternateColourEven = bgColour.ChangeLightness(105);
    alternateColourOdd = bgColour.ChangeLightness(95);
}

void clColours::InitDarkDefaults()
{
    bgColour = wxColour("#1c2833");
    itemTextColour = wxColour("#eaecee");
    selItemTextColour = *wxWHITE;
    selItemBgColour = wxColour("#566573");
    buttonColour = itemTextColour;
    hoverBgColour = wxColour("#2c3e50");
    itemBgColour = bgColour;
    scrolBarButton = selItemBgColour;
    scrollBarBgColour = wxColour("#212f3d"); // A bit darker
    alternateColourEven = bgColour.ChangeLightness(105);
    alternateColourOdd = bgColour.ChangeLightness(95);
}
