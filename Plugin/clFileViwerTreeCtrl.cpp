#include "clFileViwerTreeCtrl.h"

clFileViewerTreeCtrl::clFileViewerTreeCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size,
                                           long style)
    : clThemedTreeCtrl(parent, id, pos, size, (style & ~wxTR_FULL_ROW_HIGHLIGHT))
{
    std::function<bool(const wxTreeItemId&, const wxTreeItemId&)> SortFunc = [&](const wxTreeItemId& itemA,
                                                                                 const wxTreeItemId& itemB) {
        clTreeCtrlData* a = static_cast<clTreeCtrlData*>(GetItemData(itemA));
        clTreeCtrlData* b = static_cast<clTreeCtrlData*>(GetItemData(itemB));
        if(a->IsFolder() && b->IsFile())
            return true;
        else if(b->IsFolder() && a->IsFile())
            return false;
        // same kind
        return (a->GetName().CmpNoCase(b->GetName()) < 0);
    };
    SetSortFunction(SortFunc);
}

clFileViewerTreeCtrl::~clFileViewerTreeCtrl() {}

wxTreeItemId clTreeNodeIndex::Find(const wxString& path)
{
#ifdef __WXMSW__
    wxString lcpath = path.Lower();
    if(m_children.count(lcpath)) {
        return m_children.find(lcpath)->second;
    }
#else
    if(m_children.count(path)) {
        return m_children.find(path)->second;
    }
#endif
    return wxTreeItemId();
}

void clTreeNodeIndex::Add(const wxString& path, const wxTreeItemId& item)
{
    m_children.insert({
#ifdef __WXMSW__
        path.Lower()
#else
        path
#endif
            ,
        item });
}

void clTreeNodeIndex::Delete(const wxString& name)
{
    m_children.erase(
#ifdef __WXMSW__
        name.Lower()
#else
        name
#endif
    );
}
