//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : clprogressdlgbase.cpp
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Mar 22 2011)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "clprogressdlgbase.h"
#if wxUSE_GUI
///////////////////////////////////////////////////////////////////////////

clProgressDlgBase::clProgressDlgBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxSize( 400,-1 ), wxDefaultSize );
	
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	m_staticLabel = new wxStaticText( this, wxID_ANY, _("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticLabel->Wrap( -1 );
	mainSizer->Add( m_staticLabel, 0, wxALL|wxEXPAND, 5 );
	
	m_gauge = new wxGauge( this, wxID_ANY, 100, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL );
	m_gauge->SetMinSize( wxSize( 400,-1 ) );
	
	mainSizer->Add( m_gauge, 0, wxEXPAND|wxALL, 5 );
	
	this->SetSizer( mainSizer );
	this->Layout();
	
	this->Centre( wxBOTH );
}

clProgressDlgBase::~clProgressDlgBase()
{
}
#endif // #if wxUSE_GUI