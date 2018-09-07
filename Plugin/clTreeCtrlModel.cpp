#include "clTreeCtrl.h"
#include "clTreeCtrlModel.h"
#include <algorithm>
#include <wx/dc.h>
#include <wx/settings.h>
#include <wx/treebase.h>

#define INDENT_SIZE 16

//------------------------------------------------
//------------------------------------------------
//------------------------------------------------
//------------------------------------------------

clTreeCtrlModel::clTreeCtrlModel(clTreeCtrl* tree)
    : m_tree(tree)
{
    // Setup a default compare function
    m_shouldInsertBeforeFunc = [&](const wxTreeItemId& a, const wxTreeItemId& b) {
        return ToPtr(a)->GetLabel().CmpNoCase(ToPtr(b)->GetLabel()) < 0;
    };
}

clTreeCtrlModel::~clTreeCtrlModel()
{
    m_shutdown = true; // Disable events
    wxDELETE(m_root);
}

void clTreeCtrlModel::GetNextItems(clTreeCtrlNode* from, int count, clTreeCtrlNode::Vec_t& items) const
{
    return from->GetNextItems(count, items);
}

void clTreeCtrlModel::GetPrevItems(clTreeCtrlNode* from, int count, clTreeCtrlNode::Vec_t& items) const
{
    return from->GetPrevItems(count, items);
}

wxTreeItemId clTreeCtrlModel::AddRoot(const wxString& text, int image, int selImage, wxTreeItemData* data)
{
    if(m_root) { return wxTreeItemId(m_root); }
    m_root = new clTreeCtrlNode(m_tree);
    m_root->SetLabel(text);
    m_root->SetBitmapIndex(image);
    m_root->SetBitmapSelectedIndex(selImage);
    m_root->SetClientData(data);
    if(m_tree->GetTreeStyle() & wxTR_HIDE_ROOT) {
        m_root->SetHidden(true);
        m_root->SetExpanded(true);
    }
    return wxTreeItemId(m_root);
}

wxTreeItemId clTreeCtrlModel::GetRootItem() const
{
    if(!m_root) { return wxTreeItemId(); }
    return wxTreeItemId(const_cast<clTreeCtrlNode*>(m_root));
}

void clTreeCtrlModel::UnselectAll()
{
    for(size_t i = 0; i < m_selectedItems.size(); ++i) { m_selectedItems[i]->SetSelected(false); }
    m_selectedItems.clear();
}

void clTreeCtrlModel::SelectItem(const wxTreeItemId& item, bool select, bool addSelection, bool clear_old_selection)
{
    clTreeCtrlNode* child = ToPtr(item);
    if(!child) return;

    if(child->IsHidden()) { return; }

    if(clear_old_selection && !ClearSelections(item != GetSingleSelection())) { return; }

    if(select) {
        clTreeCtrlNode::Vec_t::iterator iter = std::find_if(
            m_selectedItems.begin(), m_selectedItems.end(), [&](clTreeCtrlNode* p) { return (p == child); });
        // If the item is already selected, don't select it again
        if(iter != m_selectedItems.end()) { return; }
    }

    if(IsMultiSelection() && addSelection) {
        // If we are unselecting it, remove it from the array
        clTreeCtrlNode::Vec_t::iterator iter = std::find_if(
            m_selectedItems.begin(), m_selectedItems.end(), [&](clTreeCtrlNode* p) { return (p == child); });
        if(iter != m_selectedItems.end() && !select) { m_selectedItems.erase(iter); }
    } else {
        if(!ClearSelections(item != GetSingleSelection())) { return; }
    }
    child->SetSelected(select);
    if(select) {
        m_selectedItems.push_back(child);
        wxTreeEvent evt(wxEVT_TREE_SEL_CHANGED);
        evt.SetEventObject(m_tree);
        evt.SetItem(wxTreeItemId(child));
        SendEvent(evt);
    }
}

void clTreeCtrlModel::Clear()
{
    m_selectedItems.clear();
    for(size_t i = 0; i < m_onScreenItems.size(); ++i) { m_onScreenItems[i]->ClearRects(); }
    m_onScreenItems.clear();
}

void clTreeCtrlModel::SetOnScreenItems(const clTreeCtrlNode::Vec_t& items)
{
    // Clear the old visible items. But only, if the item does not appear in both lists
    for(size_t i = 0; i < m_onScreenItems.size(); ++i) {
        clTreeCtrlNode* visibleItem = m_onScreenItems[i];
        clTreeCtrlNode::Vec_t::const_iterator iter
            = std::find_if(items.begin(), items.end(), [&](clTreeCtrlNode* item) { return item == visibleItem; });
        if(iter == items.end()) { m_onScreenItems[i]->ClearRects(); }
    }
    m_onScreenItems = items;
}

bool clTreeCtrlModel::ExpandToItem(const wxTreeItemId& item)
{
    clTreeCtrlNode* child = ToPtr(item);
    if(!child) { return false; }

    clTreeCtrlNode* parent = child->GetParent();
    while(parent) {
        if(!parent->SetExpanded(true)) { return false; }
        parent = parent->GetParent();
    }
    return true;
}

wxTreeItemId clTreeCtrlModel::AppendItem(
    const wxTreeItemId& parent, const wxString& text, int image, int selImage, wxTreeItemData* data)
{
    clTreeCtrlNode* parentNode = nullptr;
    if(!parent.IsOk()) { return wxTreeItemId(); }
    parentNode = ToPtr(parent);

    clTreeCtrlNode* child = new clTreeCtrlNode(m_tree, text, image, selImage);
    child->SetClientData(data);
    // Find the best insertion point
    clTreeCtrlNode* prevItem = nullptr;
    if(m_shouldInsertBeforeFunc) {
        const clTreeCtrlNode::Vec_t& children = parentNode->GetChildren();
        // Loop over the parent's children and add execute the compare function
        wxTreeItemId newItem(child);
        for(int i = ((int)children.size() - 1); i >= 0; --i) {
            if(!m_shouldInsertBeforeFunc(newItem, children[i])) {
                // Our item should be placed _after_ children[i]s
                prevItem = ToPtr(children[i]);
                break;
            }
        }
        parentNode->InsertChild(child, prevItem);
    } else {
        parentNode->AddChild(child);
    }
    return wxTreeItemId(child);
}

wxTreeItemId clTreeCtrlModel::InsertItem(const wxTreeItemId& parent, const wxTreeItemId& previous, const wxString& text,
    int image, int selImage, wxTreeItemData* data)
{
    if(!parent.IsOk()) { return wxTreeItemId(); }
    if(!previous.IsOk()) { return wxTreeItemId(); }

    clTreeCtrlNode* pPrev = ToPtr(previous);
    clTreeCtrlNode* parentNode = ToPtr(parent);
    if(pPrev->GetParent() != parentNode) { return wxTreeItemId(); }

    clTreeCtrlNode* child = new clTreeCtrlNode(m_tree, text, image, selImage);
    child->SetClientData(data);
    parentNode->InsertChild(child, pPrev);
    return wxTreeItemId(child);
}

void clTreeCtrlModel::ExpandAllChildren(const wxTreeItemId& item) { DoExpandAllChildren(item, true); }

void clTreeCtrlModel::CollapseAllChildren(const wxTreeItemId& item) { DoExpandAllChildren(item, false); }

void clTreeCtrlModel::DoExpandAllChildren(const wxTreeItemId& item, bool expand)
{
    clTreeCtrlNode* p = ToPtr(item);
    if(!p) { return; }
    while(p) {
        if(p->HasChildren()) {
            if(expand && !p->IsExpanded()) {
                p->SetExpanded(true);
            } else if(!expand && p->IsExpanded()) {
                p->SetExpanded(false);
            }
        }
        p = p->GetNext();
    }
}

wxTreeItemId clTreeCtrlModel::GetItemBefore(const wxTreeItemId& item, bool visibleItem) const
{
    clTreeCtrlNode* p = ToPtr(item);
    if(!p) { return wxTreeItemId(); }
    p = p->GetPrev();
    while(p) {
        if(visibleItem && (!p->IsVisible() || p->IsHidden())) {
            p = p->GetPrev();
            continue;
        }
        break;
    }
    return wxTreeItemId(p);
}

wxTreeItemId clTreeCtrlModel::GetItemAfter(const wxTreeItemId& item, bool visibleItem) const
{
    clTreeCtrlNode* p = ToPtr(item);
    if(!p) { return wxTreeItemId(); }
    p = p->GetNext();
    while(p) {
        if(visibleItem && !p->IsVisible()) {
            p = p->GetNext();
            continue;
        }
        break;
    }
    return wxTreeItemId(p);
}

void clTreeCtrlModel::DeleteItem(const wxTreeItemId& item)
{
    clTreeCtrlNode* node = ToPtr(item);
    if(!node) { return; }
    node->DeleteAllChildren();

    // Send the delete event
    wxTreeEvent event(wxEVT_TREE_DELETE_ITEM);
    event.SetEventObject(m_tree);
    event.SetItem(item);
    SendEvent(event);

    // Delete the item itself
    if(node->GetParent()) {
        node->GetParent()->DeleteChild(node);
    } else {
        // The root item
        m_root = nullptr;
    }
}

void clTreeCtrlModel::NodeDeleted(clTreeCtrlNode* node)
{
    // Clear the various caches
    {
        clTreeCtrlNode::Vec_t::iterator iter = std::find_if(
            m_selectedItems.begin(), m_selectedItems.end(), [&](clTreeCtrlNode* n) { return n == node; });
        if(iter != m_selectedItems.end()) {
            m_selectedItems.erase(iter);
            if(m_selectedItems.empty()) {
                // Dont leave the tree without a selected item
                if(node->GetNext()) { SelectItem(wxTreeItemId(node->GetNext())); }
            }
        }
    }

    {
        clTreeCtrlNode::Vec_t::iterator iter = std::find_if(
            m_onScreenItems.begin(), m_onScreenItems.end(), [&](clTreeCtrlNode* n) { return n == node; });
        if(iter != m_onScreenItems.end()) { m_onScreenItems.erase(iter); }
    }
    {
        if(m_firstItemOnScreen == node) { m_firstItemOnScreen = nullptr; }
    }
    {
        if(m_root == node) { m_root = nullptr; }
    }
}

bool clTreeCtrlModel::NodeExpanding(clTreeCtrlNode* node, bool expanding)
{
    wxTreeEvent before(expanding ? wxEVT_TREE_ITEM_EXPANDING : wxEVT_TREE_ITEM_COLLAPSING);
    before.SetItem(wxTreeItemId(node));
    before.SetEventObject(m_tree);
    SendEvent(before);
    return before.IsAllowed();
}

void clTreeCtrlModel::NodeExpanded(clTreeCtrlNode* node, bool expanded)
{
    wxTreeEvent after(expanded ? wxEVT_TREE_ITEM_EXPANDED : wxEVT_TREE_ITEM_COLLAPSED);
    after.SetItem(wxTreeItemId(node));
    after.SetEventObject(m_tree);
    SendEvent(after);
}

bool clTreeCtrlModel::IsSingleSelection() const { return m_tree && !(m_tree->GetTreeStyle() & wxTR_MULTIPLE); }

bool clTreeCtrlModel::IsMultiSelection() const { return m_tree && (m_tree->GetTreeStyle() & wxTR_MULTIPLE); }

bool clTreeCtrlModel::SendEvent(wxEvent& event)
{
    if(m_shutdown) { return false; }
    return m_tree->GetEventHandler()->ProcessEvent(event);
}

wxTreeItemId clTreeCtrlModel::GetSingleSelection() const
{
    if(m_selectedItems.empty()) { return wxTreeItemId(); }
    return wxTreeItemId(m_selectedItems.back());
}

int clTreeCtrlModel::GetItemIndex(clTreeCtrlNode* item) const
{
    if(item == NULL) { return 0; }
    if(!m_root) { return wxNOT_FOUND; }
    int counter = 0;
    clTreeCtrlNode* current = m_root;
    while(current) {
        if(current == item) { return counter; }
        if(current->IsVisible()) { ++counter; }
        current = current->GetNext();
    }
    return wxNOT_FOUND;
}

bool clTreeCtrlModel::GetRange(clTreeCtrlNode* from, clTreeCtrlNode* to, clTreeCtrlNode::Vec_t& items) const
{
    items.clear();
    if(from == nullptr || to == nullptr) { return false; }
    if(from == nullptr) {
        items.push_back(to);
        return true;
    }
    if(to == nullptr) {
        items.push_back(from);
        return true;
    }
    if(from == to) {
        items.push_back(from);
        return true;
    }

    int index1 = GetItemIndex(from);
    int index2 = GetItemIndex(to);

    clTreeCtrlNode* start_item = index1 > index2 ? to : from;
    clTreeCtrlNode* end_item = index1 > index2 ? from : to;
    clTreeCtrlNode* current = start_item;
    while(current) {
        if(current == end_item) {
            items.push_back(current);
            break;
        }
        if(current->IsVisible()) { items.push_back(current); }
        current = current->GetNext();
    }
    return true;
}

size_t clTreeCtrlModel::GetExpandedLines() const
{
    if(!GetRoot()) { return 0; }
    return m_root->GetExpandedLines();
}

clTreeCtrlNode* clTreeCtrlModel::GetItemFromIndex(int index) const
{
    if(index < 0) { return nullptr; }
    if(!m_root) { return nullptr; }
    int curIndex = -1;
    clTreeCtrlNode* current = m_root;
    while(current) {
        if(current->IsVisible()) { ++curIndex; }
        if(curIndex == index) { return current; }
        current = current->GetNext();
    }
    return nullptr;
}

void clTreeCtrlModel::SelectChildren(const wxTreeItemId& item)
{
    clTreeCtrlNode* parent = ToPtr(item);
    if(!parent) { return; }

    if(!ClearSelections(true)) { return; }
    std::for_each(parent->GetChildren().begin(), parent->GetChildren().end(),
        [&](clTreeCtrlNode* child) { AddSelection(wxTreeItemId(child)); });
}

clTreeCtrlNode* clTreeCtrlModel::GetNextSibling(clTreeCtrlNode* item) const
{
    if(!item->GetParent()) { return nullptr; }
    const clTreeCtrlNode::Vec_t& children = item->GetParent()->GetChildren();
    clTreeCtrlNode::Vec_t::const_iterator iter
        = std::find_if(children.begin(), children.end(), [&](clTreeCtrlNode* sibling) { return (sibling == item); });
    // If we got a match, move to the next item
    if(iter != children.end()) { ++iter; }
    return ((iter == children.end()) ? nullptr : (*iter));
}

clTreeCtrlNode* clTreeCtrlModel::GetPrevSibling(clTreeCtrlNode* item) const
{
    if(!item->GetParent()) { return nullptr; }
    const clTreeCtrlNode::Vec_t& children = item->GetParent()->GetChildren();
    clTreeCtrlNode::Vec_t::const_iterator iter
        = std::find_if(children.begin(), children.end(), [&](clTreeCtrlNode* sibling) { return (sibling == item); });
    // If we got a match, move to the next item
    if((iter != children.end()) && (iter != children.begin())) { --iter; }
    return ((iter == children.end()) ? nullptr : (*iter));
}

void clTreeCtrlModel::AddSelection(const wxTreeItemId& item)
{
    clTreeCtrlNode* child = ToPtr(item);
    if(!child) return;
    if(child->IsHidden()) { return; }

    clTreeCtrlNode::Vec_t::iterator iter
        = std::find_if(m_selectedItems.begin(), m_selectedItems.end(), [&](clTreeCtrlNode* p) { return (p == child); });
    // If the item is already selected, don't select it again
    if(iter != m_selectedItems.end()) { return; }

    // if we already got selections, notify about the change
    if(!m_selectedItems.empty()) {
        wxTreeEvent evt(wxEVT_TREE_SEL_CHANGING);
        evt.SetEventObject(m_tree);
        evt.SetOldItem(GetSingleSelection());
        SendEvent(evt);
        if(!evt.IsAllowed()) { return; }
    }

    child->SetSelected(true);
    // Send 'SEL_CHANGED' event
    m_selectedItems.push_back(child);
    wxTreeEvent evt(wxEVT_TREE_SEL_CHANGED);
    evt.SetEventObject(m_tree);
    evt.SetItem(wxTreeItemId(child));
    SendEvent(evt);
}

bool clTreeCtrlModel::ClearSelections(bool notify)
{
    if(m_selectedItems.empty()) { return true; }

    if(notify) {
        // Check if we can proceed with this operation
        wxTreeEvent evt(wxEVT_TREE_SEL_CHANGING);
        evt.SetEventObject(m_tree);
        evt.SetOldItem(GetSingleSelection());
        SendEvent(evt);
        if(!evt.IsAllowed()) { return false; }
    }
    UnselectAll();
    return true;
}

bool clTreeCtrlModel::IsItemSelected(const clTreeCtrlNode* item) const
{
    if(m_selectedItems.empty()) { return false; }
    clTreeCtrlNode::Vec_t::const_iterator iter
        = std::find_if(m_selectedItems.begin(), m_selectedItems.end(), [&](clTreeCtrlNode* p) { return (p == item); });
    return (iter != m_selectedItems.end());
}