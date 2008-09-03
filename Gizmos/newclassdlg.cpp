//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : newclassdlg.cpp              
//                                                                          
// -------------------------------------------------------------------------
// A                                                                        
//              _____           _      _     _ _                            
//             /  __ \         | |    | |   (_) |                           
//             | /  \/ ___   __| | ___| |    _| |_ ___                      
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )                     
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/                     
//              \____/\___/ \__,_|\___\_____/_|\__\___|                     
//                                                                          
//                                                  F i l e                 
//                                                                          
//    This program is free software; you can redistribute it and/or modify  
//    it under the terms of the GNU General Public License as published by  
//    the Free Software Foundation; either version 2 of the License, or     
//    (at your option) any later version.                                   
//                                                                          
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
 #include "newclassdlg.h"
#include "virtualdirectoryselector.h"
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
	if(item.m_item.IsOk() && item.m_itemType == ProjectItem::TypeVirtualDirectory){
		wxString path = VirtualDirectorySelector::DoGetPath(m_mgr->GetTree(TreeFileView), item.m_item, false);
		if(path.IsEmpty() == false){
			m_textCtrlVD->SetValue(path);
		}
	}
	
	//set the class path to be the active project path
	wxString errMsg;
	if (m_mgr->GetWorkspace()) {
		if (item.m_item.IsOk() && item.m_itemType == ProjectItem::TypeVirtualDirectory) {
			
			m_textCtrlGenFilePath->SetValue(item.m_fileName.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR));
			
		} else {
			
			wxString projname = m_mgr->GetWorkspace()->GetActiveProjectName();
			ProjectPtr proj = m_mgr->GetWorkspace()->FindProjectByName(projname, errMsg);
			if (proj) {
				m_textCtrlGenFilePath->SetValue(proj->GetFileName().GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR));
			}
			
		}
	}
	m_textCtrlFileName->Enable( false );
	GetSizer()->Layout();
	Centre();
	m_textClassName->SetFocus();
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

/**
 * \brief 
 */
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
	wxString path(m_textCtrlGenFilePath->GetValue());
	if (!wxDir::Exists(path)) {
		wxString msg;
		msg << wxT("'") << path << wxT("': directory does not exist");
		wxMessageBox(msg, wxT("CodeLite"), wxOK | wxICON_WARNING);
		return false;
	}
	
	if(GetClassFile().IsEmpty()){
		wxMessageBox(wxT("Empty file name"), wxT("CodeLite"), wxOK|wxICON_WARNING);
		return false;
	}
	
	wxString cpp_file;
	cpp_file << GetClassPath() << wxFileName::GetPathSeparator() << GetClassFile() << wxT(".cpp");
	if( wxFileName::FileExists(cpp_file) ) {
		if(wxMessageBox(wxString::Format(wxT("A file with this name: '%s' already exists, continue anyway?"), cpp_file.GetData()), wxT("CodeLite"), wxYES_NO|wxICON_WARNING) == wxNO){
			return false;
		}
	}
	wxString h_file;
	h_file << GetClassPath() << wxFileName::GetPathSeparator() << GetClassFile() << wxT(".h");
	if( wxFileName::FileExists(h_file) ) {
		if(wxMessageBox(wxString::Format(wxT("A file with this name: '%s' already exists, continue anyway?"), h_file.GetData()), wxT("CodeLite"), wxYES_NO|wxICON_WARNING) == wxNO){
			return false;
		}
	}
	
	if(GetVirtualDirectoryPath().IsEmpty()){
		wxMessageBox(wxT("Please select a virtual directory"), wxT("CodeLite"), wxOK|wxICON_WARNING);
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
	info.fileName = this->GetClassFile();
	info.isVirtualDtor = m_checkBoxVirtualDtor->IsChecked();
	info.implAllPureVirtual = m_checkBoxImplPureVirtual->IsChecked();
	info.implAllVirtual = m_checkBoxImplVirtual->IsChecked();
	info.virtualDirectory = m_textCtrlVD->GetValue();
}

wxString NewClassDlg::GetClassFile()
{
	if(m_checkBoxEnterFileName->IsChecked()) {
		return m_textCtrlFileName->GetValue();
	} else {
		wxString fileName(m_textClassName->GetValue());
		fileName.MakeLower();
		return fileName;
	}
}

void NewClassDlg::OnCheckEnterFileNameManually(wxCommandEvent &e)
{
	if(e.IsChecked()) {
		m_textCtrlFileName->Enable(true);
		m_textCtrlFileName->Clear();
	} else {
		wxString file_name( m_textClassName->GetValue() );
		file_name.MakeLower();
		m_textCtrlFileName->SetValue( file_name );
		m_textCtrlFileName->Enable(false);
	}
	e.Skip();
}

void NewClassDlg::OnTextEnter(wxCommandEvent &e)
{
	if(m_checkBoxEnterFileName->IsChecked() == false) {
		wxString file_name( m_textClassName->GetValue() );
		file_name.MakeLower();
		m_textCtrlFileName->SetValue( file_name );
	}
}

void NewClassDlg::OnCheckImpleAllVirtualFunctions(wxCommandEvent &e)
{
	if(e.IsChecked()) {
		m_checkBoxImplPureVirtual->SetValue(true);
	}
}

wxString NewClassDlg::GetClassPath()
{
	if(m_textCtrlGenFilePath->GetValue().Trim().IsEmpty()) {
		return wxT(".");
	} else {
		return m_textCtrlGenFilePath->GetValue();
	}
}

void NewClassDlg::OnBrowseFolder(wxCommandEvent& e)
{
	wxUnusedVar(e);
	wxString initPath;
	if(wxFileName::DirExists(m_textCtrlGenFilePath->GetValue())){
		initPath = m_textCtrlGenFilePath->GetValue();
	}
	wxString new_path = wxDirSelector(wxT("Select Generated Files Path:"), initPath, wxDD_DEFAULT_STYLE, wxDefaultPosition, this);
	if(new_path.IsEmpty() == false){
		m_textCtrlGenFilePath->SetValue(new_path);
	}
}

void NewClassDlg::OnBrowseVD(wxCommandEvent& e)
{
	wxUnusedVar(e);
	VirtualDirectorySelector dlg(this, m_mgr->GetWorkspace(), m_textCtrlVD->GetLabel());
	if(dlg.ShowModal() == wxID_OK){
		m_textCtrlVD->SetValue(dlg.GetVirtualDirectoryPath());
	}
}
