#include "manage_perspective_dlg.h"
#include "manager.h"
#include "perspectivemanager.h"

ManagePerspectivesDlg::ManagePerspectivesDlg( wxWindow* parent )
	: ManagePerspectivesBaseDlg( parent )
{
	m_listBox->Clear();
	m_listBox->Append(ManagerST::Get()->GetPerspectiveManager().GetAllPerspectives());
}

void ManagePerspectivesDlg::OnDeletePerspective( wxCommandEvent& event )
{
	wxString sel = m_listBox->GetStringSelection();
	if(sel.IsEmpty())
		return;

	if(::wxMessageBox(wxString::Format(_("Are you sure you want to delete perspective '%s'?"), sel.c_str()), wxT("CodeLite"), wxYES_NO|wxICON_QUESTION) == wxYES) {
		ManagerST::Get()->GetPerspectiveManager().Delete(sel);
		DoPopulateList();
	}
}

void ManagePerspectivesDlg::OnDeletePerspectiveUI( wxUpdateUIEvent& event )
{
	event.Enable(m_listBox->GetSelection() != wxNOT_FOUND);
}

void ManagePerspectivesDlg::OnRenamePerspective( wxCommandEvent& event )
{
	wxString sel = m_listBox->GetStringSelection();
	if(sel.IsEmpty())
		return;

	wxString newName = ::wxGetTextFromUser(_("Enter new name:"), _("Rename perspective"));
	if(newName.IsEmpty())
		return;

	ManagerST::Get()->GetPerspectiveManager().Rename(sel, newName);
	DoPopulateList();
}

void ManagePerspectivesDlg::OnRenamePerspectiveUI( wxUpdateUIEvent& event )
{
	event.Enable(m_listBox->GetSelection() != wxNOT_FOUND);
}

void ManagePerspectivesDlg::DoPopulateList()
{
	// Refresh the list
	m_listBox->Clear();
	m_listBox->Append(ManagerST::Get()->GetPerspectiveManager().GetAllPerspectives());
}
