//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : templateclassbasedlg.cpp
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
// C++ code generated with wxFormBuilder (version Nov 18 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "templateclassbasedlg.h"

///////////////////////////////////////////////////////////////////////////

TemplateClassBaseDlg::TemplateClassBaseDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	this->SetExtraStyle( wxWS_EX_BLOCK_EVENTS );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	ID_NOTEBOOK1 = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBK_DEFAULT );
	ID_PANEL = new wxPanel( ID_NOTEBOOK1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* sbSizer01;
	sbSizer01 = new wxStaticBoxSizer( new wxStaticBox( ID_PANEL, wxID_ANY, _("Class") ), wxVERTICAL );
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_static1 = new wxStaticText( ID_PANEL, wxID_ANY, _("Class name:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_static1->Wrap( -1 );
	fgSizer1->Add( m_static1, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5 );
	
	m_textCtrlClassName = new wxTextCtrl( ID_PANEL, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 200,-1 ), 0 );
	m_textCtrlClassName->SetToolTip( _("Name of new class") );
	
	fgSizer1->Add( m_textCtrlClassName, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxEXPAND|wxALL, 5 );
	
	m_static2 = new wxStaticText( ID_PANEL, wxID_ANY, _("Template:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_static2->Wrap( -1 );
	fgSizer1->Add( m_static2, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5 );
	
	m_comboxCurrentTemplate = new wxComboBox( ID_PANEL, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 200,-1 ), 0, NULL, wxCB_READONLY|wxCB_SORT ); 
	m_comboxCurrentTemplate->SetToolTip( _("Template for new class") );
	
	fgSizer1->Add( m_comboxCurrentTemplate, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxEXPAND|wxALL, 5 );
	
	sbSizer01->Add( fgSizer1, 0, wxEXPAND|wxALL, 5 );
	
	bSizer2->Add( sbSizer01, 0, wxEXPAND|wxALL, 5 );
	
	wxStaticBoxSizer* sbSizer02;
	sbSizer02 = new wxStaticBoxSizer( new wxStaticBox( ID_PANEL, wxID_ANY, _("Files") ), wxVERTICAL );
	
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 0, 3, 0, 0 );
	fgSizer2->AddGrowableCol( 1 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_static3 = new wxStaticText( ID_PANEL, wxID_ANY, _(".h filename:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_static3->Wrap( -1 );
	fgSizer2->Add( m_static3, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5 );
	
	m_textCtrlHeaderFile = new wxTextCtrl( ID_PANEL, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 200,-1 ), 0 );
	m_textCtrlHeaderFile->SetToolTip( _("Name of header file") );
	
	fgSizer2->Add( m_textCtrlHeaderFile, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxEXPAND|wxALL, 5 );
	
	
	fgSizer2->Add( 5, 5, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_static4 = new wxStaticText( ID_PANEL, wxID_ANY, _(".cpp filename:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_static4->Wrap( -1 );
	fgSizer2->Add( m_static4, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5 );
	
	m_textCtrlCppFile = new wxTextCtrl( ID_PANEL, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 200,-1 ), 0 );
	m_textCtrlCppFile->SetToolTip( _("Name of source file") );
	
	fgSizer2->Add( m_textCtrlCppFile, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxEXPAND|wxALL, 5 );
	
	
	fgSizer2->Add( 5, 5, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_staticProjectTreeFolder = new wxStaticText( ID_PANEL, wxID_ANY, _("Project tree folder:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticProjectTreeFolder->Wrap( -1 );
	m_staticProjectTreeFolder->SetForegroundColour( wxColour( 215, 0, 0 ) );
	
	fgSizer2->Add( m_staticProjectTreeFolder, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5 );
	
	m_textCtrlVD = new wxTextCtrl( ID_PANEL, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 230,-1 ), wxTE_READONLY );
	m_textCtrlVD->SetFont( wxFont( 8, 70, 90, 92, false, wxEmptyString ) );
	m_textCtrlVD->SetBackgroundColour( wxColour( 255, 255, 230 ) );
	m_textCtrlVD->SetToolTip( _("Virtual folder to add new files") );
	
	fgSizer2->Add( m_textCtrlVD, 0, wxEXPAND|wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_buttonBrowseVD = new wxButton( ID_PANEL, wxID_ANY, _("..."), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonBrowseVD->SetDefault(); 
	m_buttonBrowseVD->SetToolTip( _("Browse virtual folders") );
	
	fgSizer2->Add( m_buttonBrowseVD, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxRIGHT, 5 );
	
	m_static5 = new wxStaticText( ID_PANEL, wxID_ANY, _("Path:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_static5->Wrap( -1 );
	fgSizer2->Add( m_static5, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5 );
	
	m_textCtrlFilePath = new wxTextCtrl( ID_PANEL, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 230,-1 ), wxTE_READONLY );
	m_textCtrlFilePath->SetBackgroundColour( wxColour( 255, 255, 230 ) );
	m_textCtrlFilePath->SetToolTip( _("Hd folder to add new files") );
	
	fgSizer2->Add( m_textCtrlFilePath, 0, wxEXPAND|wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_buttonBrowsePath = new wxButton( ID_PANEL, wxID_ANY, _("..."), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonBrowsePath->SetDefault(); 
	m_buttonBrowsePath->SetToolTip( _("Browse  folders") );
	
	fgSizer2->Add( m_buttonBrowsePath, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxRIGHT, 5 );
	
	sbSizer02->Add( fgSizer2, 0, wxEXPAND|wxALL, 5 );
	
	bSizer2->Add( sbSizer02, 0, wxEXPAND|wxALL, 5 );
	
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxHORIZONTAL );
	
	m_buttonGenerate = new wxButton( ID_PANEL, wxID_OK, _("&Generate"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonGenerate->SetDefault(); 
	m_buttonGenerate->Enable( false );
	
	bSizer3->Add( m_buttonGenerate, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_buttonCancel = new wxButton( ID_PANEL, wxID_CANCEL, _("&Quit"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( m_buttonCancel, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	bSizer2->Add( bSizer3, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );
	
	ID_PANEL->SetSizer( bSizer2 );
	ID_PANEL->Layout();
	bSizer2->Fit( ID_PANEL );
	ID_NOTEBOOK1->AddPage( ID_PANEL, _("Generate"), true );
	ID_PANEL1 = new wxPanel( ID_NOTEBOOK1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxHORIZONTAL );
	
	m_static6 = new wxStaticText( ID_PANEL1, wxID_ANY, _("Class:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_static6->Wrap( -1 );
	bSizer5->Add( m_static6, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1 );
	
	m_comboxTemplates = new wxComboBox( ID_PANEL1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_SORT ); 
	bSizer5->Add( m_comboxTemplates, 1, wxALIGN_CENTER_VERTICAL|wxALL, 1 );
	
	m_buttonAddTemplate = new wxButton( ID_PANEL1, wxID_ANY, _("Add"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonAddTemplate->SetDefault(); 
	bSizer5->Add( m_buttonAddTemplate, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1 );
	
	m_buttonChangeTemplate = new wxButton( ID_PANEL1, wxID_ANY, _("Change"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonChangeTemplate->SetDefault(); 
	bSizer5->Add( m_buttonChangeTemplate, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1 );
	
	m_buttonRemoveTemplate = new wxButton( ID_PANEL1, wxID_ANY, _("Remove"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonRemoveTemplate->SetDefault(); 
	bSizer5->Add( m_buttonRemoveTemplate, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1 );
	
	m_buttonClear = new wxButton( ID_PANEL1, wxID_ANY, _("Clear"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer5->Add( m_buttonClear, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1 );
	
	m_buttonInsertClassMacro = new wxButton( ID_PANEL1, wxID_ANY, _("%CLASS%"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonInsertClassMacro->SetDefault(); 
	bSizer5->Add( m_buttonInsertClassMacro, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1 );
	
	bSizer4->Add( bSizer5, 0, wxEXPAND|wxALL, 1 );
	
	m_notebookFiles = new wxNotebook( ID_PANEL1, wxID_ANY, wxDefaultPosition, wxSize( 500,-1 ), wxBK_DEFAULT|wxBK_TOP );
	m_panel3 = new wxPanel( m_notebookFiles, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxVERTICAL );
	
	m_textCtrlHeader = new wxTextCtrl( m_panel3, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_PROCESS_TAB|wxTE_RICH );
	m_textCtrlHeader->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 76, 90, 90, false, wxEmptyString ) );
	
	bSizer6->Add( m_textCtrlHeader, 1, wxALL|wxEXPAND, 5 );
	
	m_panel3->SetSizer( bSizer6 );
	m_panel3->Layout();
	bSizer6->Fit( m_panel3 );
	m_notebookFiles->AddPage( m_panel3, _("Header File"), true );
	m_panel4 = new wxPanel( m_notebookFiles, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer61;
	bSizer61 = new wxBoxSizer( wxVERTICAL );
	
	m_textCtrlImpl = new wxTextCtrl( m_panel4, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_PROCESS_TAB|wxTE_RICH2 );
	m_textCtrlImpl->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 76, 90, 90, false, wxEmptyString ) );
	
	bSizer61->Add( m_textCtrlImpl, 1, wxALL|wxEXPAND, 5 );
	
	m_panel4->SetSizer( bSizer61 );
	m_panel4->Layout();
	bSizer61->Fit( m_panel4 );
	m_notebookFiles->AddPage( m_panel4, _("Implementation File"), false );
	
	bSizer4->Add( m_notebookFiles, 1, wxEXPAND|wxALL, 1 );
	
	ID_PANEL1->SetSizer( bSizer4 );
	ID_PANEL1->Layout();
	bSizer4->Fit( ID_PANEL1 );
	ID_NOTEBOOK1->AddPage( ID_PANEL1, _("Templates"), false );
	
	bSizer1->Add( ID_NOTEBOOK1, 1, wxALL|wxEXPAND, 2 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	bSizer1->Fit( this );
	
	this->Centre( wxBOTH );
	
	// Connect Events
	m_textCtrlClassName->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( TemplateClassBaseDlg::OnClassNameEntered ), NULL, this );
	m_buttonBrowseVD->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TemplateClassBaseDlg::OnBrowseVD ), NULL, this );
	m_buttonBrowsePath->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TemplateClassBaseDlg::OnBrowseFilePath ), NULL, this );
	m_buttonGenerate->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TemplateClassBaseDlg::OnGenerate ), NULL, this );
	m_buttonGenerate->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( TemplateClassBaseDlg::OnGenerateUI ), NULL, this );
	m_buttonCancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TemplateClassBaseDlg::OnQuit ), NULL, this );
	m_comboxTemplates->Connect( wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler( TemplateClassBaseDlg::OnTemplateClassSelected ), NULL, this );
	m_buttonAddTemplate->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TemplateClassBaseDlg::OnButtonAdd ), NULL, this );
	m_buttonAddTemplate->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( TemplateClassBaseDlg::OnButtonAddUI ), NULL, this );
	m_buttonChangeTemplate->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TemplateClassBaseDlg::OnButtonChange ), NULL, this );
	m_buttonChangeTemplate->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( TemplateClassBaseDlg::OnButtonChangeUI ), NULL, this );
	m_buttonRemoveTemplate->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TemplateClassBaseDlg::OnButtonRemove ), NULL, this );
	m_buttonRemoveTemplate->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( TemplateClassBaseDlg::OnButtonRemoveUI ), NULL, this );
	m_buttonClear->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TemplateClassBaseDlg::OnButtonClear ), NULL, this );
	m_buttonClear->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( TemplateClassBaseDlg::OnButtonClearUI ), NULL, this );
	m_buttonInsertClassMacro->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TemplateClassBaseDlg::OnInsertClassKeyword ), NULL, this );
	m_buttonInsertClassMacro->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( TemplateClassBaseDlg::OnInsertClassKeywordUI ), NULL, this );
	m_textCtrlHeader->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( TemplateClassBaseDlg::OnHeaderFileContentChnaged ), NULL, this );
	m_textCtrlImpl->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( TemplateClassBaseDlg::OnImplFileContentChnaged ), NULL, this );
}

TemplateClassBaseDlg::~TemplateClassBaseDlg()
{
	// Disconnect Events
	m_textCtrlClassName->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( TemplateClassBaseDlg::OnClassNameEntered ), NULL, this );
	m_buttonBrowseVD->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TemplateClassBaseDlg::OnBrowseVD ), NULL, this );
	m_buttonBrowsePath->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TemplateClassBaseDlg::OnBrowseFilePath ), NULL, this );
	m_buttonGenerate->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TemplateClassBaseDlg::OnGenerate ), NULL, this );
	m_buttonGenerate->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( TemplateClassBaseDlg::OnGenerateUI ), NULL, this );
	m_buttonCancel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TemplateClassBaseDlg::OnQuit ), NULL, this );
	m_comboxTemplates->Disconnect( wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler( TemplateClassBaseDlg::OnTemplateClassSelected ), NULL, this );
	m_buttonAddTemplate->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TemplateClassBaseDlg::OnButtonAdd ), NULL, this );
	m_buttonAddTemplate->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( TemplateClassBaseDlg::OnButtonAddUI ), NULL, this );
	m_buttonChangeTemplate->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TemplateClassBaseDlg::OnButtonChange ), NULL, this );
	m_buttonChangeTemplate->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( TemplateClassBaseDlg::OnButtonChangeUI ), NULL, this );
	m_buttonRemoveTemplate->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TemplateClassBaseDlg::OnButtonRemove ), NULL, this );
	m_buttonRemoveTemplate->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( TemplateClassBaseDlg::OnButtonRemoveUI ), NULL, this );
	m_buttonClear->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TemplateClassBaseDlg::OnButtonClear ), NULL, this );
	m_buttonClear->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( TemplateClassBaseDlg::OnButtonClearUI ), NULL, this );
	m_buttonInsertClassMacro->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TemplateClassBaseDlg::OnInsertClassKeyword ), NULL, this );
	m_buttonInsertClassMacro->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( TemplateClassBaseDlg::OnInsertClassKeywordUI ), NULL, this );
	m_textCtrlHeader->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( TemplateClassBaseDlg::OnHeaderFileContentChnaged ), NULL, this );
	m_textCtrlImpl->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( TemplateClassBaseDlg::OnImplFileContentChnaged ), NULL, this );
	
}
