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

	bool useGradients (true);
	
	wxColour baseColour = DrawingUtils::GetPanelBgColour();
	wxColor col1, col2;
	
	if(DrawingUtils::IsDark(baseColour)) {
		useGradients = false;
	}
	
	if(useGradients) {
		col1 = DrawingUtils::GetPanelBgColour();
		col2 = DrawingUtils::DarkColour(col1, 2.0);
		dc.GradientFillLinear(rect, col1, col2, wxSOUTH);
		
	} else {
		col1 = col2 = baseColour;
		dc.SetPen(col1);
		dc.SetBrush(col2);
		dc.DrawRectangle(rect);
	}
	
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

#endif
