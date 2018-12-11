#ifndef CLTHEMEDLISTCTRL_H
#define CLTHEMEDLISTCTRL_H

#include "clDataViewListCtrl.h"
#include "clTreeKeyboardInput.h"
#include "codelite_exports.h"

class WXDLLIMPEXP_SDK clThemedListCtrl : public clDataViewListCtrl
{
    clTreeKeyboardInput::Ptr_t m_keyboard;

private:
    void OnThemeChanged(wxCommandEvent& event);
    void ApplyTheme();

public:
    clThemedListCtrl(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize, long style = 0);
    virtual ~clThemedListCtrl();
};

#endif // CLTHEMEDLISTCTRL_H
