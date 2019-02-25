#ifndef CLTHEMEDBUTTON_H
#define CLTHEMEDBUTTON_H

#include "clButton.h"

class WXDLLIMPEXP_SDK clThemedButton : public clButton
{
protected:
    void OnThemeChanged(wxCommandEvent& event);
    void ApplyTheme();

public:
    clThemedButton();
    clThemedButton(wxWindow* parent, wxWindowID id, const wxString& label = wxEmptyString,
                   const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0,
                   const wxValidator& validator = wxDefaultValidator, const wxString& name = "button");
    bool Create(wxWindow* parent, wxWindowID id, const wxString& label = wxEmptyString,
                const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0,
                const wxValidator& validator = wxDefaultValidator, const wxString& name = "button");
    virtual ~clThemedButton();
};

#endif // CLTHEMEDBUTTON_H
