//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : custom_tab.cpp
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
#include <wx/xrc/xmlres.h>
#include "custom_tab.h"
#include "wx/dcbuffer.h"
#include "wx/settings.h"
#include "wx/image.h"
#include "drawingutils.h"
#include "custom_tabcontainer.h"
#include "custom_notebook.h"

// Images
#include "tabicons.h"

#define clMASK_COLOR wxColor(0, 128, 128)

#ifdef __WXMSW__
#    define TAB_RADIUS   3
#    define LIGHT_FACTOR 2
#elif defined(__WXGTK__)
#    define TAB_RADIUS   0
#    define LIGHT_FACTOR 2
#else
#    define TAB_RADIUS   3
#    define LIGHT_FACTOR 0
#endif

BEGIN_EVENT_TABLE(CustomTab, wxPanel)
	EVT_PAINT(CustomTab::OnPaint)
	EVT_ERASE_BACKGROUND(CustomTab::OnErase)
	EVT_LEFT_DOWN(CustomTab::OnLeftDown)
	EVT_ENTER_WINDOW(CustomTab::OnMouseEnterWindow)
	EVT_LEAVE_WINDOW(CustomTab::OnMouseLeaveWindow)
	EVT_LEFT_UP(CustomTab::OnLeftUp)
	EVT_MOTION(CustomTab::OnMouseMove)
	EVT_MIDDLE_DOWN(CustomTab::OnMouseMiddleButton)
	EVT_RIGHT_DOWN(CustomTab::OnRightDown)
END_EVENT_TABLE()

const wxEventType wxEVT_CMD_DELETE_TAB = XRCID("custom_tab_delete");

CustomTab::CustomTab(wxWindow *win, wxWindowID id, const wxString &text, const wxString &tooltip, const wxBitmap &bmp, bool selected, int orientation, long style)
		: wxPanel(win, id, wxDefaultPosition, wxSize(1, 1))
		, m_text(text)
		, m_tooltip(tooltip)
		, m_bmp(bmp)
		, m_selected(selected)
		, m_padding(6)
#ifdef __WXMSW__
		, m_heightPadding(2)
#else
		, m_heightPadding(4)
#endif
		, m_orientation(orientation)
		, m_window(NULL)
		, m_leftDown(false)
		, m_hovered(false)
		, m_x_state(XNone)
		, m_x_padding(3)
		, m_userData(NULL)
{
	Initialize();
	GetParent()->Connect(GetId(), wxEVT_CMD_DELETE_TAB, wxCommandEventHandler(wxTabContainer::OnDeleteTab));
}

CustomTab::~CustomTab()
{
	GetParent()->Disconnect(GetId(), wxEVT_CMD_DELETE_TAB, wxCommandEventHandler(wxTabContainer::OnDeleteTab));
	if(m_userData){
		delete m_userData;
		m_userData = NULL;
	}
}

void CustomTab::OnPaint(wxPaintEvent &event)
{
	wxUnusedVar(event);
	wxBufferedPaintDC dc(this);
	
	if (m_orientation == wxLEFT || m_orientation == wxRIGHT) {
		SetSizeHints(CalcTabWidth(), CalcTabHeight());
		DoDrawVerticalTab(dc);
	} else {
		SetSizeHints(CalcTabHeight(), CalcTabWidth());
		DoDrawHorizontalTab(dc);
	}
}

void CustomTab::OnErase(wxEraseEvent &event)
{
	wxUnusedVar(event);
}

int CustomTab::CalcTabHeight()
{
	if(IsFixedWidthTabs()) {
		return GetTabContainer()->GetFixedTabWidth();
	}

	int tmpTabHeight(0);
	int tabHeight(GetPadding());
	if (GetBmp().IsOk()) {
		tmpTabHeight = GetBmp().GetHeight();
		tmpTabHeight += GetPadding();
	}

	if (GetText().IsEmpty() == false) {
		int xx(0), yy(0);
		wxFont font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
		font.SetWeight(wxBOLD);
		wxWindow::GetTextExtent(GetText(), &xx, &yy, NULL, NULL, &font);

		tmpTabHeight += xx;
		tmpTabHeight += GetPadding();
	}

	if (GetBookStyle() & wxVB_HAS_X) {
		//this control is using x buttons
		tmpTabHeight -= GetXPadding() ;
		tmpTabHeight += 16;
		tmpTabHeight += GetXPadding();
	}

	tabHeight += tmpTabHeight;
	return tabHeight;
}

int CustomTab::CalcTabWidth()
{
	int tmpTabWidth(0);
	int tabWidth(GetHeightPadding()*2);

	tabWidth = GetHeightPadding()+4;
	tmpTabWidth = GetTabContainer()->GetBmpHeight();

	if (GetText().IsEmpty() == false) {
		int xx, yy;
		wxString stam(wxT("Tp"));
		wxFont font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
		font.SetWeight(wxBOLD);
		wxWindow::GetTextExtent(stam, &xx, &yy, NULL, NULL, &font);
		yy > tmpTabWidth ? tmpTabWidth = yy : tmpTabWidth = tmpTabWidth;
	}

	tabWidth += tmpTabWidth;
	return tabWidth;
}

void CustomTab::OnLeftDown(wxMouseEvent &e)
{
	m_leftDown = true;
	wxTabContainer *parent = (wxTabContainer*)GetParent();

	//check if the click was on x button
	wxPoint pt = e.GetPosition();
	if (m_xButtonRect.Contains(pt) && GetSelected()) {
		//click was made inside the x button area
		m_x_state = XPushed;
		Refresh();
		return;
	}

	//notify the parent that this tab has been selected
	if (GetSelected()) {
		//we are already the selected tab nothing to be done here
		return;
	}

	parent->GetParent()->Freeze();
	if (parent) {
		parent->SetSelection(this, true);
	}
	parent->GetParent()->Thaw();
}

void CustomTab::OnMouseEnterWindow(wxMouseEvent &e)
{
	if (e.LeftIsDown()) {
		wxTabContainer *parent = (wxTabContainer*)GetParent();
		if (parent) {
			if (!(GetBookStyle() & wxVB_NODND)) {
				parent->SwapTabs(this);
			}
		}
	}
	m_hovered = true;
	Refresh();
	e.Skip();
}

#if defined (__WXGTK__)
bool CustomTab::AvoidRepeatSwaps(wxWindow* win, const wxPoint& pt) const
{
	// In the wxGTK version of tab DnD doesn't (can't) use wxEVT_ENTER_WINDOW
	// If you swap a short tab with a long one, the pointer ends up inside the old window.
	// This results in a swap-back-and-forth as the mouse moves further along
	// So veto swapping with the last-swapped tab, unless mouse direction has reversed

	static wxWindow* lastwin = NULL;
	// Holds the x or y position of the last mouse-movement event
	static int lastposition = 0;
	// Were we going left/up or right/down last time we swapped
	static bool lastdirection;

	// The bools flag whether current movement is (up or) to the right, or (down) left; & has this reversed
	bool UpOrRight, SameDirection;

	wxTabContainer* parent = (wxTabContainer*)GetParent();
	if (parent->GetOrientation() == wxLEFT || parent->GetOrientation() == wxRIGHT) {
		UpOrRight = pt.y > lastposition;
		lastposition = pt.y;
	} else {
		UpOrRight = pt.x > lastposition;
		lastposition = pt.x;
	}

	SameDirection = (lastdirection==UpOrRight);
	lastdirection = UpOrRight;

	// See if we're still over the last-swapped tab
	if (win != lastwin) {
		// This is a different tab, so we must be about to swap it.
		// Permit this, and store win and direction
		lastwin = win;
		lastdirection = UpOrRight;
		return true;
	} else {
		// Same window. If we're still travelling in the original direction, don't swap back!
		return !SameDirection;
	}
}
#endif //defined (__WXGTK__)


void CustomTab::OnMouseMove(wxMouseEvent &e)
{

#if defined (__WXGTK__)
	// wxGTK doesn't recognise changes of event-window while dragging, so tab DnD fails
	// Work around this using wxFindWindowAtPointer
	if (m_leftDown && !(GetBookStyle() & wxVB_NODND)) {
		wxTabContainer* parent = (wxTabContainer*)GetParent();
		wxPoint pt;
		wxWindow* win = wxFindWindowAtPointer(pt);
		if (win != parent->GetDraggedTab()) {
			CustomTab* tab = static_cast<CustomTab *>(win);
			if (tab && AvoidRepeatSwaps(tab, pt)) {
				parent->SwapTabs(tab);
			}
		}
	}
#endif //defined (__WXGTK__)

	//mark this tab as the dragged one, only if the left down action
	//was taken place on this tab!
	if (e.LeftIsDown() && m_leftDown) {
		//left is down, check to see if the tab is being dragged
		wxTabContainer *parent = (wxTabContainer*)GetParent();
		if (parent) {
			parent->SetDraggedTab(this);
		}
	}
	e.Skip();
}

void CustomTab::OnLeftUp(wxMouseEvent &e)
{
	wxUnusedVar(e);
	m_leftDown = false;
	wxTabContainer *parent = (wxTabContainer*)GetParent();
	if (parent) {
		parent->SetDraggedTab(NULL);
	}

	if (m_x_state == XPushed && m_xButtonRect.Contains(e.GetPosition())) {
		//delete this tab
		wxCommandEvent event(wxEVT_CMD_DELETE_TAB, GetId());
		event.SetEventObject(this);

		parent->AddPendingEvent(event);
		return;
	}

	m_x_state = XNone;
	Refresh();
	e.Skip();
}

void CustomTab::OnMouseLeaveWindow(wxMouseEvent &e)
{
	m_hovered = false;
	m_x_state = XNone;
	Refresh();
	e.Skip();
}

void CustomTab::SetSelected(bool selected)
{
	if (m_selected == selected) {
		return;
	}

	m_selected = selected;
}

void CustomTab::DoDrawVerticalTab(wxDC &dc)
{
	//draw a grey border around the tab
	wxRect rr = GetClientSize();

	//create temporary memory dc and draw the image on top of it
	wxBitmap bmp(rr.GetHeight(), rr.GetWidth());
	wxMemoryDC memDc;
	memDc.SelectObject(bmp);

	//set the default GUI font
	wxFont font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	if(GetSelected()) {
		font.SetWeight(wxBOLD);
	}

	memDc.SetFont(font);
	wxRect bmpRect(0, 0, bmp.GetWidth(), bmp.GetHeight());

	memDc.SetBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
	memDc.SetPen(wxSystemSettings::GetColour  (wxSYS_COLOUR_3DFACE));
	memDc.DrawRectangle(bmpRect);

	//fill the bmp with a nice gradient colour
	bool left = m_orientation == wxLEFT;

	if (left && GetSelected()) {
		wxRect rt(bmpRect);
		rt.y += 1;

		// we want the gradient NOT to be painted on the border,
		// so we reduce the width of the fillRect by 2 pixles
		// also, we need to adjust the x & y coordinates by 1 pixel
		rt.width -= 2;
		rt.x += 1;
		rt.height -= 1;
		memDc.DrawRoundedRectangle(rt, TAB_RADIUS);
		
		wxColour bgTabColour ( wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE) );
		bgTabColour = DrawingUtils::LightColour(bgTabColour, LIGHT_FACTOR);
		wxRect topRect(rt);
		topRect.height /= 3;
		topRect.height *= 2;
		
		DrawingUtils::PaintStraightGradientBox(memDc, topRect, wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE), bgTabColour, true);
		
	} else if ( GetSelected() ) {
		wxRect rt(bmpRect);
		rt.y += 1;
		memDc.DrawRoundedRectangle(rt, TAB_RADIUS);
		
		wxRect topRect(rt);
		wxColour bgTabColour ( wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE) );
		bgTabColour = DrawingUtils::LightColour(bgTabColour, LIGHT_FACTOR);
		
		topRect.height /= 3;
		topRect.height *= 2;
		
		DrawingUtils::PaintStraightGradientBox(memDc, topRect, bgTabColour, wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE), true);

	} else {
		wxRect fillRect(bmpRect);
		fillRect.height -= 3;
		fillRect.y  += 3;

		wxColour bgTabColour ( wxSystemSettings::GetColour(wxSYS_COLOUR_3DSHADOW) );
		bgTabColour = DrawingUtils::LightColour(bgTabColour, LIGHT_FACTOR);

		DrawingUtils::PaintStraightGradientBox( memDc,
												fillRect,
												wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE),
												bgTabColour,
												true);
	}

	int posx(GetPadding());
	if ( GetBmp().IsOk() ) {

		wxCoord imgYCoord = (bmp.GetHeight() - GetBmp().GetHeight())/2;
		memDc.DrawBitmap(GetBmp(), posx, imgYCoord, true);
		posx += GetBmp().GetWidth() + GetPadding();

	}

	//draw the text
	if ( GetText().IsEmpty() == false ) {

		wxString sampleText(wxT("Tp"));
		wxCoord xx, yy;
		memDc.GetTextExtent(sampleText, &xx, &yy);

		wxCoord txtYCoord = (bmp.GetHeight() - yy)/2;

		//make sure the colour used here is the system default
		memDc.SetTextForeground(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));

		wxString truncateText;

		//calcualte the size left for drawing the text
		int x_len = 0;
		if (GetBookStyle() & wxVB_HAS_X) {
			x_len = 16 + GetXPadding();
		}

		int maxTextWidth(0);
		if(IsFixedWidthTabs()) {
			maxTextWidth = GetTabContainer()->GetFixedTabWidth() - posx - x_len- GetPadding();
		} else {
			maxTextWidth = bmp.GetWidth() - posx - x_len- GetPadding();
		}

		//truncate the text if needed
		DrawingUtils::TruncateText(memDc, GetText(), maxTextWidth, truncateText);

		//draw it
		memDc.DrawText(truncateText, posx, txtYCoord);
		posx += maxTextWidth + GetPadding();
	}

	//draw x button if needed
	if (GetBookStyle() & wxVB_HAS_X) {
		wxCoord xBtnYCoord = (bmp.GetHeight() - 16)/2 + 2;

		//draw the x button, only if we are the active tab
		memDc.DrawBitmap(GetXBmp(), posx, xBtnYCoord, true);
		
		if (m_orientation == wxLEFT) {
			m_xButtonRect = wxRect(xBtnYCoord, GetPadding(), 16, 16);
		} else {
			m_xButtonRect = wxRect(xBtnYCoord, posx, 16, 16);
		}
		posx += 16 + GetPadding();
	}

	//apply changes
	memDc.SelectObject(wxNullBitmap);

	//the text and image are now drawn into the bmp
	//rotate it
	wxImage img = bmp.ConvertToImage();
	img = img.Rotate90(!left);

	//and convert it back to wxBitmap
	bmp = wxBitmap(img);

	dc.DrawBitmap(bmp, 0, 0, true);

	//set pen & brush
	dc.SetBrush(*wxTRANSPARENT_BRUSH);

	wxColour borderColour = DrawingUtils::LightColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DDKSHADOW), DrawingUtils::GetDdkShadowLightFactor2());
	wxPen pen(borderColour);
	dc.SetPen(pen);

	wxRect tmpRect(rr);
	if (left && !GetSelected()) {
		tmpRect.x += 3;
		tmpRect.width  -= 3;

		dc.DrawLine(tmpRect.x+tmpRect.width-1, 0, tmpRect.x+tmpRect.width-1, tmpRect.y + tmpRect.height);
	} else if (!left && !GetSelected()) {
		tmpRect.width  -= 3;
		dc.DrawLine(0, 0, 0, tmpRect.y + tmpRect.height);
	}

	int radius(TAB_RADIUS ? TAB_RADIUS : 1);
	if (GetTabContainer()) {
		size_t tabIdx = GetTabContainer()->TabToIndex(this);
		bool drawBottomBorder = (tabIdx == GetTabContainer()->GetFirstVisibleTab() || GetSelected());

		if (!drawBottomBorder) {
			tmpRect.y -= 1;
			tmpRect.height += 1;
		}
	}

	// since we want to draw a full rounded rectangle, but we only want one edge roudned,
	// we adjust the rectangle height and y offset
	tmpRect.width += radius;
	left ? tmpRect.x : tmpRect.x -= radius;

	dc.DrawRoundedRectangle(tmpRect, TAB_RADIUS);

	if (GetSelected()) {
		// draw an inner white rectangle as well
		wxColour innerBorderCol (wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
		innerBorderCol = DrawingUtils::LightColour(innerBorderCol, 3);
		dc.SetPen  ( wxPen(innerBorderCol) );

		tmpRect.Deflate(1);
		dc.DrawRoundedRectangle(tmpRect, TAB_RADIUS);
	}


	if (left && GetSelected()) {
		//draw a line
		dc.SetPen(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
		dc.DrawLine(rr.x + rr.width - 1, 0, rr.x + rr.width - 1, rr.y + rr.height);

	} else if (!left && GetSelected()) {
		//draw a line
		dc.SetPen(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
		dc.DrawLine(0, 0, 0, tmpRect.y + tmpRect.height);
	}
}

void CustomTab::DoDrawHorizontalTab(wxDC &dc)
{
	wxRect rr = GetClientSize();

	//create temporary memory dc and draw the image on top of it
	wxBitmap bmp(rr.GetWidth(), rr.GetHeight());
	wxMemoryDC memDc;
	memDc.SelectObject(bmp);

	//set the default GUI font
	wxFont font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	if(GetSelected()) {
		font.SetWeight(wxBOLD);
	}
	memDc.SetFont(font);

	wxRect bmpRect(0, 0, bmp.GetWidth(), bmp.GetHeight());
	memDc.SetBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
	memDc.SetPen(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
	memDc.DrawRectangle(bmpRect);

	//fill the bmp with a nice gradient colour
	bool top = m_orientation == wxTOP;

	if (top && GetSelected()) {
		wxRect fillRect(bmpRect);
		fillRect.y += 1;
		
		// We want the gradient NOT to be painted on the border,
		// so we reduce the width of the fillRect by 2 pixles
		// also, we need to adjust the x & y coordinates by 1 pixel
		fillRect.width -= 4;
		fillRect.x += 2;
		fillRect.height -= 1;
		memDc.DrawRoundedRectangle(fillRect, TAB_RADIUS);
		
		wxColour bgTabColour ( wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE) );
		bgTabColour = DrawingUtils::LightColour(bgTabColour, LIGHT_FACTOR);
		wxRect topRect(fillRect);
		topRect.height /= 3;
		topRect.height *= 2;
		
		DrawingUtils::PaintStraightGradientBox(memDc, topRect, bgTabColour, wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE), true);
		
	} else if ( GetSelected() ) {
		wxRect fillRect(bmpRect);
		fillRect.height -= 1;
		
		memDc.DrawRoundedRectangle(fillRect, TAB_RADIUS);
		
		wxColour bgTabColour ( wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE) );
		bgTabColour = DrawingUtils::LightColour(bgTabColour, LIGHT_FACTOR);
		wxRect topRect(fillRect);
		
		topRect.height /= 3;
		topRect.height *= 2;
		topRect.y      += (topRect.height/3)+2;
		DrawingUtils::PaintStraightGradientBox(memDc, topRect, wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE), bgTabColour, true);
		
	} else {

		wxRect fillRect(bmpRect);
		fillRect.height -= 3;
		top ? fillRect.y  += 3 : fillRect.y;

		wxColour bgTabColour ( wxSystemSettings::GetColour(wxSYS_COLOUR_3DSHADOW) );
		bgTabColour = DrawingUtils::LightColour(bgTabColour, LIGHT_FACTOR);
		DrawingUtils::PaintStraightGradientBox(memDc, fillRect, wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE), bgTabColour, true);
	}

	int text_yoffset(1);
	int x_yoffset(0);
	int bmp_yoffset(1);

	if ( !top ) {
		text_yoffset = -3;
		bmp_yoffset = -1;
	}

	int posx(GetPadding());
	if ( GetBmp().IsOk() ) {

		wxCoord imgYCoord = (bmp.GetHeight() - GetBmp().GetHeight())/2;
		memDc.DrawBitmap(GetBmp(), posx, imgYCoord + bmp_yoffset, true);
		posx += GetBmp().GetWidth() + GetPadding();

	}

	//draw the text
	if ( GetText().IsEmpty() == false ) {

		wxString sampleText(wxT("Tp"));
		wxCoord xx, yy;
		memDc.GetTextExtent(sampleText, &xx, &yy);


		wxCoord txtYCoord = (bmp.GetHeight() - yy)/2 + text_yoffset;

		// Incase we are drawing the selected tab and the orientation is bottom,
		// draw the text 3 pixles "more down"
		if(!top && GetSelected()){
			txtYCoord += 3;
		} else if(!top) {
			txtYCoord += 1;
		}

		// Make sure the colour used here is the system default
		memDc.SetTextForeground(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));

		wxString truncateText;

		int x_len = 0;
		if (GetBookStyle() & wxVB_HAS_X) {
			x_len = 16 + GetXPadding();
		}

		//calcualte the size left for drawing the text

		int maxTextWidth(0);
		if(IsFixedWidthTabs()) {
			maxTextWidth = GetTabContainer()->GetFixedTabWidth() - posx - x_len- GetPadding();
		} else {
			maxTextWidth = bmp.GetWidth() - posx - x_len- GetPadding();
		}

		//truncate the text if needed
		DrawingUtils::TruncateText(memDc, GetText(), maxTextWidth, truncateText);

		//draw it
		memDc.DrawText(truncateText, posx, txtYCoord);
		posx += maxTextWidth + GetPadding();
	}

	//draw x button if needed
	if (GetBookStyle() & wxVB_HAS_X) {
		//draw the x button, only if we are the active tab
		x_yoffset = (bmp.GetHeight() - GetXBmp().GetHeight())/2;
		top ? x_yoffset += 2 : x_yoffset;
		memDc.DrawBitmap(GetXBmp(), posx, x_yoffset, true);
		
		int xWidth = GetXBmp().GetWidth();
		m_xButtonRect = wxRect(posx, x_yoffset, xWidth, GetXBmp().GetHeight());
		posx += xWidth + GetPadding();
	}

	//apply changes
	memDc.SelectObject(wxNullBitmap);
	dc.DrawBitmap(bmp, 0, 0, true);

	//set pen & brush
	dc.SetBrush(*wxTRANSPARENT_BRUSH);

	wxColour borderColour = DrawingUtils::LightColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DDKSHADOW), DrawingUtils::GetDdkShadowLightFactor2());
	wxPen pen(borderColour);

	dc.SetPen(pen);

	wxRect tmpRect(rr);
	if (top && !GetSelected()) {
		tmpRect.y += 3;
		tmpRect.height  -= 3;

		dc.DrawLine(tmpRect.x, tmpRect.y + tmpRect.height - 1, tmpRect.x+tmpRect.width, tmpRect.y + tmpRect.height -1);
	} else if (!top && !GetSelected()) {
		tmpRect.height  -= 3;
		dc.DrawLine(0, 0, tmpRect.x+tmpRect.width, 0);
	}

	int radius(TAB_RADIUS ? TAB_RADIUS : 1);
	if (GetTabContainer()) {
		size_t tabIdx = GetTabContainer()->TabToIndex(this);
		bool drawLeftBorder = (tabIdx == GetTabContainer()->GetFirstVisibleTab() || GetSelected());

		if (!drawLeftBorder) {
			tmpRect.x -= 1;
			tmpRect.width += 1;
		}
	}

	// since we want to draw a full rounded rectangle, but we only want one edge roudned,
	// we adjust the rectangle height and y offset
	tmpRect.height += radius;
	top ? tmpRect.y : tmpRect.y -= radius;

	// draw the outer dark border
	dc.SetPen(pen);
	dc.DrawRoundedRectangle(tmpRect, TAB_RADIUS);

	if (GetSelected()) {
		// draw the inner white border
		wxColour innerBorderCol (wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
		innerBorderCol = DrawingUtils::LightColour(innerBorderCol, 3);
		dc.SetPen  ( wxPen(innerBorderCol) );

		tmpRect.Deflate(1);
		dc.DrawRoundedRectangle(tmpRect, TAB_RADIUS);
	}

	if (top && GetSelected()) {
		//draw a line
		dc.SetPen(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
		dc.DrawLine(0, rr.GetHeight()-1, rr.GetWidth(), rr.GetHeight()-1);

	} else if (!top && GetSelected()) {
		//draw a line

		dc.SetPen(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
		dc.DrawLine(0, 0, rr.GetWidth(), 0);
	}
}

const wxBitmap& CustomTab::GetXBmp()
{
	//return the x bitmap by state
	switch (m_x_state) {
	case XNone:
		return m_xButtonNormalBmp;
	case XPushed:
	default:
		return m_xButtonPressedBmp;
	}
}

void CustomTab::OnMouseMiddleButton(wxMouseEvent &e)
{
	wxUnusedVar(e);
	if (GetBookStyle() & wxVB_MOUSE_MIDDLE_CLOSE_TAB) {
		wxCommandEvent event(wxEVT_CMD_DELETE_TAB, GetId());
		event.SetEventObject(this);
		GetParent()->AddPendingEvent(event);
	}
}

void CustomTab::OnRightDown(wxMouseEvent &e)
{
	wxTabContainer *parent = (wxTabContainer*)GetParent();
	if (!GetSelected()) {
		parent->SetSelection(this, true);
	}

	if (parent->GetRightClickMenu()) {
		parent->ShowPopupMenu();
	}
}

void CustomTab::SetText(const wxString &text, const wxString &tooltip)
{
	m_text = text;
	m_tooltip = tooltip;
	Initialize();
	Refresh();
}

void CustomTab::Initialize()
{
	if (m_orientation == wxLEFT || m_orientation == wxRIGHT) {
		//Vertical tab
		SetSizeHints(CalcTabWidth(), CalcTabHeight());
	} else {
		SetSizeHints(CalcTabHeight(), CalcTabWidth());
	}

	//create a drop down arrow image
	wxImage img(closetab_xpm);
	img.SetAlpha(closetab_alpha, true);
	m_xButtonNormalBmp = wxBitmap(img);

	wxImage img2(closetab_active_xpm);
	img2.SetAlpha(closetab_active_alpha, true);
	m_xButtonPressedBmp = wxBitmap(img2);

	SetToolTip( m_tooltip );
}

wxTabContainer* CustomTab::GetTabContainer()
{
	return (wxTabContainer*)GetParent();
}

void CustomTab::SetBmp(const wxBitmap& bmp)
{
	m_bmp = bmp;
	Initialize();
	Refresh();
}

bool CustomTab::IsFixedWidthTabs()
{
	return GetBookStyle() & wxVB_FIXED_WIDTH && GetTabContainer()->GetFixedTabWidth() != Notebook::npos;
}

long CustomTab::GetBookStyle()
{
	Notebook *book = (Notebook*)GetTabContainer()->GetParent();
	return book->GetBookStyle();
}
