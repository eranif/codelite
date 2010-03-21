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
	wxColor col1 = DrawingUtils::GetPanelBgColour();
	wxColor col2 = DrawingUtils::DarkColour(col1, 2.0);

	DrawingUtils::PaintStraightGradientBox(dc, rect, col1, col2, true);
}
#endif
