//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : tags_options_dlg.cpp              
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
// C++ code generated with wxFormBuilder (version Jun  6 2007)
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

#include "tags_options_dlg.h"
#include "macros.h"
#include "wx/tokenzr.h"
#include "add_option_dialog.h"

//---------------------------------------------------------

TagsOptionsDlg::TagsOptionsDlg( wxWindow* parent, const TagsOptionsData& data, int id, wxString title, wxPoint pos, wxSize size, int style ) 
: wxDialog( parent, id, title, pos, size, style )
, m_data(data)
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	long bookStyle = wxNB_DEFAULT;
	m_mainBook = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, bookStyle);
	m_generalPage = new wxPanel( m_mainBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* sbSizer2;
	sbSizer2 = new wxStaticBoxSizer( new wxStaticBox( m_generalPage, -1, wxT("General:") ), wxVERTICAL );
	
	m_checkParseComments = new wxCheckBox( m_generalPage, wxID_ANY, wxT("Parse comments"), wxDefaultPosition, wxDefaultSize, 0 );
	
	sbSizer2->Add( m_checkParseComments, 0, wxALL, 5 );
	
	m_checkDisplayComments = new wxCheckBox( m_generalPage, wxID_ANY, wxT("Display comments in tooltip"), wxDefaultPosition, wxDefaultSize, 0 );
	
	sbSizer2->Add( m_checkDisplayComments, 0, wxALL, 5 );
	
	m_checkDisplayTypeInfo = new wxCheckBox( m_generalPage, wxID_ANY, wxT("Display type info tooltips"), wxDefaultPosition, wxDefaultSize, 0 );
	
	sbSizer2->Add( m_checkDisplayTypeInfo, 0, wxALL, 5 );
	
	m_checkDisplayFunctionTip = new wxCheckBox( m_generalPage, wxID_ANY, wxT("Display function calltip"), wxDefaultPosition, wxDefaultSize, 0 );
	
	sbSizer2->Add( m_checkDisplayFunctionTip, 0, wxALL, 5 );
	
	m_checkColourLocalVars = new wxCheckBox( m_generalPage, wxID_ANY, wxT("Colour local variables"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer2->Add( m_checkColourLocalVars, 0, wxALL, 5 );
	
	m_checkColourProjTags = new wxCheckBox( m_generalPage, wxID_ANY, wxT("Colour workspace tags (functions, classes, structs etc.)"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer2->Add( m_checkColourProjTags, 0, wxALL, 5 );
	
	bSizer4->Add( sbSizer2, 0, wxEXPAND | wxALL , 5 );
	
	wxStaticBoxSizer* sbSizer21;
	sbSizer21 = new wxStaticBoxSizer( new wxStaticBox( m_generalPage, -1, wxT("External Symbols Database:") ), wxVERTICAL );
	
	m_checkLoadLastDB = new wxCheckBox( m_generalPage, wxID_ANY, wxT("Automatically load the recently used additional symbols database"), wxDefaultPosition, wxDefaultSize, 0 );
	
	sbSizer21->Add( m_checkLoadLastDB, 0, wxALL, 5 );
	
	m_checkLoadToMemory = new wxCheckBox( m_generalPage, wxID_ANY, wxT("Load external database symbols to memory"), wxDefaultPosition, wxDefaultSize, 0 );
	
	sbSizer21->Add( m_checkLoadToMemory, 0, wxALL, 5 );
	
	bSizer4->Add( sbSizer21, 0, wxEXPAND | wxALL , 5 );
	
	m_generalPage->SetSizer( bSizer4 );
	m_generalPage->Layout();
	bSizer4->Fit( m_generalPage );
	m_mainBook->AddPage( m_generalPage, wxT("Basics"), true );
	m_ctagsPage = new wxPanel( m_mainBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer2->AddGrowableCol( 1 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText1 = new wxStaticText( m_ctagsPage, wxID_ANY, wxT("Preprocessors:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	fgSizer2->Add( m_staticText1, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxHORIZONTAL );
	
	m_textPrep = new wxTextCtrl( m_ctagsPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer5->Add( m_textPrep, 1, wxEXPAND|wxTOP|wxBOTTOM|wxLEFT, 5 );
	
	m_buttonAdd = new wxButton( m_ctagsPage, wxID_ANY, wxT("..."), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	bSizer5->Add( m_buttonAdd, 0, wxTOP|wxBOTTOM|wxRIGHT, 5 );
	
	fgSizer2->Add( bSizer5, 1, wxEXPAND, 5 );
	
	m_staticText3 = new wxStaticText( m_ctagsPage, wxID_ANY, wxT("File Types:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	fgSizer2->Add( m_staticText3, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textFileSpec = new wxTextCtrl( m_ctagsPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer2->Add( m_textFileSpec, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText5 = new wxStaticText( m_ctagsPage, wxID_ANY, wxT("Force Language:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText5->Wrap( -1 );
	fgSizer2->Add( m_staticText5, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_comboBoxLang = new wxComboBox( m_ctagsPage, wxID_ANY, wxT("C++"), wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY );
	m_comboBoxLang->Append( wxT("C++") );
	m_comboBoxLang->Append( wxT("Java") );
	fgSizer2->Add( m_comboBoxLang, 0, wxALL|wxEXPAND, 5 );
	
	bSizer6->Add( fgSizer2, 0, wxEXPAND, 5 );
	
	m_checkFilesWithoutExt = new wxCheckBox( m_ctagsPage, wxID_ANY, wxT("Parse files without extension"), wxDefaultPosition, wxDefaultSize, 0 );
	
	bSizer6->Add( m_checkFilesWithoutExt, 0, wxALL, 5 );
	
	m_ctagsPage->SetSizer( bSizer6 );
	m_ctagsPage->Layout();
	bSizer6->Fit( m_ctagsPage );
	m_mainBook->AddPage( m_ctagsPage, wxT("Advance"), false );
	
	mainSizer->Add( m_mainBook, 1, wxEXPAND | wxALL, 5 );
	
	m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	mainSizer->Add( m_staticline1, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxHORIZONTAL );
	
	m_buttonOK = new wxButton( this, wxID_OK, wxT("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( m_buttonOK, 0, wxALL, 5 );
	
	m_buttonCancel = new wxButton( this, wxID_CANCEL, wxT("Close"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( m_buttonCancel, 0, wxALL, 5 );
	
	mainSizer->Add( bSizer3, 0, wxALIGN_RIGHT, 5 );
	
	this->SetSizer( mainSizer );
	this->Layout();

	InitValues();

	ConnectButton(m_buttonOK, TagsOptionsDlg::OnButtonOK);
	ConnectButton(m_buttonAdd, TagsOptionsDlg::OnButtonAdd);
	
	// center the dialog
	Centre();
	
	m_checkParseComments->SetFocus();
}

void TagsOptionsDlg::InitValues()
{
	//initialize the CodeLite page
	m_checkParseComments->SetValue(m_data.GetFlags() & CC_PARSE_COMMENTS ? true : false);
	m_checkDisplayFunctionTip->SetValue(m_data.GetFlags() & CC_DISP_FUNC_CALLTIP ? true : false);
	m_checkLoadLastDB->SetValue(m_data.GetFlags() & CC_LOAD_EXT_DB ? true : false);
	m_checkDisplayTypeInfo->SetValue(m_data.GetFlags() & CC_DISP_TYPE_INFO ? true : false);
	m_checkDisplayComments->SetValue(m_data.GetFlags() & CC_DISP_COMMENTS ? true : false);
	m_checkLoadToMemory->SetValue(m_data.GetFlags() & CC_LOAD_EXT_DB_TO_MEMORY ? true : false);
	m_checkFilesWithoutExt->SetValue(m_data.GetFlags() & CC_PARSE_EXT_LESS_FILES ? true : false);
	m_checkColourLocalVars->SetValue(m_data.GetFlags() & CC_COLOUR_VARS ? true : false);
	m_checkColourProjTags->SetValue(m_data.GetFlags() & CC_COLOUR_PROJ_TAGS ? true : false);
	
	//initialize the ctags page
	wxString prep;
	for(size_t i=0; i<m_data.GetPreprocessor().GetCount(); i++){
		prep += m_data.GetPreprocessor().Item(i);
		prep += wxT(";");
	}

	m_textPrep->SetValue(prep);
	m_textFileSpec->SetValue(m_data.GetFileSpec());
	m_comboBoxLang->Clear();
	m_comboBoxLang->Append(m_data.GetLanguages());
	wxString lan = m_data.GetLanguages().Item(0);
	m_comboBoxLang->SetStringSelection(lan);
}

void TagsOptionsDlg::OnButtonOK(wxCommandEvent &event)
{
	wxUnusedVar(event);
	CopyData();
	EndModal(wxID_OK);
}

void TagsOptionsDlg::OnButtonAdd(wxCommandEvent &event)
{
	wxUnusedVar(event);
	//pop up add option dialog
	AddOptionDlg *dlg = new AddOptionDlg(this, m_textPrep->GetValue());
	if(dlg->ShowModal() == wxID_OK){
		m_textPrep->SetValue(dlg->GetValue());
	}
	dlg->Destroy();
}

void TagsOptionsDlg::CopyData()
{
	//save data to the interal member m_data
	SetFlag(CC_DISP_COMMENTS, m_checkDisplayComments->IsChecked());
	SetFlag(CC_DISP_FUNC_CALLTIP, m_checkDisplayFunctionTip->IsChecked());
	SetFlag(CC_DISP_TYPE_INFO, m_checkDisplayTypeInfo->IsChecked());
	SetFlag(CC_LOAD_EXT_DB, m_checkLoadLastDB->IsChecked());
	SetFlag(CC_PARSE_COMMENTS, m_checkParseComments->IsChecked());
	SetFlag(CC_LOAD_EXT_DB_TO_MEMORY, m_checkLoadToMemory->IsChecked());
	SetFlag(CC_PARSE_EXT_LESS_FILES, m_checkFilesWithoutExt->IsChecked());
	SetFlag(CC_COLOUR_VARS, m_checkColourLocalVars->IsChecked());
	SetFlag(CC_COLOUR_PROJ_TAGS, m_checkColourProjTags->IsChecked());
	
	m_data.SetFileSpec(m_textFileSpec->GetValue());
	
	wxStringTokenizer tknz(m_textPrep->GetValue(), wxT(";"));
	wxArrayString prep;
	while(tknz.HasMoreTokens()){
		prep.Add(tknz.NextToken());
	}

	m_data.SetPreprocessor(prep);
	m_data.SetLanguages(m_comboBoxLang->GetStrings());
	m_data.SetLanguageSelection(m_comboBoxLang->GetStringSelection());
	
}

void TagsOptionsDlg::SetFlag(CodeCompletionOpts flag, bool set)
{
	if(set){
		m_data.SetFlags(m_data.GetFlags() | flag);
	}else{
		m_data.SetFlags(m_data.GetFlags() & ~(flag));
	}
}
