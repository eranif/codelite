#include "my_tree_view.h"
#include "tree_item_data.h"

wxIMPLEMENT_DYNAMIC_CLASS(MyTreeView, wxTreeCtrl);

MyTreeView::MyTreeView(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
    : wxTreeCtrl(parent, id, pos, size, style)
{
}

MyTreeView::~MyTreeView()
{
}

int MyTreeView::OnCompareItems(const wxTreeItemId& item1, const wxTreeItemId& item2)
{
    if(!item1.IsOk() || !item2.IsOk())
        return wxTreeCtrl::OnCompareItems(item1, item2);

    ItemData *data1 = static_cast<ItemData*>(GetItemData(item1));
    ItemData *data2 = static_cast<ItemData*>(GetItemData(item2));
    
    if ( data1->IsFolder() && data2->IsFile() ) {
        return -1;
        
    } else if ( data2->IsFolder() && data1->IsFile() ) {
        return 1;
        
    } else {
        return wxTreeCtrl::OnCompareItems(item1, item2);
    }
}
