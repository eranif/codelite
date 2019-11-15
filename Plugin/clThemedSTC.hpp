#ifndef CLTHEMEDSTC_HPP
#define CLTHEMEDSTC_HPP

#include "codelite_exports.h"
#include <wx/stc/stc.h>

class WXDLLIMPEXP_SDK clThemedSTC : public wxStyledTextCtrl
{
public:
    clThemedSTC();
    clThemedSTC(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = wxSTCNameStr);
    bool Create(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = wxSTCNameStr);
    virtual ~clThemedSTC();
};

#endif // CLTHEMEDSTC_HPP
