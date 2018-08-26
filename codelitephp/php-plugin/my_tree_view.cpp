#include "my_tree_view.h"
#include "tree_item_data.h"
#include <wx/settings.h>

IMPLEMENT_DYNAMIC_CLASS(MyTreeView, wxTreeCtrl)

MyTreeView::MyTreeView(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
{
    Create(parent, id, pos, size, style);
    SetFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));
}

MyTreeView::~MyTreeView()
{
}

int MyTreeView::OnCompareItems(const wxTreeItemId& item1, const wxTreeItemId& item2)
{
    if(!item1.IsOk() || !item2.IsOk())
        return wxTreeCtrl::OnCompareItems(item1, item2);

    ItemData *a = static_cast<ItemData*>(GetItemData(item1));
    ItemData *b = static_cast<ItemData*>(GetItemData(item2));
    
    return OnCompareItems(a, b);
}

int MyTreeView::OnCompareItems(const ItemData* a, const ItemData* b)
{
    // if dir and other is not, dir has preference
    if(a->IsFolder() && b->IsFile())
        return -1;
    else if(b->IsFolder() && a->IsFile())
        return 1;
    return a->GetDisplayName().CmpNoCase(b->GetDisplayName());
}
