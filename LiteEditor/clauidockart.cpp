#include "clauidockart.h"
#include <wx/image.h>
#include <wx/dc.h>
#include "drawingutils.h"

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

}

CLAuiDockArt::~CLAuiDockArt()
{
}

void CLAuiDockArt::DrawPaneButton(wxDC& dc, wxWindow* window, int button, int button_state, const wxRect& rect, wxAuiPaneInfo& pane)
{
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
}

void CLAuiDockArt::DrawBackground(wxDC& dc, wxWindow* window, int oriantation, const wxRect& rect)
{
    dc.SetPen(*wxTRANSPARENT_PEN);
#ifdef __WXMAC__
    // we have to clear first, otherwise we are drawing a light striped pattern
    // over an already darker striped background
    dc.SetBrush(*wxWHITE_BRUSH) ;
    dc.DrawRectangle(rect.x, rect.y, rect.width, rect.height);
#endif
	dc.SetBrush( DrawingUtils::GetPanelBgColour() );
    dc.DrawRectangle(rect.x, rect.y, rect.width, rect.height);
}

void CLAuiDockArt::DrawGripper(wxDC& dc, wxWindow* window, const wxRect& rect, wxAuiPaneInfo& pane)
{
#if defined(__WXMSW__)|| defined(__WXMAC__)
	wxAuiDefaultDockArt::DrawGripper(dc, window, rect, pane);
#else
	wxUnusedVar(window);
	wxUnusedVar(pane);
	
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush(DrawingUtils::GetPanelBgColour());
    dc.DrawRectangle(rect.x, rect.y, rect.width,rect.height);
	
#endif
}
