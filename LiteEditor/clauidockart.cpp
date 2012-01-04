#include "clauidockart.h"
#include <wx/image.h>
#include <wx/dc.h>
#include <wx/dcmemory.h>
#include "imanager.h"
#include "pluginmanager.h"
#include <wx/settings.h>
#include "drawingutils.h"
#include "editor_config.h"
#include "closetab-active.xpm"
#include "closetab-active.xpm.alpha"
#include "closetab.xpm"
#include "closetab.xpm.alpha"

CLAuiDockArt::CLAuiDockArt()
{
	SetMetric(wxAUI_DOCKART_CAPTION_SIZE, 20);
	BitmapLoader *bmpLoader = PluginManager::Get()->GetStdIcons();
	m_bmp_caption_active_bg = bmpLoader->LoadBitmap(wxT("aui/caption-active"));
	
	wxImage img(closetab_xpm);
	img.SetAlpha(closetab_alpha, true);
	m_bmp_close = wxBitmap(img);

	wxImage img2(closetab_active_xpm);
	img2.SetAlpha(closetab_active_alpha, true);
	m_bmp_close_active = wxBitmap(img2);

#if wxCHECK_VERSION(2, 9, 3)
	if(EditorConfigST::Get()->GetOptions()->GetMswTheme()) {
		m_borderPen = wxPen(wxSystemSettings::GetColour(wxSYS_COLOUR_ACTIVECAPTION));
	}

	m_gripperBrush    = wxBrush(DrawingUtils::GetPanelBgColour());
	m_backgroundBrush = wxBrush(DrawingUtils::GetPanelBgColour());
	m_gripperPen3     = wxPen  (DrawingUtils::GetPanelBgColour());
#else
	if(EditorConfigST::Get()->GetOptions()->GetMswTheme()) {
		m_border_pen = wxPen(wxSystemSettings::GetColour(wxSYS_COLOUR_ACTIVECAPTION));
	}

	m_gripper_brush    = wxBrush(DrawingUtils::GetPanelBgColour());
	m_background_brush = wxBrush(DrawingUtils::GetPanelBgColour());
	m_gripper_pen3     = wxPen  (DrawingUtils::GetPanelBgColour());
#endif
}

CLAuiDockArt::~CLAuiDockArt()
{
}

void CLAuiDockArt::DrawPaneButton(wxDC& dc, wxWindow* window, int button, int button_state, const wxRect& rect, wxAuiPaneInfo& pane)
{
#if defined(__WXMSW__ ) || defined(__WXGTK__)
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
#if wxCHECK_VERSION(2, 9, 3)
    dc.SetPen(m_borderPen);
#else
    dc.SetPen(m_border_pen);
#endif
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
#if wxCHECK_VERSION(2, 9, 3)
            dc.SetPen(m_borderPen);
#else
            dc.SetPen(m_border_pen);
#endif
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

void CLAuiDockArt::DrawCaption(wxDC& dc, wxWindow* win, const wxString& text, const wxRect& rect, wxAuiPaneInfo& pane)
{
	wxBitmap tmpBmp(rect.width, rect.height);
	wxMemoryDC memDc(tmpBmp);
	if(memDc.IsOk()) {
		int posx(0);
		
		// Draw the background image
		while(posx < rect.width) {
			memDc.DrawBitmap(m_bmp_caption_active_bg, posx, 0);
			posx += m_bmp_caption_active_bg.GetWidth();
		}
		
		// calculate the text drawing position
		int textx, texty;
		int textWidth, textHeight;
		textx = 5;
		wxFont font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
		memDc.GetTextExtent(text, &textWidth, &textHeight, NULL, NULL, &font);
		memDc.SetTextForeground(*wxWHITE);
		memDc.SetFont(font);
		texty = (rect.GetHeight() - textHeight) / 2;
		memDc.DrawText(text, textx, texty);
		memDc.SelectObject(wxNullBitmap);
		dc.DrawBitmap(tmpBmp, rect.x, rect.y);
		
		
	} else {
		wxAuiDefaultDockArt::DrawCaption(dc, win, text, rect, pane);
	}
}
