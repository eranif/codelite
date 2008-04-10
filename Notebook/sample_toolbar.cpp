///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Mar 19 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "sample_toolbar.h"

///////////////////////////////////////////////////////////////////////////

MyToolbar::MyToolbar( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxString m_choice1Choices[] = { wxT("Choice1"), wxT("Choice2"), wxT("Choice3") };
	int m_choice1NChoices = sizeof( m_choice1Choices ) / sizeof( wxString );
	m_choice1 = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice1NChoices, m_choice1Choices, 0 );
	m_choice1->SetSelection( 0 );
	fgSizer1->Add( m_choice1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_toolBar1 = new wxToolBar( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT|wxTB_HORIZONTAL|wxTB_NODIVIDER );
	m_toolBar1->SetToolBitmapSize( wxSize( 24,24 ) );
	m_toolBar1->AddTool( wxID_ANY, wxT("Tip One"), wxBitmap( wxT("../bookmark.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString );
	m_toolBar1->AddTool( wxID_ANY, wxT("Tip Two"), wxBitmap( wxT("../bookmark.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString );
	m_toolBar1->AddTool( wxID_ANY, wxT("tool"), wxBitmap( wxT("../bookmark.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString );
	m_toolBar1->AddTool( wxID_ANY, wxT("tool"), wxBitmap( wxT("../bookmark.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString );
	m_toolBar1->AddTool( wxID_ANY, wxT("tool"), wxBitmap( wxT("../bookmark.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString );
	m_toolBar1->AddTool( wxID_ANY, wxT("tool"), wxBitmap( wxT("../bookmark.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString );
	m_toolBar1->Realize();
	
	fgSizer1->Add( m_toolBar1, 1, wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
	
	this->SetSizer( fgSizer1 );
	this->Layout();
}

MyToolbar::~MyToolbar()
{
}
