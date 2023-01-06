#ifndef CLTHEMEDSTC_HPP
#define CLTHEMEDSTC_HPP

#include "clEditorEditEventsHandler.h"
#include "codelite_exports.h"

#include <wx/stc/stc.h>

class WXDLLIMPEXP_SDK clThemedSTC : public wxStyledTextCtrl
{
    clEditEventsHandler::Ptr_t m_editEventsHandler;

public:
    clThemedSTC();
    clThemedSTC(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = wxSTCNameStr);
    bool Create(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = wxSTCNameStr);
    clThemedSTC(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& defaultValue = wxEmptyString,
                const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0,
                const wxString& name = wxSTCNameStr);
    virtual ~clThemedSTC();
};

#endif // CLTHEMEDSTC_HPP
