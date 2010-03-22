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
	wxColor col1 = DrawingUtils::GetPanelBgColour();
	wxColor col2 = DrawingUtils::GetPanelBgColour();
	dc.SetPen(col1);
	dc.SetBrush(col2);
	dc.DrawRectangle(rect);
	
#elif defined(__WXMSW__)
	wxColor col1 = DrawingUtils::GetPanelBgColour();
	wxColor col2 = DrawingUtils::DarkColour(col1, 2.0);
	DrawingUtils::PaintStraightGradientBox(dc, rect, col1, col2, true);
	
#else // Mac
	wxColor col1 = DrawingUtils::DarkColour(wxSystemSettings::GetColour(wxSYS_COLOUR_MENUBAR), 2.0);
	wxColor col2 = DrawingUtils::DarkColour(wxSystemSettings::GetColour(wxSYS_COLOUR_MENUBAR), 6.0);
	DrawingUtils::PaintStraightGradientBox(dc, rect, col1, col2, true);
#endif
}

#endif
