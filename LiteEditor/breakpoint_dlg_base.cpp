///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jul 28 2007)
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

#include "breakpoint_dlg_base.h"

///////////////////////////////////////////////////////////////////////////

BreakpointDlgBase::BreakpointDlgBase( wxWindow* parent, int id, wxPoint pos, wxSize size, int style ) 
: wxPanel( parent, id, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer16;
	bSizer16 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer19;
	bSizer19 = new wxBoxSizer( wxHORIZONTAL );
	
	m_listBreakpoints = new wxListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	bSizer19->Add( m_listBreakpoints, 1, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer20;
	bSizer20 = new wxBoxSizer( wxVERTICAL );
	
	m_buttonDelete = new wxButton( this, wxID_ANY, wxT("&Delete"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer20->Add( m_buttonDelete, 0, wxALL, 5 );
	
	m_buttonDeleteAll = new wxButton( this, wxID_ANY, wxT("Delete &All"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer20->Add( m_buttonDeleteAll, 0, wxALL, 5 );
	
	bSizer19->Add( bSizer20, 0, wxEXPAND, 5 );
	bSizer16->Add( bSizer19, 1, wxEXPAND, 5 );
	
	this->SetSizer( bSizer16 );
	this->Layout();
}
