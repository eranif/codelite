#ifndef __clauidockart__
#define __clauidockart__

#include <wx/aui/framemanager.h>
#include <wx/aui/dockart.h>

class CLAuiDockArt : public wxAuiDefaultDockArt
{
	wxBitmap m_bmp_close;
	wxBitmap m_bmp_close_active;
	wxBitmap m_bmp_caption_active_bg;
	wxBitmap m_bmp_caption_active_left_bg;
	wxBitmap m_bmp_caption_active_right_bg;
	
public:
	CLAuiDockArt();
	virtual ~CLAuiDockArt();

	void DrawPaneButton(wxDC& dc, wxWindow *window, int button, int button_state, const wxRect& rect, wxAuiPaneInfo& pane);
	void DrawBorder(wxDC& dc, wxWindow *WXUNUSED(window), const wxRect& _rect, wxAuiPaneInfo& pane);
	void DrawCaption(wxDC& dc, wxWindow *win, const wxString &text, const wxRect &rect, wxAuiPaneInfo &pane);
};
#endif // __clauidockart__
