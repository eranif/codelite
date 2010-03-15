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
#include "wx/image.h"
#include "notebook_ex.h"
#include "drawingutils.h"
#include "windowstack.h"
#include "wx/menu.h"
#include "wx/settings.h"
#include "wx/dcbuffer.h"
#include "dropbutton.h"

#define PNAEL_BG_COLOUR DrawingUtils::GetPanelBgColour()

static unsigned char list_bits[] = {
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0x0f, 0xf8, 0xff, 0xff, 0x0f, 0xf8, 0x1f, 0xfc, 0x3f, 0xfe, 0x7f, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

BEGIN_EVENT_TABLE(DropButtonBase, wxPanel)
	EVT_LEFT_DOWN(DropButtonBase::OnLeftDown)
	EVT_PAINT(DropButtonBase::OnPaint)
	EVT_ERASE_BACKGROUND(DropButtonBase::OnEraseBg)
END_EVENT_TABLE()

DropButtonBase::DropButtonBase(wxWindow *parent)
		: wxPanel(parent)
		, m_state (BTN_NONE)
{
	SetSizeHints(16, 16);

	//create a drop down arrow image
	wxColour color(*wxBLACK);
	wxImage img = wxBitmap((const char*)list_bits, 16, 16).ConvertToImage();
	img.Replace(0, 0, 0, 123, 123, 123);
	img.Replace(255, 255, 255, color.Red(), color.Green(), color.Blue());
	img.SetMaskColour(123, 123, 123);
	m_arrowDownBmp = wxBitmap(img);
}

DropButtonBase::~DropButtonBase()
{
}

void DropButtonBase::OnLeftDown(wxMouseEvent &e)
{
	size_t count = GetItemCount();
	if (count == 0) {
		return;
	}

	wxRect rr = GetSize();
	wxMenu popupMenu;

#ifdef __WXMSW__
	wxFont font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
#endif

	for (size_t i=0; i<count; i++) {
		wxString text = GetItem(i);
		bool selected = IsItemSelected(i);

		wxMenuItem *item = new wxMenuItem(&popupMenu, static_cast<int>(i), text, text, wxITEM_CHECK);

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
	popupMenu.Connect(wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(DropButtonBase::OnMenuSelection), NULL, this);

	m_state = BTN_PUSHED;
	Refresh();
	PopupMenu( &popupMenu, 0, rr.y + rr.height );

	m_state = BTN_NONE;
	Refresh();
}

void DropButtonBase::OnPaint(wxPaintEvent &e)
{
	wxUnusedVar(e);

	wxRect rr = GetSize();

	wxBufferedPaintDC dc(this);
	wxColour lightColour  = PNAEL_BG_COLOUR;
	dc.SetPen(lightColour);
	dc.SetBrush(lightColour);
	dc.DrawRectangle(rr);

	if (IsEnabled() && GetItemCount() > 0) {
		dc.DrawBitmap(m_arrowDownBmp, 0, 0, true);
	}
}

//-----------------------------------------------------------------------------------

StackButton::StackButton(wxWindow* parent, WindowStack* windowStack)
		: DropButtonBase(parent)
		, m_windowStack(windowStack)
{
}



StackButton::~StackButton()
{
}

size_t StackButton::GetItemCount()
{
	m_windowKeys.clear();
	if (m_windowStack) {
		m_windowStack->GetKeys(m_windowKeys);
	}
	return m_windowKeys.size();
}

wxString StackButton::GetItem(size_t n)
{
	return m_windowKeys[n];
}

void StackButton::SetWindowStack(WindowStack* windowStack)
{
	m_windowStack = windowStack;
}

bool StackButton::IsItemSelected(size_t n)
{
	return m_windowKeys[n] == m_windowStack->GetSelectedKey();
}

void StackButton::OnMenuSelection(wxCommandEvent& e)
{
	m_windowStack->Select(m_windowKeys[e.GetId()]);
}
