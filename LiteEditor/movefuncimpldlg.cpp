#include "wx/filename.h"
#include "movefuncimpldlg.h"

MoveFuncImplDlg::MoveFuncImplDlg( wxWindow* parent, const wxString &text, const wxString &fileName )
: MoveFuncImplBaseDlg( parent )
{
	m_preview->SetText(text);
	m_filePicker->SetPath(fileName);
}

void MoveFuncImplDlg::SetText(const wxString &text)
{
	m_preview->SetText(text);
}

wxString MoveFuncImplDlg::GetText()
{
	return m_preview->GetText();
}

void MoveFuncImplDlg::SetFileName(const wxString &fileName)
{
	m_filePicker->SetPath(fileName);
}

wxString MoveFuncImplDlg::GetFileName()
{
	return m_filePicker->GetPath();
}

void MoveFuncImplDlg::OnButtonCancel(wxCommandEvent &e)
{
	wxUnusedVar(e);
	EndModal(wxID_CANCEL);
}

void MoveFuncImplDlg::OnButtonOK(wxCommandEvent &e)
{
	wxUnusedVar(e);
	//make sure that the file exist
	if(!wxFileName::FileExists(m_filePicker->GetPath())){
		wxMessageBox(wxT("File: ") + m_filePicker->GetPath() + wxT(" does not exist"), 
					 wxT("CodeLite"), wxICON_WARNING| wxOK);
		return;
	}
	EndModal(wxID_OK);
}
