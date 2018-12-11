#include "clTreeCtrlColourHelper.h"
#include "macros.h"
#include <deque>

clTreeCtrlColourHelper::clTreeCtrlColourHelper(clThemedTreeCtrl* tree)
    : m_tree(tree)
{
}

clTreeCtrlColourHelper::~clTreeCtrlColourHelper() {}

void clTreeCtrlColourHelper::DoSetBgColour(const wxTreeItemId& item, const wxColour& currentBgColour,
                                           const FolderColour::Map_t& coloursMap) const
{
    CHECK_ITEM_RET(item);
    wxString path = GetItemPath(item);
    wxColour bgColour = currentBgColour;
    FolderColour::Map_t::const_iterator iter = coloursMap.find(path);
    if(iter != coloursMap.end()) { bgColour = iter->second.GetColour(); }
    
    // It's OK for the colour to be "invalid", it will get reset to the tree's default
    // colouring
    m_tree->SetItemBackgroundColour(item, bgColour);

    if(m_tree->ItemHasChildren(item)) {
        wxTreeItemIdValue cookie;
        wxTreeItemId child = m_tree->GetFirstChild(item, cookie);
        while(child.IsOk()) {
            DoSetBgColour(child, bgColour, coloursMap);
            child = m_tree->GetNextChild(item, cookie);
        }
    }
}

void clTreeCtrlColourHelper::DoClearBgColour(const wxTreeItemId& item, const wxColour& colourToSet,
                                             FolderColour::Map_t& coloursMap) const
{
    CHECK_ITEM_RET(item);
    wxString path = GetItemPath(item);
    FolderColour::Map_t::const_iterator iter = coloursMap.find(path);
    if(iter != coloursMap.end()) { coloursMap.erase(iter); }
    m_tree->SetItemBackgroundColour(item, colourToSet);
    if(m_tree->ItemHasChildren(item)) {
        wxTreeItemIdValue cookie;
        wxTreeItemId child = m_tree->GetFirstChild(item, cookie);
        while(child.IsOk()) {
            DoClearBgColour(child, colourToSet, coloursMap);
            child = m_tree->GetNextChild(item, cookie);
        }
    }
}

void clTreeCtrlColourHelper::ResetBgColour(const wxTreeItemId& item, FolderColour::Map_t& coloursMap) const
{
    CHECK_ITEM_RET(item);
    wxTreeItemId itemParent = m_tree->GetItemParent(item);
    wxColour col = (itemParent.IsOk() && m_tree->GetItemBackgroundColour(itemParent).IsOk())
                       ? m_tree->GetItemBackgroundColour(itemParent)
                       : wxNullColour;

    DoClearBgColour(item, col, coloursMap);
}

void clTreeCtrlColourHelper::SetBgColour(const wxTreeItemId& item, const wxColour& currentBgColour,
                                         FolderColour::Map_t& coloursMap) const
{
    wxString path = GetItemPath(item);
    FolderColour vdc(path, currentBgColour);
    if(coloursMap.count(path)) { coloursMap.erase(path); }
    coloursMap.insert(std::make_pair(path, vdc));
    DoSetBgColour(item, currentBgColour, coloursMap);
}

wxString clTreeCtrlColourHelper::GetItemPath(clThemedTreeCtrl* tree, const wxTreeItemId& item)
{
    std::deque<wxString> queue;
    wxString text = tree->GetItemText(item);
    queue.push_front(text);

    wxTreeItemId p = tree->GetItemParent(item);
    while(p.IsOk()) {
        text = tree->GetItemText(p);
        queue.push_front(text);
        p = tree->GetItemParent(p);
    }

    wxString path;
    while(!queue.empty()) {
        path << queue.front() << "/";
        queue.pop_front();
    }

    if(!path.IsEmpty()) { path.RemoveLast(); }
    return path;
}

wxString clTreeCtrlColourHelper::GetItemPath(const wxTreeItemId& item) const { return GetItemPath(m_tree, item); }
