//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : findresultscontainer.cpp              
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
 #include "findresultscontainer.h"
#include "output_pane.h"
#include "findresultstab.h"
#include "custom_notebook.h"

FindResultsContainer::FindResultsContainer(wxWindow *win, wxWindowID id)
		: wxPanel(win, id)
{
	Initialize();
}

FindResultsContainer::~FindResultsContainer()
{
}

void FindResultsContainer::Initialize()
{
	wxBoxSizer *sz = new wxBoxSizer(wxVERTICAL);
	SetSizer(sz);

	m_book = new Notebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVB_BOTTOM|wxVB_NODND);

	//add 5 tabs for the find results
	FindResultsTab *tab = new FindResultsTab(m_book, wxID_ANY, OutputPane::FIND_IN_FILES_WIN);
	m_book->AddPage(tab, wxT("Find Results 1"), wxNullBitmap, false);

	tab = new FindResultsTab(m_book, wxID_ANY, OutputPane::FIND_IN_FILES_WIN);
	m_book->AddPage(tab, wxT("Find Results 2"),  wxNullBitmap, false);

	tab = new FindResultsTab(m_book, wxID_ANY, OutputPane::FIND_IN_FILES_WIN);
	m_book->AddPage(tab, wxT("Find Results 3"), wxNullBitmap,  false);

	tab = new FindResultsTab(m_book, wxID_ANY, OutputPane::FIND_IN_FILES_WIN);
	m_book->AddPage(tab, wxT("Find Results 4"), wxNullBitmap,  false);

	tab = new FindResultsTab(m_book, wxID_ANY, OutputPane::FIND_IN_FILES_WIN);
	m_book->AddPage(tab, wxT("Find Results 5"), wxNullBitmap,  false);

	sz->Add(m_book, 1, wxALL|wxEXPAND);
	sz->Layout();
}

FindResultsTab* FindResultsContainer::GetActiveTab()
{
	int i = m_book->GetSelection();
	if (i == wxNOT_FOUND) {
		return NULL;
	}

	return (FindResultsTab*) m_book->GetPage((size_t)i);
}

void FindResultsContainer::SetActiveTab(int selection)
{
	m_book->SetSelection((size_t)selection);
}
