#ifndef MYTREEVIEW_H
#define MYTREEVIEW_H

#include <wx/treectrl.h>
#include "tree_item_data.h"

class MyTreeView : public wxTreeCtrl
{
public:
    MyTreeView() {}
    MyTreeView(wxWindow* parent,
               wxWindowID id = wxID_ANY,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = wxTR_HAS_BUTTONS | wxTR_LINES_AT_ROOT);
    virtual ~MyTreeView();
    virtual int OnCompareItems(const wxTreeItemId& item1, const wxTreeItemId& item2);
    virtual int OnCompareItems(const ItemData* a, const ItemData* b);
    void SortItem(const wxTreeItemId& item)
    {
        if(item.IsOk() && ItemHasChildren(item)) {
            SortChildren(item);
        }
    }
    wxDECLARE_DYNAMIC_CLASS(MyTreeView);
};

#endif // MYTREEVIEW_H
