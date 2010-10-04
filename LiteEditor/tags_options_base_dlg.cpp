///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version May  4 2010)
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
	
	m_mainBook = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	m_generalPage = new wxPanel( m_mainBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* sbSizer2;
	sbSizer2 = new wxStaticBoxSizer( new wxStaticBox( m_generalPage, wxID_ANY, wxT("General:") ), wxVERTICAL );
	
	wxFlexGridSizer* fgSizer31;
	fgSizer31 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer31->SetFlexibleDirection( wxBOTH );
	fgSizer31->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_checkBoxMarkTagsFilesInBold = new wxCheckBox( m_generalPage, wxID_ANY, wxT("Mark files and dirs in file explorer that contribute tags in bold"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer31->Add( m_checkBoxMarkTagsFilesInBold, 0, wxALL, 5 );
	
	m_checkDisableParseOnSave = new wxCheckBox( m_generalPage, wxID_ANY, wxT("Disable auto parsing of saved files"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer31->Add( m_checkDisableParseOnSave, 0, wxALL, 5 );
	
	m_checkBoxretagWorkspaceOnStartup = new wxCheckBox( m_generalPage, wxID_ANY, wxT("Retag workspace on workspace startup"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer31->Add( m_checkBoxretagWorkspaceOnStartup, 0, wxALL, 5 );
	
	m_checkDisplayTypeInfo = new wxCheckBox( m_generalPage, wxID_ANY, wxT("Display type info tooltips"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer31->Add( m_checkDisplayTypeInfo, 0, wxALL, 5 );
	
	m_checkDisplayFunctionTip = new wxCheckBox( m_generalPage, wxID_ANY, wxT("Display function calltip"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer31->Add( m_checkDisplayFunctionTip, 0, wxALL, 5 );
	
	m_checkCppKeywordAssist = new wxCheckBox( m_generalPage, wxID_ANY, wxT("Display completion box for language keywords"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer31->Add( m_checkCppKeywordAssist, 0, wxALL, 5 );
	
	m_checkBoxDeepUsingNamespaceResolving = new wxCheckBox( m_generalPage, wxID_ANY, wxT("Scan all included files to locate 'using namespace' statements"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer31->Add( m_checkBoxDeepUsingNamespaceResolving, 0, wxALL, 5 );
	
	
	fgSizer31->Add( 0, 0, 1, wxEXPAND, 5 );
	
	sbSizer2->Add( fgSizer31, 0, wxEXPAND, 5 );
	
	bSizer4->Add( sbSizer2, 0, wxEXPAND|wxALL, 5 );
	
	wxStaticBoxSizer* sbSizer7;
	sbSizer7 = new wxStaticBoxSizer( new wxStaticBox( m_generalPage, wxID_ANY, wxT("Automatic Word Completion:") ), wxVERTICAL );
	
	wxFlexGridSizer* fgSizer4;
	fgSizer4 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer4->AddGrowableCol( 1 );
	fgSizer4->SetFlexibleDirection( wxBOTH );
	fgSizer4->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_checkWordAssist = new wxCheckBox( m_generalPage, wxID_ANY, wxT("Auto-Display wordcompletion-box on typing"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer4->Add( m_checkWordAssist, 0, wxALL, 5 );
	
	
	fgSizer4->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_staticTextMinWordLen = new wxStaticText( m_generalPage, wxID_ANY, wxT("Minimum number of chars to type to show wordcompletion-box:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextMinWordLen->Wrap( -1 );
	fgSizer4->Add( m_staticTextMinWordLen, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_sliderMinWordLen = new wxSlider( m_generalPage, wxID_ANY, 3, 1, 25, wxDefaultPosition, wxDefaultSize, wxSL_AUTOTICKS|wxSL_HORIZONTAL|wxSL_LABELS );
	fgSizer4->Add( m_sliderMinWordLen, 0, wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
	
	sbSizer7->Add( fgSizer4, 0, wxEXPAND|wxALL, 5 );
	
	bSizer4->Add( sbSizer7, 0, wxEXPAND|wxALL, 5 );
	
	wxStaticBoxSizer* sbSizer4;
	sbSizer4 = new wxStaticBoxSizer( new wxStaticBox( m_generalPage, wxID_ANY, wxT("Keywords Highlight:") ), wxVERTICAL );
	
	wxBoxSizer* bSizer11;
	bSizer11 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText72 = new wxStaticText( m_generalPage, wxID_ANY, wxT("Maixmum number of *workspace* symbols to colour:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText72->Wrap( -1 );
	bSizer11->Add( m_staticText72, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_spinCtrlMaxItemToColour = new wxSpinCtrl( m_generalPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS|wxSP_WRAP, 500, 10000, 1000 );
	bSizer11->Add( m_spinCtrlMaxItemToColour, 1, wxALL|wxEXPAND, 5 );
	
	sbSizer4->Add( bSizer11, 1, wxEXPAND, 5 );
	
	m_checkColourLocalVars = new wxCheckBox( m_generalPage, wxID_ANY, wxT("Colour local variables"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer4->Add( m_checkColourLocalVars, 0, wxALL|wxEXPAND, 5 );
	
	m_checkColourProjTags = new wxCheckBox( m_generalPage, wxID_ANY, wxT("Colour workspace tags"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer4->Add( m_checkColourProjTags, 0, wxALL|wxEXPAND, 5 );
	
	wxFlexGridSizer* fgSizer3;
	fgSizer3 = new wxFlexGridSizer( 0, 3, 0, 0 );
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
	
	m_generalPage->SetSizer( bSizer4 );
	m_generalPage->Layout();
	bSizer4->Fit( m_generalPage );
	m_mainBook->AddPage( m_generalPage, wxT("General"), true );
	m_panelParser = new wxPanel( m_mainBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* sbSizer5;
	sbSizer5 = new wxStaticBoxSizer( new wxStaticBox( m_panelParser, wxID_ANY, wxEmptyString ), wxVERTICAL );
	
	m_staticText7 = new wxStaticText( m_panelParser, wxID_ANY, wxT("Add search path(s) for the parser.\nThe search paths are used for locating include files"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText7->Wrap( -1 );
	sbSizer5->Add( m_staticText7, 0, wxEXPAND|wxALL, 5 );
	
	bSizer7->Add( sbSizer5, 0, wxEXPAND|wxALL, 5 );
	
	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer( wxHORIZONTAL );
	
	m_listBoxSearchPaths = new wxListBox( m_panelParser, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	bSizer9->Add( m_listBoxSearchPaths, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxVERTICAL );
	
	m_buttonAddSearchPath = new wxButton( m_panelParser, wxID_ANY, wxT("&Add..."), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer8->Add( m_buttonAddSearchPath, 0, wxALL|wxEXPAND, 5 );
	
	m_buttonRemovePath = new wxButton( m_panelParser, wxID_ANY, wxT("&Remove"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer8->Add( m_buttonRemovePath, 0, wxALL|wxEXPAND, 5 );
	
	m_buttonClearAll = new wxButton( m_panelParser, wxID_ANY, wxT("Clea&r All"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer8->Add( m_buttonClearAll, 0, wxALL|wxEXPAND, 5 );
	
	bSizer9->Add( bSizer8, 0, wxEXPAND, 5 );
	
	bSizer7->Add( bSizer9, 1, wxEXPAND|wxALL, 5 );
	
	wxStaticBoxSizer* sbSizer51;
	sbSizer51 = new wxStaticBoxSizer( new wxStaticBox( m_panelParser, wxID_ANY, wxEmptyString ), wxVERTICAL );
	
	m_staticText71 = new wxStaticText( m_panelParser, wxID_ANY, wxT("Add exclude path(s) for the parser.\nCodeLite will skip any file found inside these paths while scanning for include files"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText71->Wrap( -1 );
	sbSizer51->Add( m_staticText71, 0, wxEXPAND|wxALL, 5 );
	
	bSizer7->Add( sbSizer51, 0, wxEXPAND|wxALL, 5 );
	
	wxBoxSizer* bSizer91;
	bSizer91 = new wxBoxSizer( wxHORIZONTAL );
	
	m_listBoxSearchPaths1 = new wxListBox( m_panelParser, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	bSizer91->Add( m_listBoxSearchPaths1, 1, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer81;
	bSizer81 = new wxBoxSizer( wxVERTICAL );
	
	m_buttonAddExcludePath = new wxButton( m_panelParser, wxID_ANY, wxT("&Add..."), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer81->Add( m_buttonAddExcludePath, 0, wxALL|wxEXPAND, 5 );
	
	m_buttonRemoveExcludePath = new wxButton( m_panelParser, wxID_ANY, wxT("&Remove"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer81->Add( m_buttonRemoveExcludePath, 0, wxALL|wxEXPAND, 5 );
	
	m_buttonClearAllExcludPath = new wxButton( m_panelParser, wxID_ANY, wxT("Clea&r All"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer81->Add( m_buttonClearAllExcludPath, 0, wxALL|wxEXPAND, 5 );
	
	bSizer91->Add( bSizer81, 0, wxEXPAND, 5 );
	
	bSizer7->Add( bSizer91, 1, wxEXPAND, 5 );
	
	m_panelParser->SetSizer( bSizer7 );
	m_panelParser->Layout();
	bSizer7->Fit( m_panelParser );
	m_mainBook->AddPage( m_panelParser, wxT("Include Files"), false );
	m_ctagsPage = new wxPanel( m_mainBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer2->AddGrowableCol( 1 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
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
	
	m_staticline2 = new wxStaticLine( m_ctagsPage, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer6->Add( m_staticline2, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* bSizer14;
	bSizer14 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText92 = new wxStaticText( m_ctagsPage, wxID_ANY, wxT("CodeLite can suggest a list of 'Tokens' that will be added to the 'Tokens' table based on parsing the following header files \n(space separated list):"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText92->Wrap( -1 );
	bSizer14->Add( m_staticText92, 0, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer15;
	bSizer15 = new wxBoxSizer( wxHORIZONTAL );
	
	m_textCtrlFilesList = new wxTextCtrl( m_ctagsPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_RICH2 );
	bSizer15->Add( m_textCtrlFilesList, 1, wxALL|wxEXPAND, 5 );
	
	m_buttonParse = new wxButton( m_ctagsPage, wxID_ANY, wxT("Parse!"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer15->Add( m_buttonParse, 0, wxALL, 5 );
	
	bSizer14->Add( bSizer15, 1, wxEXPAND, 5 );
	
	bSizer6->Add( bSizer14, 0, wxEXPAND, 5 );
	
	m_staticline21 = new wxStaticLine( m_ctagsPage, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer6->Add( m_staticline21, 0, wxEXPAND | wxALL, 5 );
	
	m_notebook2 = new wxNotebook( m_ctagsPage, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	m_panel4 = new wxPanel( m_notebook2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_panel4->SetToolTip( wxT("List here list of tokens to be pre-processed by codelite-indexer. \nUsually, you would like to add here macros which confuse the parser\nClick the below link to read more about this feature and the syntax supported.\n") );
	
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxVERTICAL );
	
	m_textPrep = new wxTextCtrl( m_panel4, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_DONTWRAP|wxTE_MULTILINE|wxTE_PROCESS_ENTER|wxTE_PROCESS_TAB|wxTE_RICH2 );
	m_textPrep->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 76, 90, 90, false, wxEmptyString ) );
	m_textPrep->SetMinSize( wxSize( 300,-1 ) );
	
	bSizer5->Add( m_textPrep, 1, wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer52;
	sbSizer52 = new wxStaticBoxSizer( new wxStaticBox( m_panel4, wxID_ANY, wxEmptyString ), wxVERTICAL );
	
	m_staticText9 = new wxStaticText( m_panel4, wxID_ANY, wxT("List here list of tokens to be pre-processed by codelite-indexer. \nUsually, you would like to add here macros which confuse the parser\nClick the below link to read more about this feature and the syntax supported."), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText9->Wrap( -1 );
	m_staticText9->SetToolTip( wxT("List here list of tokens to be pre-processed by codelite-indexer. \nUsually, you would like to add here macros which confuse the parser\nClick the below link to read more about this feature and the syntax supported.\n") );
	
	sbSizer52->Add( m_staticText9, 0, wxALL|wxEXPAND, 5 );
	
	m_hyperlink1 = new wxHyperlinkCtrl( m_panel4, wxID_ANY, wxT("Macros Handling"), wxT("http://www.codelite.org/LiteEditor/MacrosHandling101"), wxDefaultPosition, wxDefaultSize, wxHL_DEFAULT_STYLE );
	sbSizer52->Add( m_hyperlink1, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	bSizer5->Add( sbSizer52, 0, wxEXPAND|wxALL, 5 );
	
	wxBoxSizer* bSizer131;
	bSizer131 = new wxBoxSizer( wxHORIZONTAL );
	
	bSizer5->Add( bSizer131, 0, wxEXPAND, 5 );
	
	m_panel4->SetSizer( bSizer5 );
	m_panel4->Layout();
	bSizer5->Fit( m_panel4 );
	m_notebook2->AddPage( m_panel4, wxT("Tokens"), true );
	m_panel5 = new wxPanel( m_notebook2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer13;
	bSizer13 = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* sbSizer521;
	sbSizer521 = new wxStaticBoxSizer( new wxStaticBox( m_panel5, wxID_ANY, wxEmptyString ), wxVERTICAL );
	
	m_staticText91 = new wxStaticText( m_panel5, wxID_ANY, wxT("Specify here a list of types which are to be specially handled while parsing C and C++ source files in the format of TYPE1=TYPE2.\nSo when TYPE1 is found, CodeLite will offer completion as if it was TYPE2 was found"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText91->Wrap( -1 );
	sbSizer521->Add( m_staticText91, 0, wxALL|wxEXPAND, 5 );
	
	bSizer13->Add( sbSizer521, 0, wxEXPAND|wxALL, 5 );
	
	m_textTypes = new wxTextCtrl( m_panel5, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_RICH2 );
	m_textTypes->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 76, 90, 90, false, wxEmptyString ) );
	
	bSizer13->Add( m_textTypes, 1, wxEXPAND, 5 );
	
	m_panel5->SetSizer( bSizer13 );
	m_panel5->Layout();
	bSizer13->Fit( m_panel5 );
	m_notebook2->AddPage( m_panel5, wxT("Types"), false );
	
	bSizer6->Add( m_notebook2, 1, wxEXPAND | wxALL, 5 );
	
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
	
	this->Centre( wxBOTH );
	
	// Connect Events
	m_staticTextMinWordLen->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( TagsOptionsBaseDlg::OnAutoShowWordAssitUI ), NULL, this );
	m_sliderMinWordLen->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( TagsOptionsBaseDlg::OnAutoShowWordAssitUI ), NULL, this );
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
	m_buttonAddSearchPath->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TagsOptionsBaseDlg::OnAddSearchPath ), NULL, this );
	m_buttonAddSearchPath->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( TagsOptionsBaseDlg::OnAddSearchPathUI ), NULL, this );
	m_buttonRemovePath->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TagsOptionsBaseDlg::OnRemoveSearchPath ), NULL, this );
	m_buttonRemovePath->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( TagsOptionsBaseDlg::OnRemoveSearchPathUI ), NULL, this );
	m_buttonClearAll->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TagsOptionsBaseDlg::OnClearAll ), NULL, this );
	m_buttonClearAll->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( TagsOptionsBaseDlg::OnClearAllUI ), NULL, this );
	m_buttonAddExcludePath->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TagsOptionsBaseDlg::OnAddExcludePath ), NULL, this );
	m_buttonAddExcludePath->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( TagsOptionsBaseDlg::OnAddExcludePathUI ), NULL, this );
	m_buttonRemoveExcludePath->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TagsOptionsBaseDlg::OnRemoveExcludePath ), NULL, this );
	m_buttonRemoveExcludePath->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( TagsOptionsBaseDlg::OnRemoveExcludePathUI ), NULL, this );
	m_buttonClearAllExcludPath->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TagsOptionsBaseDlg::OnClearAllExcludePaths ), NULL, this );
	m_buttonClearAllExcludPath->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( TagsOptionsBaseDlg::OnClearAllExcludePathsUI ), NULL, this );
	m_buttonParse->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TagsOptionsBaseDlg::OnParse ), NULL, this );
	m_buttonParse->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( TagsOptionsBaseDlg::OnFileSelectedUI ), NULL, this );
	m_buttonOK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TagsOptionsBaseDlg::OnButtonOK ), NULL, this );
}

TagsOptionsBaseDlg::~TagsOptionsBaseDlg()
{
	// Disconnect Events
	m_staticTextMinWordLen->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( TagsOptionsBaseDlg::OnAutoShowWordAssitUI ), NULL, this );
	m_sliderMinWordLen->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( TagsOptionsBaseDlg::OnAutoShowWordAssitUI ), NULL, this );
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
	m_buttonAddSearchPath->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TagsOptionsBaseDlg::OnAddSearchPath ), NULL, this );
	m_buttonAddSearchPath->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( TagsOptionsBaseDlg::OnAddSearchPathUI ), NULL, this );
	m_buttonRemovePath->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TagsOptionsBaseDlg::OnRemoveSearchPath ), NULL, this );
	m_buttonRemovePath->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( TagsOptionsBaseDlg::OnRemoveSearchPathUI ), NULL, this );
	m_buttonClearAll->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TagsOptionsBaseDlg::OnClearAll ), NULL, this );
	m_buttonClearAll->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( TagsOptionsBaseDlg::OnClearAllUI ), NULL, this );
	m_buttonAddExcludePath->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TagsOptionsBaseDlg::OnAddExcludePath ), NULL, this );
	m_buttonAddExcludePath->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( TagsOptionsBaseDlg::OnAddExcludePathUI ), NULL, this );
	m_buttonRemoveExcludePath->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TagsOptionsBaseDlg::OnRemoveExcludePath ), NULL, this );
	m_buttonRemoveExcludePath->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( TagsOptionsBaseDlg::OnRemoveExcludePathUI ), NULL, this );
	m_buttonClearAllExcludPath->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TagsOptionsBaseDlg::OnClearAllExcludePaths ), NULL, this );
	m_buttonClearAllExcludPath->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( TagsOptionsBaseDlg::OnClearAllExcludePathsUI ), NULL, this );
	m_buttonParse->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TagsOptionsBaseDlg::OnParse ), NULL, this );
	m_buttonParse->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( TagsOptionsBaseDlg::OnFileSelectedUI ), NULL, this );
	m_buttonOK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TagsOptionsBaseDlg::OnButtonOK ), NULL, this );
	
}
