#include "macrosdlg.h"
#include "editcmpfileinfodlg.h"

EditCmpFileInfo::EditCmpFileInfo( wxWindow* parent)
		: EditCmpFileInfoBase( parent )
{

}

void EditCmpFileInfo::OnFileTypeText( wxCommandEvent& event )
{
	event.Skip();
}

void EditCmpFileInfo::OnCompilationLine( wxCommandEvent& event )
{
	event.Skip();
}

void EditCmpFileInfo::OnButtonOk( wxCommandEvent& event )
{
	wxUnusedVar(event);
	EndModal(wxID_OK);
}

void EditCmpFileInfo::OnButtonOkUI( wxUpdateUIEvent& event )
{
	event.Enable(!m_textCtrl2->IsEmpty() && !m_textCtrlFileType->IsEmpty());
}

void EditCmpFileInfo::OnButtonCancel( wxCommandEvent& event )
{
	wxUnusedVar(event);
	EndModal(wxID_CANCEL);
}

void EditCmpFileInfo::SetCompilationLine(const wxString& s)
{
	m_textCtrl2->SetValue(s);
}

void EditCmpFileInfo::SetExtension(const wxString& s)
{
	m_textCtrlFileType->SetValue(s);
}

void EditCmpFileInfo::SetKind(const wxString& s)
{
	int where = m_choiceFileTypeIs->FindString(s);
	if (where != wxNOT_FOUND) {
		m_choiceFileTypeIs->Select(where);
	}
}

void EditCmpFileInfo::OnButtonHelp(wxCommandEvent& event)
{
	wxUnusedVar(event);
	MacrosDlg dlg(this, MacrosDlg::MacrosCompiler);
	dlg.ShowModal();
}
