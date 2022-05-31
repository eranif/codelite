#ifndef CLEDITOREDITEVENTSHANDLER_H
#define CLEDITOREDITEVENTSHANDLER_H

#include "codelite_exports.h"
#include "smart_ptr.h"

#include <wx/event.h>
#include <wx/window.h>

class wxComboBox;
class wxStyledTextCtrl;
class wxTextCtrl;

class WXDLLIMPEXP_SDK clEditEventsHandler : public wxEvtHandler
{
    wxStyledTextCtrl* m_stc = nullptr;
    wxTextCtrl* m_textCtrl = nullptr;
    wxComboBox* m_combo = nullptr;
    bool m_noUnbind = false;
    wxString m_name;

private:
    void DoInitialize();

protected:
    void OnCopy(wxCommandEvent& event);
    void OnPaste(wxCommandEvent& event);
    void OnCut(wxCommandEvent& event);
    void OnSelectAll(wxCommandEvent& event);
    void OnUndo(wxCommandEvent& event);
    void OnRedo(wxCommandEvent& event);

public:
    clEditEventsHandler(wxTextCtrl* wnd, const wxString& name = wxEmptyString);
    clEditEventsHandler(wxStyledTextCtrl* wnd, const wxString& name = wxEmptyString);
    clEditEventsHandler(wxComboBox* wnd, const wxString& name = wxEmptyString);
    virtual ~clEditEventsHandler();
    void NoUnbind() { m_noUnbind = true; }
    typedef SmartPtr<clEditEventsHandler> Ptr_t;
};

#endif // CLEDITOREDITEVENTSHANDLER_H
