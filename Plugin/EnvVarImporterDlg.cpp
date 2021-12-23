#include "EnvVarImporterDlg.h"

EnvVarImporterDlg::EnvVarImporterDlg(wxWindow* parent, const wxString& projectName, const wxString& cfgName,
                                     std::set<wxString> listEnvVar, BuildConfigPtr le_conf, bool* showDlg)
    : EnvVarImporterDlgBase(parent)
    , le_conf(le_conf)
    , showDlg(showDlg)
{
    wxString value = wxT("");

    for(wxString envVar : listEnvVar) {
        value += envVar + wxT("=?") + wxT("\n");
    }

    m_projectName->SetLabel(projectName);
    m_confName->SetLabel(cfgName);
    m_envVars->SetValue(value);
}

EnvVarImporterDlg::~EnvVarImporterDlg() {}

void EnvVarImporterDlg::OnImport(wxCommandEvent& event)
{
    le_conf->SetEnvvars(m_envVars->GetValue());
    Close();
}

void EnvVarImporterDlg::OnContinue(wxCommandEvent& event) { Close(); }

void EnvVarImporterDlg::OnSkip(wxCommandEvent& event)
{
    *showDlg = false;
    Close();
}
