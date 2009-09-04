///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "editoroptionsgeneralguidespanelbase.h"

///////////////////////////////////////////////////////////////////////////

EditorOptionsGeneralGuidesPanelBase::EditorOptionsGeneralGuidesPanelBase( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer1->AddGrowableCol( 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_displayLineNumbers = new wxCheckBox( this, wxID_ANY, _("Display line numbers"), wxDefaultPosition, wxDefaultSize, 0 );
	
	fgSizer1->Add( m_displayLineNumbers, 0, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_showIndentationGuideLines = new wxCheckBox( this, wxID_ANY, _("Show indentation guidelines"), wxDefaultPosition, wxDefaultSize, 0 );
	
	fgSizer1->Add( m_showIndentationGuideLines, 0, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_checkBoxMatchBraces = new wxCheckBox( this, wxID_ANY, _("Highlight matched braces"), wxDefaultPosition, wxDefaultSize, 0 );
	
	fgSizer1->Add( m_checkBoxMatchBraces, 0, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_checkBoxAutoCompleteBraces = new wxCheckBox( this, wxID_ANY, _("Auto Complete Braces"), wxDefaultPosition, wxDefaultSize, 0 );
	
	fgSizer1->Add( m_checkBoxAutoCompleteBraces, 0, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_highlightCaretLine = new wxCheckBox( this, wxID_ANY, _("Highlight caret line"), wxDefaultPosition, wxDefaultSize, 0 );
	
	fgSizer1->Add( m_highlightCaretLine, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
	
	m_checkBoxAdjustScrollbarSize = new wxCheckBox( this, wxID_ANY, _("Auto adjust editor's horizontal scrollbar width"), wxDefaultPosition, wxDefaultSize, 0 );
	
	fgSizer1->Add( m_checkBoxAdjustScrollbarSize, 0, wxALL, 5 );
	
	m_checkBoxHideChangeMarkerMargin = new wxCheckBox( this, wxID_ANY, _("Hide change marker margin"), wxDefaultPosition, wxDefaultSize, 0 );
	
	fgSizer1->Add( m_checkBoxHideChangeMarkerMargin, 0, wxALL, 5 );
	
	
	fgSizer1->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_staticText1 = new wxStaticText( this, wxID_ANY, _("Caret line backgound color:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	fgSizer1->Add( m_staticText1, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxEXPAND, 5 );
	
	m_caretLineColourPicker = new wxColourPickerCtrl( this, wxID_ANY, *wxBLACK, wxDefaultPosition, wxDefaultSize, wxCLRP_DEFAULT_STYLE|wxCLRP_SHOW_LABEL );
	fgSizer1->Add( m_caretLineColourPicker, 0, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_staticText2 = new wxStaticText( this, wxID_ANY, _("Whitespace visibility:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	fgSizer1->Add( m_staticText2, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxEXPAND, 5 );
	
	wxString m_whitespaceStyleChoices[] = { _("Invisible"), _("Visible always"), _("Visible after indentation"), _("Indentation only") };
	int m_whitespaceStyleNChoices = sizeof( m_whitespaceStyleChoices ) / sizeof( wxString );
	m_whitespaceStyle = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_whitespaceStyleNChoices, m_whitespaceStyleChoices, 0 );
	m_whitespaceStyle->SetSelection( 0 );
	fgSizer1->Add( m_whitespaceStyle, 0, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_staticText4 = new wxStaticText( this, wxID_ANY, _("EOL Mode:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( -1 );
	m_staticText4->SetToolTip( _("Set the editor's EOL mode (End Of Line)") );
	
	fgSizer1->Add( m_staticText4, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
	
	wxString m_choiceEOLChoices[] = { _("Default"), _("Mac (CR)"), _("Windows (CRLF)"), _("Unix (LF)") };
	int m_choiceEOLNChoices = sizeof( m_choiceEOLChoices ) / sizeof( wxString );
	m_choiceEOL = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceEOLNChoices, m_choiceEOLChoices, 0 );
	m_choiceEOL->SetSelection( 0 );
	m_choiceEOL->SetToolTip( _("Set the editor's EOL mode (End Of Line). When set to 'Default' CodeLite will set the EOL according to the hosting OS") );
	
	fgSizer1->Add( m_choiceEOL, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
	
	bSizer1->Add( fgSizer1, 1, wxALL|wxEXPAND, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	bSizer1->Fit( this );
	
	// Connect Events
	m_highlightCaretLine->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( EditorOptionsGeneralGuidesPanelBase::OnHighlightCaretLine ), NULL, this );
}

EditorOptionsGeneralGuidesPanelBase::~EditorOptionsGeneralGuidesPanelBase()
{
	// Disconnect Events
	m_highlightCaretLine->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( EditorOptionsGeneralGuidesPanelBase::OnHighlightCaretLine ), NULL, this );
}
