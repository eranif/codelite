//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : ViewSettings.cpp
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

#include "DbViewerPanel.h"
#include "ViewSettings.h"

ViewSettings::ViewSettings( wxWindow* parent,IDbAdapter* pDbAdapter):_ViewSettings(parent)
{
	DbViewerPanel::InitStyledTextCtrl( m_scintilla2 );
	m_pDbAdapter = pDbAdapter;
}

ViewSettings::~ViewSettings()
{
}

void ViewSettings::OnOKClick(wxCommandEvent& event)
{
	m_pView->SetName(m_txName->GetValue());
	m_pView->SetSelect(m_scintilla2->GetText());
	EndModal(wxID_OK);
}
void ViewSettings::SetView(View* pView, wxSFDiagramManager* pManager)
{
	m_pView = pView;
	m_pDiagramManager = pManager;
	
	m_txName->SetValue(pView->GetName());
	m_scintilla2->SetText(pView->GetSelect());	
}

