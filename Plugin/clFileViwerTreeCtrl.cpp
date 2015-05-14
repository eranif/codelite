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
