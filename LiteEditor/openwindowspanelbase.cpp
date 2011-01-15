///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "openwindowspanelbase.h"

///////////////////////////////////////////////////////////////////////////

OpenWindowsPanelBase::OpenWindowsPanelBase( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	m_fileList = new wxListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_EXTENDED|wxLB_SORT ); 
	mainSizer->Add( m_fileList, 1, wxALL|wxEXPAND, 0 );
	
	this->SetSizer( mainSizer );
	this->Layout();
	
	// Connect Events
	m_fileList->Connect( wxEVT_CHAR, wxKeyEventHandler( OpenWindowsPanelBase::OnChar ), NULL, this );
	m_fileList->Connect( wxEVT_KEY_DOWN, wxKeyEventHandler( OpenWindowsPanelBase::OnKeyDown ), NULL, this );
	m_fileList->Connect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( OpenWindowsPanelBase::OnItemSelected ), NULL, this );
	m_fileList->Connect( wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler( OpenWindowsPanelBase::OnItemDClicked ), NULL, this );
	m_fileList->Connect( wxEVT_RIGHT_DOWN, wxMouseEventHandler( OpenWindowsPanelBase::OnRightDown ), NULL, this );
}

OpenWindowsPanelBase::~OpenWindowsPanelBase()
{
	// Disconnect Events
	m_fileList->Disconnect( wxEVT_CHAR, wxKeyEventHandler( OpenWindowsPanelBase::OnChar ), NULL, this );
	m_fileList->Disconnect( wxEVT_KEY_DOWN, wxKeyEventHandler( OpenWindowsPanelBase::OnKeyDown ), NULL, this );
	m_fileList->Disconnect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( OpenWindowsPanelBase::OnItemSelected ), NULL, this );
	m_fileList->Disconnect( wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler( OpenWindowsPanelBase::OnItemDClicked ), NULL, this );
	m_fileList->Disconnect( wxEVT_RIGHT_DOWN, wxMouseEventHandler( OpenWindowsPanelBase::OnRightDown ), NULL, this );
	
}
