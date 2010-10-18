///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Feb  8 2009)
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

#include "breakpointdlg.h"

#include "breakpoint_dlg_base.h"

///////////////////////////////////////////////////////////////////////////

BreakpointTab::BreakpointTab( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxHORIZONTAL );
	
	m_listCtrlBreakpoints = new BreakpointsListctrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT|wxLC_SINGLE_SEL );
	bSizer5->Add( m_listCtrlBreakpoints, 1, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxVERTICAL );
	
	m_buttonAdd = new wxButton( this, wxID_ANY, wxT("&Add"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonAdd->SetToolTip( wxT("Add a new breakpoint or watchpoint") );
	
	bSizer6->Add( m_buttonAdd, 0, wxALL, 5 );
	
	m_buttonEdit = new wxButton( this, wxID_ANY, wxT("&Edit"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonEdit->SetToolTip( wxT("Edit the selected item") );
	
	bSizer6->Add( m_buttonEdit, 0, wxALL, 5 );
	
	m_buttonDelete = new wxButton( this, wxID_ANY, wxT("&Delete"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonDelete->SetToolTip( wxT("Delete the selected item") );
	
	bSizer6->Add( m_buttonDelete, 0, wxALL, 5 );
	
	m_buttonDeleteAll = new wxButton( this, wxID_ANY, wxT("Delete &All"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonDeleteAll->SetToolTip( wxT("Delete all breakpoints and watchpoints") );
	
	bSizer6->Add( m_buttonDeleteAll, 0, wxALL, 5 );
	
	m_buttonApplyPending = new wxButton( this, wxID_ANY, wxT("Set Pending"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonApplyPending->SetToolTip( wxT("If this button is visible, there are breakpoints that you tried to set, but that the debugger refused. This most often happens when the breakpoint is inside a library that hadn't been loaded when the debugger started.\n\nClick to offer the breakpoints to the debugger again.") );
	
	bSizer6->Add( m_buttonApplyPending, 0, wxALL, 5 );
	
	bSizer5->Add( bSizer6, 0, wxEXPAND|wxTOP, 25 );
	
	this->SetSizer( bSizer5 );
	this->Layout();
	
	// Connect Events
	m_listCtrlBreakpoints->Connect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( BreakpointTab::OnItemActivated ), NULL, this );
	m_listCtrlBreakpoints->Connect( wxEVT_COMMAND_LIST_ITEM_DESELECTED, wxListEventHandler( BreakpointTab::OnItemDeselected ), NULL, this );
	m_listCtrlBreakpoints->Connect( wxEVT_COMMAND_LIST_ITEM_RIGHT_CLICK, wxListEventHandler( BreakpointTab::OnItemRightClick ), NULL, this );
	m_listCtrlBreakpoints->Connect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( BreakpointTab::OnItemSelected ), NULL, this );
	m_buttonAdd->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BreakpointTab::OnAdd ), NULL, this );
	m_buttonEdit->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BreakpointTab::OnEdit ), NULL, this );
	m_buttonDelete->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BreakpointTab::OnDelete ), NULL, this );
	m_buttonDeleteAll->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BreakpointTab::OnDeleteAll ), NULL, this );
	m_buttonApplyPending->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BreakpointTab::OnApplyPending ), NULL, this );
}

BreakpointTab::~BreakpointTab()
{
	// Disconnect Events
	m_listCtrlBreakpoints->Disconnect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( BreakpointTab::OnItemActivated ), NULL, this );
	m_listCtrlBreakpoints->Disconnect( wxEVT_COMMAND_LIST_ITEM_DESELECTED, wxListEventHandler( BreakpointTab::OnItemDeselected ), NULL, this );
	m_listCtrlBreakpoints->Disconnect( wxEVT_COMMAND_LIST_ITEM_RIGHT_CLICK, wxListEventHandler( BreakpointTab::OnItemRightClick ), NULL, this );
	m_listCtrlBreakpoints->Disconnect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( BreakpointTab::OnItemSelected ), NULL, this );
	m_buttonAdd->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BreakpointTab::OnAdd ), NULL, this );
	m_buttonEdit->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BreakpointTab::OnEdit ), NULL, this );
	m_buttonDelete->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BreakpointTab::OnDelete ), NULL, this );
	m_buttonDeleteAll->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BreakpointTab::OnDeleteAll ), NULL, this );
	m_buttonApplyPending->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BreakpointTab::OnApplyPending ), NULL, this );
}

BreakpointPropertiesDlg::BreakpointPropertiesDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText9 = new wxStaticText( this, wxID_ANY, wxT("Select the type of the breakpoint:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText9->Wrap( -1 );
	bSizer3->Add( m_staticText9, 0, wxALL, 5 );
	
	m_choicebook = new wxChoicebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxCHB_DEFAULT );
	m_panel1 = new wxPanel( m_choicebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer14;
	bSizer14 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText51 = new wxStaticText( m_panel1, wxID_ANY, wxT("Break by line, function or memory address:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText51->Wrap( -1 );
	bSizer14->Add( m_staticText51, 0, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer111;
	bSizer111 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer2->AddGrowableCol( 1 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_checkLineno = new wxCheckBox( m_panel1, wxID_ANY, wxT("Line number"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer2->Add( m_checkLineno, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textLineno = new wxTextCtrl( m_panel1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_textLineno->SetToolTip( wxT("Enter the line-number on which you wish to break. It's assumed to refer to the current file: if it doesn't, please enter the correct filepath below.") );
	
	fgSizer2->Add( m_textLineno, 0, wxALL|wxEXPAND, 5 );
	
	m_checkBreakFunction = new wxCheckBox( m_panel1, wxID_ANY, wxT("Function name"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer2->Add( m_checkBreakFunction, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textFunctionname = new wxTextCtrl( m_panel1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_textFunctionname->SetToolTip( wxT("If you wish to break when a particular function is entered, insert its name here. In C just the name will do e.g. 'main' or 'myFoo'. For C++ class methods, you need to do 'MyClass::myFoo'\n\nAlternatively you can enter a regular expression, and tick the checkbox below. A breakpoint will then be set on all matching functions.") );
	
	fgSizer2->Add( m_textFunctionname, 0, wxALL|wxEXPAND, 5 );
	
	
	fgSizer2->Add( 0, 0, 1, 0, 5 );
	
	m_checkRegex = new wxCheckBox( m_panel1, wxID_ANY, wxT("This is a regex"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkRegex->SetToolTip( wxT("If you wish to insert a breakpoint on several functions, you can tick this box, then enter a suitable regular expression in the textctrl above.") );
	
	fgSizer2->Add( m_checkRegex, 0, wxALL|wxEXPAND, 5 );
	
	m_checkBreakMemory = new wxCheckBox( m_panel1, wxID_ANY, wxT("Memory address:"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer2->Add( m_checkBreakMemory, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textBreakMemory = new wxTextCtrl( m_panel1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_textBreakMemory->SetToolTip( wxT("To break on a memory address, enter the address here.\ne.g. 0x0a1b2c3d or 12345678") );
	
	fgSizer2->Add( m_textBreakMemory, 1, wxALL|wxEXPAND, 5 );
	
	m_staticText6 = new wxStaticText( m_panel1, wxID_ANY, wxT("In file: "), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText6->Wrap( -1 );
	fgSizer2->Add( m_staticText6, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	wxBoxSizer* bSizer18;
	bSizer18 = new wxBoxSizer( wxHORIZONTAL );
	
	m_textFilename = new wxTextCtrl( m_panel1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_textFilename->SetToolTip( wxT("If you've entered a line-number, its assumed to refer to the current file. If it isn't, enter the correct filename here.\n\nFor a function, a file is usually not required. However, if you have several functions with the same name, in several different files (do people _really_ do that?) and you want to break on only one of those, enter the correct filename here.") );
	
	bSizer18->Add( m_textFilename, 1, wxALL, 5 );
	
	m_buttonBrowse = new wxButton( m_panel1, wxID_ANY, wxT("Browse"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer18->Add( m_buttonBrowse, 0, wxALL, 5 );
	
	fgSizer2->Add( bSizer18, 0, wxEXPAND|wxTOP|wxBOTTOM, 5 );
	
	bSizer111->Add( fgSizer2, 0, wxEXPAND, 5 );
	
	bSizer14->Add( bSizer111, 1, wxEXPAND|wxALL, 0 );
	
	m_staticline5 = new wxStaticLine( m_panel1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer14->Add( m_staticline5, 0, wxEXPAND | wxALL, 5 );
	
	m_panel1->SetSizer( bSizer14 );
	m_panel1->Layout();
	bSizer14->Fit( m_panel1 );
	m_choicebook->AddPage( m_panel1, wxT("Breakpoint"), true );
	m_panel2 = new wxPanel( m_choicebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer13;
	bSizer13 = new wxBoxSizer( wxVERTICAL );
	
	wxString m_radioWatchtypeChoices[] = { wxT("write only"), wxT("read only"), wxT("read-write") };
	int m_radioWatchtypeNChoices = sizeof( m_radioWatchtypeChoices ) / sizeof( wxString );
	m_radioWatchtype = new wxRadioBox( m_panel2, wxID_ANY, wxT("Type of watchpoint:"), wxDefaultPosition, wxDefaultSize, m_radioWatchtypeNChoices, m_radioWatchtypeChoices, 3, wxRA_SPECIFY_ROWS );
	m_radioWatchtype->SetSelection( 0 );
	m_radioWatchtype->SetToolTip( wxT("The usual type of watchpoint is 'write-only': that is, it's triggered whenever the target is changed.\n\nAlternatively you can choose for it to trigger when the target is read from, or either written to or read from.") );
	
	bSizer13->Add( m_radioWatchtype, 0, wxALIGN_CENTER_HORIZONTAL|wxEXPAND, 5 );
	
	m_staticText5 = new wxStaticText( m_panel2, wxID_ANY, wxT("Data to watch:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText5->Wrap( -1 );
	bSizer13->Add( m_staticText5, 0, wxALL|wxEXPAND, 5 );
	
	m_textWatchExpression = new wxTextCtrl( m_panel2, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE );
	m_textWatchExpression->SetToolTip( wxT("Identify the data to be watched. It can be one of:\n1) Any variable name e.g. 'foo'\n2) A memory address, suitably cast e.g.*(int*)0x12345678 will watch an int-sized block starting at this address.\nDon't include spaces in the expression: gdb can't understand them.\n3) A complex expression e.g. a*b + c/d'. The expression can use any operators valid in the program's native language.\n\nNB. A watchpoint set on a local variable will automatically be removed when the variable loses scope.") );	
	
	bSizer13->Add( m_textWatchExpression, 1, wxEXPAND|wxALL, 5 );
	
	m_panel2->SetSizer( bSizer13 );
	m_panel2->Layout();
	bSizer13->Fit( m_panel2 );
	m_choicebook->AddPage( m_panel2, wxT("Watchpoint"), false );
	bSizer3->Add( m_choicebook, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText1 = new wxStaticText( this, wxID_ANY, wxT("Conditional Breaks: Add any condition here"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	bSizer5->Add( m_staticText1, 0, wxALL|wxEXPAND, 5 );
	
	m_textCond = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( -1,-1 ), wxTE_MULTILINE );
	m_textCond->SetToolTip( wxT("You can add a condition to any breakpoint or watchpoint. The debugger will then stop only if the condition is met.\n\nThe condition can be any simple or complex expression in your programming language,providing it returns a bool. However any variables that you use must be in scope.\n\nIf you've previously set a condition and no longer want it, just clear this textctrl.") );
	
	bSizer5->Add( m_textCond, 1, wxEXPAND|wxALL, 5 );
	
	m_staticText11 = new wxStaticText( this, wxID_ANY, wxT("Command List: Add any command(s) here"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText11->Wrap( -1 );
	bSizer5->Add( m_staticText11, 0, wxALL|wxEXPAND, 5 );
	
	m_textCommands = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE );
	m_textCommands->SetToolTip( wxT("You can add a list of commands to any breakpoint or watchpoint. When the breakpoint is hit and the program interrupted, those commands will be executed.\n\nFor example, to print the value of the variable foo and then continue running the program, enter:\nprint foo\ncont\n\nIf you've previously entered commands, and no longer want them, just clear this textctrl.") );
	
	bSizer5->Add( m_textCommands, 1, wxEXPAND|wxALL, 5 );
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 2, 4, 0, 0 );
	fgSizer1->AddGrowableCol( 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->AddGrowableCol( 2 );
	fgSizer1->AddGrowableCol( 3 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_checkDisable = new wxCheckBox( this, wxID_ANY, wxT("Disable"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkDisable->SetToolTip( wxT("If you check this box, the breakpoint (or watchpoint) will still exist, but it won't trigger. If you uncheck it in the future, the breakpoint will work again.") );
	
	fgSizer1->Add( m_checkDisable, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
	
	m_checkTemp = new wxCheckBox( this, wxID_ANY, wxT("Make temporary"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkTemp->SetToolTip( wxT("A temporary breakpoint (or watchpoint) is one that works only once. When it's hit it behaves like any other, except that it's then deleted.") );
	
	fgSizer1->Add( m_checkTemp, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
	
	m_staticText7 = new wxStaticText( this, wxID_ANY, wxT("Ignore count:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText7->Wrap( -1 );
	m_staticText7->SetToolTip( wxT("Enter a count >0 to ignore this breakpoint (or watchpoint) for that number of times. It then behaves as though it is disabled, except that every time it would have triggered, the ignore count decrements.\nWhen the count reaches zero, the breakpoint becomes active again.") );
	
	fgSizer1->Add( m_staticText7, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_spinIgnore = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 1000000, 0 );
	fgSizer1->Add( m_spinIgnore, 1, wxALL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
	
	bSizer5->Add( fgSizer1, 0, wxEXPAND, 5 );
	
	bSizer3->Add( bSizer5, 1, wxALL|wxEXPAND, 5 );
	
	m_staticline4 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer3->Add( m_staticline4, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer( wxHORIZONTAL );
	
	m_button6 = new wxButton( this, wxID_OK, wxT("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
	m_button6->SetDefault(); 
	bSizer9->Add( m_button6, 0, wxALL, 5 );
	
	m_button7 = new wxButton( this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer9->Add( m_button7, 0, wxALL, 5 );
	
	bSizer3->Add( bSizer9, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	this->SetSizer( bSizer3 );
	this->Layout();
	bSizer3->Fit( this );
	
	// Connect Events
	m_choicebook->Connect( wxEVT_COMMAND_CHOICEBOOK_PAGE_CHANGED, wxChoicebookEventHandler( BreakpointPropertiesDlg::OnPageChanged ), NULL, this );
	m_choicebook->Connect( wxEVT_COMMAND_CHOICEBOOK_PAGE_CHANGING, wxChoicebookEventHandler( BreakpointPropertiesDlg::OnPageChanging ), NULL, this );
	m_checkLineno->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( BreakpointPropertiesDlg::OnCheckBreakLineno ), NULL, this );
	m_checkLineno->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( BreakpointPropertiesDlg::OnCheckBreakLinenoUI ), NULL, this );
	m_checkBreakFunction->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( BreakpointPropertiesDlg::OnCheckBreakFunction ), NULL, this );
	m_checkBreakFunction->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( BreakpointPropertiesDlg::OnCheckBreakFunctionUI ), NULL, this );
	m_checkBreakMemory->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( BreakpointPropertiesDlg::OnCheckBreakMemory ), NULL, this );
	m_checkBreakMemory->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( BreakpointPropertiesDlg::OnCheckBreakMemoryUI ), NULL, this );
	m_buttonBrowse->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BreakpointPropertiesDlg::OnBrowse ), NULL, this );
}

BreakpointPropertiesDlg::~BreakpointPropertiesDlg()
{
	// Disconnect Events
	m_choicebook->Disconnect( wxEVT_COMMAND_CHOICEBOOK_PAGE_CHANGED, wxChoicebookEventHandler( BreakpointPropertiesDlg::OnPageChanged ), NULL, this );
	m_choicebook->Disconnect( wxEVT_COMMAND_CHOICEBOOK_PAGE_CHANGING, wxChoicebookEventHandler( BreakpointPropertiesDlg::OnPageChanging ), NULL, this );
	m_checkLineno->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( BreakpointPropertiesDlg::OnCheckBreakLineno ), NULL, this );
	m_checkLineno->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( BreakpointPropertiesDlg::OnCheckBreakLinenoUI ), NULL, this );
	m_checkBreakFunction->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( BreakpointPropertiesDlg::OnCheckBreakFunction ), NULL, this );
	m_checkBreakFunction->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( BreakpointPropertiesDlg::OnCheckBreakFunctionUI ), NULL, this );
	m_checkBreakMemory->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( BreakpointPropertiesDlg::OnCheckBreakMemory ), NULL, this );
	m_checkBreakMemory->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( BreakpointPropertiesDlg::OnCheckBreakMemoryUI ), NULL, this );
	m_buttonBrowse->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BreakpointPropertiesDlg::OnBrowse ), NULL, this );
}
