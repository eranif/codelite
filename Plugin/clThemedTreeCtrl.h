#ifndef CLTHEMEDTREECTRL_H
#define CLTHEMEDTREECTRL_H

#include "clTreeCtrl.h"

class WXDLLIMPEXP_SDK clThemedTreeCtrl : public clTreeCtrl
{
public:
    clThemedTreeCtrl(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize, long style = 0);
    virtual ~clThemedTreeCtrl();
};

#endif // CLTHEMEDTREECTRL_H
