///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Aug 25 2009)
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
	
	m_notebook2 = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	m_panel1 = new wxPanel( m_notebook2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );
	
	m_book = new wxListbook( m_panel1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLB_DEFAULT );
	#ifndef __WXGTK__ // Small icon style not supported in GTK
	wxListView* m_bookListView = m_book->GetListView();
	long m_bookFlags = m_bookListView->GetWindowStyleFlag();
	m_bookFlags = ( m_bookFlags & ~wxLC_ICON ) | wxLC_SMALL_ICON;
	m_bookListView->SetWindowStyleFlag( m_bookFlags );
	#endif
	
	bSizer3->Add( m_book, 1, wxEXPAND | wxALL, 5 );
	
	m_panel1->SetSizer( bSizer3 );
	m_panel1->Layout();
	bSizer3->Fit( m_panel1 );
	m_notebook2->AddPage( m_panel1, wxT("Loaded Debuggers"), true );
	m_panel2 = new wxPanel( m_notebook2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxHORIZONTAL );
	
	m_listCtrl1 = new wxListCtrl( m_panel2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT );
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
	m_notebook2->AddPage( m_panel2, wxT("PreDefined Types"), false );
	
	bSizer1->Add( m_notebook2, 1, wxEXPAND | wxALL, 5 );
	
	m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer1->Add( m_staticline1, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxHORIZONTAL );
	
	m_buttonOK = new wxButton( this, wxID_OK, wxT("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_buttonOK, 0, wxALL, 5 );
	
	m_buttonCancel = new wxButton( this, wxID_CANCEL, wxT("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_buttonCancel, 0, wxALL, 5 );
	
	bSizer1->Add( bSizer2, 0, wxALIGN_RIGHT, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	
	// Connect Events
	m_listCtrl1->Connect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( DebuggerSettingsBaseDlg::OnItemActivated ), NULL, this );
	m_listCtrl1->Connect( wxEVT_COMMAND_LIST_ITEM_DESELECTED, wxListEventHandler( DebuggerSettingsBaseDlg::OnItemDeselected ), NULL, this );
	m_listCtrl1->Connect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( DebuggerSettingsBaseDlg::OnItemSelected ), NULL, this );
	m_buttonNewType->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DebuggerSettingsBaseDlg::OnNewShortcut ), NULL, this );
	m_buttonEdit->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DebuggerSettingsBaseDlg::OnEditShortcut ), NULL, this );
	m_buttonDelete->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DebuggerSettingsBaseDlg::OnDeleteShortcut ), NULL, this );
	m_buttonCancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DebuggerSettingsBaseDlg::OnButtonCancel ), NULL, this );
}

DebuggerSettingsBaseDlg::~DebuggerSettingsBaseDlg()
{
	// Disconnect Events
	m_listCtrl1->Disconnect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( DebuggerSettingsBaseDlg::OnItemActivated ), NULL, this );
	m_listCtrl1->Disconnect( wxEVT_COMMAND_LIST_ITEM_DESELECTED, wxListEventHandler( DebuggerSettingsBaseDlg::OnItemDeselected ), NULL, this );
	m_listCtrl1->Disconnect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( DebuggerSettingsBaseDlg::OnItemSelected ), NULL, this );
	m_buttonNewType->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DebuggerSettingsBaseDlg::OnNewShortcut ), NULL, this );
	m_buttonEdit->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DebuggerSettingsBaseDlg::OnEditShortcut ), NULL, this );
	m_buttonDelete->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DebuggerSettingsBaseDlg::OnDeleteShortcut ), NULL, this );
	m_buttonCancel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DebuggerSettingsBaseDlg::OnButtonCancel ), NULL, this );
}

DebuggerPageBase::DebuggerPageBase( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxVERTICAL );
	
	m_choicebook1 = new wxChoicebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxCHB_DEFAULT );
	m_panel3 = new wxPanel( m_choicebook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 1, 0, 0, 0 );
	fgSizer2->AddGrowableCol( 1 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText1 = new wxStaticText( m_panel3, wxID_ANY, wxT("Debugger path:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	fgSizer2->Add( m_staticText1, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCtrDbgPath = new wxTextCtrl( m_panel3, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer2->Add( m_textCtrDbgPath, 0, wxALL|wxEXPAND, 5 );
	
	m_buttonBrowse = new wxButton( m_panel3, wxID_ANY, wxT("Browse"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer2->Add( m_buttonBrowse, 0, wxALL, 5 );
	
	bSizer8->Add( fgSizer2, 0, wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer1;
	sbSizer1 = new wxStaticBoxSizer( new wxStaticBox( m_panel3, wxID_ANY, wxT("Options:") ), wxVERTICAL );
	
	wxGridSizer* gSizer3;
	gSizer3 = new wxGridSizer( 4, 1, 0, 0 );
	
	m_checkBoxEnablePendingBreakpoints = new wxCheckBox( m_panel3, wxID_ANY, wxT("Enable pending breakpoint"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer3->Add( m_checkBoxEnablePendingBreakpoints, 0, wxALL, 5 );
	
	m_checkBreakAtWinMain = new wxCheckBox( m_panel3, wxID_ANY, wxT("Automatically set breakpoint at main"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer3->Add( m_checkBreakAtWinMain, 0, wxALL, 5 );
	
	m_catchThrow = new wxCheckBox( m_panel3, wxID_ANY, wxT("Break when C++ execption is thrown"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer3->Add( m_catchThrow, 0, wxALL, 5 );
	
	m_checkBoxDebugAssert = new wxCheckBox( m_panel3, wxID_ANY, wxT("Break at assertion failure (MinGW only)"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer3->Add( m_checkBoxDebugAssert, 0, wxALL, 5 );
	
	sbSizer1->Add( gSizer3, 1, wxEXPAND|wxALL, 5 );
	
	bSizer8->Add( sbSizer1, 0, wxEXPAND|wxALL, 5 );
	
	wxStaticBoxSizer* sbSizer2;
	sbSizer2 = new wxStaticBoxSizer( new wxStaticBox( m_panel3, wxID_ANY, wxT("Misc:") ), wxVERTICAL );
	
	wxGridSizer* gSizer2;
	gSizer2 = new wxGridSizer( 4, 1, 0, 0 );
	
	m_checkBoxEnableLog = new wxCheckBox( m_panel3, wxID_ANY, wxT("Enable full debugger logging"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer2->Add( m_checkBoxEnableLog, 0, wxALL, 5 );
	
	m_checkShowTerminal = new wxCheckBox( m_panel3, wxID_ANY, wxT("Show debugger terminal"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer2->Add( m_checkShowTerminal, 0, wxALL, 5 );
	
	m_checkUseRelativePaths = new wxCheckBox( m_panel3, wxID_ANY, wxT("Use file name only for breakpoints (NO full paths)"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer2->Add( m_checkUseRelativePaths, 0, wxALL, 5 );
	
	m_showTooltips = new wxCheckBox( m_panel3, wxID_ANY, wxT("While debugging, show debugger tooltips"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer2->Add( m_showTooltips, 0, wxALL, 5 );
	
	sbSizer2->Add( gSizer2, 1, wxEXPAND|wxALL, 5 );
	
	bSizer8->Add( sbSizer2, 0, wxEXPAND|wxALL, 5 );
	
	wxStaticBoxSizer* sbSizer3;
	sbSizer3 = new wxStaticBoxSizer( new wxStaticBox( m_panel3, wxID_ANY, wxT("Display:") ), wxHORIZONTAL );
	
	wxFlexGridSizer* fgSizer21;
	fgSizer21 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer21->SetFlexibleDirection( wxBOTH );
	fgSizer21->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText2 = new wxStaticText( m_panel3, wxID_ANY, wxT("Set a limit on how many elements of an array GDB will print (including strings)"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	m_staticText2->SetToolTip( wxT(" For no limit, set it to 0") );
	
	fgSizer21->Add( m_staticText2, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_spinCtrlNumElements = new wxSpinCtrl( m_panel3, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 10000, 200 );
	m_spinCtrlNumElements->SetToolTip( wxT("For no limit, set it to 0") );
	
	fgSizer21->Add( m_spinCtrlNumElements, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_checkBoxExpandLocals = new wxCheckBox( m_panel3, wxID_ANY, wxT("Evaluate items in the 'Locals' view based on the 'PreDefined' types where possible"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer21->Add( m_checkBoxExpandLocals, 0, wxALL, 5 );
	
	
	fgSizer21->Add( 0, 0, 1, wxEXPAND, 5 );
	
	sbSizer3->Add( fgSizer21, 1, wxEXPAND|wxALL, 5 );
	
	bSizer8->Add( sbSizer3, 0, wxEXPAND|wxALL, 5 );
	
	m_panel3->SetSizer( bSizer8 );
	m_panel3->Layout();
	bSizer8->Fit( m_panel3 );
	m_choicebook1->AddPage( m_panel3, wxT("General"), false );
	m_panel4 = new wxPanel( m_choicebook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer( wxVERTICAL );
	
	m_textCtrlStartupCommands = new wxTextCtrl( m_panel4, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_PROCESS_ENTER|wxTE_PROCESS_TAB|wxTE_RICH2 );
	m_textCtrlStartupCommands->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 76, 90, 90, false, wxEmptyString ) );
	
	bSizer9->Add( m_textCtrlStartupCommands, 1, wxALL|wxEXPAND, 5 );
	
	m_panel4->SetSizer( bSizer9 );
	m_panel4->Layout();
	bSizer9->Fit( m_panel4 );
	m_choicebook1->AddPage( m_panel4, wxT("Startup Commands"), false );
	bSizer7->Add( m_choicebook1, 1, wxEXPAND | wxALL, 5 );
	
	this->SetSizer( bSizer7 );
	this->Layout();
	bSizer7->Fit( this );
	
	// Connect Events
	m_buttonBrowse->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DebuggerPageBase::OnBrowse ), NULL, this );
	m_checkBoxDebugAssert->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( DebuggerPageBase::OnDebugAssert ), NULL, this );
}

DebuggerPageBase::~DebuggerPageBase()
{
	// Disconnect Events
	m_buttonBrowse->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DebuggerPageBase::OnBrowse ), NULL, this );
	m_checkBoxDebugAssert->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( DebuggerPageBase::OnDebugAssert ), NULL, this );
}
