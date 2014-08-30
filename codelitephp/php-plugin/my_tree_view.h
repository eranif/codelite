#ifndef MYTREEVIEW_H
#define MYTREEVIEW_H

#include <wx/treectrl.h>

class MyTreeView : public wxTreeCtrl
{
    wxDECLARE_DYNAMIC_CLASS(MyTreeView);

public:
    MyTreeView() {}
    MyTreeView(wxWindow *parent, wxWindowID id = wxID_ANY,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = wxTR_HAS_BUTTONS | wxTR_LINES_AT_ROOT);
    virtual ~MyTreeView();
    virtual int OnCompareItems(const wxTreeItemId &item1, const wxTreeItemId &item2);
};

#endif // MYTREEVIEW_H
