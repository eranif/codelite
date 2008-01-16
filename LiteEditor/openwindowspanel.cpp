#include "openwindowspanel.h"
#include "wx/wxFlatNotebook/wxFlatNotebook.h"
#include "frame.h"
#include <wx/clntdata.h>
#include "manager.h"

OpenWindowsPanel::OpenWindowsPanel( wxWindow* parent )
		: 
		OpenWindowsPanelBase( parent )
{
}

void OpenWindowsPanel::Clear()
{
	m_fileList->Clear();
}

void OpenWindowsPanel::UpdateList()
{
	m_fileList->Freeze();
	m_fileList->Clear();
	wxFlatNotebook *book = Frame::Get()->GetNotebook();
	for (int i=0; i< (int)book->GetPageCount(); i++) {
		LEditor *editor = dynamic_cast<LEditor*>(book->GetPage((size_t)i));
		if (editor) {
			wxString txt = book->GetPageText((size_t)i);
			m_fileList->Append(txt, new wxStringClientData(editor->GetFileName().GetFullPath()));
		}
	}
	SyncSelection();
	m_fileList->Thaw();
}

void OpenWindowsPanel::SyncSelection()
{
	wxFlatNotebook *book = Frame::Get()->GetNotebook();
	int sel = book->GetSelection();
	if (sel != wxNOT_FOUND) {
		LEditor *editor = dynamic_cast<LEditor*>(book->GetPage((size_t)sel));
		if (editor) {
			//Ok, the selection is a valid editor (unlike the Welcome page)
			wxString fullname = editor->GetFileName().GetFullPath();
			for (size_t i=0; i< m_fileList->GetCount(); i++) {
				wxStringClientData *data = dynamic_cast<wxStringClientData *>(m_fileList->GetClientObject((unsigned int)i));
				if (data && data->GetData() == fullname) {
					m_fileList->Select((int)i);
				}
			}
		}
	}
}

void OpenWindowsPanel::OnKeyDown( wxKeyEvent& event )
{
	if(	event.GetKeyCode() == WXK_RETURN || event.GetKeyCode() == WXK_NUMPAD_ENTER){
		int selection = m_fileList->GetSelection();
		if(selection != wxNOT_FOUND){
			DoOpenSelectedItem(selection);
			return;
		}
	}
	event.Skip();
}

void OpenWindowsPanel::OnItemDClicked( wxCommandEvent& event )
{
	int item = event.GetSelection();
	DoOpenSelectedItem(item);
}

void OpenWindowsPanel::OnItemSelected(wxCommandEvent &e)
{
//	int item = e.GetSelection();
//	DoOpenSelectedItem(item);
	e.Skip();
}

void OpenWindowsPanel::OnChar(wxKeyEvent& event)
{
	if(	event.GetKeyCode() == WXK_RETURN || event.GetKeyCode() == WXK_NUMPAD_ENTER){
		int selection = m_fileList->GetSelection();
		if(selection != wxNOT_FOUND){
			DoOpenSelectedItem(selection);
			return;
		}
	}
	event.Skip();
}

void OpenWindowsPanel::OnRightUp( wxMouseEvent& event )
{
	wxUnusedVar(event);
}

void OpenWindowsPanel::DoOpenSelectedItem(int item)
{
	wxStringClientData *data = dynamic_cast<wxStringClientData *>(m_fileList->GetClientObject((unsigned int)item));
	if (data) {
		wxString fullpath = data->GetData();
		//open this file...
		Manager *mgr = ManagerST::Get();
		mgr->OpenFile(fullpath, wxEmptyString);
	}
}
