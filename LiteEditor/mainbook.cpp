//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : mainbook.cpp              
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
#include <wx/xrc/xmlres.h>
#include "frame.h"
 #include "mainbook.h"
#include "wx/choice.h"
#include "wx/xrc/xmlres.h"
#include "wx/log.h"
#include "manager.h"
#include "ctags_manager.h"
#include "cl_editor.h"
#include "macros.h"
#include "custom_notebook.h"
#include "custom_tabcontainer.h"

MainBook::MainBook(wxWindow *parent)
: wxPanel( parent )
{
	wxArrayString chcs;
	wxBoxSizer *sz = new wxBoxSizer(wxVERTICAL);
	m_hsz = new wxBoxSizer(wxHORIZONTAL);
	
	SetSizer(sz);
	sz->Add(m_hsz, 0, wxEXPAND);
	
	m_choiceScope = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxSize(200, -1), chcs);
	m_choiceFunc = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxSize(600, -1), chcs);
	m_hsz->Add(m_choiceScope, 0, wxEXPAND);
	m_hsz->Add(m_choiceFunc, 1, wxEXPAND);
	
	//Connect events
	m_choiceFunc->Connect(wxEVT_LEFT_DOWN, wxMouseEventHandler(MainBook::OnFuncListMouseDown), NULL, this);
	//m_choiceScope->Connect(wxEVT_LEFT_DOWN, wxMouseEventHandler(MainBook::OnScopeListMouseDown), NULL, this);
	ConnectChoice(m_choiceFunc, MainBook::OnFunction);
	//ConnectChoice(m_choiceScope, MainBook::OnScope);
	
	long style = wxVB_TOP|wxVB_HAS_X|wxVB_BORDER|wxVB_TAB_DECORATION|wxVB_MOUSE_MIDDLE_CLOSE_TAB;
	m_book = new Notebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, style);
	
	m_book->GetTabContainer()->Connect(wxEVT_LEFT_DCLICK, wxMouseEventHandler(MainBook::OnMouseDClick), NULL, this);
	
	sz->Add(m_book, 1, wxEXPAND);
	sz->Layout();
}

MainBook::~MainBook()
{
}

void MainBook::OnScope(wxCommandEvent &e)
{
//	wxUnusedVar(e);
//	int sel = e.GetSelection();
//	if(sel != wxNOT_FOUND){
//		TagEntry *tag = (TagEntry*) m_choiceScope->GetClientData(sel);
//		if(tag){
//			ManagerST::Get()->OpenFile(tag->GetFile(), wxEmptyString, tag->GetLine()-1);
//		}
//	}
}

void MainBook::OnFunction(wxCommandEvent &e)
{
	int sel = e.GetSelection();
	if(sel != wxNOT_FOUND){
		TagEntry *tag = (TagEntry*) m_choiceFunc->GetClientData(sel);
		if(tag){
			LEditor *editor = ManagerST::Get()->GetActiveEditor();
			if(editor) {
				wxString pattern(tag->GetPattern());
				ManagerST::Get()->FindAndSelect(editor, pattern, tag->GetName());
				editor->SetActive();
			}
		}
	}
}

void MainBook::Clear()
{
	if(!m_choiceFunc ||  !m_choiceScope)
		return;
		
	m_choiceFunc->Freeze();
	m_choiceScope->Freeze();
	m_choiceFunc->Clear();
	m_choiceScope->Clear();
	m_choiceFunc->Thaw();
	m_choiceScope->Thaw();
}

void MainBook::UpdateScope(TagEntryPtr tag)
{
	if(!tag){
		return;
	}

	if(!m_choiceFunc ||  !m_choiceScope)
		return;

	wxString function;
	function << tag->GetName() << tag->GetSignature();
	
	m_choiceFunc->Freeze();
	m_choiceScope->Freeze();
	
	m_choiceFunc->Clear(); 
	m_choiceScope->Clear();
	
	m_choiceFunc->AppendString(function);
	m_choiceScope->AppendString(tag->GetScope());
	
	m_choiceFunc->SetSelection(0);
	m_choiceScope->SetSelection(0);

	m_choiceFunc->Thaw();
	m_choiceScope->Thaw();
}

void MainBook::OnScopeListMouseDown(wxMouseEvent &e)
{
//	TagsManager *tagsmgr = TagsManagerST::Get();
//	Manager *mgr = ManagerST::Get();
//	
//	if(mgr->IsWorkspaceOpen()){
//		LEditor *editor = mgr->GetActiveEditor();
//		if(editor){
//			std::vector< TagEntryPtr > tags;
//			tagsmgr->GetScopesFromFile(editor->GetFileName(), tags);
//			m_choiceScope->Freeze();
//			wxString cursel;
//			if(m_choiceScope->GetCount() > 0){
//				cursel = m_choiceScope->GetStringSelection();
//			}
//			
//			m_choiceScope->Clear();
//			
//			if(tags.empty()){
//				if(cursel.IsEmpty() == false && cursel != wxT("<global>")){
//					m_choiceScope->Append(wxT("<global>"), (void*)NULL);
//				}
//			}
//			
//			for(size_t i=0; i< tags.size(); i++){
//				m_choiceScope->Append(tags.at(i)->GetPath(), new TagEntry(*tags.at(i)));
//			}
//			
//			if(cursel.IsEmpty() == false){
//				m_choiceScope->SetStringSelection(cursel);
//			}
//			m_choiceScope->Thaw();
//		}
//	}
//	
//	e.Skip();
}

void MainBook::OnFuncListMouseDown(wxMouseEvent &e)
{
	TagsManager *tagsmgr = TagsManagerST::Get();
	Manager *mgr = ManagerST::Get();
	if(mgr->IsWorkspaceOpen()){
		LEditor *editor = mgr->GetActiveEditor();
		if(editor){
			std::vector< TagEntryPtr > tags;
			tagsmgr->TagsFromFileAndScope(editor->GetFileName(), m_choiceScope->GetStringSelection(), tags);
			m_choiceFunc->Freeze();
			
			wxString cursel;
			if(m_choiceFunc->GetCount() > 0){
				cursel = m_choiceFunc->GetStringSelection();
			}
			
			m_choiceFunc->Clear();
			for(size_t i=0; i< tags.size(); i++){
				m_choiceFunc->Append(tags.at(i)->GetName() + tags.at(i)->GetSignature(), new TagEntry(*tags.at(i)));
			}
			
			if(cursel.IsEmpty() == false){
				m_choiceFunc->SetStringSelection(cursel);
			}
			m_choiceFunc->Thaw();
		}
	}
	e.Skip();
}

void MainBook::ShowNavBar(bool s)
{
	if( s ) {
		if( !m_hsz->IsShown(m_choiceFunc) ) {
			m_hsz->Show(m_choiceFunc);
		}
		
		if( !m_hsz->IsShown(m_choiceScope) ) {
			m_hsz->Show(m_choiceScope);
		}
	} else { // Hide
		if( m_hsz->IsShown(m_choiceFunc) ) {
			m_hsz->Hide(m_choiceFunc);
		}
		
		if( m_hsz->IsShown(m_choiceScope) ) {
			m_hsz->Hide(m_choiceScope);
		}
	}
	m_book->Refresh();
	GetSizer()->Layout();
}

bool MainBook::IsNavBarShown()
{
	// it is enough to test only control
	return m_hsz->IsShown(m_choiceFunc);
}

void MainBook::OnMouseDClick(wxMouseEvent& e)
{
	wxUnusedVar(e);
	Frame::Get()->DoAddNewFile();
}
