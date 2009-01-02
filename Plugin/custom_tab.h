//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : custom_tab.h
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
#ifndef __CustomTab__
#define __CustomTab__

#include "wx/dc.h"
#include "wx/bitmap.h"
#include "wx/panel.h"

extern const wxEventType wxEVT_CMD_DELETE_TAB;

enum XState {
	XNone,
	XPushed
};

class wxWindow;
class wxTabContainer;

class CustomTab : public wxPanel
{

	wxString m_text;
	wxString m_tooltip;
	wxBitmap m_bmp;
	bool m_selected;
	int m_padding;
	int m_heightPadding;
	int m_orientation;
	wxWindow *m_window;
	bool m_leftDown;
	bool m_hovered;
	wxRect m_xButtonRect;
	wxBitmap m_xButtonNormalBmp;
	wxBitmap m_xButtonPressedBmp;
	long m_style;
	XState m_x_state;
	int m_x_padding;

protected:
	int CalcTabHeight();
	int CalcTabWidth();

	void DoDrawVerticalTab(wxDC &dc);
	void DoDrawHorizontalTab(wxDC &dc);
	const wxBitmap &GetXBmp();
	void Initialize();

	wxTabContainer *GetTabContainer();

public:
	CustomTab(wxWindow *win, wxWindowID id, const wxString &text, const wxString &tooltip, const wxBitmap &bmp = wxNullBitmap, bool selected = false, int orientation = wxLEFT, long style=0);
	virtual ~CustomTab();

	//Getters
	void SetPadding(const int& padding) {
		this->m_padding = padding;
	}
	void SetOrientation(const int& orientation) {
		this->m_orientation = orientation;
	}
	void SetWindow(wxWindow* window) {
		this->m_window = window;
	}
	void SetSelected(bool selected) ;
	void SetText(const wxString &text);

	//Getters
	const wxBitmap& GetBmp() const {
		return m_bmp;
	}
	const bool& GetSelected() const {
		return m_selected;
	}
	const wxString& GetText() const {
		return m_text;
	}
	const int& GetOrientation() const {
		return m_orientation;
	}
	const int& GetPadding() const {
		return m_padding;
	}
	wxWindow* GetWindow() const {
		return m_window;
	}

	void SetHeightPadding(const int& heightPadding) {
		this->m_heightPadding = heightPadding;
	}
	const int& GetHeightPadding() const {
		return m_heightPadding;
	}

#if defined (__WXGTK__)
	bool AvoidRepeatSwaps(wxWindow* win, const wxPoint& pt) const;
#endif //defined (__WXGTK__)

	//Setters
	void SetXPadding(const int& x_padding) {
		this->m_x_padding = x_padding;
	}
	const int& GetXPadding() const {
		return m_x_padding;
	}

	DECLARE_EVENT_TABLE()

	//Event handlers
	virtual void OnPaint(wxPaintEvent &event);
	virtual void OnErase(wxEraseEvent &event);
	virtual void OnLeftDown(wxMouseEvent &e);
	virtual void OnMouseEnterWindow(wxMouseEvent &e);
	virtual void OnMouseLeaveWindow(wxMouseEvent &e);
	virtual void OnMouseMove(wxMouseEvent &e);
	virtual void OnLeftUp(wxMouseEvent &e);
	virtual void OnMouseMiddleButton(wxMouseEvent &e);
	virtual void OnRightDown(wxMouseEvent &e);
};
#endif // __CustomTab__
