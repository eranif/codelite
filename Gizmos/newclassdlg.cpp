#include "newclassdlg.h"
#include "wx/xrc/xmlres.h"
#include "newinheritancedlg.h"
#include "imanager.h"
#include "globals.h"
#include "wx/dir.h"
#include "workspace.h"

NewClassDlg::NewClassDlg( wxWindow* parent, IManager *mgr )
		: NewClassBaseDlg( parent, wxID_ANY, wxT("New Class"), wxDefaultPosition, wxSize( 690,631 ), wxDEFAULT_DIALOG_STYLE)
		, m_selectedItem(wxNOT_FOUND)
		, m_mgr(mgr)
{
	m_bmp->SetBitmap(wxXmlResource::Get()->LoadBitmap(wxT("new_class_title")));

	//set two columns to our list
	m_listCtrl1->InsertColumn(0, wxT("Name"));
	m_listCtrl1->InsertColumn(1, wxT("Access"));

	TreeItemInfo item = mgr->GetSelectedTreeItemInfo(TreeFileView);
	//set the class path to be the active project path
	wxString errMsg;
	if (m_mgr->GetWorkspace()) {
		if (item.m_item.IsOk() && item.m_itemType == ProjectItem::TypeVirtualDirectory) {
			m_dirPicker->SetPath(item.m_fileName.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR));
		} else {
			wxString projname = m_mgr->GetWorkspace()->GetActiveProjectName();
			ProjectPtr proj = m_mgr->GetWorkspace()->FindProjectByName(projname, errMsg);
			if (proj) {
				m_dirPicker->SetPath(proj->GetFileName().GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR));
			}
		}
	}
	GetSizer()->Layout();
}

void NewClassDlg::OnListItemActivated( wxListEvent& event )
{
	m_selectedItem = event.m_itemIndex;
	//open the inheritance dialog
	wxString parentName = GetColumnText(m_listCtrl1, m_selectedItem, 0);
	wxString access = GetColumnText(m_listCtrl1, m_selectedItem, 1);
	NewIneritanceDlg *dlg = new NewIneritanceDlg(NULL, m_mgr, parentName, access);
	if (dlg->ShowModal() == wxID_OK) {
		//now set the text to this column
		SetColumnText(m_listCtrl1, m_selectedItem, 0, dlg->GetParentName());
		SetColumnText(m_listCtrl1, m_selectedItem, 1, dlg->GetAccess());
		m_listCtrl1->Refresh();
	}
	dlg->Destroy();
}

void NewClassDlg::OnListItemSelected( wxListEvent& event )
{
	m_selectedItem = event.m_itemIndex;
}

void NewClassDlg::OnButtonAdd( wxCommandEvent& event )
{
	NewIneritanceDlg *dlg = new NewIneritanceDlg(this, m_mgr);
	if (dlg->ShowModal() == wxID_OK) {
		//add new parent to our class
		//now set the text to this column
		long item = AppendListCtrlRow(m_listCtrl1);
		
		SetColumnText(m_listCtrl1, item, 0, dlg->GetParentName());
		SetColumnText(m_listCtrl1, item, 1, dlg->GetAccess());
		m_listCtrl1->Refresh();
	}
	dlg->Destroy();
}

void NewClassDlg::OnListItemDeSelected(wxListEvent &e)
{
	wxUnusedVar(e);
	m_selectedItem = wxNOT_FOUND;
}

void NewClassDlg::OnButtonDelete( wxCommandEvent& event )
{
	wxUnusedVar(event);
	m_listCtrl1->DeleteItem(m_selectedItem);
	m_selectedItem = wxNOT_FOUND;
}

void NewClassDlg::OnButtonDeleteUI( wxUpdateUIEvent& event )
{
	event.Enable(m_selectedItem != wxNOT_FOUND);
}

void NewClassDlg::GetInheritance(std::vector< ClassParentInfo > &inheritVec)
{
	long item = -1;
	for ( ;; ) {
		item = m_listCtrl1->GetNextItem(item);
		if ( item == -1 )
			break;

		ClassParentInfo info;
		info.name = GetColumnText(m_listCtrl1, item, 0);
		info.access = GetColumnText(m_listCtrl1, item, 1);
		inheritVec.push_back(info);
	}
}

void NewClassDlg::OnButtonOK(wxCommandEvent &e)
{
	wxUnusedVar(e);
	if (!ValidateInput()) {
		return;
	}
	EndModal(wxID_OK);
}

bool NewClassDlg::ValidateInput()
{
	//validate the class name
	if (!IsValidCppIndetifier( m_textClassName->GetValue() )) {
		wxString msg;
		msg << wxT("'") << m_textClassName->GetValue() << wxT("' is not a valid C++ qualifier");
		wxMessageBox(msg, wxT("CodeLite"), wxOK | wxICON_WARNING);
		return false;
	}

	//validate the path of the class
	wxString path(m_dirPicker->GetPath());
	if (!wxDir::Exists(path)) {
		wxString msg;
		msg << wxT("'") << path << wxT("': directory does not exist");
		wxMessageBox(msg, wxT("CodeLite"), wxOK | wxICON_WARNING);
		return false;
	}
	return true;
}

void NewClassDlg::GetNewClassInfo(NewClassInfo &info)
{
	info.isSingleton = this->IsSingleton();
	info.name = this->GetClassName();
	this->GetInheritance(info.parents);
	info.path = this->GetClassPath();
	info.isAssingable = this->IsCopyableClass();
	info.isVirtualDtor = m_checkBoxVirtualDtor->IsChecked();
}
