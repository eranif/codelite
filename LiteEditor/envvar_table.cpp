#include "envvar_table.h"
#include "evnvarlist.h"
#include "environmentconfig.h"
#include "windowattrmanager.h"

EnvVarsTableDlg::EnvVarsTableDlg( wxWindow* parent )
		: EnvVarsTableDlgBase( parent )
{
	EvnVarList vars;
	EnvironmentConfig::Instance()->ReadObject(wxT("Variables"), &vars);
	std::map<wxString, wxString> envSets = vars.GetEnvVarSets();
	wxString activePage = vars.GetActiveSet();

	WindowAttrManager::Load(this, wxT("EnvVarsTableDlg"), NULL);
	std::map<wxString, wxString>::iterator iter = envSets.begin();
	for (; iter != envSets.end(); iter++) {
		wxString name    = iter->first;
		wxString content = iter->second;

		if (name == wxT("Default")) {
			m_textCtrlDefault->SetText(content);
		} else {
			EnvVarSetPage *page = new EnvVarSetPage(m_notebook1);
			page->m_textCtrl->SetText(content);
			m_notebook1->AddPage(page, name);
		}
	}

	m_notebook1->SetSelection(0);
	for (size_t i=0; i<m_notebook1->GetPageCount(); i++) {
		if (m_notebook1->GetPageText(i) == activePage) {
			m_notebook1->GetPage(i)->SetFocus();
			m_notebook1->SetSelection(i);
			break;
		}
	}
}

EnvVarsTableDlg::~EnvVarsTableDlg()
{
	WindowAttrManager::Save(this, wxT("EnvVarsTableDlg"), NULL);
}

void EnvVarsTableDlg::OnLeftUp( wxMouseEvent& event )
{
}

void EnvVarsTableDlg::OnButtonOk( wxCommandEvent& event )
{
	EvnVarList vars;

	std::map<wxString, wxString> envSets;

	wxString content = m_textCtrlDefault->GetText();
	wxString name    = wxT("Default");
	content.Trim().Trim(false);
	envSets[name] = content;

	for (size_t i=1; i<m_notebook1->GetPageCount(); i++) {
		if (i == (size_t)m_notebook1->GetSelection()) {
			vars.SetActiveSet(m_notebook1->GetPageText(i));
		}

		EnvVarSetPage *page = (EnvVarSetPage*) m_notebook1->GetPage(i);
		wxString content = page->m_textCtrl->GetText();
		wxString name    = m_notebook1->GetPageText(i);
		content.Trim().Trim(false);
		envSets[name] = content;
	}
	vars.SetEnvVarSets(envSets);
	EnvironmentConfig::Instance()->WriteObject(wxT("Variables"), &vars);
	event.Skip();
}

void EnvVarsTableDlg::OnNewSet( wxCommandEvent& event )
{
}

void EnvVarsTableDlg::OnDeleteSet(wxCommandEvent& event)
{
}

void EnvVarsTableDlg::OnDeleteSetUI(wxUpdateUIEvent& event)
{
	int i = m_notebook1->GetSelection();
	event.Enable(i != wxNOT_FOUND && m_notebook1->GetPageText(i) != wxT("Default"));
}
