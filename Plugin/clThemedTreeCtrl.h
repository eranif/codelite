#ifndef CLTHEMEDTREECTRL_H
#define CLTHEMEDTREECTRL_H

#include "clTreeCtrl.h"
#include "clTreeKeyboardInput.h"

class WXDLLIMPEXP_SDK clThemedTreeCtrl : public clTreeCtrl
{
    clTreeKeyboardInput::Ptr_t m_keyboard;

protected:
    void OnThemeChanged(wxCommandEvent& event);
    void ApplyTheme();

public:
    clThemedTreeCtrl(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize, long style = 0);
    clThemedTreeCtrl();
    virtual ~clThemedTreeCtrl();
    bool Create(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize, long style = 0);
};

#endif // CLTHEMEDTREECTRL_H
