//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : testclassdlg.cpp              
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

#include "testclassdlg.h"
#include "imanager.h"
#include "ctags_manager.h"

TestClassDlg::TestClassDlg( wxWindow* parent, IManager *mgr )
		: TestClassBaseDlg( parent )
		, m_manager(mgr)
{
	m_manager->GetTagsManager()->GetClasses(m_tags);
	IEditor *editor = m_manager->GetActiveEditor();
	if (editor) {
		m_textCtrlFileName->SetValue(editor->GetFileName().GetFullPath());
		m_textCtrlFileName->Enable(false);
	}
}

void TestClassDlg::OnRefreshFunctions( wxCommandEvent& event )
{
	std::vector<TagEntryPtr> matches;

	// search m_tags for suitable name
	for (size_t i=0; i<m_tags.size(); i++) {
		TagEntryPtr tag = m_tags.at(i);
		if (tag->GetName() == m_textCtrlClassName->GetValue()) {
			matches.push_back(tag);
		}
	}

	if (matches.empty()) {
		wxMessageBox(wxT("Could not find match for class '")+m_textCtrlClassName->GetValue()+wxT("'"), wxT("CodeLite"), wxICON_WARNING|wxOK);
		return;
	}

	wxString theClass;
	if (matches.size() == 1) {
		//single match we are good
		theClass = matches.at(0)->GetPath();
	} else {
		// suggest the user a multiple choice
		wxArrayString choices;

		for (size_t i=0; i<matches.size(); i++) {
			wxString option;
			TagEntryPtr t = matches.at(i);
			choices.Add(t->GetPath());
		}

		theClass = wxGetSingleChoice(wxT("Select class:"), wxT("Select class:"), choices, this);
	}

	if (theClass.empty()) {//user clicked 'Cancel'
		return;
	}

	// get list of methods for the given path
	matches.clear();
	m_manager->GetTagsManager()->TagsByScope(theClass, wxT("prototype"), matches, false, true);

	// populate the list control
	wxArrayString methods;
	for (size_t i=0; i<matches.size(); i++) {
		TagEntryPtr t = matches.at(i);
		methods.Add(t->GetName() + t->GetSignature());
	}
	m_checkListMethods->Clear();
	m_checkListMethods->Append(methods);

	// check all items
	for (unsigned int idx = 0; idx < m_checkListMethods->GetCount(); idx++) {
		m_checkListMethods->Check(idx, true);
	}
}

void TestClassDlg::OnUseActiveEditor( wxCommandEvent& event )
{
	if (event.IsChecked()) {
		IEditor *editor = m_manager->GetActiveEditor();
		if (editor) {
			m_textCtrlFileName->SetValue(editor->GetFileName().GetFullPath());
		}
		m_textCtrlFileName->Enable(true);
	} else {
		m_textCtrlFileName->Enable(false);
	}
}

void TestClassDlg::OnRefreshButtonUI(wxUpdateUIEvent& e)
{
	e.Enable(m_textCtrlClassName->IsEmpty() ? false : true );
}

void TestClassDlg::OnCheckAll(wxCommandEvent& e)
{
	// check all items
	for (unsigned int idx = 0; idx < m_checkListMethods->GetCount(); idx++) {
		m_checkListMethods->Check(idx, true);
	}
}

void TestClassDlg::OnUnCheckAll(wxCommandEvent& e)
{
	// check all items
	for (unsigned int idx = 0; idx < m_checkListMethods->GetCount(); idx++) {
		m_checkListMethods->Check(idx, false);
	}
}
wxArrayString TestClassDlg::GetTestsList()
{
	wxArrayString results;
	for (unsigned int idx = 0; idx < m_checkListMethods->GetCount(); idx++) {
		if(m_checkListMethods->IsChecked(idx)){
			wxString str = m_checkListMethods->GetString(idx);
			results.Add(str.BeforeFirst(wxT('(')));
		}
	}
	return results;
}

void TestClassDlg::OnUseFixture(wxCommandEvent& e)
{
	m_textCtrlFixtureName->Enable(e.IsChecked());
}

void TestClassDlg::OnButtonOk(wxCommandEvent& e)
{
	// validate the class name
	if( m_textCtrlFileName->GetValue().IsEmpty() ){
		wxMessageBox(wxT("Please provide a class name"), wxT("CodeLite"), wxICON_WARNING|wxOK);
		return;
	}
	
	if( m_checkListMethods->GetCount() == 0 ){
		wxMessageBox(wxT("There are no tests to generate"), wxT("CodeLite"), wxICON_WARNING|wxOK);
		return;
	}
	
	EndModal(wxID_OK);
}

void TestClassDlg::OnClassNameTyped(wxCommandEvent& e)
{
	// scane the database for classes
	if(!m_checkBox1->IsChecked()) {
		wxString file_name = m_textCtrlFileName->GetValue();
		wxFileName fn(file_name);
		fn.SetName(wxT("test_") + m_textCtrlClassName->GetValue().MakeLower());
		
		m_textCtrlFileName->SetValue(fn.GetFullPath());
	}
	e.Skip();
}
