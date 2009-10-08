///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
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

#include "tags_options_base_dlg.h"

///////////////////////////////////////////////////////////////////////////

TagsOptionsBaseDlg::TagsOptionsBaseDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	m_mainBook = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNB_FIXEDWIDTH );
	m_generalPage = new wxPanel( m_mainBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* sbSizer2;
	sbSizer2 = new wxStaticBoxSizer( new wxStaticBox( m_generalPage, wxID_ANY, wxT("General:") ), wxVERTICAL );
	
	wxFlexGridSizer* fgSizer31;
	fgSizer31 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer31->SetFlexibleDirection( wxBOTH );
	fgSizer31->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_checkBoxMarkTagsFilesInBold = new wxCheckBox( m_generalPage, wxID_ANY, wxT("Mark files and dirs in file explorer that contribute tags in bold"), wxDefaultPosition, wxDefaultSize, 0 );
	
	fgSizer31->Add( m_checkBoxMarkTagsFilesInBold, 0, wxALL, 5 );
	
	m_checkDisableParseOnSave = new wxCheckBox( m_generalPage, wxID_ANY, wxT("Disable auto parsing of saved files"), wxDefaultPosition, wxDefaultSize, 0 );
	
	fgSizer31->Add( m_checkDisableParseOnSave, 0, wxALL, 5 );
	
	m_checkBoxretagWorkspaceOnStartup = new wxCheckBox( m_generalPage, wxID_ANY, wxT("Retag workspace on workspace startup"), wxDefaultPosition, wxDefaultSize, 0 );
	
	fgSizer31->Add( m_checkBoxretagWorkspaceOnStartup, 0, wxALL, 5 );
	
	m_checkParseComments = new wxCheckBox( m_generalPage, wxID_ANY, wxT("Parse comments"), wxDefaultPosition, wxDefaultSize, 0 );
	
	fgSizer31->Add( m_checkParseComments, 0, wxALL, 5 );
	
	m_checkDisplayComments = new wxCheckBox( m_generalPage, wxID_ANY, wxT("Display comments in tooltip"), wxDefaultPosition, wxDefaultSize, 0 );
	
	fgSizer31->Add( m_checkDisplayComments, 0, wxALL, 5 );
	
	m_checkDisplayTypeInfo = new wxCheckBox( m_generalPage, wxID_ANY, wxT("Display type info tooltips"), wxDefaultPosition, wxDefaultSize, 0 );
	
	fgSizer31->Add( m_checkDisplayTypeInfo, 0, wxALL, 5 );
	
	m_checkDisplayFunctionTip = new wxCheckBox( m_generalPage, wxID_ANY, wxT("Display function calltip"), wxDefaultPosition, wxDefaultSize, 0 );
	
	fgSizer31->Add( m_checkDisplayFunctionTip, 0, wxALL, 5 );
	
	m_checkCppKeywordAssist = new wxCheckBox( m_generalPage, wxID_ANY, wxT("Display completion box for language keywords"), wxDefaultPosition, wxDefaultSize, 0 );
	
	fgSizer31->Add( m_checkCppKeywordAssist, 0, wxALL, 5 );
	
	sbSizer2->Add( fgSizer31, 0, wxEXPAND, 5 );
	
	bSizer4->Add( sbSizer2, 0, wxEXPAND|wxALL, 5 );
	
	wxStaticBoxSizer* sbSizer4;
	sbSizer4 = new wxStaticBoxSizer( new wxStaticBox( m_generalPage, wxID_ANY, wxT("Keywords Highlight:") ), wxVERTICAL );
	
	m_checkColourLocalVars = new wxCheckBox( m_generalPage, wxID_ANY, wxT("Colour local variables"), wxDefaultPosition, wxDefaultSize, 0 );
	
	sbSizer4->Add( m_checkColourLocalVars, 0, wxALL|wxEXPAND, 5 );
	
	m_checkColourProjTags = new wxCheckBox( m_generalPage, wxID_ANY, wxT("Colour workspace tags"), wxDefaultPosition, wxDefaultSize, 0 );
	
	sbSizer4->Add( m_checkColourProjTags, 0, wxALL|wxEXPAND, 5 );
	
	wxFlexGridSizer* fgSizer3;
	fgSizer3 = new wxFlexGridSizer( 2, 3, 0, 0 );
	fgSizer3->SetFlexibleDirection( wxBOTH );
	fgSizer3->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_checkBoxClass = new wxCheckBox( m_generalPage, wxID_ANY, wxT("class"), wxDefaultPosition, wxDefaultSize, 0 );
	
	fgSizer3->Add( m_checkBoxClass, 0, wxALL, 5 );
	
	m_checkBoxStruct = new wxCheckBox( m_generalPage, wxID_ANY, wxT("struct"), wxDefaultPosition, wxDefaultSize, 0 );
	
	fgSizer3->Add( m_checkBoxStruct, 0, wxALL, 5 );
	
	m_checkBoxFunction = new wxCheckBox( m_generalPage, wxID_ANY, wxT("function"), wxDefaultPosition, wxDefaultSize, 0 );
	
	fgSizer3->Add( m_checkBoxFunction, 0, wxALL, 5 );
	
	m_checkBoxEnum = new wxCheckBox( m_generalPage, wxID_ANY, wxT("enum"), wxDefaultPosition, wxDefaultSize, 0 );
	
	fgSizer3->Add( m_checkBoxEnum, 0, wxALL, 5 );
	
	m_checkBoxEnumerator = new wxCheckBox( m_generalPage, wxID_ANY, wxT("enumerator"), wxDefaultPosition, wxDefaultSize, 0 );
	
	fgSizer3->Add( m_checkBoxEnumerator, 0, wxALL, 5 );
	
	m_checkBoxUnion = new wxCheckBox( m_generalPage, wxID_ANY, wxT("union"), wxDefaultPosition, wxDefaultSize, 0 );
	
	fgSizer3->Add( m_checkBoxUnion, 0, wxALL, 5 );
	
	m_checkBoxPrototype = new wxCheckBox( m_generalPage, wxID_ANY, wxT("prototype"), wxDefaultPosition, wxDefaultSize, 0 );
	
	fgSizer3->Add( m_checkBoxPrototype, 0, wxALL, 5 );
	
	m_checkBoxTypedef = new wxCheckBox( m_generalPage, wxID_ANY, wxT("typedef"), wxDefaultPosition, wxDefaultSize, 0 );
	
	fgSizer3->Add( m_checkBoxTypedef, 0, wxALL, 5 );
	
	m_checkBoxMacro = new wxCheckBox( m_generalPage, wxID_ANY, wxT("macro"), wxDefaultPosition, wxDefaultSize, 0 );
	
	fgSizer3->Add( m_checkBoxMacro, 0, wxALL, 5 );
	
	m_checkBoxNamespace = new wxCheckBox( m_generalPage, wxID_ANY, wxT("namespace"), wxDefaultPosition, wxDefaultSize, 0 );
	
	fgSizer3->Add( m_checkBoxNamespace, 0, wxALL, 5 );
	
	m_checkBoxMember = new wxCheckBox( m_generalPage, wxID_ANY, wxT("member"), wxDefaultPosition, wxDefaultSize, 0 );
	
	fgSizer3->Add( m_checkBoxMember, 0, wxALL, 5 );
	
	m_checkBoxVariable = new wxCheckBox( m_generalPage, wxID_ANY, wxT("variable"), wxDefaultPosition, wxDefaultSize, 0 );
	
	fgSizer3->Add( m_checkBoxVariable, 0, wxALL, 5 );
	
	sbSizer4->Add( fgSizer3, 0, wxEXPAND|wxRIGHT|wxLEFT, 15 );
	
	bSizer4->Add( sbSizer4, 0, wxEXPAND|wxALL, 5 );
	
	wxStaticBoxSizer* sbSizer21;
	sbSizer21 = new wxStaticBoxSizer( new wxStaticBox( m_generalPage, wxID_ANY, wxT("External Symbols Database:") ), wxVERTICAL );
	
	m_checkLoadLastDB = new wxCheckBox( m_generalPage, wxID_ANY, wxT("Automatically load the recently used additional symbols database"), wxDefaultPosition, wxDefaultSize, 0 );
	
	sbSizer21->Add( m_checkLoadLastDB, 0, wxALL|wxEXPAND, 5 );
	
	bSizer4->Add( sbSizer21, 0, wxEXPAND|wxALL, 5 );
	
	m_generalPage->SetSizer( bSizer4 );
	m_generalPage->Layout();
	bSizer4->Fit( m_generalPage );
	m_mainBook->AddPage( m_generalPage, wxT("General"), true );
	m_panelCache = new wxPanel( m_mainBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer61;
	bSizer61 = new wxBoxSizer( wxVERTICAL );
	
	m_checkBoxDisableCaching = new wxCheckBox( m_panelCache, wxID_ANY, wxT("Disable caching"), wxDefaultPosition, wxDefaultSize, 0 );
	
	bSizer61->Add( m_checkBoxDisableCaching, 0, wxALL|wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer41;
	sbSizer41 = new wxStaticBoxSizer( new wxStaticBox( m_panelCache, wxID_ANY, wxEmptyString ), wxVERTICAL );
	
	m_staticText4 = new wxStaticText( m_panelCache, wxID_ANY, wxT("To improve the responsiveness of code completion functionality, codelite manages an internal caching.\nHere, you may modify some of the cache settings, or disable it completly."), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( -1 );
	sbSizer41->Add( m_staticText4, 0, wxALL|wxEXPAND, 5 );
	
	bSizer61->Add( sbSizer41, 0, wxEXPAND|wxALL, 5 );
	
	wxFlexGridSizer* fgSizer4;
	fgSizer4 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer4->AddGrowableCol( 1 );
	fgSizer4->SetFlexibleDirection( wxBOTH );
	fgSizer4->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText51 = new wxStaticText( m_panelCache, wxID_ANY, wxT("Maximum items kept in cache:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText51->Wrap( -1 );
	fgSizer4->Add( m_staticText51, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5 );
	
	m_spinCtrlCacheSize = new wxSpinCtrl( m_panelCache, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 500, 5000, 1000 );
	fgSizer4->Add( m_spinCtrlCacheSize, 0, wxALL|wxALIGN_RIGHT, 5 );
	
	m_staticText6 = new wxStaticText( m_panelCache, wxID_ANY, wxT("Clear cache content"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText6->Wrap( -1 );
	fgSizer4->Add( m_staticText6, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_buttonClearCache = new wxButton( m_panelCache, wxID_ANY, wxT("Clear Cache"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer4->Add( m_buttonClearCache, 0, wxALL|wxALIGN_RIGHT, 5 );
	
	bSizer61->Add( fgSizer4, 0, wxEXPAND|wxALL, 5 );
	
	m_panelCache->SetSizer( bSizer61 );
	m_panelCache->Layout();
	bSizer61->Fit( m_panelCache );
	m_mainBook->AddPage( m_panelCache, wxT("Cache"), false );
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
	m_textPrep->SetMinSize( wxSize( 300,-1 ) );
	
	bSizer5->Add( m_textPrep, 1, wxEXPAND|wxTOP|wxBOTTOM|wxLEFT, 5 );
	
	m_buttonAdd = new wxButton( m_ctagsPage, wxID_ANY, wxT("..."), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer5->Add( m_buttonAdd, 0, wxTOP|wxBOTTOM|wxLEFT, 5 );
	
	fgSizer2->Add( bSizer5, 1, wxEXPAND|wxALL, 5 );
	
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
	
	bSizer6->Add( fgSizer2, 0, wxEXPAND|wxALL, 5 );
	
	m_checkFilesWithoutExt = new wxCheckBox( m_ctagsPage, wxID_ANY, wxT("Parse files without extension"), wxDefaultPosition, wxDefaultSize, 0 );
	
	bSizer6->Add( m_checkFilesWithoutExt, 0, wxALL|wxEXPAND, 5 );
	
	m_checkBoxFullRetagging = new wxCheckBox( m_ctagsPage, wxID_ANY, wxT("Use full re-tagging"), wxDefaultPosition, wxDefaultSize, 0 );
	
	m_checkBoxFullRetagging->SetToolTip( wxT("When re-tagging workspace/project re-tag all files regardless of their modification time") );
	
	bSizer6->Add( m_checkBoxFullRetagging, 0, wxALL|wxEXPAND, 5 );
	
	m_checkBoxAccurateScopeNameResolving = new wxCheckBox( m_ctagsPage, wxID_ANY, wxT("Use more accurate scope name resolving (you might notice a minor performance impact on large files)"), wxDefaultPosition, wxDefaultSize, 0 );
	
	bSizer6->Add( m_checkBoxAccurateScopeNameResolving, 0, wxALL|wxEXPAND, 5 );
	
	m_ctagsPage->SetSizer( bSizer6 );
	m_ctagsPage->Layout();
	bSizer6->Fit( m_ctagsPage );
	m_mainBook->AddPage( m_ctagsPage, wxT("Advanced"), false );
	
	mainSizer->Add( m_mainBook, 1, wxEXPAND | wxALL, 5 );
	
	m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	mainSizer->Add( m_staticline1, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxHORIZONTAL );
	
	m_buttonOK = new wxButton( this, wxID_OK, wxT("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonOK->SetDefault(); 
	bSizer3->Add( m_buttonOK, 0, wxALL, 5 );
	
	m_buttonCancel = new wxButton( this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( m_buttonCancel, 0, wxALL, 5 );
	
	mainSizer->Add( bSizer3, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	this->SetSizer( mainSizer );
	this->Layout();
	mainSizer->Fit( this );
	
	this->Centre( wxBOTH );
	
	// Connect Events
	m_checkBoxClass->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( TagsOptionsBaseDlg::OnColourWorkspaceUI ), NULL, this );
	m_checkBoxStruct->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( TagsOptionsBaseDlg::OnColourWorkspaceUI ), NULL, this );
	m_checkBoxFunction->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( TagsOptionsBaseDlg::OnColourWorkspaceUI ), NULL, this );
	m_checkBoxEnum->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( TagsOptionsBaseDlg::OnColourWorkspaceUI ), NULL, this );
	m_checkBoxEnumerator->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( TagsOptionsBaseDlg::OnColourWorkspaceUI ), NULL, this );
	m_checkBoxUnion->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( TagsOptionsBaseDlg::OnColourWorkspaceUI ), NULL, this );
	m_checkBoxPrototype->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( TagsOptionsBaseDlg::OnColourWorkspaceUI ), NULL, this );
	m_checkBoxTypedef->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( TagsOptionsBaseDlg::OnColourWorkspaceUI ), NULL, this );
	m_checkBoxMacro->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( TagsOptionsBaseDlg::OnColourWorkspaceUI ), NULL, this );
	m_checkBoxNamespace->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( TagsOptionsBaseDlg::OnColourWorkspaceUI ), NULL, this );
	m_checkBoxMember->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( TagsOptionsBaseDlg::OnColourWorkspaceUI ), NULL, this );
	m_checkBoxVariable->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( TagsOptionsBaseDlg::OnColourWorkspaceUI ), NULL, this );
	m_staticText4->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( TagsOptionsBaseDlg::OnCachePageUI ), NULL, this );
	m_staticText51->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( TagsOptionsBaseDlg::OnCachePageUI ), NULL, this );
	m_spinCtrlCacheSize->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( TagsOptionsBaseDlg::OnCachePageUI ), NULL, this );
	m_staticText6->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( TagsOptionsBaseDlg::OnCachePageUI ), NULL, this );
	m_buttonClearCache->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TagsOptionsBaseDlg::OnCleanCache ), NULL, this );
	m_buttonClearCache->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( TagsOptionsBaseDlg::OnClearCacheUI ), NULL, this );
	m_buttonAdd->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TagsOptionsBaseDlg::OnButtonAdd ), NULL, this );
	m_buttonOK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TagsOptionsBaseDlg::OnButtonOK ), NULL, this );
	m_buttonCancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TagsOptionsBaseDlg::OnButtonOK ), NULL, this );
}

TagsOptionsBaseDlg::~TagsOptionsBaseDlg()
{
	// Disconnect Events
	m_checkBoxClass->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( TagsOptionsBaseDlg::OnColourWorkspaceUI ), NULL, this );
	m_checkBoxStruct->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( TagsOptionsBaseDlg::OnColourWorkspaceUI ), NULL, this );
	m_checkBoxFunction->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( TagsOptionsBaseDlg::OnColourWorkspaceUI ), NULL, this );
	m_checkBoxEnum->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( TagsOptionsBaseDlg::OnColourWorkspaceUI ), NULL, this );
	m_checkBoxEnumerator->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( TagsOptionsBaseDlg::OnColourWorkspaceUI ), NULL, this );
	m_checkBoxUnion->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( TagsOptionsBaseDlg::OnColourWorkspaceUI ), NULL, this );
	m_checkBoxPrototype->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( TagsOptionsBaseDlg::OnColourWorkspaceUI ), NULL, this );
	m_checkBoxTypedef->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( TagsOptionsBaseDlg::OnColourWorkspaceUI ), NULL, this );
	m_checkBoxMacro->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( TagsOptionsBaseDlg::OnColourWorkspaceUI ), NULL, this );
	m_checkBoxNamespace->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( TagsOptionsBaseDlg::OnColourWorkspaceUI ), NULL, this );
	m_checkBoxMember->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( TagsOptionsBaseDlg::OnColourWorkspaceUI ), NULL, this );
	m_checkBoxVariable->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( TagsOptionsBaseDlg::OnColourWorkspaceUI ), NULL, this );
	m_staticText4->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( TagsOptionsBaseDlg::OnCachePageUI ), NULL, this );
	m_staticText51->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( TagsOptionsBaseDlg::OnCachePageUI ), NULL, this );
	m_spinCtrlCacheSize->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( TagsOptionsBaseDlg::OnCachePageUI ), NULL, this );
	m_staticText6->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( TagsOptionsBaseDlg::OnCachePageUI ), NULL, this );
	m_buttonClearCache->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TagsOptionsBaseDlg::OnCleanCache ), NULL, this );
	m_buttonClearCache->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( TagsOptionsBaseDlg::OnClearCacheUI ), NULL, this );
	m_buttonAdd->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TagsOptionsBaseDlg::OnButtonAdd ), NULL, this );
	m_buttonOK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TagsOptionsBaseDlg::OnButtonOK ), NULL, this );
	m_buttonCancel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TagsOptionsBaseDlg::OnButtonOK ), NULL, this );
}
