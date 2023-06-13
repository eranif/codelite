#ifndef CLEDITOREDITEVENTSHANDLER_H
#define CLEDITOREDITEVENTSHANDLER_H

#include "codelite_exports.h"
#include "smart_ptr.h"

#include <wx/combobox.h>
#include <wx/event.h>
#include <wx/stc/stc.h>
#include <wx/textctrl.h>
#include <wx/window.h>

class wxComboBox;
class wxStyledTextCtrl;
class wxTextCtrl;

#define CHECK_FOCUS_WINDOW()                                                     \
    wxWindow* focus = wxWindow::FindFocus();                                     \
    if(!focus) {                                                                 \
        event.Skip();                                                            \
        return;                                                                  \
    } else if((focus != m_stc) && (focus != m_textCtrl) && (focus != m_combo)) { \
        event.Skip();                                                            \
        return;                                                                  \
    }

#define CALL_FUNC(func)     \
    if(m_stc) {             \
        m_stc->func();      \
    } else if(m_combo) {    \
        m_combo->func();    \
    } else {                \
        m_textCtrl->func(); \
    }

class WXDLLIMPEXP_SDK clEditEventsHandler : public wxEvtHandler
{
protected:
    wxStyledTextCtrl* m_stc = nullptr;
    wxTextCtrl* m_textCtrl = nullptr;
    wxComboBox* m_combo = nullptr;
    bool m_noUnbind = false;
    wxString m_name;

private:
    void DoInitialize();

protected:
    virtual void OnCopy(wxCommandEvent& event);
    virtual void OnPaste(wxCommandEvent& event);
    virtual void OnCut(wxCommandEvent& event);
    virtual void OnSelectAll(wxCommandEvent& event);
    virtual void OnUndo(wxCommandEvent& event);
    virtual void OnRedo(wxCommandEvent& event);

public:
    clEditEventsHandler(wxTextCtrl* wnd, const wxString& name = wxEmptyString);
    clEditEventsHandler(wxStyledTextCtrl* wnd, const wxString& name = wxEmptyString);
    clEditEventsHandler(wxComboBox* wnd, const wxString& name = wxEmptyString);
    virtual ~clEditEventsHandler();
    void NoUnbind() { m_noUnbind = true; }
    typedef SmartPtr<clEditEventsHandler> Ptr_t;
};

#endif // CLEDITOREDITEVENTSHANDLER_H
