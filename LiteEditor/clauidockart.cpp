#include "clauidockart.h"
#include <wx/image.h>
#include <wx/dc.h>
#include <wx/settings.h>
#include "drawingutils.h"
#include "editor_config.h"
#include "closetab-active.xpm"
#include "closetab-active.xpm.alpha"
#include "closetab.xpm"
#include "closetab.xpm.alpha"

CLAuiDockArt::CLAuiDockArt()
{
	wxImage img(closetab_xpm);
	img.SetAlpha(closetab_alpha, true);
	m_bmp_close = wxBitmap(img);

	wxImage img2(closetab_active_xpm);
	img2.SetAlpha(closetab_active_alpha, true);
	m_bmp_close_active = wxBitmap(img2);

	if(EditorConfigST::Get()->GetOptions()->GetMswTheme()) {
		m_border_pen = wxPen(wxSystemSettings::GetColour(wxSYS_COLOUR_ACTIVECAPTION));
	}

	m_gripper_brush    = wxBrush(DrawingUtils::GetPanelBgColour());
	m_background_brush = wxBrush(DrawingUtils::GetPanelBgColour());
	m_gripper_pen3     = wxPen  (DrawingUtils::GetPanelBgColour());
}

CLAuiDockArt::~CLAuiDockArt()
{
}

void CLAuiDockArt::DrawPaneButton(wxDC& dc, wxWindow* window, int button, int button_state, const wxRect& rect, wxAuiPaneInfo& pane)
{
#ifdef __WXMSW__
	wxAuiDefaultDockArt::DrawPaneButton(dc, window, button, button_state, rect, pane);
#else
	switch(button){
	case wxAUI_BUTTON_CLOSE: {
		wxBitmap bmp;
		switch(button_state){
		case wxAUI_BUTTON_STATE_PRESSED:
			bmp = m_bmp_close_active;
			break;
		case wxAUI_BUTTON_STATE_HIDDEN:
			break;
		case wxAUI_BUTTON_STATE_HOVER:
		case wxAUI_BUTTON_STATE_NORMAL:
		default:
			bmp = m_bmp_close;
			break;
		}
		dc.DrawBitmap(bmp, rect.x, rect.y+1, true);
		break;
	}
	default:
		wxAuiDefaultDockArt::DrawPaneButton(dc, window, button, button_state, rect, pane);
		break;
	}
#endif
}

void CLAuiDockArt::DrawBorder(wxDC& dc, wxWindow *WXUNUSED(window), const wxRect& _rect, wxAuiPaneInfo& pane)
{
    dc.SetPen(m_border_pen);
    dc.SetBrush(*wxTRANSPARENT_BRUSH);

    wxRect rect = _rect;
    int i, border_width = GetMetric(wxAUI_DOCKART_PANE_BORDER_SIZE);
	wxPen lightColor = DrawingUtils::LightColour(DrawingUtils::GetPanelBgColour(), 2.0);
    if (pane.IsToolbar())
    {
        for (i = 0; i < border_width; ++i)
        {
            dc.SetPen(lightColor);
            dc.DrawLine(rect.x, rect.y, rect.x+rect.width, rect.y);
            dc.DrawLine(rect.x, rect.y, rect.x, rect.y+rect.height);
            dc.SetPen(m_border_pen);
            dc.DrawLine(rect.x, rect.y+rect.height-1,
                        rect.x+rect.width, rect.y+rect.height-1);
            dc.DrawLine(rect.x+rect.width-1, rect.y,
                        rect.x+rect.width-1, rect.y+rect.height);
            rect.Deflate(1);
        }
    }
    else
    {
        for (i = 0; i < border_width; ++i)
        {
            dc.DrawRectangle(rect.x, rect.y, rect.width, rect.height);
            rect.Deflate(1);
        }
    }
}
