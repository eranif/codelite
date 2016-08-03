#ifndef ENVIRONMENTVARIABLESDLG_H
#define ENVIRONMENTVARIABLESDLG_H
#include "EnvironmentVariablesDlgBase.h"

class WXDLLIMPEXP_SDK EnvironmentVariablesDlg : public EnvVarsTableDlgBase
{
    static EnvironmentVariablesDlg* m_dlg;

public:
    EnvironmentVariablesDlg(wxWindow* parent);
    virtual ~EnvironmentVariablesDlg();
    static void ShowDialog(wxWindow* parent);
    static void ReleaseDialog();

protected:
    virtual void OnCancel(wxCommandEvent& event);
    virtual void OnClose(wxCloseEvent& event);
    void DoAddPage(const wxString& name, const wxString& content, bool select);
    void DoDestroy();

protected:
    virtual void OnButtonOk(wxCommandEvent& event);
    virtual void OnDeleteSet(wxCommandEvent& event);
    virtual void OnDeleteSetUI(wxUpdateUIEvent& event);
    virtual void OnExport(wxCommandEvent& event);
    virtual void OnNewSet(wxCommandEvent& event);
};
#endif // ENVIRONMENTVARIABLESDLG_H
