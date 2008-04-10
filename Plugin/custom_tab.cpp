#include "custom_tab.h"
#include "tab_x_pressed.xpm"
#include "tab_x.xpm"
#include "wx/dcbuffer.h"
#include "wx/settings.h"
#include "wx/image.h"
#include "drawingutils.h"
#include "custom_tabcontainer.h"
#include "custom_notebook.h"

#define clMASK_COLOR wxColor(0, 128, 128)

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

const wxEventType wxEVT_CMD_DELETE_TAB = wxNewEventType();

CustomTab::CustomTab(wxWindow *win, wxWindowID id, const wxString &text, const wxBitmap &bmp, bool selected, int orientation, long style)
		: wxPanel(win, id)
		, m_text(text)
		, m_bmp(bmp)
		, m_selected(selected)
		, m_padding(6)
		, m_heightPadding(6)
		, m_orientation(orientation)
		, m_window(NULL)
		, m_leftDown(false)
		, m_hovered(false)
		, m_style(style)
		, m_x_state(XNone)
		, m_x_padding(3)
{
	Initialize();
}

CustomTab::~CustomTab()
{
}

void CustomTab::OnPaint(wxPaintEvent &event)
{
	wxUnusedVar(event);
	wxBufferedPaintDC dc(this);

	if (m_orientation == wxLEFT || m_orientation == wxRIGHT) {
		DoDrawVerticalTab(dc);
	} else {
		DoDrawHorizontalTab(dc);
	}
}

void CustomTab::OnErase(wxEraseEvent &event)
{
	wxUnusedVar(event);
}

int CustomTab::CalcTabHeight()
{
	int tmpTabHeight(0);
	int tabHeight(GetPadding());
	if (GetBmp().IsOk()) {
		tmpTabHeight = GetBmp().GetWidth();
		tmpTabHeight += GetPadding();
	}

	if (GetText().IsEmpty() == false) {
		int xx(0), yy(0);
		wxFont font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
		wxWindow::GetTextExtent(GetText(), &xx, &yy, NULL, NULL, &font);

		tmpTabHeight += xx;
		tmpTabHeight += GetPadding();
	}

	if (m_style & wxVB_HAS_X) {
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
	if (GetBmp().IsOk()) {
		tabWidth = GetHeightPadding()+4;
		tmpTabWidth = GetBmp().GetHeight();
	}

	if (GetText().IsEmpty() == false) {
		int xx, yy;
		wxString stam(wxT("Tp"));
		wxFont font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
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
			parent->SwapTabs(this);
		}
	}
	m_hovered = true;
	Refresh();
	e.Skip();
}

void CustomTab::OnMouseMove(wxMouseEvent &e)
{
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

		GetParent()->AddPendingEvent(event);
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
	memDc.SetFont(font);
	wxRect bmpRect(0, 0, bmp.GetWidth(), bmp.GetHeight());

	memDc.SetBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
	memDc.SetPen(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
	memDc.DrawRectangle(bmpRect);

	//fill the bmp with a nice gradient colour
	bool hovered = (m_hovered && (GetSelected() == false));
	bool left = m_orientation == wxLEFT;

	if (left) {
		wxRect rt(bmpRect);
		if (GetSelected()) {
			rt.y += 1;
		} else {
			rt.y += 3;
		}

		DrawingUtils::DrawVerticalButton(memDc, rt, GetSelected(), left, true, hovered);

	} else {
		wxRect rt(bmpRect);
		if (GetSelected()) {
			rt.y += 1;
		} else {
			rt.y += 3;
		}
		DrawingUtils::DrawVerticalButton(memDc, rt, GetSelected(), left, true, hovered);
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
		if (m_style & wxVB_HAS_X) {
			x_len = 16 + GetXPadding();
		}

		int maxTextWidth = bmp.GetWidth() - posx - x_len - GetPadding();

		//truncate the text if needed
		DrawingUtils::TruncateText(memDc, GetText(), maxTextWidth, truncateText);

		//draw it
		memDc.DrawText(truncateText, posx, txtYCoord);
		posx += maxTextWidth + GetPadding();
	}

	//draw x button if needed
	if (m_style & wxVB_HAS_X) {
		wxCoord xBtnYCoord = (bmp.GetHeight() - 16)/2 + 2;

		//draw the x button, only if we are the active tab
		if (GetSelected()) {
			memDc.DrawBitmap(GetXBmp(), posx, xBtnYCoord, true);
		}

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

	wxColour borderColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DSHADOW));
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
	
	int radius(0);
	if(GetSelected()) {radius = 2;}
	
	dc.DrawRoundedRectangle(tmpRect, radius);

	if (left && GetSelected()) {
		//draw a line
		dc.SetPen(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
		dc.DrawLine(rr.x + rr.width - 1, 0, rr.x + rr.width - 1, rr.y + rr.height);
		if (m_style & wxVB_TAB_DECORATION) {
			//draw a single caption colour on top of the active tab
			wxPen p(wxSystemSettings::GetColour(wxSYS_COLOUR_ACTIVECAPTION), 1, wxSOLID);
			dc.SetPen(p);
			dc.DrawLine(rr.x+1, 1, rr.x+1, rr.y+rr.height+1);
			dc.DrawLine(rr.x+2, 1, rr.x+2, rr.y+rr.height+1);
			//draw third line on top of the second line, but 1 pixel shorter
			p = wxPen( DrawingUtils::LightColour(wxSystemSettings::GetColour(wxSYS_COLOUR_ACTIVECAPTION), 40), 1, wxSOLID);
			dc.SetPen(p);
			dc.DrawLine(rr.x+2, 2, rr.x+2, rr.y+rr.height);
		}

	} else if (!left && GetSelected()) {
		//draw a line
		dc.SetPen(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
		dc.DrawLine(0, 0, 0, tmpRect.y + tmpRect.height);
		if (m_style & wxVB_TAB_DECORATION) {
			//draw a single caption colour on top of the active tab
			wxPen p(wxSystemSettings::GetColour(wxSYS_COLOUR_ACTIVECAPTION), 1, wxSOLID);
			dc.SetPen(p);

			dc.DrawLine(rr.x+rr.width-2, 0, rr.x+rr.width-2, rr.y+rr.height);
			dc.DrawLine(rr.x+rr.width-3, 0, rr.x+rr.width-3, rr.y+rr.height);
			//draw third line on top of the second line, but 1 pixel shorter
			p = wxPen( DrawingUtils::LightColour(wxSystemSettings::GetColour(wxSYS_COLOUR_ACTIVECAPTION), 40), 1, wxSOLID);
			dc.SetPen(p);
			dc.DrawLine(rr.x+rr.width-3, 1, rr.x+rr.width-3, rr.y+rr.height-1);
		}
	}
}

void CustomTab::DoDrawHorizontalTab(wxDC &dc)
{
	//draw a grey border around the tab
	wxRect rr = GetClientSize();

	//create temporary memory dc and draw the image on top of it
	wxBitmap bmp(rr.GetWidth(), rr.GetHeight());
	wxMemoryDC memDc;
	memDc.SelectObject(bmp);

	//set the default GUI font
	wxFont font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	memDc.SetFont(font);

	wxRect bmpRect(0, 0, bmp.GetWidth(), bmp.GetHeight());

	memDc.SetBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
	memDc.SetPen(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
	memDc.DrawRectangle(bmpRect);

	//fill the bmp with a nice gradient colour
	bool hovered = (m_hovered && (GetSelected() == false));
	bool top = m_orientation == wxTOP;

	if (top) {
		wxRect fillRect(bmpRect);

		if (GetSelected()) {
			fillRect.y += 1;
		} else {
			fillRect.y += 3;
		}
		DrawingUtils::DrawHorizontalButton(memDc, fillRect, GetSelected(), top, true, hovered);
	} else {
		wxRect fillRect(bmpRect);
		if (GetSelected()) {
			fillRect.height -= 1;
		} else {
			fillRect.height -= 3;
		}
		DrawingUtils::DrawHorizontalButton(memDc, fillRect, GetSelected(), top, true, hovered);
	}

	int text_yoffset(1);
	int x_yoffset(1);
	int bmp_yoffset(1);
	
	if ( !top ) {
		text_yoffset = -3;
		x_yoffset = -1;
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

		//make sure the colour used here is the system default
		memDc.SetTextForeground(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));

		wxString truncateText;

		int x_len = 0;
		if (m_style & wxVB_HAS_X) {
			x_len = 16 + GetXPadding();
		}

		//calcualte the size left for drawing the text
		int maxTextWidth = bmp.GetWidth() - posx - x_len- GetPadding();

		//truncate the text if needed
		DrawingUtils::TruncateText(memDc, GetText(), maxTextWidth, truncateText);

		//draw it
		memDc.DrawText(truncateText, posx, txtYCoord);
		posx += maxTextWidth + GetPadding();
	}

	//draw x button if needed
	if (m_style & wxVB_HAS_X) {
		wxCoord xBtnYCoord = (rr.height - 16)/2;
		if ( m_orientation == wxTOP ) {
			xBtnYCoord += 2;
		}

		//draw the x button, only if we are the active tab
		if (GetSelected()) {
			memDc.DrawBitmap(GetXBmp(), posx, xBtnYCoord + x_yoffset, true);
		}
		m_xButtonRect = wxRect(posx, xBtnYCoord, 16, 16);
		posx += 16 + GetPadding();
	}

	//apply changes
	memDc.SelectObject(wxNullBitmap);
	dc.DrawBitmap(bmp, 0, 0, true);

	//set pen & brush
	dc.SetBrush(*wxTRANSPARENT_BRUSH);

	wxColour borderColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DSHADOW));
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

	int radius(0);
	if(GetSelected()) {radius = 2;}

	dc.DrawRoundedRectangle(tmpRect, radius);

	if (top && GetSelected()) {
		//draw a line
		dc.SetPen(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
		dc.DrawLine(0, rr.GetHeight()-1, rr.GetWidth(), rr.GetHeight()-1);

		if (m_style & wxVB_TAB_DECORATION) {
			wxPen p(wxSystemSettings::GetColour(wxSYS_COLOUR_ACTIVECAPTION), 1, wxSOLID);
			dc.SetPen(p);

			dc.DrawLine(rr.x+1, rr.y+1, rr.x+rr.width-1, rr.y+1);
			dc.DrawLine(rr.x+1, rr.y+2, rr.x+rr.width-1, rr.y+2);

			//draw third line on top of the second line, but 1 pixel shorter
			p = wxPen( DrawingUtils::LightColour(wxSystemSettings::GetColour(wxSYS_COLOUR_ACTIVECAPTION), 40), 1, wxSOLID);
			dc.SetPen(p);
			dc.DrawLine(rr.x+2, rr.y+2, rr.x+rr.width-2, rr.y+2);
		}
	} else if (!top && GetSelected()) {
		//draw a line
		dc.SetPen(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
		dc.DrawLine(0, 0, rr.GetWidth(), 0);

		if (m_style & wxVB_TAB_DECORATION) {
			wxPen p(wxSystemSettings::GetColour(wxSYS_COLOUR_ACTIVECAPTION), 1, wxSOLID);
			dc.SetPen(p);
			dc.DrawLine(rr.x+1, rr.y+rr.height-2, rr.x+rr.width-1, rr.y+rr.height-2);
			dc.DrawLine(rr.x+1, rr.y+rr.height-3, rr.x+rr.width-1, rr.y+rr.height-3);
			//draw third line on top of the second line, but 1 pixel shorter
			p = wxPen( DrawingUtils::LightColour(wxSystemSettings::GetColour(wxSYS_COLOUR_ACTIVECAPTION), 40), 1, wxSOLID);
			dc.SetPen(p);
			dc.DrawLine(rr.x+2, rr.y+rr.height-3, rr.x+rr.width-2, rr.y+rr.height-3);
		}
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
	if (m_style & wxVB_MOUSE_MIDDLE_CLOSE_TAB) {
		wxCommandEvent event(wxEVT_CMD_DELETE_TAB, GetId());
		event.SetEventObject(this);
		GetParent()->AddPendingEvent(event);
	}
}

void CustomTab::OnRightDown(wxMouseEvent &e)
{
	wxTabContainer *parent = (wxTabContainer*)GetParent();
	if (!GetSelected()) {
		parent->SetSelection(this);
	}

	if (parent->GetRightClickMenu()) {
		parent->ShowPopupMenu();
	}
}

void CustomTab::SetText(const wxString &text)
{
	m_text = text;
	Initialize();
	
	//we need to tell the container to layout since our size might have changed
	wxTabContainer *parent = (wxTabContainer*)GetParent();
	parent->Resize();
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
	m_xButtonNormalBmp = wxBitmap(_tab_x_button_xpm);
	m_xButtonNormalBmp.SetMask(new wxMask(m_xButtonNormalBmp, clMASK_COLOR));

	m_xButtonPressedBmp = wxBitmap(_tab_x_button_pressed_xpm);
	m_xButtonPressedBmp.SetMask(new wxMask(m_xButtonPressedBmp, clMASK_COLOR));
}
