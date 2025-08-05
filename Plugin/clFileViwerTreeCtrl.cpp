#include "clFileViwerTreeCtrl.h"

namespace
{
size_t GetItemScore(clTreeCtrlData* item)
{
    size_t score = 0;
    if (item->IsFolder() || item->IsDummy()) {
        score += 100;
    }

    if (!item->GetName().empty()) {
        auto ch = item->GetName()[0];
        if (ch == '_' || ch == '.') {
            // Hidden file
            score += 10;
        }
    }
    return score;
}
} // namespace

clFileViewerTreeCtrl::clFileViewerTreeCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size,
                                           long style)
    : clThemedTreeCtrl(parent, id, pos, size, (style & ~wxTR_FULL_ROW_HIGHLIGHT))
{
    std::function<bool(const wxTreeItemId&, const wxTreeItemId&)> SortFunc = [&](const wxTreeItemId& itemA,
                                                                                 const wxTreeItemId& itemB) {
        clTreeCtrlData* a = static_cast<clTreeCtrlData*>(GetItemData(itemA));
        clTreeCtrlData* b = static_cast<clTreeCtrlData*>(GetItemData(itemB));

        size_t score1 = GetItemScore(a);
        size_t score2 = GetItemScore(b);

        if (score1 > score2) {
            return true;
        } else if (score2 > score1) {
            return false;
        } else {
            // same score
            return (a->GetName().CmpNoCase(b->GetName()) < 0);
        }
    };
    SetSortFunction(SortFunc);
}

wxTreeItemId clTreeNodeIndex::Find(const wxString& path)
{
#ifdef __WXMSW__
    wxString lcpath = path.Lower();
    if (m_children.count(lcpath)) {
        return m_children.find(lcpath)->second;
    }
#else
    if (m_children.count(path)) {
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
