#include "active_tip.h"
#include <wx/xrc/xmlres.h>
#include <wx/sizer.h>
#include <wx/dcbuffer.h>
#include <wx/settings.h>
#include "ico_plus.xpm"
#include "ico_minus.xpm"
#include <wx/msgdlg.h>

ATLine::~ATLine()
{
	if ( m_clientData ) {
		delete m_clientData;
		m_clientData = NULL;
	}
}

#define MARGIN_WIDTH        10
#define MARGIN_HEIGHT       10
#define TEXT_Y_PADDING      2
#define TEXT_X_PADDING      2
#define TIP_WIDTH           500

const wxEventType ACTIVETIP_LINE_EXPANDING = XRCID("at_line_expanding");

BEGIN_EVENT_TABLE(ActiveTip, wxPanel)
	EVT_PAINT(ActiveTip::OnPaint)
	EVT_ERASE_BACKGROUND(ActiveTip::OnEraseBg)
	EVT_LEFT_DOWN(ActiveTip::OnMouseLeftDown)
	EVT_LEFT_DCLICK(ActiveTip::OnMouseLeftDown)
	EVT_LEAVE_WINDOW(ActiveTip::OnMouseLeaveWindow)
	EVT_MOTION(ActiveTip::OnMouseMove)
END_EVENT_TABLE()

ActiveTip::ActiveTip(wxWindow* parent, const ATFrame& frameContent)
		: wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(300, 300))
{
	m_plusBmp  = wxBitmap(ico_plus_xpm);
	m_minusBmp = wxBitmap(ico_minus_xpm);
	wxSizer *sz = new wxBoxSizer(wxVERTICAL);
	SetSizer( sz );

	m_rects.push_back( frameContent );
	DoDrawFrame();
}

ActiveTip::~ActiveTip()
{
}

void ActiveTip::OnEraseBg(wxEraseEvent& event)
{
	wxUnusedVar(event);
}

void ActiveTip::OnMouseLeaveWindow(wxMouseEvent& event)
{
	event.Skip();
	DoUnselectAll();
	Refresh();
}

void ActiveTip::OnMouseLeftDown(wxMouseEvent& event)
{
	event.Skip();
	if ( m_rects.size() ) {
		// check to see where did we hit
		size_t idx;
		int where = DoHitTest(event.GetPosition(), idx);
		switch ( where ) {
		case AT_COLLAPSE_BUTTON:
			// Remove the last frame from the list and re-draw
			m_rects.pop_back();
			DoDrawFrame();
			break;

		case AT_LINE: {
			ATFrame &vf = m_rects.at(m_rects.size()-1);
			ATLine &line = vf.m_lines.at(idx);
			if (line.GetIsExpandable()) {
				// send event to the user requesting new data
				ActiveTipData requestData;
				wxCommandEvent e(ACTIVETIP_LINE_EXPANDING);
				e.SetClientData( &requestData );
				GetParent()->ProcessEvent( e );

				if ( requestData.hasNewFrame ) {
					// Add new frame to the tip
					requestData.newFrame.m_title = line.GetKey();
					m_rects.push_back( requestData.newFrame );
					DoDrawFrame();
				}
			}
			break;

		}
		default:
		case AT_NOWHERE:
			break;
		}

	}
}

void ActiveTip::OnMouseMove(wxMouseEvent& event)
{
	event.Skip();
	DoUnselectAll();
	if ( m_rects.size() ) {

		for (size_t i=0; i<m_rects.size()-1; i++) {
			ATFrame &fr = m_rects.at(i);
			fr.m_active = fr.m_rect.Contains( event.GetPosition() );
			if (fr.m_active) break;
		}

		ATFrame &vf = m_rects.at(m_rects.size()-1);
		for (size_t i=0; i<vf.m_lines.size(); i++) {
			ATLine &line = vf.m_lines.at(i);
			line.SetActive( line.GetRect().Contains( event.GetPosition() ) );
			if (line.GetActive()) break;
		}
	}
	Refresh();
}

void ActiveTip::OnPaint(wxPaintEvent& event)
{
	wxUnusedVar( event );
	wxBufferedPaintDC dc(this);
	wxRect rr = GetClientSize();
	wxFont font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	font.SetPointSize( 8 );

	dc.SetPen( wxColour(wxT("GREY")) );
	dc.SetBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_INFOBK));
	dc.DrawRectangle(rr);
	dc.SetFont( font );

	if ( m_rects.size() ) {
		if ( m_rects.size() > 1 ) {
			// draw the collapse button as well
			dc.DrawBitmap(m_minusBmp, TEXT_X_PADDING, TEXT_Y_PADDING, true);
			m_collapseRect.SetX( 0 );
			m_collapseRect.SetY( 0 );
			m_collapseRect.SetWidth( m_minusBmp.GetWidth() + TEXT_X_PADDING*2);
			m_collapseRect.SetHeight( m_minusBmp.GetHeight() + TEXT_Y_PADDING*2);
			dc.SetTextForeground( wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT) );
			wxString title;
			for(size_t i=1; i<m_rects.size(); i++){
				title << m_rects.at(i).m_title << wxT("::");
			}
			title.RemoveLast();
			title.RemoveLast();

			dc.DrawText( title, MARGIN_WIDTH +  2* TEXT_X_PADDING, TEXT_Y_PADDING );

		} else {
			m_collapseRect = wxRect();
		}

		ATFrame &vf = m_rects.at(m_rects.size()-1);
		for (size_t i=0; i<vf.m_lines.size(); i++) {
			ATLine &line = vf.m_lines.at(i);
			DoDrawLine( dc, line );
		}
	}

	dc.SetPen  ( wxColour(wxT("GREY")) );
	dc.SetBrush( *wxTRANSPARENT_BRUSH );
	dc.DrawRectangle(rr);
}

wxSize ActiveTip::DoCalcSize()
{
	// Sanity
	if ( m_rects.empty() ) return wxSize();

	// we got at least one frame to display

	// each non-visible frame requires HIDDEN_FRAME_HEIGHT pixels in height
	int yy ( 0 );
	int xx ( 0 );
	int textWidth, textHeight;

	// update the hidden frames rectangles
	wxFont font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	font.SetPointSize( 8 );
	wxWindow::GetTextExtent(wxT("Tp"), &textWidth, &textHeight, NULL, NULL, &font);

	m_collapseRect = wxRect();
	if ( m_rects.size() > 1 ) {
		// keep margins for the collapse button
		yy += textHeight + (TEXT_Y_PADDING * 2);
		xx += MARGIN_WIDTH +  2* TEXT_X_PADDING;
	}

	ATFrame &fr = m_rects.at( m_rects.size() - 1 );
	fr.m_rect.SetX(xx);
	fr.m_rect.SetY(yy);
	fr.m_rect.SetWidth(TIP_WIDTH);

	// Calcualte the height based on the number of rows
	int visibleFrameHeight(0);
	for (size_t i=0; i<fr.m_lines.size(); i++) {
		wxRect rr;
		rr.SetY(yy);
		rr.SetX(xx);
		rr.SetHeight(textHeight + (TEXT_Y_PADDING * 2) +1 /* +1 to overlap this line with the one below it */);
		rr.SetWidth( TIP_WIDTH );
		ATLine &line = fr.m_lines.at(i);
		line.SetRect( rr );
		yy += textHeight + (TEXT_Y_PADDING * 2);
		visibleFrameHeight += textHeight + (TEXT_Y_PADDING * 2);
	}
	fr.m_rect.SetHeight( visibleFrameHeight );
	return wxSize(TIP_WIDTH, yy);
}

void ActiveTip::DoDrawLine(wxDC& dc, ATLine& line)
{
	wxRect rr = line.GetRect();
	rr.Deflate(2);

	// draw the background
	dc.SetBrush( wxSystemSettings::GetColour(wxSYS_COLOUR_INFOBK) );
	dc.SetPen  ( wxColour(wxT("GREY")) );
	dc.SetTextForeground( wxSystemSettings::GetColour(wxSYS_COLOUR_INFOTEXT) );
	dc.DrawRectangle( line.GetRect() );

	int plusIconMarginWidth = TEXT_X_PADDING + m_plusBmp.GetWidth() + TEXT_X_PADDING;

	// if the line is active, highlight the inner rectangle,
	if ( line.GetActive() ) {
		dc.SetBrush( wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT) );
		dc.SetPen  ( wxColour(wxT("GREY")) );
		dc.SetTextForeground( wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT) );
		wxRect highlightRect = line.GetRect();
		highlightRect.SetX( highlightRect.GetX() + plusIconMarginWidth );
		highlightRect.SetWidth( highlightRect.GetWidth() - plusIconMarginWidth );
		dc.DrawRectangle( highlightRect );
	}

	// Draw [+] sign if this line is exapandable
	int xx ( TEXT_X_PADDING + line.GetRect().x );
	if ( line.GetIsExpandable() ) {
		int bmpX, bmpY;
		bmpX = xx;
		bmpY = line.GetRect().y + (line.GetRect().GetHeight() - m_plusBmp.GetHeight())/2;
		dc.DrawBitmap(m_plusBmp, bmpX, bmpY, true);
	}

	xx += m_plusBmp.GetWidth();
	xx += TEXT_X_PADDING;
	dc.DrawLine(xx, line.GetRect().y, xx, line.GetRect().y + line.GetRect().height);
	xx += TEXT_X_PADDING;

	// draw the text
	wxString displayText;
	displayText << line.GetKey() << wxT(" = ") << line.GetValue();

	// TODO: shrink the text to fit the width
	dc.DrawText( displayText, xx, line.GetRect().y + TEXT_Y_PADDING );
}

void ActiveTip::DoUnselectAll()
{
	if ( m_rects.size() ) {

		// Reset
		for (size_t i=0; i<m_rects.size()-1; i++) {
			ATFrame &fr = m_rects.at(i);
			fr.m_active = false;
		}
		ATFrame &vf = m_rects.at(m_rects.size()-1);
		for (size_t i=0; i<vf.m_lines.size(); i++) {
			ATLine &line = vf.m_lines.at(i);
			line.SetActive( false );
		}
	}
}

int ActiveTip::DoHitTest(const wxPoint& pt, size_t& idx)
{
	idx = static_cast<size_t>(-1);
	if ( m_rects.size() ) {
		// Reset
		if ( m_collapseRect.Contains( pt ) ) {
			return AT_COLLAPSE_BUTTON;
		}

		ATFrame &vf = m_rects.at(m_rects.size()-1);
		for (size_t i=0; i<vf.m_lines.size(); i++) {
			ATLine &line = vf.m_lines.at(i);
			if ( line.GetRect().Contains(pt)) {
				idx = i;
				return AT_LINE;
			}
		}
	}
	return AT_NOWHERE;
}

void ActiveTip::DoDrawFrame()
{
	SetSizeHints( DoCalcSize() );
	Layout();
	if (GetParent()->GetSizer()) {
		GetParent()->GetSizer()->Layout();
	}
	Refresh();
}
