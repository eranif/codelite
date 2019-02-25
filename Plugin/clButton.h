#ifndef CLBUTTON_H
#define CLBUTTON_H

#include "codelite_exports.h"
#include "clButtonBase.h"

class WXDLLIMPEXP_SDK clButton : public clButtonBase
{
public:
    clButton();
    clButton(wxWindow* parent, wxWindowID id, const wxString& label = wxEmptyString,
             const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0,
             const wxValidator& validator = wxDefaultValidator, const wxString& name = "button");
    bool Create(wxWindow* parent, wxWindowID id, const wxString& label = wxEmptyString,
                const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0,
                const wxValidator& validator = wxDefaultValidator, const wxString& name = "button");
    virtual ~clButton();
};

#endif // CLBUTTON_H
