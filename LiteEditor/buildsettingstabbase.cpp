///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun 30 2011)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "buildsettingstabbase.h"

///////////////////////////////////////////////////////////////////////////

BuildTabSettingsBase::BuildTabSettingsBase( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 3, 3, 0, 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->AddGrowableCol( 2 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_ALL );
	
	m_staticText3 = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	fgSizer1->Add( m_staticText3, 0, wxALL, 5 );
	
	m_staticText5 = new wxStaticText( this, wxID_ANY, _("Foreground colour:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText5->Wrap( -1 );
	m_staticText5->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString ) );
	
	fgSizer1->Add( m_staticText5, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_staticText6 = new wxStaticText( this, wxID_ANY, _("Font weight:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText6->Wrap( -1 );
	m_staticText6->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString ) );
	
	fgSizer1->Add( m_staticText6, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_staticText1 = new wxStaticText( this, wxID_ANY, _("Compiler errors colour:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	fgSizer1->Add( m_staticText1, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_colourPickerErrorFg = new wxColourPickerCtrl( this, wxID_ANY, *wxBLACK, wxDefaultPosition, wxDefaultSize, wxCLRP_DEFAULT_STYLE );
	fgSizer1->Add( m_colourPickerErrorFg, 0, wxALL|wxEXPAND, 5 );
	
	m_checkBoxBoldErrFont = new wxCheckBox( this, wxID_ANY, _("Bold"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkBoxBoldErrFont->SetValue(true); 
	fgSizer1->Add( m_checkBoxBoldErrFont, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_staticText2 = new wxStaticText( this, wxID_ANY, _("Compiler warnings colour:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	m_staticText2->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 90, false, wxEmptyString ) );
	
	fgSizer1->Add( m_staticText2, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_colourPickerWarningsFg = new wxColourPickerCtrl( this, wxID_ANY, *wxBLACK, wxDefaultPosition, wxDefaultSize, wxCLRP_DEFAULT_STYLE );
	fgSizer1->Add( m_colourPickerWarningsFg, 0, wxALL|wxEXPAND, 5 );
	
	m_checkBoxBoldWarnFont = new wxCheckBox( this, wxID_ANY, _("Bold"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_checkBoxBoldWarnFont, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	bSizer1->Add( fgSizer1, 0, wxALL|wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer2;
	sbSizer2 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("General:") ), wxVERTICAL );
	
	m_checkBoxSkipWarnings = new wxCheckBox( this, wxID_ANY, _("When using the menu to jump to errors, skip warnings"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer2->Add( m_checkBoxSkipWarnings, 0, wxALL|wxEXPAND, 5 );
	
	m_checkBoxAutoHide = new wxCheckBox( this, wxID_ANY, _("Automatically hide the build pane when there are neither errors nor warnings"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer2->Add( m_checkBoxAutoHide, 0, wxALL|wxEXPAND, 5 );
	
	wxString m_radioBuildPaneScrollDestinationChoices[] = { _("The first error"), _("The first warning or error"), _("The end") };
	int m_radioBuildPaneScrollDestinationNChoices = sizeof( m_radioBuildPaneScrollDestinationChoices ) / sizeof( wxString );
	m_radioBuildPaneScrollDestination = new wxRadioBox( this, wxID_ANY, _("After build finishes, if showing the build pane scroll to:"), wxDefaultPosition, wxDefaultSize, m_radioBuildPaneScrollDestinationNChoices, m_radioBuildPaneScrollDestinationChoices, 1, wxRA_SPECIFY_ROWS );
	m_radioBuildPaneScrollDestination->SetSelection( 0 );
	sbSizer2->Add( m_radioBuildPaneScrollDestination, 0, wxALL|wxEXPAND, 5 );
	
	bSizer1->Add( sbSizer2, 0, wxEXPAND|wxALL, 5 );
	
	wxString m_radioBoxShowBuildTabChoices[] = { _("When build starts"), _("When build ends"), _("Don't automatically show") };
	int m_radioBoxShowBuildTabNChoices = sizeof( m_radioBoxShowBuildTabChoices ) / sizeof( wxString );
	m_radioBoxShowBuildTab = new wxRadioBox( this, wxID_ANY, _("Show build pane:"), wxDefaultPosition, wxDefaultSize, m_radioBoxShowBuildTabNChoices, m_radioBoxShowBuildTabChoices, 1, wxRA_SPECIFY_COLS );
	m_radioBoxShowBuildTab->SetSelection( 0 );
	bSizer1->Add( m_radioBoxShowBuildTab, 0, wxALL|wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer1;
	sbSizer1 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Errors / warnings markers:") ), wxVERTICAL );
	
	m_checkBoxDisplayMarkers = new wxCheckBox( this, wxID_ANY, _("Compiler errors / warnings marked with bookmarks"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer1->Add( m_checkBoxDisplayMarkers, 0, wxALL, 5 );
	
	bSizer1->Add( sbSizer1, 0, wxEXPAND|wxALL, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	bSizer1->Fit( this );
}

BuildTabSettingsBase::~BuildTabSettingsBase()
{
}
