#include "cl_aui_tb_are.h"
#include <wx/settings.h>
#include "editor_config.h"

#if USE_AUI_TOOLBAR
CLMainAuiTBArt::CLMainAuiTBArt()
	: m_mswWithThemeEnabled(false)
{
	m_mswWithThemeEnabled = EditorConfigST::Get()->GetOptions()->GetMswTheme();

	if(m_mswWithThemeEnabled) {
#if wxCHECK_VERSION(2, 9, 3)
		m_gripperPen1 = wxPen(DrawingUtils::DarkColour(wxSystemSettings::GetColour(wxSYS_COLOUR_GRADIENTACTIVECAPTION), 2.0));
		m_gripperPen2 = wxPen(DrawingUtils::DarkColour(wxSystemSettings::GetColour(wxSYS_COLOUR_GRADIENTACTIVECAPTION), 3.0));
		m_gripperPen3 = *wxWHITE_PEN;
#else
		m_gripper_pen1 = wxPen(DrawingUtils::DarkColour(wxSystemSettings::GetColour(wxSYS_COLOUR_GRADIENTACTIVECAPTION), 2.0));
		m_gripper_pen2 = wxPen(DrawingUtils::DarkColour(wxSystemSettings::GetColour(wxSYS_COLOUR_GRADIENTACTIVECAPTION), 3.0));
		m_gripper_pen3 = *wxWHITE_PEN;
#endif
	}
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
#ifdef __WXMSW__
	if(m_mswWithThemeEnabled) {
		col1 = DrawingUtils::LightColour(wxSystemSettings::GetColour(wxSYS_COLOUR_GRADIENTACTIVECAPTION), 4.0);
		col2 = DrawingUtils::LightColour(wxSystemSettings::GetColour(wxSYS_COLOUR_GRADIENTACTIVECAPTION), 2.0);
	}
	DrawingUtils::PaintStraightGradientBox(dc, rect, col1, col2, true);

#else // Mac / Linux
	dc.SetPen  (col1);
	dc.SetBrush(col1);
	dc.DrawRectangle(rect);
#endif
}

void CLMainAuiTBArt::DrawGripper(wxDC& dc, wxWindow* wnd, const wxRect& rect)
{
	wxAuiDefaultToolBarArt::DrawGripper(dc, wnd, rect);
}
void CLMainAuiTBArt::DrawSeparator(wxDC& dc, wxWindow* wnd, const wxRect& _rect)
{
	if(m_mswWithThemeEnabled) {
		bool horizontal = true;
		if (m_flags & wxAUI_TB_VERTICAL)
			horizontal = false;

		wxRect rect = _rect;

		if (horizontal) {
			rect.x += (rect.width/2);
			rect.width = 1;
			int new_height = (rect.height*3)/4;
			rect.y += (rect.height/2) - (new_height/2);
			rect.height = new_height;
		} else {
			rect.y += (rect.height/2);
			rect.height = 1;
			int new_width = (rect.width*3)/4;
			rect.x += (rect.width/2) - (new_width/2);
			rect.width = new_width;
		}

		wxColour start_colour = DrawingUtils::LightColour(wxSystemSettings::GetColour(wxSYS_COLOUR_GRADIENTACTIVECAPTION), 2.0);
		wxColour end_colour   = DrawingUtils::LightColour(wxSystemSettings::GetColour(wxSYS_COLOUR_GRADIENTACTIVECAPTION), 2.0);
		dc.GradientFillLinear(rect, start_colour, end_colour, horizontal ? wxSOUTH : wxEAST);
		
	} else {
		wxAuiDefaultToolBarArt::DrawSeparator(dc, wnd, _rect);
	}
}

#endif
