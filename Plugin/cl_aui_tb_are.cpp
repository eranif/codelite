#include "cl_aui_tb_are.h"
#include <wx/settings.h>

#if USE_AUI_TOOLBAR
CLMainAuiTBArt::CLMainAuiTBArt()
{
}

CLMainAuiTBArt::~CLMainAuiTBArt()
{
}

void CLMainAuiTBArt::DrawBackground(wxDC& dc, wxWindow* wnd, const wxRect& rect)
{
#ifdef __WXGTK__

	wxColour baseColour = DrawingUtils::GetMenuBarBgColour();
	dc.SetPen(baseColour);
	dc.SetBrush(baseColour);
	dc.DrawRectangle(rect);

#elif defined(__WXMSW__)
	wxColor col1 = DrawingUtils::GetPanelBgColour();
	wxColor col2 = DrawingUtils::DarkColour(col1, 2.0);
	DrawingUtils::PaintStraightGradientBox(dc, rect, col1, col2, true);

#else // Mac
	wxColour baseColour = DrawingUtils::GetPanelBgColour();
	dc.SetPen(baseColour);
	dc.SetBrush(baseColour);
	dc.DrawRectangle(rect);
#endif
}
void CLMainAuiTBArt::DrawGripper(wxDC& dc, wxWindow* wnd, const wxRect& rect)
{
#ifdef __WXGTK__
	// dont draw anything
#else
	wxAuiDefaultToolBarArt::DrawGripper(dc, wnd, rect);
#endif	
}

#endif

