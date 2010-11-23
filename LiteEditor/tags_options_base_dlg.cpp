///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
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
	
	m_notebook3 = new wxNotebook( m_generalPage, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	m_panel8 = new wxPanel( m_notebook3, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer19;
	bSizer19 = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* sbSizer2;
	sbSizer2 = new wxStaticBoxSizer( new wxStaticBox( m_panel8, wxID_ANY, _("Display:") ), wxVERTICAL );
	
	wxGridSizer* gSizer1;
	gSizer1 = new wxGridSizer( 2, 2, 0, 0 );
	
	m_checkBoxMarkTagsFilesInBold = new wxCheckBox( m_panel8, wxID_ANY, _("Mark files and dirs in file explorer that contribute tags in bold"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkBoxMarkTagsFilesInBold->SetToolTip( _("Mark files and dirs in file explorer that contribute tags in bold") );
	
	gSizer1->Add( m_checkBoxMarkTagsFilesInBold, 0, wxALL, 5 );
	
	m_checkDisplayTypeInfo = new wxCheckBox( m_panel8, wxID_ANY, _("Display type info tooltips"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkDisplayTypeInfo->SetToolTip( _("Display information about the hovered text") );
	
	gSizer1->Add( m_checkDisplayTypeInfo, 0, wxALL, 5 );
	
	m_checkDisplayFunctionTip = new wxCheckBox( m_panel8, wxID_ANY, _("Display function calltip"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkDisplayFunctionTip->SetToolTip( _("Display function argument list after typing an open brace '('") );
	
	gSizer1->Add( m_checkDisplayFunctionTip, 0, wxALL, 5 );
	
	m_checkCppKeywordAssist = new wxCheckBox( m_panel8, wxID_ANY, _("Display completion box for language keywords"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkCppKeywordAssist->SetToolTip( _("When enabled, codelite will auto show the code completion box for C/C++ keywords after typing 2 chars") );
	
	gSizer1->Add( m_checkCppKeywordAssist, 0, wxALL, 5 );
	
	sbSizer2->Add( gSizer1, 1, wxEXPAND|wxALL, 5 );
	
	bSizer19->Add( sbSizer2, 0, wxEXPAND|wxALL, 5 );
	
	wxStaticBoxSizer* sbSizer15;
	sbSizer15 = new wxStaticBoxSizer( new wxStaticBox( m_panel8, wxID_ANY, _("Behavior:") ), wxVERTICAL );
	
	wxGridSizer* gSizer2;
	gSizer2 = new wxGridSizer( 2, 2, 0, 0 );
	
	m_checkBoxretagWorkspaceOnStartup = new wxCheckBox( m_panel8, wxID_ANY, _("Retag workspace on workspace startup"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkBoxretagWorkspaceOnStartup->SetToolTip( _("Retag workspace once loaded") );
	
	gSizer2->Add( m_checkBoxretagWorkspaceOnStartup, 0, wxALL, 5 );
	
	m_checkDisableParseOnSave = new wxCheckBox( m_panel8, wxID_ANY, _("Disable auto parsing of saved files"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkDisableParseOnSave->SetToolTip( _("Do not trigger file parsing after saving a file") );
	
	gSizer2->Add( m_checkDisableParseOnSave, 0, wxALL, 5 );
	
	m_checkBoxDeepUsingNamespaceResolving = new wxCheckBox( m_panel8, wxID_ANY, _("Scan all included files to locate 'using namespace' statements"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkBoxDeepUsingNamespaceResolving->SetToolTip( _("Allways search for 'using namespace' statements in all included files") );
	
	gSizer2->Add( m_checkBoxDeepUsingNamespaceResolving, 0, wxALL, 5 );
	
	sbSizer15->Add( gSizer2, 1, wxEXPAND, 5 );
	
	bSizer19->Add( sbSizer15, 0, wxEXPAND|wxALL, 5 );
	
	m_panel8->SetSizer( bSizer19 );
	m_panel8->Layout();
	bSizer19->Fit( m_panel8 );
	m_notebook3->AddPage( m_panel8, _("Display and Behavior"), true );
	m_panel6 = new wxPanel( m_notebook3, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer16;
	bSizer16 = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* sbSizer4;
	sbSizer4 = new wxStaticBoxSizer( new wxStaticBox( m_panel6, wxID_ANY, wxEmptyString ), wxVERTICAL );
	
	wxBoxSizer* bSizer11;
	bSizer11 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText72 = new wxStaticText( m_panel6, wxID_ANY, _("Maixmum number of *workspace* symbols to colour:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText72->Wrap( -1 );
	bSizer11->Add( m_staticText72, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_spinCtrlMaxItemToColour = new wxSpinCtrl( m_panel6, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS|wxSP_WRAP, 500, 10000, 1000 );
	bSizer11->Add( m_spinCtrlMaxItemToColour, 1, wxALL|wxEXPAND, 5 );
	
	sbSizer4->Add( bSizer11, 0, wxEXPAND, 5 );
	
	m_checkColourLocalVars = new wxCheckBox( m_panel6, wxID_ANY, _("Colour local variables"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer4->Add( m_checkColourLocalVars, 0, wxALL|wxEXPAND, 5 );
	
	m_checkBoxColourMacroBlocks = new wxCheckBox( m_panel6, wxID_ANY, _("Colour macro blocks with gray text colour when the macro is not defined"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer4->Add( m_checkBoxColourMacroBlocks, 0, wxALL|wxEXPAND, 5 );
	
	m_checkColourProjTags = new wxCheckBox( m_panel6, wxID_ANY, _("Colour workspace tags"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer4->Add( m_checkColourProjTags, 0, wxALL|wxEXPAND, 5 );
	
	wxFlexGridSizer* fgSizer3;
	fgSizer3 = new wxFlexGridSizer( 0, 3, 0, 0 );
	fgSizer3->SetFlexibleDirection( wxBOTH );
	fgSizer3->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_checkBoxClass = new wxCheckBox( m_panel6, wxID_ANY, _("class"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer3->Add( m_checkBoxClass, 0, wxALL, 5 );
	
	m_checkBoxStruct = new wxCheckBox( m_panel6, wxID_ANY, _("struct"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer3->Add( m_checkBoxStruct, 0, wxALL, 5 );
	
	m_checkBoxFunction = new wxCheckBox( m_panel6, wxID_ANY, _("function"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer3->Add( m_checkBoxFunction, 0, wxALL, 5 );
	
	m_checkBoxEnum = new wxCheckBox( m_panel6, wxID_ANY, _("enum"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer3->Add( m_checkBoxEnum, 0, wxALL, 5 );
	
	m_checkBoxEnumerator = new wxCheckBox( m_panel6, wxID_ANY, _("enumerator"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer3->Add( m_checkBoxEnumerator, 0, wxALL, 5 );
	
	m_checkBoxUnion = new wxCheckBox( m_panel6, wxID_ANY, _("union"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer3->Add( m_checkBoxUnion, 0, wxALL, 5 );
	
	m_checkBoxPrototype = new wxCheckBox( m_panel6, wxID_ANY, _("prototype"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer3->Add( m_checkBoxPrototype, 0, wxALL, 5 );
	
	m_checkBoxTypedef = new wxCheckBox( m_panel6, wxID_ANY, _("typedef"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer3->Add( m_checkBoxTypedef, 0, wxALL, 5 );
	
	m_checkBoxMacro = new wxCheckBox( m_panel6, wxID_ANY, _("macro"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer3->Add( m_checkBoxMacro, 0, wxALL, 5 );
	
	m_checkBoxNamespace = new wxCheckBox( m_panel6, wxID_ANY, _("namespace"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer3->Add( m_checkBoxNamespace, 0, wxALL, 5 );
	
	m_checkBoxMember = new wxCheckBox( m_panel6, wxID_ANY, _("member"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer3->Add( m_checkBoxMember, 0, wxALL, 5 );
	
	m_checkBoxVariable = new wxCheckBox( m_panel6, wxID_ANY, _("variable"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer3->Add( m_checkBoxVariable, 0, wxALL, 5 );
	
	sbSizer4->Add( fgSizer3, 0, wxEXPAND|wxRIGHT|wxLEFT, 15 );
	
	bSizer16->Add( sbSizer4, 0, wxEXPAND|wxALL, 5 );
	
	m_panel6->SetSizer( bSizer16 );
	m_panel6->Layout();
	bSizer16->Fit( m_panel6 );
	m_notebook3->AddPage( m_panel6, _("Colouring"), false );
	m_panel7 = new wxPanel( m_notebook3, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer17;
	bSizer17 = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* sbSizer7;
	sbSizer7 = new wxStaticBoxSizer( new wxStaticBox( m_panel7, wxID_ANY, _("Automatic Word Completion:") ), wxVERTICAL );
	
	wxFlexGridSizer* fgSizer4;
	fgSizer4 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer4->AddGrowableCol( 1 );
	fgSizer4->SetFlexibleDirection( wxBOTH );
	fgSizer4->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_checkWordAssist = new wxCheckBox( m_panel7, wxID_ANY, _("Auto-Display wordcompletion-box on typing"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkWordAssist->SetToolTip( _("When enabled, codelite will auto show the code completion box after N chars were typed") );
	
	fgSizer4->Add( m_checkWordAssist, 0, wxALL, 5 );
	
	
	fgSizer4->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_staticTextMinWordLen = new wxStaticText( m_panel7, wxID_ANY, _("Minimum number of chars to type to show wordcompletion-box:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextMinWordLen->Wrap( -1 );
	m_staticTextMinWordLen->SetToolTip( _("Number of chars to type before showing the code completion box") );
	
	fgSizer4->Add( m_staticTextMinWordLen, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_sliderMinWordLen = new wxSlider( m_panel7, wxID_ANY, 3, 1, 25, wxDefaultPosition, wxDefaultSize, wxSL_AUTOTICKS|wxSL_HORIZONTAL|wxSL_LABELS );
	m_sliderMinWordLen->SetToolTip( _("Number of chars to type before showing the code completion box") );
	
	fgSizer4->Add( m_sliderMinWordLen, 0, wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
	
	m_checkAutoInsertSingleChoice = new wxCheckBox( m_panel7, wxID_ANY, _("Auto-Insert single match"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkAutoInsertSingleChoice->SetValue(true); 
	m_checkAutoInsertSingleChoice->SetToolTip( _("When there is only a single match dont show the code completion box but rather insert the match") );
	
	fgSizer4->Add( m_checkAutoInsertSingleChoice, 0, wxALL, 5 );
	
	sbSizer7->Add( fgSizer4, 0, wxEXPAND|wxALL, 5 );
	
	bSizer17->Add( sbSizer7, 0, wxEXPAND|wxALL, 5 );
	
	m_panel7->SetSizer( bSizer17 );
	m_panel7->Layout();
	bSizer17->Fit( m_panel7 );
	m_notebook3->AddPage( m_panel7, _("Triggering"), false );
	
	bSizer4->Add( m_notebook3, 1, wxEXPAND | wxALL, 5 );
	
	m_generalPage->SetSizer( bSizer4 );
	m_generalPage->Layout();
	bSizer4->Fit( m_generalPage );
	m_mainBook->AddPage( m_generalPage, _("General"), true );
	m_panelParser = new wxPanel( m_mainBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* sbSizer5;
	sbSizer5 = new wxStaticBoxSizer( new wxStaticBox( m_panelParser, wxID_ANY, wxEmptyString ), wxVERTICAL );
	
	m_staticText7 = new wxStaticText( m_panelParser, wxID_ANY, _("Add search path(s) for the parser.\nThe search paths are used for locating include files"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText7->Wrap( -1 );
	sbSizer5->Add( m_staticText7, 0, wxEXPAND|wxALL, 5 );
	
	bSizer7->Add( sbSizer5, 0, wxEXPAND|wxALL, 5 );
	
	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer( wxHORIZONTAL );
	
	m_listBoxSearchPaths = new wxListBox( m_panelParser, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	bSizer9->Add( m_listBoxSearchPaths, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxVERTICAL );
	
	m_buttonAddSearchPath = new wxButton( m_panelParser, wxID_ANY, _("&Add..."), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer8->Add( m_buttonAddSearchPath, 0, wxALL|wxEXPAND, 5 );
	
	m_buttonRemovePath = new wxButton( m_panelParser, wxID_ANY, _("&Remove"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer8->Add( m_buttonRemovePath, 0, wxALL|wxEXPAND, 5 );
	
	m_buttonClearAll = new wxButton( m_panelParser, wxID_ANY, _("Clea&r All"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer8->Add( m_buttonClearAll, 0, wxALL|wxEXPAND, 5 );
	
	bSizer9->Add( bSizer8, 0, wxEXPAND, 5 );
	
	bSizer7->Add( bSizer9, 1, wxEXPAND|wxALL, 5 );
	
	wxStaticBoxSizer* sbSizer51;
	sbSizer51 = new wxStaticBoxSizer( new wxStaticBox( m_panelParser, wxID_ANY, wxEmptyString ), wxVERTICAL );
	
	m_staticText71 = new wxStaticText( m_panelParser, wxID_ANY, _("Add exclude path(s) for the parser.\nCodeLite will skip any file found inside these paths while scanning for include files"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText71->Wrap( -1 );
	sbSizer51->Add( m_staticText71, 0, wxEXPAND|wxALL, 5 );
	
	bSizer7->Add( sbSizer51, 0, wxEXPAND|wxALL, 5 );
	
	wxBoxSizer* bSizer91;
	bSizer91 = new wxBoxSizer( wxHORIZONTAL );
	
	m_listBoxSearchPaths1 = new wxListBox( m_panelParser, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	bSizer91->Add( m_listBoxSearchPaths1, 1, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer81;
	bSizer81 = new wxBoxSizer( wxVERTICAL );
	
	m_buttonAddExcludePath = new wxButton( m_panelParser, wxID_ANY, _("&Add..."), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer81->Add( m_buttonAddExcludePath, 0, wxALL|wxEXPAND, 5 );
	
	m_buttonRemoveExcludePath = new wxButton( m_panelParser, wxID_ANY, _("&Remove"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer81->Add( m_buttonRemoveExcludePath, 0, wxALL|wxEXPAND, 5 );
	
	m_buttonClearAllExcludPath = new wxButton( m_panelParser, wxID_ANY, _("Clea&r All"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer81->Add( m_buttonClearAllExcludPath, 0, wxALL|wxEXPAND, 5 );
	
	bSizer91->Add( bSizer81, 0, wxEXPAND, 5 );
	
	bSizer7->Add( bSizer91, 1, wxEXPAND, 5 );
	
	m_panelParser->SetSizer( bSizer7 );
	m_panelParser->Layout();
	bSizer7->Fit( m_panelParser );
	m_mainBook->AddPage( m_panelParser, _("Include Files"), false );
	m_ctagsPage = new wxPanel( m_mainBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer2->AddGrowableCol( 1 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText3 = new wxStaticText( m_ctagsPage, wxID_ANY, _("File Types:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	fgSizer2->Add( m_staticText3, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textFileSpec = new wxTextCtrl( m_ctagsPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer2->Add( m_textFileSpec, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText5 = new wxStaticText( m_ctagsPage, wxID_ANY, _("Force Language:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText5->Wrap( -1 );
	fgSizer2->Add( m_staticText5, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_comboBoxLang = new wxComboBox( m_ctagsPage, wxID_ANY, _("C++"), wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY );
	m_comboBoxLang->Append( _("C++") );
	m_comboBoxLang->Append( _("Java") );
	fgSizer2->Add( m_comboBoxLang, 0, wxALL|wxEXPAND, 5 );
	
	bSizer6->Add( fgSizer2, 0, wxEXPAND|wxALL, 5 );
	
	m_checkFilesWithoutExt = new wxCheckBox( m_ctagsPage, wxID_ANY, _("Parse files without extension"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer6->Add( m_checkFilesWithoutExt, 0, wxALL|wxEXPAND, 5 );
	
	m_staticline2 = new wxStaticLine( m_ctagsPage, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer6->Add( m_staticline2, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* bSizer14;
	bSizer14 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText92 = new wxStaticText( m_ctagsPage, wxID_ANY, _("CodeLite can suggest a list of 'Tokens' that will be added to the 'Tokens' table based on parsing the following header files \n(space separated list):"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText92->Wrap( -1 );
	bSizer14->Add( m_staticText92, 0, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer15;
	bSizer15 = new wxBoxSizer( wxHORIZONTAL );
	
	m_textCtrlFilesList = new wxTextCtrl( m_ctagsPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_RICH2 );
	bSizer15->Add( m_textCtrlFilesList, 1, wxALL|wxEXPAND, 5 );
	
	m_buttonParse = new wxButton( m_ctagsPage, wxID_ANY, _("Parse!"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer15->Add( m_buttonParse, 0, wxALL, 5 );
	
	bSizer14->Add( bSizer15, 1, wxEXPAND, 5 );
	
	bSizer6->Add( bSizer14, 0, wxEXPAND, 5 );
	
	m_staticline21 = new wxStaticLine( m_ctagsPage, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer6->Add( m_staticline21, 0, wxEXPAND | wxALL, 5 );
	
	m_notebook2 = new wxNotebook( m_ctagsPage, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	m_panel4 = new wxPanel( m_notebook2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_panel4->SetToolTip( _("List here list of tokens to be pre-processed by codelite-indexer. \nUsually, you would like to add here macros which confuse the parser\nClick the below link to read more about this feature and the syntax supported.\n") );
	
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxVERTICAL );
	
	m_textPrep = new wxTextCtrl( m_panel4, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_DONTWRAP|wxTE_MULTILINE|wxTE_PROCESS_ENTER|wxTE_PROCESS_TAB|wxTE_RICH2 );
	m_textPrep->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 76, 90, 90, false, wxEmptyString ) );
	m_textPrep->SetMinSize( wxSize( 300,-1 ) );
	
	bSizer5->Add( m_textPrep, 1, wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer52;
	sbSizer52 = new wxStaticBoxSizer( new wxStaticBox( m_panel4, wxID_ANY, wxEmptyString ), wxVERTICAL );
	
	m_staticText9 = new wxStaticText( m_panel4, wxID_ANY, _("List here list of tokens to be pre-processed by codelite-indexer. \nUsually, you would like to add here macros which confuse the parser\nClick the below link to read more about this feature and the syntax supported."), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText9->Wrap( -1 );
	m_staticText9->SetToolTip( _("List here list of tokens to be pre-processed by codelite-indexer. \nUsually, you would like to add here macros which confuse the parser\nClick the below link to read more about this feature and the syntax supported.\n") );
	
	sbSizer52->Add( m_staticText9, 0, wxALL|wxEXPAND, 5 );
	
	m_hyperlink1 = new wxHyperlinkCtrl( m_panel4, wxID_ANY, _("Macros Handling"), wxT("http://www.codelite.org/LiteEditor/MacrosHandling101"), wxDefaultPosition, wxDefaultSize, wxHL_DEFAULT_STYLE );
	sbSizer52->Add( m_hyperlink1, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	bSizer5->Add( sbSizer52, 0, wxEXPAND|wxALL, 5 );
	
	wxBoxSizer* bSizer131;
	bSizer131 = new wxBoxSizer( wxHORIZONTAL );
	
	bSizer5->Add( bSizer131, 0, wxEXPAND, 5 );
	
	m_panel4->SetSizer( bSizer5 );
	m_panel4->Layout();
	bSizer5->Fit( m_panel4 );
	m_notebook2->AddPage( m_panel4, _("Tokens"), true );
	m_panel5 = new wxPanel( m_notebook2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer13;
	bSizer13 = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* sbSizer521;
	sbSizer521 = new wxStaticBoxSizer( new wxStaticBox( m_panel5, wxID_ANY, wxEmptyString ), wxVERTICAL );
	
	m_staticText91 = new wxStaticText( m_panel5, wxID_ANY, _("Specify here a list of types which are to be specially handled while parsing C and C++ source files in the format of TYPE1=TYPE2.\nSo when TYPE1 is found, CodeLite will offer completion as if it was TYPE2 was found"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText91->Wrap( -1 );
	sbSizer521->Add( m_staticText91, 0, wxALL|wxEXPAND, 5 );
	
	bSizer13->Add( sbSizer521, 0, wxEXPAND|wxALL, 5 );
	
	m_textTypes = new wxTextCtrl( m_panel5, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_RICH2 );
	m_textTypes->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 76, 90, 90, false, wxEmptyString ) );
	
	bSizer13->Add( m_textTypes, 1, wxEXPAND, 5 );
	
	m_panel5->SetSizer( bSizer13 );
	m_panel5->Layout();
	bSizer13->Fit( m_panel5 );
	m_notebook2->AddPage( m_panel5, _("Types"), false );
	
	bSizer6->Add( m_notebook2, 1, wxEXPAND | wxALL, 5 );
	
	m_ctagsPage->SetSizer( bSizer6 );
	m_ctagsPage->Layout();
	bSizer6->Fit( m_ctagsPage );
	m_mainBook->AddPage( m_ctagsPage, _("Advanced"), false );
	
	mainSizer->Add( m_mainBook, 1, wxEXPAND | wxALL, 5 );
	
	m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	mainSizer->Add( m_staticline1, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxHORIZONTAL );
	
	m_buttonOK = new wxButton( this, wxID_OK, _("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonOK->SetDefault(); 
	bSizer3->Add( m_buttonOK, 0, wxALL, 5 );
	
	m_buttonCancel = new wxButton( this, wxID_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( m_buttonCancel, 0, wxALL, 5 );
	
	mainSizer->Add( bSizer3, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	this->SetSizer( mainSizer );
	this->Layout();
	
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
	m_staticTextMinWordLen->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( TagsOptionsBaseDlg::OnAutoShowWordAssitUI ), NULL, this );
	m_sliderMinWordLen->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( TagsOptionsBaseDlg::OnAutoShowWordAssitUI ), NULL, this );
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
	m_staticTextMinWordLen->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( TagsOptionsBaseDlg::OnAutoShowWordAssitUI ), NULL, this );
	m_sliderMinWordLen->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( TagsOptionsBaseDlg::OnAutoShowWordAssitUI ), NULL, this );
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
