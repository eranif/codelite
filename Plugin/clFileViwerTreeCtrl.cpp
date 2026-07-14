#include "clFileViwerTreeCtrl.h"

clFileViewerTreeCtrl::clFileViewerTreeCtrl(
    wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
    : wxDataViewTreeCtrl(parent, id, pos, size, style)
{
}

void clFileViewerTreeCtrl::SetBitmaps(std::vector<wxBitmap>* bitmaps)
{
    wxWithImages::Images images;
    if (bitmaps) {
        images.reserve(bitmaps->size());
        for (const auto& bmp : *bitmaps) {
            images.push_back(wxBitmapBundle::FromBitmap(bmp));
        }
    }
    SetImages(images);
}

bool clFileViewerTreeCtrl::ShouldComeBefore(clTreeCtrlData* a, clTreeCtrlData* b) const
{
    auto GetItemScore = [](clTreeCtrlData* item) -> size_t {
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
    };

    size_t scoreA = GetItemScore(a);
    size_t scoreB = GetItemScore(b);
    if (scoreA != scoreB) {
        return scoreA > scoreB;
    }
    return a->GetName().CmpNoCase(b->GetName()) < 0;
}

wxDataViewItem clFileViewerTreeCtrl::InsertSorted(
    const wxDataViewItem& parent, const wxString& text, int icon, int expandedIcon, bool isContainer,
    wxClientData* data)
{
    clTreeCtrlData* newData = static_cast<clTreeCtrlData*>(data);
    int count = GetChildCount(parent);

    // Despite its name, wxDataViewTreeStore::InsertItem(parent, "previous", ...) inserts the new
    // item immediately BEFORE "previous" in the child list. Find the first existing child that
    // should come after the new item and use it as that anchor.
    wxDataViewItem nextItem;
    for (int i = 0; i < count; ++i) {
        wxDataViewItem child = GetNthChild(parent, i);
        clTreeCtrlData* childData = GetItemData(child);
        if (childData && ShouldComeBefore(newData, childData)) {
            nextItem = child;
            break;
        }
    }

    if (!nextItem.IsOk()) {
        // the new item goes at the end (this also covers the "no children yet" case)
        return isContainer ? AppendContainer(parent, text, icon, expandedIcon, data)
                           : AppendItem(parent, text, icon, data);
    }
    return isContainer ? InsertContainer(parent, nextItem, text, icon, expandedIcon, data)
                       : InsertItem(parent, nextItem, text, icon, data);
}

wxDataViewItem clTreeNodeIndex::Find(const wxString& path)
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
    return wxDataViewItem();
}

void clTreeNodeIndex::Add(const wxString& path, const wxDataViewItem& item)
{
    m_children.insert({
#ifdef __WXMSW__
        path.Lower()
#else
        path
#endif
            ,
        item});
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
