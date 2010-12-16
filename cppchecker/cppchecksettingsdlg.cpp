#include "cppchecksettingsdlg.h"
#include "windowattrmanager.h"
#include <wx/filedlg.h>

CppCheckSettingsDialog::CppCheckSettingsDialog(wxWindow* parent, CppCheckSettings* settings, IConfigTool *conf)
		: CppCheckSettingsDialogBase(parent)
		, m_settings(settings)
		, m_conf(conf)
{
	m_cbOptionAll->SetValue(settings->All());
	m_cbOptionForce->SetValue(settings->Force());
	m_cbOptionStyle->SetValue(settings->Style());
	m_cbOptionUnusedFunctions->SetValue(settings->UnusedFunctions());
	m_listBoxExcludelist->Append(settings->GetExcludeFiles());
	WindowAttrManager::Load(this, wxT("CppCheckSettingsDialog"), m_conf);
}

void CppCheckSettingsDialog::OnBtnOK(wxCommandEvent& e)
{
	m_settings->All( m_cbOptionAll->IsChecked() );
	m_settings->Force( m_cbOptionForce->IsChecked() );
	m_settings->Style( m_cbOptionStyle->IsChecked() );
	m_settings->UnusedFunctions( m_cbOptionUnusedFunctions->IsChecked() );
	m_settings->SetExcludeFiles( m_listBoxExcludelist->GetStrings() );
	e.Skip();
}

CppCheckSettingsDialog::~CppCheckSettingsDialog()
{
	WindowAttrManager::Save(this, wxT("CppCheckSettingsDialog"), m_conf);
}

void CppCheckSettingsDialog::OnAddFile(wxCommandEvent& e)
{
	const wxString ALL(wxString(_("All Files")) + wxT(" (*)|*"));
	wxFileDialog dlg(this,
					 _("Add File:"),
					 wxEmptyString,
					 wxEmptyString,
					 ALL,
					 wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_MULTIPLE , wxDefaultPosition);

	if (dlg.ShowModal() == wxID_OK) {
		wxArrayString paths;
		dlg.GetPaths( paths );

		m_listBoxExcludelist->Append( paths );
	}
}

void CppCheckSettingsDialog::OnRemoveFile(wxCommandEvent& e)
{
	int sel = m_listBoxExcludelist->GetSelection();
	if ( sel != wxNOT_FOUND ) {
		m_listBoxExcludelist->Delete((unsigned int) sel);
	}
}

void CppCheckSettingsDialog::OnRemoveFileUI(wxUpdateUIEvent& e)
{
	e.Enable( m_listBoxExcludelist->GetSelection() != wxNOT_FOUND );
}


void CppCheckSettingsDialog::OnClearList(wxCommandEvent& e)
{
	wxUnusedVar(e);
	m_listBoxExcludelist->Clear();
}

void CppCheckSettingsDialog::OnClearListUI(wxUpdateUIEvent& e)
{
	e.Enable( !m_listBoxExcludelist->IsEmpty() );
}

