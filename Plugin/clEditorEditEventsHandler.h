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
    wxStyledTextCtrl* m_stc;
    wxTextCtrl* m_textCtrl;
    wxComboBox* m_combo;
    bool m_noUnbind;

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
    clEditEventsHandler(wxTextCtrl* wnd);
    clEditEventsHandler(wxStyledTextCtrl* wnd);
    clEditEventsHandler(wxComboBox* wnd);
    virtual ~clEditEventsHandler();
    void NoUnbind() { m_noUnbind = true; }
    typedef SmartPtr<clEditEventsHandler> Ptr_t;
};

#endif // CLEDITOREDITEVENTSHANDLER_H
