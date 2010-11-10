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
	static wxColor col1, col2;
	if(col1.IsOk() == false) {
		col1 = DrawingUtils::GetPanelBgColour();
	}
	if(col2.IsOk() == false) {
		col2 = DrawingUtils::DarkColour(col1, 2.0);
	}
	
#if defined(__WXMSW__)
	DrawingUtils::PaintStraightGradientBox(dc, rect, col1, col2, true);

#else // Mac
	dc.SetPen  (col1);
	dc.SetBrush(col1);
	dc.DrawRectangle(rect);
#endif
}

void CLMainAuiTBArt::DrawGripper(wxDC& dc, wxWindow* wnd, const wxRect& rect)
{
	wxAuiDefaultToolBarArt::DrawGripper(dc, wnd, rect);
}

#endif

