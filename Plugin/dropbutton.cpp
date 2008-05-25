//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : dropbutton.cpp              
//                                                                          
// -------------------------------------------------------------------------
// A                                                                        
//              _____           _      _     _ _                            
//             /  __ \         | |    | |   (_) |                           
//             | /  \/ ___   __| | ___| |    _| |_ ___                      
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ \                     
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
 #include "wx/image.h"
#include "drawingutils.h"
#include "custom_tab.h"
#include "wx/menu.h"
#include "wx/settings.h"
#include "wx/dcbuffer.h"
#include "dropbutton.h"
#include "custom_tabcontainer.h"

static unsigned char list_bits[] = {
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0x0f, 0xf8, 0xff, 0xff, 0x0f, 0xf8, 0x1f, 0xfc, 0x3f, 0xfe, 0x7f, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
   
BEGIN_EVENT_TABLE(DropButton, wxPanel)
	EVT_LEFT_DOWN(DropButton::OnLeftDown)
	EVT_LEFT_UP(DropButton::OnLeftUp)
	EVT_PAINT(DropButton::OnPaint)
	EVT_ERASE_BACKGROUND(DropButton::OnEraseBg)
END_EVENT_TABLE()

DropButton::DropButton(wxWindow *parent, wxTabContainer *tabContainer)
		: wxPanel(parent)
		, m_tabContainer(tabContainer)
		, m_state (BTN_NONE)
{
	SetSizeHints(16, 16);
	
	//create a drop down arrow image
	wxColour color(*wxBLACK);
	wxImage img = wxBitmap((const char*)list_bits, 16, 16).ConvertToImage();
    img.Replace(0, 0, 0, 123, 123, 123);
    img.Replace(255,255,255,color.Red(),color.Green(),color.Blue());
    img.SetMaskColour(123, 123, 123);
	m_arrowDownBmp = wxBitmap(img);
}

DropButton::~DropButton()
{
}

void DropButton::OnLeftDown(wxMouseEvent &e)
{
	size_t count = m_tabContainer->GetTabsCount();
	if (count == 0) {
		return;
	}

	wxRect rr = GetSize();
	wxMenu popupMenu;

#ifdef __WXMSW__
	wxFont font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
#endif

	for (size_t i=0; i<count; i++) {
		CustomTab *tab = m_tabContainer->IndexToTab(i);
		bool selected( false );

		if (m_tabContainer->GetSelection() == tab) {
			selected = true;
		}

		wxMenuItem *item = new wxMenuItem(&popupMenu, static_cast<int>(i), tab->GetText(), tab->GetText(), wxITEM_CHECK);
		
		//set the font
#ifdef __WXMSW__
		if (selected) {
			font.SetWeight(wxBOLD);
		}
		item->SetFont(font);
#endif
		popupMenu.Append( item );

		//mark the selected item
		item->Check(selected);

		//restore font
#ifdef __WXMSW__
		font.SetWeight(wxNORMAL);
#endif
}

	// connect an event handler to our menu
	popupMenu.Connect(wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(DropButton::OnMenuSelection), NULL, this);

	m_state = BTN_PUSHED;
	Refresh();
	PopupMenu( &popupMenu, 0, rr.y + rr.height );
	
	m_state = BTN_NONE;
	Refresh();
}

void DropButton::OnLeftUp(wxMouseEvent &e)
{
}

void DropButton::OnEraseBg(wxEraseEvent &e)
{
	//do noting
}

void DropButton::OnPaint(wxPaintEvent &e)
{
	wxUnusedVar(e);
	wxBufferedPaintDC dc(this);

	wxRect rr = GetSize();

	wxColour darkColour  = DrawingUtils::LightColour( wxSystemSettings::GetColour(wxSYS_COLOUR_3DSHADOW), 30 );
	wxColour borderColour  = wxSystemSettings::GetColour(wxSYS_COLOUR_3DSHADOW);
	wxColour lightColour  = wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);
	
	wxRect r1(rr.x, rr.y, rr.width, rr.height/2);
	wxRect r2(rr.x, rr.y+rr.height/2, rr.width, rr.height/2);
	
	dc.SetPen(lightColour);
	dc.SetBrush(lightColour);
	dc.DrawRectangle(rr);
	
	if(m_tabContainer->GetTabsCount() == 0) {
		return;
	}
	
	//draw the arrow bitmp
	dc.DrawBitmap(m_arrowDownBmp, 0, 0, true);
}

void DropButton::OnMenuSelection(wxCommandEvent &e)
{
	size_t item = (size_t)e.GetId();

	CustomTab *tab = m_tabContainer->IndexToTab(item);
	m_tabContainer->SetSelection(tab, true);
}
