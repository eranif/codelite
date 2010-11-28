///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Nov 18 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "editorsettingsbookmarksbasepanel.h"

///////////////////////////////////////////////////////////////////////////

EditorSettingsBookmarksBasePanel::EditorSettingsBookmarksBasePanel( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );
	
	m_displaySelection = new wxCheckBox( this, wxID_ANY, _("Display Breakpoints / Bookmarks margin"), wxDefaultPosition, wxDefaultSize, 0 );
	m_displaySelection->SetValue(true); 
	bSizer2->Add( m_displaySelection, 0, wxALL, 5 );
	
	m_staticText3 = new wxStaticText( this, wxID_ANY, _("Bookmark Shape:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	bSizer2->Add( m_staticText3, 0, wxALL, 5 );
	
	wxString m_bookMarkShapeChoices[] = { _("Small Rectangle"), _("Rounded Rectangle"), _("Circle"), _("Small Arrow") };
	int m_bookMarkShapeNChoices = sizeof( m_bookMarkShapeChoices ) / sizeof( wxString );
	m_bookMarkShape = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_bookMarkShapeNChoices, m_bookMarkShapeChoices, 0 );
	m_bookMarkShape->SetSelection( 0 );
	bSizer2->Add( m_bookMarkShape, 0, wxALL|wxEXPAND, 5 );
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer1->AddGrowableCol( 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText4 = new wxStaticText( this, wxID_ANY, _("Select the bookmark background colour:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( -1 );
	fgSizer1->Add( m_staticText4, 0, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_backgroundColor = new wxColourPickerCtrl( this, wxID_ANY, *wxBLACK, wxDefaultPosition, wxDefaultSize, wxCLRP_DEFAULT_STYLE );
	fgSizer1->Add( m_backgroundColor, 0, wxALIGN_RIGHT|wxALL|wxEXPAND, 5 );
	
	m_staticText5 = new wxStaticText( this, wxID_ANY, _("Select the bookmark foreground colour:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText5->Wrap( -1 );
	fgSizer1->Add( m_staticText5, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxEXPAND, 5 );
	
	m_foregroundColor = new wxColourPickerCtrl( this, wxID_ANY, *wxBLACK, wxDefaultPosition, wxDefaultSize, wxCLRP_DEFAULT_STYLE );
	fgSizer1->Add( m_foregroundColor, 0, wxALIGN_RIGHT|wxALL|wxEXPAND, 5 );
	
	m_staticText6 = new wxStaticText( this, wxID_ANY, _("Select word highlight colour:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText6->Wrap( -1 );
	fgSizer1->Add( m_staticText6, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxEXPAND, 5 );
	
	m_highlightColor = new wxColourPickerCtrl( this, wxID_ANY, *wxBLACK, wxDefaultPosition, wxDefaultSize, wxCLRP_DEFAULT_STYLE );
	fgSizer1->Add( m_highlightColor, 0, wxALIGN_RIGHT|wxALL|wxEXPAND, 5 );
	
	bSizer2->Add( fgSizer1, 0, wxALL|wxEXPAND, 5 );
	
	this->SetSizer( bSizer2 );
	this->Layout();
}

EditorSettingsBookmarksBasePanel::~EditorSettingsBookmarksBasePanel()
{
}
