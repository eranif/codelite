//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : quickoutlinedlg.cpp              
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
 ///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jul 28 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifdef WX_PRECOMP

#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#else
#include <wx/wx.h>
#endif //WX_PRECOMP

#include "quickoutlinedlg.h"
#include "cpp_symbol_tree.h"
#include "macros.h"

extern wxImageList* CreateSymbolTreeImages();

///////////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE(QuickOutlineDlg, wxDialog)
EVT_CHAR_HOOK(QuickOutlineDlg::OnCharHook)
EVT_TEXT(wxID_ANY, QuickOutlineDlg::OnTextEntered)
END_EVENT_TABLE()

QuickOutlineDlg::QuickOutlineDlg(wxWindow* parent, const wxString &fileName, int id, wxString title, wxPoint pos, wxSize size, int style ) 
: wxDialog( parent, id, title, pos, size, style|wxRAISED_BORDER )
, m_fileName(fileName)
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	this->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_INFOBK ) );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	m_textFilter = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0|wxNO_BORDER );
	m_textFilter->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_INFOBK ) );
	bSizer1->Add( m_textFilter, 0, wxALL|wxEXPAND, 5 );
	m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	m_staticline1->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_INFOBK ) );
	bSizer1->Add( m_staticline1, 0, wxEXPAND|wxLEFT|wxRIGHT, 5 );

	//build the outline view
	m_treeOutline = new CppSymbolTree( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE|wxNO_BORDER);
	m_treeOutline->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_INFOBK ) );
	m_treeOutline->SetSymbolsImages(CreateSymbolTreeImages());
	
	//no hidden root
	m_treeOutline->BuildTree(m_fileName);
	m_treeOutline->ExpandAll();

	bSizer1->Add( m_treeOutline, 1, wxALL|wxEXPAND, 5 );
	this->SetSizer( bSizer1 );
	this->Layout();
	Centre();
	m_textFilter->SetFocus();
}

QuickOutlineDlg::~QuickOutlineDlg()
{
}

void QuickOutlineDlg::OnCharHook(wxKeyEvent &e)
{
	if(e.GetKeyCode() == WXK_ESCAPE){
		EndModal(wxID_CANCEL);
	}else if(e.GetKeyCode() == WXK_NUMPAD_ENTER || e.GetKeyCode() == WXK_RETURN){
		if(m_treeOutline->ActivateSelectedItem()){
			EndModal(wxID_OK);
		}
	}else if(e.GetKeyCode() == WXK_UP){
		m_treeOutline->AdvanceSelection(false);
	}else if(e.GetKeyCode() == WXK_DOWN){
		m_treeOutline->AdvanceSelection();
	}else{
		e.Skip();
	}
}

void QuickOutlineDlg::OnTextEntered(wxCommandEvent &WXUNUSED(e))
{
	wxString curname = m_textFilter->GetValue();
	if(curname.IsEmpty() == false){
		m_treeOutline->SelectItemByName(curname);
	}
}
