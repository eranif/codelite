#include "my_tree_view.h"
#include "tree_item_data.h"
#include <wx/settings.h>

MyTreeView::MyTreeView(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
    : clThemedTreeCtrl(parent, id, pos, size, style)
{
    SetFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));
    std::function<bool(const wxTreeItemId&, const wxTreeItemId&)> SortFunc = [&](const wxTreeItemId& itemA,
                                                                                 const wxTreeItemId& itemB) {
        ItemData* a = static_cast<ItemData*>(GetItemData(itemA));
        ItemData* b = static_cast<ItemData*>(GetItemData(itemB));
        if(a->IsFolder() && b->IsFile())
            return true;
        else if(b->IsFolder() && a->IsFile())
            return false;
        return (a->GetDisplayName().CmpNoCase(b->GetDisplayName()) < 0);
    };
    SetSortFunction(SortFunc);
}

MyTreeView::~MyTreeView() {}
