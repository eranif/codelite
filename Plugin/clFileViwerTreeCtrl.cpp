#include "clFileViwerTreeCtrl.h"

IMPLEMENT_DYNAMIC_CLASS(clFileViewerTreeCtrl, wxTreeCtrl)

clFileViewerTreeCtrl::clFileViewerTreeCtrl(wxWindow* parent,
                                         wxWindowID id,
                                         const wxPoint& pos,
                                         const wxSize& size,
                                         long style)
    : wxTreeCtrl(parent, id, pos, size, style)
{
}

clFileViewerTreeCtrl::clFileViewerTreeCtrl() {}

clFileViewerTreeCtrl::~clFileViewerTreeCtrl() {}

int clFileViewerTreeCtrl::OnCompareItems(const clTreeCtrlData* a, const clTreeCtrlData* b)
{
    // if dir and other is not, dir has preference
    if(a->IsFolder() && b->IsFile())
        return -1;
    else if(b->IsFolder() && a->IsFile())
        return 1;
    return a->GetName().CmpNoCase(b->GetName());
}

int clFileViewerTreeCtrl::OnCompareItems(const wxTreeItemId& item1, const wxTreeItemId& item2)
{
    if(!item1.IsOk() || !item2.IsOk())
        return wxTreeCtrl::OnCompareItems(item1, item2);

    clTreeCtrlData *a = static_cast<clTreeCtrlData*>(GetItemData(item1));
    clTreeCtrlData *b = static_cast<clTreeCtrlData*>(GetItemData(item2));
    
    return OnCompareItems(a, b);
}
