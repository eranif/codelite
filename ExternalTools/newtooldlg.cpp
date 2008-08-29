#include "newtooldlg.h"
#include <wx/dirdlg.h>
#include <wx/filedlg.h>
#include "macrosdlg.h"

NewToolDlg::NewToolDlg( wxWindow* parent, const wxString &id, const wxString &path, const wxString &wd, const wxString &args  )
		: NewToolBase( parent )
{
	m_choiceId->SetFocus();
	m_textCtrlArguments->SetValue(args);
	m_choiceId->SetStringSelection(id);
	m_textCtrlPath->SetValue(path);
	m_textCtrlWd->SetValue(wd);
}

void NewToolDlg::OnButtonBrowsePath( wxCommandEvent& event )
{
	wxUnusedVar(event);
	wxString path = m_textCtrlPath->GetValue();
	wxString new_path = wxFileSelector(wxT("Select a program:"), path.c_str());
	if (new_path.IsEmpty() == false) {
		m_textCtrlPath->SetValue(new_path);
	}
}

void NewToolDlg::OnButtonBrowseWD( wxCommandEvent& event )
{
	wxUnusedVar(event);
	wxString path(m_textCtrlWd->GetValue());
	wxString new_path = wxDirSelector(wxT("Select working directory:"), path);
	if(new_path.IsEmpty() == false){
		m_textCtrlWd->SetValue(new_path);
	}
}

void NewToolDlg::OnButtonHelp( wxCommandEvent& event )
{
	wxUnusedVar(event);
	MacrosDlg dlg(this);
	dlg.ShowModal();
}

void NewToolDlg::OnButtonOk( wxCommandEvent& event )
{
	wxUnusedVar(event);
	EndModal(wxID_OK);
}

void NewToolDlg::OnButtonCancel( wxCommandEvent& event )
{
	wxUnusedVar(event);
	EndModal(wxID_CANCEL);
}
