#ifndef CLTHEMEDLISTCTRL_H
#define CLTHEMEDLISTCTRL_H

#include "codelite_exports.h"
#include "clDataViewListCtrl.h"

class WXDLLIMPEXP_SDK clThemedListCtrl : public clDataViewListCtrl
{
    void OnThemeChanged(wxCommandEvent& event);
    
public:
    clThemedListCtrl(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize, long style = 0);
    virtual ~clThemedListCtrl();
};

#endif // CLTHEMEDLISTCTRL_H
