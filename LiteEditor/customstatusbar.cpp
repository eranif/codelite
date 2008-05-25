//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : customstatusbar.cpp              
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
 #include "progressctrl.h"
#include "customstatusbar.h"

BEGIN_EVENT_TABLE(CustomStatusBar, wxStatusBar)
EVT_SIZE(CustomStatusBar::OnSize)
END_EVENT_TABLE()

CustomStatusBar::CustomStatusBar(wxWindow *parent, wxWindowID id)
: wxStatusBar(parent, id)
{
	SetFieldsCount(5);
	m_gauge = new ProgressCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
	m_gauge->SetMaxRange(200);
}

CustomStatusBar::~CustomStatusBar()
{
}

void CustomStatusBar::OnSize(wxSizeEvent& event)
{
	if ( !m_gauge ){
		event.Skip();
		return;
	}

	wxRect rect;
	GetFieldRect(4, rect);
	m_gauge->SetSize(rect.x+1, rect.y+1, rect.width-2, rect.height-2);
	event.Skip();
}

void CustomStatusBar::ResetGauge(int range)
{
	m_gauge->SetMaxRange((size_t)range);
	m_gauge->Update(0, wxEmptyString);
}

void CustomStatusBar::Update(int value, const wxString& message)
{
	m_gauge->Update((size_t)value, message);
}
