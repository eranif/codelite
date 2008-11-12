//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : navbar.cpp              
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
#include <vector>
#include "ctags_manager.h"
#include "cl_editor.h"
#include "manager.h"
#include "macros.h"
#include "navbar.h"

NavBar::NavBar(wxWindow* parent)
    : wxPanel(parent)
{
    wxBoxSizer *sz = new wxBoxSizer(wxHORIZONTAL);
    SetSizer(sz);
    
    m_scope = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxSize(200, -1));
    sz->Add(m_scope, 0, wxEXPAND);
    m_scope->Connect(wxEVT_LEFT_DOWN, wxMouseEventHandler(NavBar::OnScopeListMouseDown), NULL, this);
    ConnectChoice(m_scope, NavBar::OnScope);
    
    m_func = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxSize(600, -1));
    sz->Add(m_func, 1, wxEXPAND);
    m_func->Connect(wxEVT_LEFT_DOWN, wxMouseEventHandler(NavBar::OnFuncListMouseDown), NULL, this);
    ConnectChoice(m_func, NavBar::OnFunction);
    
    sz->Layout();
}

NavBar::~NavBar()
{
}

void NavBar::OnScopeListMouseDown(wxMouseEvent& e)
{
	Manager *mgr = ManagerST::Get();
    if (!mgr->IsWorkspaceOpen())
        return;
        
    LEditor *editor = mgr->GetActiveEditor();
    if (!editor)
        return;
        
    std::vector<wxString> scopes;
    TagsManagerST::Get()->GetScopesFromFile(editor->GetFileName(), scopes);
    
    m_scope->Freeze();
    
    wxString cursel = m_scope->GetStringSelection();
    m_scope->Clear();
    for (unsigned i = 0; i < scopes.size(); i++) {
        m_scope->AppendString(scopes[i]);
    }
    if (!cursel.IsEmpty()) {
        m_scope->SetStringSelection(cursel);
    }
    
    m_scope->Thaw();
            
    e.Skip();
}

void NavBar::OnScope(wxCommandEvent& e)
{
    size_t sel = e.GetSelection();
    if (sel < m_scope->GetCount()) {
        m_tags.clear();
        m_func->Clear();
    }
}

void NavBar::OnFuncListMouseDown(wxMouseEvent& e)
{
	Manager *mgr = ManagerST::Get();
    if (!mgr->IsWorkspaceOpen())
        return;
        
    LEditor *editor = mgr->GetActiveEditor();
    if (!editor)
        return;
        
    wxString scope = m_scope->GetStringSelection();
    if (scope.IsEmpty())
        return;
        
    m_tags.clear();
    TagsManagerST::Get()->TagsFromFileAndScope(editor->GetFileName(), scope, m_tags);
    
    m_func->Freeze();
    
    wxString cursel = m_func->GetStringSelection();
    m_func->Clear();
    for (size_t i = 0; i < m_tags.size(); i++) {
        m_func->AppendString(m_tags[i]->GetDisplayName());
    }
    if (!cursel.IsEmpty()) {
        m_func->SetStringSelection(cursel);
    }
    
    m_func->Thaw();
    
    e.Skip();
}

void NavBar::OnFunction(wxCommandEvent& e)
{
    LEditor *editor = ManagerST::Get()->GetActiveEditor();
    if (!editor)
        return;
        
	size_t sel = e.GetSelection();
	if (sel >= m_tags.size())
        return;
        
    wxString pattern = m_tags[sel]->GetPattern();
    wxString name = m_tags[sel]->GetName();
    ManagerST::Get()->FindAndSelect(editor, pattern, name);
    editor->SetActive();
}

void NavBar::DoShow(bool s)
{
    if (Show(s)) {
        wxSizer *sz = GetParent()->GetSizer();
        sz->Show(this, s, true);
        sz->Layout();
    }
}

void NavBar::UpdateScope(TagEntryPtr tag)
{
    size_t sel = m_func->GetSelection();
    if (tag && sel < m_tags.size() && *m_tags[sel] == *tag)
        return;
        
    Freeze();
    
    m_tags.clear();
    m_scope->Clear();
    m_func->Clear();
    
    if (tag) {
        m_tags.push_back(tag);
        m_scope->AppendString(tag->GetScope());
        m_func->AppendString(tag->GetDisplayName());
        m_scope->SetSelection(0);
        m_func->SetSelection(0);
    }
    
    Thaw();
}

//void NavBar::UpdateScope(LEditor *editor)
//{
//    if (!editor || !IsShown())
//        return;
//        
//	static long lastPos(wxNOT_FOUND);
//	static long lastLine(wxNOT_FOUND);
//
//	//check the current position
//	long curpos = editor->GetCurrentPos();
//	if (curpos == lastPos) 
//        return;
//    lastPos = curpos;
//    
//    //check the current line
//    long curline = editor->LineFromPosition(curpos);
//    if (curline == lastLine)
//        return;
//    lastLine = curline;
//    
//    TagEntryPtr tag = TagsManagerST::Get()->FunctionFromFileLine(editor->GetFileName(), lastLine+1);
//	UpdateScope(tag);
//}

