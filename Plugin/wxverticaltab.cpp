#include "wxverticaltab.h"
#include "wx/dcbuffer.h"
#include "wx/settings.h"
#include "wx/image.h"
#include "drawingutils.h"
#include "wxtabcontainer.h"

BEGIN_EVENT_TABLE(wxVerticalTab, wxPanel)
	EVT_PAINT(wxVerticalTab::OnPaint)
	EVT_ERASE_BACKGROUND(wxVerticalTab::OnErase)
	EVT_LEFT_DOWN(wxVerticalTab::OnLeftDown)
	EVT_ENTER_WINDOW(wxVerticalTab::OnMouseEnterWindow)
	EVT_LEFT_UP(wxVerticalTab::OnLeftUp)
	EVT_MOTION(wxVerticalTab::OnMouseMove)
END_EVENT_TABLE()

wxVerticalTab::wxVerticalTab(wxWindow *win, wxWindowID id, const wxString &text, const wxBitmap &bmp, bool selected, int orientation)
		: wxPanel(win, id)
		, m_text(text)
		, m_bmp(bmp)
		, m_selected(selected)
		, m_padding(6)
#ifdef __WXMSW__		
		, m_heightPadding(4)
#else
		, m_heightPadding(6)
#endif
		, m_orientation(orientation)
		, m_window(NULL)
		, m_leftDown(false)
{
	SetSizeHints(CalcTabWidth(), CalcTabHeight());
}

wxVerticalTab::~wxVerticalTab()
{
}

void wxVerticalTab::OnPaint(wxPaintEvent &event)
{
	wxUnusedVar(event);
	wxBufferedPaintDC dc(this);

	//draw a grey border around the tab
	wxRect rr = GetClientSize();

	//create temporary memory dc and draw the image on top of it
	wxBitmap bmp(rr.GetHeight(), rr.GetWidth());
	wxMemoryDC memDc;
	memDc.SelectObject(bmp);

	//set the default GUI font
	wxFont font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	font.SetWeight(GetSelected() ? wxBOLD : wxNORMAL);
	memDc.SetFont(font);
	wxRect bmpRect(0, 0, bmp.GetWidth(), bmp.GetHeight());

	memDc.SetBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
	memDc.SetPen(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
	memDc.DrawRectangle(bmpRect);

	//fill the bmp with a nice gradient colour
	bool left = (GetOrientation() == wxLEFT);
	DrawingUtils::DrawButton(memDc, bmpRect.Deflate(2), GetSelected(), left, true);

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
		int maxTextWidth = bmp.GetWidth() - posx - GetPadding();

		//truncate the text if needed
		DrawingUtils::TruncateText(memDc, GetText(), maxTextWidth, truncateText);

		//draw it
		memDc.DrawText(truncateText, posx, txtYCoord);
		posx += maxTextWidth + GetPadding();
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
	wxPen pen(wxSystemSettings::GetColour(wxSYS_COLOUR_3DSHADOW));
	dc.SetPen(pen);

	if (left) {
		dc.DrawRoundedRectangle(0, 0, rr.GetWidth()+2, rr.GetHeight(), 0);
		if (!GetSelected()) {
			//draw a line
			dc.DrawLine(rr.GetWidth()-1, 0, rr.GetWidth()-1, rr.GetHeight());
		}
	} else {
		dc.DrawRoundedRectangle(-2, 0, rr.GetWidth(), rr.GetHeight(), 0);
		if (!GetSelected()) {
			//draw a line
			dc.DrawLine(0, 0, 0, rr.GetHeight());
		}
	}
}

void wxVerticalTab::OnErase(wxEraseEvent &event)
{
	wxUnusedVar(event);
}

int wxVerticalTab::CalcTabHeight()
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
		font.SetWeight(wxBOLD);
		wxWindow::GetTextExtent(GetText(), &xx, &yy, NULL, NULL, &font);

		tmpTabHeight += xx;
		tmpTabHeight += GetPadding();
	}

	tabHeight += tmpTabHeight;
	return tabHeight;

}

int wxVerticalTab::CalcTabWidth()
{
	int tmpTabWidth(0);
	int tabWidth(GetHeightPadding()*2);
	if (GetBmp().IsOk()) {
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

void wxVerticalTab::OnLeftDown(wxMouseEvent &e)
{
	m_leftDown = true;
	//notify the parent that this tab has been selected
	if (GetSelected()) {
		//we are already the selected tab nothing to be done here
		return;
	}

	wxTabContainer *parent = dynamic_cast<wxTabContainer*>( GetParent() );
	if (parent) {
		parent->SetSelection(this, true);
	}
	
	e.Skip();
}

void wxVerticalTab::OnMouseEnterWindow(wxMouseEvent &e)
{
	wxUnusedVar(e);
	if (e.LeftIsDown()) {
		wxTabContainer *parent = dynamic_cast<wxTabContainer*>( GetParent() );
		if (parent) {
			parent->SwapTabs(this);
		}
	}
}

void wxVerticalTab::OnMouseMove(wxMouseEvent &e)
{
	//mark this tab as the dragged one, only if the left down action
	//was taken place on this tab!
	if (e.LeftIsDown() && m_leftDown) {
		//left is down, check to see if the tab is being dragged
		wxTabContainer *parent = dynamic_cast<wxTabContainer*>( GetParent() );
		if (parent) {
			parent->SetDraggedTab(this);
		}
	}
	
}

void wxVerticalTab::OnLeftUp(wxMouseEvent &e)
{
	wxUnusedVar(e);
	m_leftDown = false;
	wxTabContainer *parent = dynamic_cast<wxTabContainer*>( GetParent() );
	if (parent) {
		parent->SetDraggedTab(NULL);
	}
	
}
