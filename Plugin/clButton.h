#ifndef CLBUTTON_H
#define CLBUTTON_H

#include "clButtonBase.h"
#include "codelite_exports.h"

class WXDLLIMPEXP_SDK clButton : public clButtonBase
{
public:
    clButton() = default;
    clButton(wxWindow* parent, wxWindowID id, const wxString& label = wxEmptyString,
             const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0,
             const wxValidator& validator = wxDefaultValidator, const wxString& name = "button");
    bool Create(wxWindow* parent, wxWindowID id, const wxString& label = wxEmptyString,
                const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0,
                const wxValidator& validator = wxDefaultValidator, const wxString& name = "button");
    ~clButton() override = default;
};

#endif // CLBUTTON_H
