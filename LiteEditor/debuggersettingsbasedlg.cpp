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

#include "debuggersettingsbasedlg.h"

///////////////////////////////////////////////////////////////////////////

DebuggerSettingsBaseDlg::DebuggerSettingsBaseDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	m_notebook2 = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNB_NOPAGETHEME );
	m_panel1 = new wxPanel( m_notebook2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );
	
	m_book = new wxNotebook( m_panel1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNB_NOPAGETHEME|wxNB_TOP );
	
	bSizer3->Add( m_book, 1, wxEXPAND | wxALL, 5 );
	
	m_panel1->SetSizer( bSizer3 );
	m_panel1->Layout();
	bSizer3->Fit( m_panel1 );
	m_notebook2->AddPage( m_panel1, wxT("Loaded Debuggers"), true );
	m_panel6 = new wxPanel( m_notebook2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer16;
	bSizer16 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer14;
	bSizer14 = new wxBoxSizer( wxHORIZONTAL );
	
	m_buttonNewSet = new wxButton( m_panel6, wxID_NEW, wxT("&New"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonNewSet->SetToolTip( wxT("Create new 'PreDefined' set") );
	
	bSizer14->Add( m_buttonNewSet, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_buttonDeleteSet = new wxButton( m_panel6, wxID_DELETE, wxT("&Delete"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonDeleteSet->SetToolTip( wxT("Delete the currently selected set") );
	
	bSizer14->Add( m_buttonDeleteSet, 0, wxALL, 5 );
	
	bSizer16->Add( bSizer14, 0, wxALIGN_RIGHT, 5 );
	
	wxStaticBoxSizer* sbSizer5;
	sbSizer5 = new wxStaticBoxSizer( new wxStaticBox( m_panel6, wxID_ANY, wxT("Available sets:") ), wxVERTICAL );
	
	m_notebookPreDefTypes = new wxChoicebook( m_panel6, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxCHB_DEFAULT );
	sbSizer5->Add( m_notebookPreDefTypes, 1, wxEXPAND, 5 );
	
	bSizer16->Add( sbSizer5, 1, wxEXPAND, 5 );
	
	m_panel6->SetSizer( bSizer16 );
	m_panel6->Layout();
	bSizer16->Fit( m_panel6 );
	m_notebook2->AddPage( m_panel6, wxT("PreDefined Types"), false );
	
	bSizer1->Add( m_notebook2, 1, wxEXPAND | wxALL, 5 );
	
	m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer1->Add( m_staticline1, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxHORIZONTAL );
	
	m_buttonOK = new wxButton( this, wxID_OK, wxT("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonOK->SetDefault(); 
	bSizer2->Add( m_buttonOK, 0, wxALL, 5 );
	
	m_buttonCancel = new wxButton( this, wxID_CANCEL, wxT("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_buttonCancel, 0, wxALL, 5 );
	
	bSizer1->Add( bSizer2, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	bSizer1->Fit( this );
	
	// Connect Events
	m_buttonNewSet->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DebuggerSettingsBaseDlg::OnNewSet ), NULL, this );
	m_buttonDeleteSet->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DebuggerSettingsBaseDlg::OnDeleteSet ), NULL, this );
	m_buttonDeleteSet->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( DebuggerSettingsBaseDlg::OnDeleteSetUI ), NULL, this );
	m_notebookPreDefTypes->Connect( wxEVT_COMMAND_CHOICEBOOK_PAGE_CHANGED, wxChoicebookEventHandler( DebuggerSettingsBaseDlg::OnPageChanged ), NULL, this );
	m_buttonCancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DebuggerSettingsBaseDlg::OnButtonCancel ), NULL, this );
}

DebuggerSettingsBaseDlg::~DebuggerSettingsBaseDlg()
{
	// Disconnect Events
	m_buttonNewSet->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DebuggerSettingsBaseDlg::OnNewSet ), NULL, this );
	m_buttonDeleteSet->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DebuggerSettingsBaseDlg::OnDeleteSet ), NULL, this );
	m_buttonDeleteSet->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( DebuggerSettingsBaseDlg::OnDeleteSetUI ), NULL, this );
	m_notebookPreDefTypes->Disconnect( wxEVT_COMMAND_CHOICEBOOK_PAGE_CHANGED, wxChoicebookEventHandler( DebuggerSettingsBaseDlg::OnPageChanged ), NULL, this );
	m_buttonCancel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DebuggerSettingsBaseDlg::OnButtonCancel ), NULL, this );
	
}

DebuggerPageBase::DebuggerPageBase( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxVERTICAL );
	
	m_choicebook3 = new wxChoicebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxCHB_DEFAULT );
	m_panel3 = new wxPanel( m_choicebook3, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxVERTICAL );
	
	m_notebook3 = new wxNotebook( m_panel3, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	m_panel6 = new wxPanel( m_notebook3, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	wxBoxSizer* bSizer16;
	bSizer16 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer10;
	bSizer10 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText1 = new wxStaticText( m_panel6, wxID_ANY, wxT("Debugger path:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	bSizer10->Add( m_staticText1, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCtrDbgPath = new wxTextCtrl( m_panel6, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_RICH2 );
	bSizer10->Add( m_textCtrDbgPath, 1, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM|wxLEFT, 5 );
	
	m_buttonBrowse = new wxButton( m_panel6, wxID_ANY, wxT("Browse"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer10->Add( m_buttonBrowse, 0, wxTOP|wxBOTTOM|wxRIGHT, 5 );
	
	bSizer16->Add( bSizer10, 0, wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer1;
	sbSizer1 = new wxStaticBoxSizer( new wxStaticBox( m_panel6, wxID_ANY, wxT("Options:") ), wxVERTICAL );
	
	wxGridSizer* gSizer3;
	gSizer3 = new wxGridSizer( 4, 1, 0, 0 );
	
	m_checkBreakAtWinMain = new wxCheckBox( m_panel6, wxID_ANY, wxT("Automatically set breakpoint at main"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer3->Add( m_checkBreakAtWinMain, 0, wxALL, 5 );
	
	m_catchThrow = new wxCheckBox( m_panel6, wxID_ANY, wxT("Break when C++ exception is thrown"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer3->Add( m_catchThrow, 0, wxALL, 5 );
	
	m_checkBoxSetBreakpointsAfterMain = new wxCheckBox( m_panel6, wxID_ANY, wxT("Apply breakpoints after main function is hit"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer3->Add( m_checkBoxSetBreakpointsAfterMain, 0, wxALL, 5 );
	
	m_checkBoxEnablePendingBreakpoints = new wxCheckBox( m_panel6, wxID_ANY, wxT("Enable pending breakpoints"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer3->Add( m_checkBoxEnablePendingBreakpoints, 0, wxALL, 5 );
	
	sbSizer1->Add( gSizer3, 0, wxEXPAND, 5 );
	
	bSizer16->Add( sbSizer1, 0, wxEXPAND|wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	wxStaticBoxSizer* sbSizer4;
	sbSizer4 = new wxStaticBoxSizer( new wxStaticBox( m_panel6, wxID_ANY, wxT("Debugger Tooltip:") ), wxVERTICAL );
	
	wxGridSizer* gSizer31;
	gSizer31 = new wxGridSizer( 2, 1, 0, 0 );
	
	m_showTooltips = new wxCheckBox( m_panel6, wxID_ANY, wxT("While debugging, show debugger tooltips"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer31->Add( m_showTooltips, 0, wxEXPAND|wxALL, 5 );
	
	m_checkBoxAutoExpand = new wxCheckBox( m_panel6, wxID_ANY, wxT("Auto expand items under the cursor"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer31->Add( m_checkBoxAutoExpand, 0, wxEXPAND|wxALL, 5 );
	
	sbSizer4->Add( gSizer31, 0, wxEXPAND, 5 );
	
	bSizer16->Add( sbSizer4, 0, wxEXPAND|wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	wxStaticBoxSizer* sbSizer3;
	sbSizer3 = new wxStaticBoxSizer( new wxStaticBox( m_panel6, wxID_ANY, wxT("Display:") ), wxHORIZONTAL );
	
	wxFlexGridSizer* fgSizer21;
	fgSizer21 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer21->SetFlexibleDirection( wxBOTH );
	fgSizer21->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText2 = new wxStaticText( m_panel6, wxID_ANY, wxT("Number of elements to display for arrays / strings:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	m_staticText2->SetToolTip( wxT(" For no limit, set it to 0") );
	
	fgSizer21->Add( m_staticText2, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_spinCtrlNumElements = new wxSpinCtrl( m_panel6, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 10000, 200 );
	m_spinCtrlNumElements->SetToolTip( wxT("For no limit, set it to 0") );
	
	fgSizer21->Add( m_spinCtrlNumElements, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_checkBoxExpandLocals = new wxCheckBox( m_panel6, wxID_ANY, wxT("Use the 'PreDefiend types for the 'Locals' view"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer21->Add( m_checkBoxExpandLocals, 0, wxALL|wxEXPAND, 5 );
	
	
	fgSizer21->Add( 0, 0, 1, wxEXPAND, 5 );
	
	sbSizer3->Add( fgSizer21, 0, wxEXPAND, 5 );
	
	bSizer16->Add( sbSizer3, 0, wxEXPAND|wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	m_panel6->SetSizer( bSizer16 );
	m_panel6->Layout();
	bSizer16->Fit( m_panel6 );
	m_notebook3->AddPage( m_panel6, wxT("General"), true );
	m_panel7 = new wxPanel( m_notebook3, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer17;
	bSizer17 = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* sbSizer2;
	sbSizer2 = new wxStaticBoxSizer( new wxStaticBox( m_panel7, wxID_ANY, wxEmptyString ), wxVERTICAL );
	
	wxGridSizer* gSizer2;
	gSizer2 = new wxGridSizer( 4, 1, 0, 0 );
	
	m_checkBoxEnableLog = new wxCheckBox( m_panel7, wxID_ANY, wxT("Enable full debugger logging"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer2->Add( m_checkBoxEnableLog, 0, wxALL, 5 );
	
	m_checkShowTerminal = new wxCheckBox( m_panel7, wxID_ANY, wxT("Show debugger terminal"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer2->Add( m_checkShowTerminal, 0, wxALL, 5 );
	
	m_checkUseRelativePaths = new wxCheckBox( m_panel7, wxID_ANY, wxT("Use file name only for breakpoints (NO full paths)"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer2->Add( m_checkUseRelativePaths, 0, wxALL, 5 );
	
	sbSizer2->Add( gSizer2, 0, wxEXPAND, 5 );
	
	bSizer17->Add( sbSizer2, 0, wxEXPAND|wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	wxStaticBoxSizer* sbSizer6;
	sbSizer6 = new wxStaticBoxSizer( new wxStaticBox( m_panel7, wxID_ANY, wxT("MinGW / Cygwin:") ), wxVERTICAL );
	
	wxGridSizer* gSizer5;
	gSizer5 = new wxGridSizer( 3, 1, 0, 0 );
	
	m_checkBoxDebugAssert = new wxCheckBox( m_panel7, wxID_ANY, wxT("Break at assertion failure (MinGW only)"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer5->Add( m_checkBoxDebugAssert, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText5 = new wxStaticText( m_panel7, wxID_ANY, wxT("Cygwin path conversion command:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText5->Wrap( -1 );
	m_staticText5->SetToolTip( wxT("Set here the command to use in order to convert cygwin paths into native Windows paths (use $(File) as a place holder for the file name)") );
	
	gSizer5->Add( m_staticText5, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxRIGHT|wxLEFT, 5 );
	
	m_textCtrlCygwinPathCommand = new wxTextCtrl( m_panel7, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_RICH2 );
	m_textCtrlCygwinPathCommand->SetToolTip( wxT("Set here the command to use in order to convert cygwin paths into native Windows paths (use $(File) as a place holder for the file name)") );
	
	gSizer5->Add( m_textCtrlCygwinPathCommand, 0, wxALIGN_CENTER_VERTICAL|wxEXPAND|wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	sbSizer6->Add( gSizer5, 0, wxEXPAND, 5 );
	
	bSizer17->Add( sbSizer6, 0, wxEXPAND|wxRIGHT|wxLEFT, 5 );
	
	m_panel7->SetSizer( bSizer17 );
	m_panel7->Layout();
	bSizer17->Fit( m_panel7 );
	m_notebook3->AddPage( m_panel7, wxT("Misc"), false );
	
	bSizer8->Add( m_notebook3, 1, wxEXPAND, 5 );
	
	m_panel3->SetSizer( bSizer8 );
	m_panel3->Layout();
	bSizer8->Fit( m_panel3 );
	m_choicebook3->AddPage( m_panel3, wxT("Settings"), false );
	m_panel4 = new wxPanel( m_choicebook3, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer( wxVERTICAL );
	
	m_textCtrlStartupCommands = new wxTextCtrl( m_panel4, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_PROCESS_ENTER|wxTE_PROCESS_TAB|wxTE_RICH2 );
	m_textCtrlStartupCommands->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 76, 90, 90, false, wxEmptyString ) );
	
	bSizer9->Add( m_textCtrlStartupCommands, 1, wxALL|wxEXPAND, 5 );
	
	m_panel4->SetSizer( bSizer9 );
	m_panel4->Layout();
	bSizer9->Fit( m_panel4 );
	m_choicebook3->AddPage( m_panel4, wxT("Startup commands"), false );
	bSizer7->Add( m_choicebook3, 1, wxEXPAND | wxALL, 5 );
	
	this->SetSizer( bSizer7 );
	this->Layout();
	bSizer7->Fit( this );
	
	// Connect Events
	m_buttonBrowse->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DebuggerPageBase::OnBrowse ), NULL, this );
	m_checkBoxDebugAssert->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( DebuggerPageBase::OnDebugAssert ), NULL, this );
	m_checkBoxDebugAssert->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( DebuggerPageBase::OnWindowsUI ), NULL, this );
	m_staticText5->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( DebuggerPageBase::OnWindowsUI ), NULL, this );
	m_textCtrlCygwinPathCommand->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( DebuggerPageBase::OnWindowsUI ), NULL, this );
}

DebuggerPageBase::~DebuggerPageBase()
{
	// Disconnect Events
	m_buttonBrowse->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DebuggerPageBase::OnBrowse ), NULL, this );
	m_checkBoxDebugAssert->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( DebuggerPageBase::OnDebugAssert ), NULL, this );
	m_checkBoxDebugAssert->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( DebuggerPageBase::OnWindowsUI ), NULL, this );
	m_staticText5->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( DebuggerPageBase::OnWindowsUI ), NULL, this );
	m_textCtrlCygwinPathCommand->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( DebuggerPageBase::OnWindowsUI ), NULL, this );
	
}

PreDefinedTypesPageBase::PreDefinedTypesPageBase( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer11;
	bSizer11 = new wxBoxSizer( wxVERTICAL );
	
	m_panel2 = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxHORIZONTAL );
	
	m_listCtrl1 = new wxListCtrl( m_panel2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_HRULES|wxLC_REPORT|wxLC_SINGLE_SEL|wxLC_VRULES );
	bSizer5->Add( m_listCtrl1, 1, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxVERTICAL );
	
	m_buttonNewType = new wxButton( m_panel2, wxID_ANY, wxT("&New..."), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer6->Add( m_buttonNewType, 0, wxALL|wxEXPAND, 5 );
	
	m_buttonEdit = new wxButton( m_panel2, wxID_ANY, wxT("Edit..."), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer6->Add( m_buttonEdit, 0, wxALL|wxEXPAND, 5 );
	
	m_buttonDelete = new wxButton( m_panel2, wxID_ANY, wxT("&Delete"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer6->Add( m_buttonDelete, 0, wxALL|wxEXPAND, 5 );
	
	bSizer5->Add( bSizer6, 0, wxEXPAND, 5 );
	
	bSizer4->Add( bSizer5, 1, wxEXPAND, 5 );
	
	m_panel2->SetSizer( bSizer4 );
	m_panel2->Layout();
	bSizer4->Fit( m_panel2 );
	bSizer11->Add( m_panel2, 1, wxEXPAND, 5 );
	
	this->SetSizer( bSizer11 );
	this->Layout();
	bSizer11->Fit( this );
	
	// Connect Events
	m_listCtrl1->Connect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( PreDefinedTypesPageBase::OnItemActivated ), NULL, this );
	m_listCtrl1->Connect( wxEVT_COMMAND_LIST_ITEM_DESELECTED, wxListEventHandler( PreDefinedTypesPageBase::OnItemDeselected ), NULL, this );
	m_listCtrl1->Connect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( PreDefinedTypesPageBase::OnItemSelected ), NULL, this );
	m_buttonNewType->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PreDefinedTypesPageBase::OnNewShortcut ), NULL, this );
	m_buttonEdit->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PreDefinedTypesPageBase::OnEditShortcut ), NULL, this );
	m_buttonDelete->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PreDefinedTypesPageBase::OnDeleteShortcut ), NULL, this );
}

PreDefinedTypesPageBase::~PreDefinedTypesPageBase()
{
	// Disconnect Events
	m_listCtrl1->Disconnect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( PreDefinedTypesPageBase::OnItemActivated ), NULL, this );
	m_listCtrl1->Disconnect( wxEVT_COMMAND_LIST_ITEM_DESELECTED, wxListEventHandler( PreDefinedTypesPageBase::OnItemDeselected ), NULL, this );
	m_listCtrl1->Disconnect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( PreDefinedTypesPageBase::OnItemSelected ), NULL, this );
	m_buttonNewType->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PreDefinedTypesPageBase::OnNewShortcut ), NULL, this );
	m_buttonEdit->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PreDefinedTypesPageBase::OnEditShortcut ), NULL, this );
	m_buttonDelete->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PreDefinedTypesPageBase::OnDeleteShortcut ), NULL, this );
	
}

NewPreDefinedSetDlg::NewPreDefinedSetDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer17;
	bSizer17 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer2->AddGrowableCol( 1 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText3 = new wxStaticText( this, wxID_ANY, wxT("Name:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	fgSizer2->Add( m_staticText3, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCtrlName = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_RICH2 );
	fgSizer2->Add( m_textCtrlName, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText4 = new wxStaticText( this, wxID_ANY, wxT("Copy Values From:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( -1 );
	fgSizer2->Add( m_staticText4, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxArrayString m_choiceCopyFromChoices;
	m_choiceCopyFrom = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceCopyFromChoices, 0 );
	m_choiceCopyFrom->SetSelection( 0 );
	fgSizer2->Add( m_choiceCopyFrom, 0, wxALL|wxEXPAND, 5 );
	
	
	fgSizer2->Add( 0, 0, 0, wxEXPAND, 5 );
	
	m_checkBoxMakeActive = new wxCheckBox( this, wxID_ANY, wxT("Make this 'PreDefined Types' set active"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer2->Add( m_checkBoxMakeActive, 0, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
	
	bSizer17->Add( fgSizer2, 0, wxEXPAND, 5 );
	
	
	bSizer17->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_staticline2 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer17->Add( m_staticline2, 0, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer18;
	bSizer18 = new wxBoxSizer( wxHORIZONTAL );
	
	m_button9 = new wxButton( this, wxID_OK, wxT("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
	m_button9->SetDefault(); 
	bSizer18->Add( m_button9, 0, wxALL, 5 );
	
	m_button10 = new wxButton( this, wxID_CANCEL, wxT("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer18->Add( m_button10, 0, wxALL, 5 );
	
	bSizer17->Add( bSizer18, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	this->SetSizer( bSizer17 );
	this->Layout();
	bSizer17->Fit( this );
	
	this->Centre( wxBOTH );
}

NewPreDefinedSetDlg::~NewPreDefinedSetDlg()
{
}
