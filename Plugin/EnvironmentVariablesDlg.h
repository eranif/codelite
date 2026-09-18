#ifndef ENVIRONMENTVARIABLESDLG_H
#define ENVIRONMENTVARIABLESDLG_H

#include "EnvironmentVariablesDlgBase.hpp"
#include "clEditorEditEventsHandler.h"

class WXDLLIMPEXP_SDK EnvironmentVariablesDlg : public EnvVarsTableDlgBase
{
    clEditEventsHandler m_editEventsHander;

public:
    EnvironmentVariablesDlg(wxWindow* parent);
    ~EnvironmentVariablesDlg() override;

protected:
    void OnCancel(wxCommandEvent& event) override;
    void OnClose(wxCloseEvent& event) override;
    void DoAddPage(const wxString& name, const wxString& content, bool select);
    void DoAddNewSet();

protected:
    void OnButtonOk(wxCommandEvent& event) override;
    void OnDeleteSet(wxCommandEvent& event) override;
    void OnDeleteSetUI(wxUpdateUIEvent& event) override;
    void OnExport(wxCommandEvent& event) override;
    void OnNewSet(wxCommandEvent& event) override;
};
#endif // ENVIRONMENTVARIABLESDLG_H
