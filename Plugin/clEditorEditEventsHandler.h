#ifndef CLEDITOREDITEVENTSHANDLER_H
#define CLEDITOREDITEVENTSHANDLER_H

#include <wx/event.h>
#include "codelite_exports.h"
#include <wx/window.h>
#include "smart_ptr.h"

class wxComboBox;
class wxStyledTextCtrl;
class wxTextCtrl;

class WXDLLIMPEXP_SDK clEditEventsHandler : public wxEvtHandler
{
    wxStyledTextCtrl* m_stc;
    wxTextCtrl* m_textCtrl;
    wxComboBox* m_combo;
    
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
    
    typedef SmartPtr<clEditEventsHandler> Ptr_t;
};

#endif // CLEDITOREDITEVENTSHANDLER_H
