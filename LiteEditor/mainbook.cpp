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
#include "quickfindbar.h"
#include "frame.h"
#include "mainbook.h"
#include "manager.h"
#include "custom_notebook.h"
#include "custom_tabcontainer.h"
#include "navbar.h"

MainBook::MainBook(wxWindow *parent)
    : wxPanel( parent )
{
	wxBoxSizer *sz = new wxBoxSizer(wxVERTICAL);
	SetSizer(sz);
    
    m_navBar = new NavBar(this);
    sz->Add(m_navBar, 0, wxEXPAND);
    
	long style = wxVB_TOP|wxVB_HAS_X|wxVB_BORDER|wxVB_TAB_DECORATION|wxVB_MOUSE_MIDDLE_CLOSE_TAB;
	m_book = new Notebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, style);
	m_book->GetTabContainer()->Connect(wxEVT_LEFT_DCLICK, wxMouseEventHandler(MainBook::OnMouseDClick), NULL, this);
	sz->Add(m_book, 1, wxEXPAND);
    
	m_quickFindBar = new QuickFindBar(this);
	sz->Add(m_quickFindBar, 0, wxEXPAND);
    
	sz->Layout();
}

MainBook::~MainBook()
{
}

void MainBook::Clear()
{
	m_navBar->UpdateScope(NULL);
	m_quickFindBar->DoShow(false);
}

void MainBook::UpdateScope(TagEntryPtr tag)
{
    m_navBar->UpdateScope(tag);
}

void MainBook::ShowNavBar(bool s)
{
	Freeze();
    m_navBar->DoShow(s);
	Thaw();
	
	Refresh();
}

bool MainBook::IsNavBarShown()
{
	return m_navBar->IsShown();
}

void MainBook::OnMouseDClick(wxMouseEvent& e)
{
	wxUnusedVar(e);
	Frame::Get()->DoAddNewFile();
}

void MainBook::ShowQuickBar(bool s)
{
    m_quickFindBar->DoShow(s);
}
