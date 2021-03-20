#ifndef CLTHEMEDBUTTON_H
#define CLTHEMEDBUTTON_H

#include "clButton.h"
#include "cl_command_event.h"

class WXDLLIMPEXP_SDK clThemedButton : public clButton
{
protected:
    void OnThemeChanged(clCommandEvent& event);
    void ApplyTheme();

public:
    clThemedButton();
    clThemedButton(wxWindow* parent, wxWindowID id, const wxString& label = wxEmptyString,
                   const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0,
                   const wxValidator& validator = wxDefaultValidator, const wxString& name = "button");
    clThemedButton(wxWindow* parent, wxWindowID id, const wxString& label, const wxString& note,
                   const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0,
                   const wxValidator& validator = wxDefaultValidator, const wxString& name = "button");

    bool Create(wxWindow* parent, wxWindowID id, const wxString& label = wxEmptyString,
                const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0,
                const wxValidator& validator = wxDefaultValidator, const wxString& name = "button");
    virtual ~clThemedButton();
};

#endif // CLTHEMEDBUTTON_H
