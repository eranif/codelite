#include "cppchecksettingsdlg.h"
#include "windowattrmanager.h"
#include <wx/msgdlg.h>
#include <wx/filedlg.h>

CppCheckSettingsDialog::CppCheckSettingsDialog(wxWindow* parent, CppCheckSettings* settings, IConfigTool *conf, const wxString& defaultpath)
		: CppCheckSettingsDialogBase(parent)
		, m_settings(settings)
		, m_conf(conf)
		, m_defaultpath(defaultpath)
{
	m_checkListExtraWarnings->Check(0, settings->GetStyle());
	m_checkListExtraWarnings->Check(1, settings->GetPerformance());
	m_checkListExtraWarnings->Check(2, settings->GetPortability());
	m_checkListExtraWarnings->Check(3, settings->GetUnusedFunctions());
	m_checkListExtraWarnings->Check(4, settings->GetMissingIncludes());
	m_checkListExtraWarnings->Check(5, settings->GetInformation());
	m_checkListExtraWarnings->Check(6, settings->GetPosixStandards());
	m_checkListExtraWarnings->Check(7, settings->GetC99Standards());
	m_checkListExtraWarnings->Check(8, settings->GetCpp11Standards());
	m_cbOptionForce->SetValue(settings->GetForce());

	m_listBoxExcludelist->Append(settings->GetExcludeFiles());
	
	// The Suppressed Warnings strings are stored in a string/string map:
	// e.g. <"cstyleCast", "C-style pointer casting">
	// We display the second, so store the keys in a wxArrayInt so that we can identify each again
	m_SuppressionsKeys.Clear();
	std::map<wxString, wxString>::const_iterator iter = settings->GetSuppressedWarningsStrings1()->begin();
	for (; iter != settings->GetSuppressedWarningsStrings1()->end(); ++iter) {
		// First the checked ones
		int index = m_checkListSuppress->Append((*iter).second);
		m_checkListSuppress->Check(index, true);
		m_SuppressionsKeys.Add((*iter).first);
	}

	for (iter = settings->GetSuppressedWarningsStrings0()->begin(); iter != settings->GetSuppressedWarningsStrings0()->end(); ++iter) {
		// Then the unchecked ones
		int index = m_checkListSuppress->Append((*iter).second);
		m_checkListSuppress->Check(index, false);
		m_SuppressionsKeys.Add((*iter).first);
	}

	WindowAttrManager::Load(this, wxT("CppCheckSettingsDialog"), m_conf);
}

void CppCheckSettingsDialog::OnBtnOK(wxCommandEvent& e)
{
	m_settings->SetStyle(m_checkListExtraWarnings->IsChecked(0));
	m_settings->SetPerformance(m_checkListExtraWarnings->IsChecked(1));
	m_settings->SetPortability(m_checkListExtraWarnings->IsChecked(2));
	m_settings->SetUnusedFunctions(m_checkListExtraWarnings->IsChecked(3));
	m_settings->SetMissingIncludes(m_checkListExtraWarnings->IsChecked(4));
	m_settings->SetInformation(m_checkListExtraWarnings->IsChecked(5));
	m_settings->SetPosixStandards(m_checkListExtraWarnings->IsChecked(6));
	m_settings->SetC99Standards(m_checkListExtraWarnings->IsChecked(7));
	m_settings->SetCpp11Standards(m_checkListExtraWarnings->IsChecked(8));
	m_settings->SetForce(m_cbOptionForce->IsChecked());
	
	m_settings->SetExcludeFiles(m_listBoxExcludelist->GetStrings());

	m_settings->SetSuppressedWarnings(m_checkListSuppress, m_SuppressionsKeys);
	m_settings->SetSaveSuppressedWarnings(m_checkBoxSerialise->IsChecked());
	
	e.Skip();
}

CppCheckSettingsDialog::~CppCheckSettingsDialog()
{
	WindowAttrManager::Save(this, wxT("CppCheckSettingsDialog"), m_conf);
}

void CppCheckSettingsDialog::OnChecksTickAll(wxCommandEvent& WXUNUSED(e))
{
	for (size_t n=0; n < m_checkListExtraWarnings->GetCount(); ++n) {
		m_checkListExtraWarnings->Check(n);
	}
}

void CppCheckSettingsDialog::OnChecksUntickAll(wxCommandEvent& WXUNUSED(e))
{
	for (size_t n=0; n < m_checkListExtraWarnings->GetCount(); ++n) {
		m_checkListExtraWarnings->Check(n, false);
	}
}

void CppCheckSettingsDialog::OnAddFile(wxCommandEvent& WXUNUSED(e))
{
	wxString filter(wxString(_("C/C++ Files ")));
	filter << wxT("(*.c;*.cpp)|*.c;*.cpp|") 
	  << wxString(_("All Files")) <<  wxT(" (") << wxFileSelectorDefaultWildcardStr << wxT(")|") << wxFileSelectorDefaultWildcardStr;

	wxFileDialog dlg(this,
					 _("Add File(s):"),
					 m_defaultpath,
					 wxEmptyString,
					 filter,
					 wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_MULTIPLE , wxDefaultPosition);

	if (dlg.ShowModal() == wxID_OK) {
		wxArrayString paths;
		dlg.GetPaths( paths );

		m_listBoxExcludelist->Append( paths );
	}
}

void CppCheckSettingsDialog::OnRemoveFile(wxCommandEvent& WXUNUSED(e))
{
	int sel = m_listBoxExcludelist->GetSelection();
	if ( sel != wxNOT_FOUND ) {
		m_listBoxExcludelist->Delete((unsigned int) sel);
	}
}

void CppCheckSettingsDialog::OnClearList(wxCommandEvent& e)
{
	wxUnusedVar(e);
	m_listBoxExcludelist->Clear();
}

void CppCheckSettingsDialog::OnAddSuppression(wxCommandEvent& WXUNUSED(e))
{
	CppCheckAddSuppressionDialog dlg(this);
	while (true) {
		int ret = dlg.ShowModal();
		if (ret != wxID_OK) {
			return;
		}
		wxString key = dlg.GetKey()->GetValue();
		if (m_settings->GetSuppressedWarningsStrings0()->count(key) || m_settings->GetSuppressedWarningsStrings1()->count(key)) {
			int ans = wxMessageBox(_("There is already an entry with ID string. Try again?"), _("CppCheck"), wxYES_NO | wxICON_QUESTION, this);
			if (ans != wxID_YES && ans != wxYES) {
				return;
			} else {
				dlg.GetKey()->Clear();
				continue;
			}
		}

		int index = m_checkListSuppress->Append(dlg.GetDescription()->GetValue());	// If we're here, it worked
		m_checkListSuppress->Check(index, false);
		m_settings->AddSuppressedWarning(dlg.GetKey()->GetValue(), dlg.GetDescription()->GetValue(), false);
		m_SuppressionsKeys.Add(dlg.GetKey()->GetValue());
		return;
	}
}

void CppCheckSettingsDialog::OnRemoveSuppression(wxCommandEvent& WXUNUSED(e))
{
	int ans = wxMessageBox(_("Really remove this warning suppression, rather than just unticking it?"), _("CppCheck"), wxYES_NO | wxICON_QUESTION, this);
	if (ans != wxID_YES && ans != wxYES) {
		return;
	}

	int sel = m_checkListSuppress->GetSelection();
	if (sel != wxNOT_FOUND) {
		m_settings->RemoveSuppressedWarning(m_SuppressionsKeys.Item(sel));
		m_checkListSuppress->Delete((unsigned int)sel);
		m_SuppressionsKeys.RemoveAt(sel);
	}
}

void CppCheckSettingsDialog::OnSuppressTickAll(wxCommandEvent& WXUNUSED(e))
{
	for (size_t n=0; n < m_checkListSuppress->GetCount(); ++n) {
		m_checkListSuppress->Check(n);
	}
}

void CppCheckSettingsDialog::OnSuppressUntickAll(wxCommandEvent& WXUNUSED(e))
{
	for (size_t n=0; n < m_checkListSuppress->GetCount(); ++n) {
		m_checkListSuppress->Check(n, false);
	}
}

void CppCheckSettingsDialog::OnChecksTickAllUI(wxUpdateUIEvent& e)
{
	for (size_t n=0; n < m_checkListExtraWarnings->GetCount(); ++n) {
		if (!m_checkListExtraWarnings->IsChecked(n)) {
			e.Enable(true);
			return;
		}
	}
	e.Enable(false);
}

void CppCheckSettingsDialog::OnChecksUntickAllUI(wxUpdateUIEvent& e)
{
	for (size_t n=0; n < m_checkListExtraWarnings->GetCount(); ++n) {
		if (m_checkListExtraWarnings->IsChecked(n)) {
			e.Enable(true);
			return;
		}
	}
	e.Enable(false);
}

void CppCheckSettingsDialog::OnRemoveFileUI(wxUpdateUIEvent& e)
{
	e.Enable( m_listBoxExcludelist->GetSelection() != wxNOT_FOUND );
}

void CppCheckSettingsDialog::OnClearListUI(wxUpdateUIEvent& e)
{
	e.Enable( !m_listBoxExcludelist->IsEmpty() );
}

void CppCheckSettingsDialog::OnRemoveSuppressionUI(wxUpdateUIEvent& e)
{
	e.Enable(m_checkListSuppress->GetSelection() != wxNOT_FOUND);
}

void CppCheckSettingsDialog::OnSuppressTickAllUI(wxUpdateUIEvent& e)
{
	for (size_t n=0; n < m_checkListSuppress->GetCount(); ++n) {
		if (!m_checkListSuppress->IsChecked(n)) {
			e.Enable(true);
			return;
		}
	}
	e.Enable(false);
}

void CppCheckSettingsDialog::OnSuppressUntickAllUI(wxUpdateUIEvent& e)
{
	for (size_t n=0; n < m_checkListSuppress->GetCount(); ++n) {
		if (m_checkListSuppress->IsChecked(n)) {
			e.Enable(true);
			return;
		}
	}
	e.Enable(false);
}

void CppCheckAddSuppressionDialog::OnOKButtonUpdateUI(wxUpdateUIEvent& e)
{
	e.Enable(!GetKey()->IsEmpty() && !GetDescription()->IsEmpty());
}
