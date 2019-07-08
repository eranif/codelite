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
    m_shouldInsertBeforeFunc = [&](clRowEntry* a, clRowEntry* b) {
        return a->GetLabel(0).CmpNoCase(b->GetLabel(0)) < 0;
    };
}

clTreeCtrlModel::~clTreeCtrlModel()
{
    m_shutdown = true; // Disable events
    wxDELETE(m_root);
}

void clTreeCtrlModel::GetNextItems(clRowEntry* from, int count, clRowEntry::Vec_t& items, bool selfIncluded) const
{
    if(count < 0) { count = 0; }
    items.reserve(count);
    return from->GetNextItems(count, items, selfIncluded);
}

void clTreeCtrlModel::GetPrevItems(clRowEntry* from, int count, clRowEntry::Vec_t& items, bool selfIncluded) const
{
    return from->GetPrevItems(count, items, selfIncluded);
}

wxTreeItemId clTreeCtrlModel::AddRoot(const wxString& text, int image, int selImage, wxTreeItemData* data)
{
    if(m_root) { return wxTreeItemId(m_root); }
    m_root = new clRowEntry(m_tree, text, image, selImage);
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
    return wxTreeItemId(const_cast<clRowEntry*>(m_root));
}

void clTreeCtrlModel::UnselectAll()
{
    for(size_t i = 0; i < m_selectedItems.size(); ++i) {
        m_selectedItems[i]->SetSelected(false);
    }
    m_selectedItems.clear();
}

void clTreeCtrlModel::SelectItem(const wxTreeItemId& item, bool select_it, bool addSelection, bool clear_old_selection)
{
    clRowEntry* child = ToPtr(item);
    if(!child) return;

    if(child->IsHidden()) { return; }

    if(clear_old_selection && !ClearSelections(item != GetSingleSelection())) { return; }

    if(select_it) {
        clRowEntry::Vec_t::iterator iter =
            std::find_if(m_selectedItems.begin(), m_selectedItems.end(), [&](clRowEntry* p) { return (p == child); });
        // If the item is already selected, don't select it again
        if(iter != m_selectedItems.end()) { return; }
    }

    // Fire an event only if the was no selection prior to this item
    bool fire_event = (m_selectedItems.empty() && select_it);

    if(IsMultiSelection() && addSelection) {
        // If we are unselecting it, remove it from the array
        clRowEntry::Vec_t::iterator iter =
            std::find_if(m_selectedItems.begin(), m_selectedItems.end(), [&](clRowEntry* p) { return (p == child); });
        if(iter != m_selectedItems.end() && !select_it) { m_selectedItems.erase(iter); }
    } else {
        if(!ClearSelections(item != GetSingleSelection())) { return; }
    }
    child->SetSelected(select_it);
    m_selectedItems.push_back(child);
    if(fire_event) {
        wxTreeEvent evt(wxEVT_TREE_SEL_CHANGED);
        evt.SetEventObject(m_tree);
        evt.SetItem(wxTreeItemId(child));
        SendEvent(evt);
    }
}

void clTreeCtrlModel::Clear()
{
    m_selectedItems.clear();
    for(size_t i = 0; i < m_onScreenItems.size(); ++i) {
        m_onScreenItems[i]->ClearRects();
    }
    m_onScreenItems.clear();
}

void clTreeCtrlModel::SetOnScreenItems(const clRowEntry::Vec_t& items)
{
    // Clear the old visible items. But only, if the item does not appear in both lists
    for(size_t i = 0; i < m_onScreenItems.size(); ++i) {
        clRowEntry* visibleItem = m_onScreenItems[i];
        clRowEntry::Vec_t::const_iterator iter =
            std::find_if(items.begin(), items.end(), [&](clRowEntry* item) { return item == visibleItem; });
        if(iter == items.end()) { m_onScreenItems[i]->ClearRects(); }
    }
    m_onScreenItems = items;
}

bool clTreeCtrlModel::ExpandToItem(const wxTreeItemId& item)
{
    clRowEntry* child = ToPtr(item);
    if(!child) { return false; }

    clRowEntry* parent = child->GetParent();
    while(parent) {
        if(!parent->SetExpanded(true)) { return false; }
        parent = parent->GetParent();
    }
    return true;
}

wxTreeItemId clTreeCtrlModel::AppendItem(const wxTreeItemId& parent, const wxString& text, int image, int selImage,
                                         wxTreeItemData* data)
{
    clRowEntry* parentNode = nullptr;
    if(!parent.IsOk()) { return wxTreeItemId(); }
    parentNode = ToPtr(parent);

    clRowEntry* child = new clRowEntry(m_tree, text, image, selImage);
    child->SetClientData(data);
    // Find the best insertion point
    clRowEntry* prevItem = nullptr;
    if(!parentNode->IsRoot() && (m_tree->GetTreeStyle() & wxTR_SORT_TOP_LEVEL)) {
        // We have been requested to sort top level items only
        parentNode->AddChild(child);
    } else if(m_shouldInsertBeforeFunc != nullptr) {
        const clRowEntry::Vec_t& children = parentNode->GetChildren();
        // Loop over the parent's children and add execute the compare function
        for(int i = ((int)children.size() - 1); i >= 0; --i) {
            if(!m_shouldInsertBeforeFunc(child, children[i])) {
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

    clRowEntry* pPrev = ToPtr(previous);
    clRowEntry* parentNode = ToPtr(parent);
    if(pPrev->GetParent() != parentNode) { return wxTreeItemId(); }

    clRowEntry* child = new clRowEntry(m_tree, text, image, selImage);
    child->SetClientData(data);
    parentNode->InsertChild(child, pPrev);
    return wxTreeItemId(child);
}

void clTreeCtrlModel::ExpandAllChildren(const wxTreeItemId& item) { DoExpandAllChildren(item, true); }

void clTreeCtrlModel::CollapseAllChildren(const wxTreeItemId& item) { DoExpandAllChildren(item, false); }

void clTreeCtrlModel::DoExpandAllChildren(const wxTreeItemId& item, bool expand)
{
    clRowEntry* p = ToPtr(item);
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
    return wxTreeItemId(GetRowBefore(ToPtr(item), visibleItem));
}

wxTreeItemId clTreeCtrlModel::GetItemAfter(const wxTreeItemId& item, bool visibleItem) const
{
    return wxTreeItemId(GetRowAfter(ToPtr(item), visibleItem));
}

void clTreeCtrlModel::DeleteItem(const wxTreeItemId& item)
{
    clRowEntry* node = ToPtr(item);
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
        wxDELETE(m_root);
    }
}

void clTreeCtrlModel::NodeDeleted(clRowEntry* node)
{
    // Clear the various caches
    {
        clRowEntry::Vec_t::iterator iter =
            std::find_if(m_selectedItems.begin(), m_selectedItems.end(), [&](clRowEntry* n) { return n == node; });
        if(iter != m_selectedItems.end()) {
            m_selectedItems.erase(iter);
            if(m_selectedItems.empty()) {
                // Dont leave the tree without a selected item
                if(node->GetNext()) { SelectItem(wxTreeItemId(node->GetNext())); }
            }
        }
    }

    {
        clRowEntry::Vec_t::iterator iter =
            std::find_if(m_onScreenItems.begin(), m_onScreenItems.end(), [&](clRowEntry* n) { return n == node; });
        if(iter != m_onScreenItems.end()) { m_onScreenItems.erase(iter); }
    }
    {
        if(m_firstItemOnScreen == node) { m_firstItemOnScreen = nullptr; }
    }
    {
        if(m_root == node) { m_root = nullptr; }
    }
}

bool clTreeCtrlModel::NodeExpanding(clRowEntry* node, bool expanding)
{
    wxTreeEvent before(expanding ? wxEVT_TREE_ITEM_EXPANDING : wxEVT_TREE_ITEM_COLLAPSING);
    before.SetItem(wxTreeItemId(node));
    before.SetEventObject(m_tree);
    SendEvent(before);
    return before.IsAllowed();
}

void clTreeCtrlModel::NodeExpanded(clRowEntry* node, bool expanded)
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

int clTreeCtrlModel::GetItemIndex(clRowEntry* item) const
{
    if(item == NULL) { return wxNOT_FOUND; }
    if(!m_root) { return wxNOT_FOUND; }
    int counter = 0;
    clRowEntry* current = m_root;
    while(current) {
        if(current == item) { return counter; }
        if(current->IsVisible()) { ++counter; }
        current = current->GetNext();
    }
    return wxNOT_FOUND;
}

bool clTreeCtrlModel::GetRange(clRowEntry* from, clRowEntry* to, clRowEntry::Vec_t& items) const
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

    clRowEntry* start_item = index1 > index2 ? to : from;
    clRowEntry* end_item = index1 > index2 ? from : to;
    clRowEntry* current = start_item;
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

clRowEntry* clTreeCtrlModel::GetItemFromIndex(int index) const
{
    if(index < 0) { return nullptr; }
    if(!m_root) { return nullptr; }
    int curIndex = -1;
    clRowEntry* current = m_root;
    while(current) {
        if(current->IsVisible()) { ++curIndex; }
        if(curIndex == index) { return current; }
        current = current->GetNext();
    }
    return nullptr;
}

void clTreeCtrlModel::SelectChildren(const wxTreeItemId& item)
{
    clRowEntry* parent = ToPtr(item);
    if(!parent) { return; }

    if(!ClearSelections(true)) { return; }
    std::for_each(parent->GetChildren().begin(), parent->GetChildren().end(),
                  [&](clRowEntry* child) { AddSelection(wxTreeItemId(child)); });
}

clRowEntry* clTreeCtrlModel::GetNextSibling(clRowEntry* item) const
{
    if(!item->GetParent()) { return nullptr; }
    const clRowEntry::Vec_t& children = item->GetParent()->GetChildren();
    if(children.empty()) { return nullptr; }
    size_t where = -1;
    for(size_t i = 0; i < children.size(); ++i) {
        if(item == children[i]) {
            where = i;
            break;
        }
    }

    // if we couldnt find 'item' in the children list or if it's the last child
    // return nullptr
    if((where == (size_t)-1) || (where == (children.size() - 1))) { return nullptr; }
    ++where;
    return children[where];
}

clRowEntry* clTreeCtrlModel::GetPrevSibling(clRowEntry* item) const
{
    if(!item->GetParent()) { return nullptr; }
    const clRowEntry::Vec_t& children = item->GetParent()->GetChildren();
    if(children.empty()) { return nullptr; }
    size_t where = -1;
    for(size_t i = 0; i < children.size(); ++i) {
        if(item == children[i]) {
            where = i;
            break;
        }
    }

    // if we couldnt find item in the children list or if it's the first child
    // we return nullptr
    if((where == (size_t)-1) || (where == (size_t)0)) { return nullptr; }

    --where;
    return children[where];
}

void clTreeCtrlModel::AddSelection(const wxTreeItemId& item)
{
    clRowEntry* child = ToPtr(item);
    if(!child) return;
    if(child->IsHidden()) { return; }

    clRowEntry::Vec_t::iterator iter =
        std::find_if(m_selectedItems.begin(), m_selectedItems.end(), [&](clRowEntry* p) { return (p == child); });
    // If the item is already selected, don't select it again
    if(iter != m_selectedItems.end()) { return; }

    // if we already got selections, notify about the change
    //    if(!m_selectedItems.empty()) {
    //        wxTreeEvent evt(wxEVT_TREE_SEL_CHANGING);
    //        evt.SetEventObject(m_tree);
    //        evt.SetOldItem(GetSingleSelection());
    //        SendEvent(evt);
    //        if(!evt.IsAllowed()) { return; }
    //    }

    child->SetSelected(true);
    // Send 'SEL_CHANGED' event
    m_selectedItems.push_back(child);
    if(m_selectedItems.size() == 1) {
        wxTreeEvent evt(wxEVT_TREE_SEL_CHANGED);
        evt.SetEventObject(m_tree);
        evt.SetItem(wxTreeItemId(child));
        SendEvent(evt);
    }
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

bool clTreeCtrlModel::IsItemSelected(const clRowEntry* item) const
{
    if(item == nullptr) { return false; }
    if(m_selectedItems.empty()) { return false; }
    clRowEntry::Vec_t::const_iterator iter =
        std::find_if(m_selectedItems.begin(), m_selectedItems.end(), [&](clRowEntry* p) { return (p == item); });
    return (iter != m_selectedItems.end());
}

bool clTreeCtrlModel::IsVisible(const wxTreeItemId& item) const
{
    if(!item.IsOk()) { return false; }
    clRowEntry* entry = ToPtr(item);
    clRowEntry::Vec_t::const_iterator iter =
        std::find_if(m_onScreenItems.begin(), m_onScreenItems.end(), [&](clRowEntry* p) { return (p == entry); });
    return (iter != m_onScreenItems.end());
}

clRowEntry* clTreeCtrlModel::GetRowBefore(clRowEntry* item, bool visibleItem) const
{
    clRowEntry* curp = item;
    if(!curp) { return nullptr; }
    curp = curp->GetPrev();
    while(curp) {
        if(visibleItem && !curp->IsVisible()) {
            curp = curp->GetPrev();
            continue;
        }
        break;
    }
    return curp;
}

clRowEntry* clTreeCtrlModel::GetRowAfter(clRowEntry* item, bool visibleItem) const
{
    clRowEntry* curp = item;
    if(!curp) { return nullptr; }
    curp = curp->GetNext();
    while(curp) {
        if(visibleItem && !curp->IsVisible()) {
            curp = curp->GetNext();
            continue;
        }
        break;
    }
    return curp;
}
