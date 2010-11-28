 ///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Nov 18 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "addincludefiledlgbase.h"

///////////////////////////////////////////////////////////////////////////

AddIncludeFileDlgBase::AddIncludeFileDlgBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxSize( 250,-1 ), wxDefaultSize );
	
	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxVERTICAL );
	
	m_mainPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_mainPanel->SetMinSize( wxSize( 200,-1 ) );
	
	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText4 = new wxStaticText( m_mainPanel, wxID_ANY, _("Line that will be added:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( -1 );
	bSizer9->Add( m_staticText4, 0, wxTOP|wxRIGHT, 5 );
	
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxHORIZONTAL );
	
	m_textCtrlLineToAdd = new wxTextCtrl( m_mainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer6->Add( m_textCtrlLineToAdd, 1, wxEXPAND|wxTOP|wxBOTTOM, 5 );
	
	m_buttonClearCachedPaths = new wxButton( m_mainPanel, wxID_ANY, _("Clear Cached Paths"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer6->Add( m_buttonClearCachedPaths, 0, wxALL, 5 );
	
	bSizer9->Add( bSizer6, 0, wxEXPAND, 5 );
	
	m_staticText5 = new wxStaticText( m_mainPanel, wxID_ANY, _("File's full path:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText5->Wrap( -1 );
	bSizer9->Add( m_staticText5, 0, wxTOP|wxRIGHT, 5 );
	
	m_textCtrlFullPath = new wxTextCtrl( m_mainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	bSizer9->Add( m_textCtrlFullPath, 0, wxEXPAND|wxTOP|wxBOTTOM, 5 );
	
	m_staticText6 = new wxStaticText( m_mainPanel, wxID_ANY, _("Preview:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText6->Wrap( -1 );
	bSizer9->Add( m_staticText6, 0, wxTOP|wxRIGHT, 5 );
	
	wxBoxSizer* bSizer12;
	bSizer12 = new wxBoxSizer( wxHORIZONTAL );
	
	m_textCtrlPreview = new wxScintilla( m_mainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, wxEmptyString );
	m_textCtrlPreview->SetUseTabs( true );
	m_textCtrlPreview->SetTabWidth( 4 );
	m_textCtrlPreview->SetIndent( 4 );
	m_textCtrlPreview->SetTabIndents( true );
	m_textCtrlPreview->SetBackSpaceUnIndents( true );
	m_textCtrlPreview->SetViewEOL( false );
	m_textCtrlPreview->SetViewWhiteSpace( false );
	m_textCtrlPreview->SetMarginWidth( 2, 0 );
	m_textCtrlPreview->SetIndentationGuides( false );
	m_textCtrlPreview->SetMarginWidth( 1, 0 );
	m_textCtrlPreview->SetMarginType( 0, wxSCI_MARGIN_NUMBER );
	m_textCtrlPreview->SetMarginWidth( 0, m_textCtrlPreview->TextWidth( wxSCI_STYLE_LINENUMBER, wxT("_99999") ) );
	m_textCtrlPreview->MarkerDefine( wxSCI_MARKNUM_FOLDER, wxSCI_MARK_BOXPLUS );
	m_textCtrlPreview->MarkerSetBackground( wxSCI_MARKNUM_FOLDER, wxColour( wxT("BLACK") ) );
	m_textCtrlPreview->MarkerSetForeground( wxSCI_MARKNUM_FOLDER, wxColour( wxT("WHITE") ) );
	m_textCtrlPreview->MarkerDefine( wxSCI_MARKNUM_FOLDEROPEN, wxSCI_MARK_BOXMINUS );
	m_textCtrlPreview->MarkerSetBackground( wxSCI_MARKNUM_FOLDEROPEN, wxColour( wxT("BLACK") ) );
	m_textCtrlPreview->MarkerSetForeground( wxSCI_MARKNUM_FOLDEROPEN, wxColour( wxT("WHITE") ) );
	m_textCtrlPreview->MarkerDefine( wxSCI_MARKNUM_FOLDERSUB, wxSCI_MARK_EMPTY );
	m_textCtrlPreview->MarkerDefine( wxSCI_MARKNUM_FOLDEREND, wxSCI_MARK_BOXPLUS );
	m_textCtrlPreview->MarkerSetBackground( wxSCI_MARKNUM_FOLDEREND, wxColour( wxT("BLACK") ) );
	m_textCtrlPreview->MarkerSetForeground( wxSCI_MARKNUM_FOLDEREND, wxColour( wxT("WHITE") ) );
	m_textCtrlPreview->MarkerDefine( wxSCI_MARKNUM_FOLDEROPENMID, wxSCI_MARK_BOXMINUS );
	m_textCtrlPreview->MarkerSetBackground( wxSCI_MARKNUM_FOLDEROPENMID, wxColour( wxT("BLACK") ) );
	m_textCtrlPreview->MarkerSetForeground( wxSCI_MARKNUM_FOLDEROPENMID, wxColour( wxT("WHITE") ) );
	m_textCtrlPreview->MarkerDefine( wxSCI_MARKNUM_FOLDERMIDTAIL, wxSCI_MARK_EMPTY );
	m_textCtrlPreview->MarkerDefine( wxSCI_MARKNUM_FOLDERTAIL, wxSCI_MARK_EMPTY );
	m_textCtrlPreview->SetSelBackground( true, wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHT ) );
	m_textCtrlPreview->SetSelForeground( true, wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHTTEXT ) );
	bSizer12->Add( m_textCtrlPreview, 1, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* bSizer11;
	bSizer11 = new wxBoxSizer( wxVERTICAL );
	
	m_buttonUp = new wxButton( m_mainPanel, wxID_ANY, _("&Up"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer11->Add( m_buttonUp, 0, wxTOP|wxBOTTOM|wxLEFT, 5 );
	
	m_buttonDown = new wxButton( m_mainPanel, wxID_ANY, _("&Down"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer11->Add( m_buttonDown, 0, wxTOP|wxBOTTOM|wxLEFT, 5 );
	
	bSizer12->Add( bSizer11, 0, wxEXPAND, 0 );
	
	bSizer9->Add( bSizer12, 1, wxEXPAND, 5 );
	
	m_mainPanel->SetSizer( bSizer9 );
	m_mainPanel->Layout();
	bSizer9->Fit( m_mainPanel );
	bSizer7->Add( m_mainPanel, 1, wxEXPAND | wxALL, 5 );
	
	m_staticline5 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer7->Add( m_staticline5, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxHORIZONTAL );
	
	m_buttonOK = new wxButton( this, wxID_OK, _("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonOK->SetDefault(); 
	bSizer8->Add( m_buttonOK, 0, wxALL, 5 );
	
	m_buttonCancel = new wxButton( this, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer8->Add( m_buttonCancel, 0, wxALL, 5 );
	
	bSizer7->Add( bSizer8, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	this->SetSizer( bSizer7 );
	this->Layout();
	
	// Connect Events
	m_textCtrlLineToAdd->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( AddIncludeFileDlgBase::OnTextUpdated ), NULL, this );
	m_buttonClearCachedPaths->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( AddIncludeFileDlgBase::OnClearCachedPaths ), NULL, this );
	m_buttonUp->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( AddIncludeFileDlgBase::OnButtonUp ), NULL, this );
	m_buttonDown->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( AddIncludeFileDlgBase::OnButtonDown ), NULL, this );
	m_buttonOK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( AddIncludeFileDlgBase::OnButtonOK ), NULL, this );
}

AddIncludeFileDlgBase::~AddIncludeFileDlgBase()
{
	// Disconnect Events
	m_textCtrlLineToAdd->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( AddIncludeFileDlgBase::OnTextUpdated ), NULL, this );
	m_buttonClearCachedPaths->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( AddIncludeFileDlgBase::OnClearCachedPaths ), NULL, this );
	m_buttonUp->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( AddIncludeFileDlgBase::OnButtonUp ), NULL, this );
	m_buttonDown->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( AddIncludeFileDlgBase::OnButtonDown ), NULL, this );
	m_buttonOK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( AddIncludeFileDlgBase::OnButtonOK ), NULL, this );
	
}
