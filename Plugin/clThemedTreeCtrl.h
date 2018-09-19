#ifndef CLTHEMEDTREECTRL_H
#define CLTHEMEDTREECTRL_H

#include "clTreeCtrl.h"

class WXDLLIMPEXP_SDK clThemedTreeCtrl : public clTreeCtrl
{
    void OnThemeChanged(wxCommandEvent& event);

public:
    clThemedTreeCtrl(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize, long style = 0);
    clThemedTreeCtrl();
    virtual ~clThemedTreeCtrl();
    bool Create(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize, long style = 0);
};

#endif // CLTHEMEDTREECTRL_H
