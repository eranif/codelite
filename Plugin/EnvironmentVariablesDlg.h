#ifndef ENVIRONMENTVARIABLESDLG_H
#define ENVIRONMENTVARIABLESDLG_H

#include "EnvironmentVariablesDlgBase.h"
#include "clEditorEditEventsHandler.h"

class WXDLLIMPEXP_SDK EnvironmentVariablesDlg : public EnvVarsTableDlgBase
{
    clEditEventsHandler m_editEventsHander;

public:
    EnvironmentVariablesDlg(wxWindow* parent);
    virtual ~EnvironmentVariablesDlg();

protected:
    virtual void OnCancel(wxCommandEvent& event);
    virtual void OnClose(wxCloseEvent& event);
    void DoAddPage(const wxString& name, const wxString& content, bool select);
    void DoAddNewSet();

protected:
    virtual void OnButtonOk(wxCommandEvent& event);
    virtual void OnDeleteSet(wxCommandEvent& event);
    virtual void OnDeleteSetUI(wxUpdateUIEvent& event);
    virtual void OnExport(wxCommandEvent& event);
    virtual void OnNewSet(wxCommandEvent& event);
};
#endif // ENVIRONMENTVARIABLESDLG_H
