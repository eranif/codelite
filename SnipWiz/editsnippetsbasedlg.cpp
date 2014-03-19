///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Nov 18 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "editsnippetsbasedlg.h"

///////////////////////////////////////////////////////////////////////////

EditSnippetsBaseDlg::EditSnippetsBaseDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxSize( 400,-1 ), wxDefaultSize );
	
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );
	
	m_notebook1 = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	m_panel2 = new wxPanel( m_notebook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxHORIZONTAL );
	
	m_listBox1 = new wxListBox( m_panel2, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	bSizer5->Add( m_listBox1, 0, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 2, 3, 0, 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText1 = new wxStaticText( m_panel2, wxID_ANY, _("Menu Entry:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	fgSizer1->Add( m_staticText1, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCtrlMenuEntry = new wxTextCtrl( m_panel2, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_textCtrlMenuEntry, 1, wxALL|wxEXPAND, 5 );
	
	
	fgSizer1->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_staticText3 = new wxStaticText( m_panel2, wxID_ANY, _("Keyboard Shortcut:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	fgSizer1->Add( m_staticText3, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCtrlAccelerator = new wxTextCtrl( m_panel2, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	fgSizer1->Add( m_textCtrlAccelerator, 0, wxALL|wxEXPAND, 5 );
	
	m_buttonShortcut = new wxButton( m_panel2, wxID_ANY, _("..."), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_buttonShortcut, 0, wxALL, 5 );
	
	bSizer6->Add( fgSizer1, 1, wxEXPAND, 5 );
	
	m_staticText2 = new wxStaticText( m_panel2, wxID_ANY, _("Use $ as placeholder for the selection and @ to set the caret position.\ne.g. for($ = 0; $ < @; $++)\nNOTE:\nIf your snippet contains @ or $, you can escape the placeholders with a backslash: \\@ OR \\$ "), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	bSizer6->Add( m_staticText2, 0, wxALL, 5 );
	
	m_textCtrlSnippet = new wxTextCtrl( m_panel2, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_PROCESS_TAB|wxTE_RICH2 );
	m_textCtrlSnippet->SetMinSize( wxSize( 300,200 ) );
	
	bSizer6->Add( m_textCtrlSnippet, 0, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxHORIZONTAL );
	
	m_buttonAdd = new wxButton( m_panel2, wxID_ANY, _("Add"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer8->Add( m_buttonAdd, 0, wxALL, 5 );
	
	m_buttonChange = new wxButton( m_panel2, wxID_ANY, _("Change"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer8->Add( m_buttonChange, 0, wxALL, 5 );
	
	m_buttonRemove = new wxButton( m_panel2, wxID_ANY, _("Remove"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer8->Add( m_buttonRemove, 0, wxALL, 5 );
	
	bSizer6->Add( bSizer8, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );
	
	bSizer5->Add( bSizer6, 1, 0, 5 );
	
	bSizer4->Add( bSizer5, 1, wxEXPAND, 5 );
	
	m_panel2->SetSizer( bSizer4 );
	m_panel2->Layout();
	bSizer4->Fit( m_panel2 );
	m_notebook1->AddPage( m_panel2, _("Snippets"), true );
	m_panel3 = new wxPanel( m_notebook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer( wxVERTICAL );
	
	m_htmlWinAbout = new wxHtmlWindow( m_panel3, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHW_SCROLLBAR_AUTO );
	bSizer9->Add( m_htmlWinAbout, 1, wxALL|wxEXPAND, 5 );
	
	m_panel3->SetSizer( bSizer9 );
	m_panel3->Layout();
	bSizer9->Fit( m_panel3 );
	m_notebook1->AddPage( m_panel3, _("About"), false );
	
	bSizer3->Add( m_notebook1, 1, wxEXPAND | wxALL, 5 );
	
	m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer3->Add( m_staticline1, 0, wxEXPAND | wxALL, 5 );
	
	m_buttonClose = new wxButton( this, wxID_OK, _("Close"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonClose->SetDefault(); 
	bSizer3->Add( m_buttonClose, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	this->SetSizer( bSizer3 );
	this->Layout();
	bSizer3->Fit( this );
	
	this->Centre( wxBOTH );
	
	// Connect Events
	m_listBox1->Connect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( EditSnippetsBaseDlg::OnItemSelected ), NULL, this );
	m_buttonShortcut->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( EditSnippetsBaseDlg::OnButtonKeyShortcut ), NULL, this );
	m_buttonAdd->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( EditSnippetsBaseDlg::OnAddSnippet ), NULL, this );
	m_buttonAdd->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( EditSnippetsBaseDlg::OnAddSnippetUI ), NULL, this );
	m_buttonChange->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( EditSnippetsBaseDlg::OnChangeSnippet ), NULL, this );
	m_buttonChange->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( EditSnippetsBaseDlg::OnChangeSnippetUI ), NULL, this );
	m_buttonRemove->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( EditSnippetsBaseDlg::OnRemoveSnippet ), NULL, this );
	m_buttonRemove->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( EditSnippetsBaseDlg::OnRemoveSnippetUI ), NULL, this );
}

EditSnippetsBaseDlg::~EditSnippetsBaseDlg()
{
	// Disconnect Events
	m_listBox1->Disconnect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( EditSnippetsBaseDlg::OnItemSelected ), NULL, this );
	m_buttonShortcut->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( EditSnippetsBaseDlg::OnButtonKeyShortcut ), NULL, this );
	m_buttonAdd->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( EditSnippetsBaseDlg::OnAddSnippet ), NULL, this );
	m_buttonAdd->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( EditSnippetsBaseDlg::OnAddSnippetUI ), NULL, this );
	m_buttonChange->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( EditSnippetsBaseDlg::OnChangeSnippet ), NULL, this );
	m_buttonChange->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( EditSnippetsBaseDlg::OnChangeSnippetUI ), NULL, this );
	m_buttonRemove->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( EditSnippetsBaseDlg::OnRemoveSnippet ), NULL, this );
	m_buttonRemove->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( EditSnippetsBaseDlg::OnRemoveSnippetUI ), NULL, this );
	
}
