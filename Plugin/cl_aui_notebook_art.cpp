#include "cl_aui_notebook_art.h"

//-------------------------------------------------------------
// The following code was copy as is from wxWidgets source tree
//-------------------------------------------------------------

#include <wx/image.h>
#include "drawingutils.h"
#include <wx/settings.h>
#include <wx/menu.h>
#include <wx/xrc/xmlres.h>
#include <wx/dcclient.h>

#ifdef __WXMSW__
#include <wx/msw/uxtheme.h>
#endif

// these functions live in dockart.cpp -- they'll eventually
// be moved to a new utility cpp file

wxColor wxAuiStepColour(const wxColor& c, int percent)
{
	return DrawingUtils::LightColour(c, (float)(percent / 10));
}

wxBitmap wxAuiBitmapFromBits(const unsigned char bits[], int w, int h, const wxColour& color)
{
	wxImage img = wxBitmap((const char*)bits, w, h).ConvertToImage();
	img.Replace(0,0,0,123,123,123);
	img.Replace(255,255,255,color.Red(),color.Green(),color.Blue());
	img.SetMaskColour(123,123,123);
	return wxBitmap(img);
}

wxString wxAuiChopText(wxDC& dc, const wxString& text, int max_size)
{
	wxCoord x,y;

	// first check if the text fits with no problems
	dc.GetTextExtent(text, &x, &y);
	if (x <= max_size)
		return text;

	size_t i, len = text.Length();
	size_t last_good_length = 0;
	for (i = 0; i < len; ++i) {
		wxString s = text.Left(i);
		s += wxT("...");

		dc.GetTextExtent(s, &x, &y);
		if (x > max_size)
			break;

		last_good_length = i;
	}

	wxString ret = text.Left(last_good_length);
	ret += wxT("...");
	return ret;
}

static void IndentPressedBitmap(wxRect* rect, int button_state)
{
	if (button_state == wxAUI_BUTTON_STATE_PRESSED) {
		rect->x++;
		rect->y++;
	}
}

static unsigned char left_bits[] = {
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x7f, 0xfe, 0x3f, 0xfe,
	0x1f, 0xfe, 0x0f, 0xfe, 0x1f, 0xfe, 0x3f, 0xfe, 0x7f, 0xfe, 0xff, 0xfe,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

static unsigned char right_bits[] = {
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xdf, 0xff, 0x9f, 0xff, 0x1f, 0xff,
	0x1f, 0xfe, 0x1f, 0xfc, 0x1f, 0xfe, 0x1f, 0xff, 0x9f, 0xff, 0xdf, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

static unsigned char list_bits[] = {
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0x0f, 0xf8, 0xff, 0xff, 0x0f, 0xf8, 0x1f, 0xfc, 0x3f, 0xfe, 0x7f, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};


clAuiTabArt::clAuiTabArt()
{
	m_normal_font = *wxNORMAL_FONT;
	m_selected_font = *wxNORMAL_FONT;
	m_measuring_font = m_selected_font;

	m_fixed_tab_width = 100;
	m_tab_ctrl_height = 0;

	wxColour base_colour = DrawingUtils::GetPanelBgColour();
	m_base_colour = base_colour;

#ifdef __WXGTK__
	wxColor border_colour = wxSystemSettings::GetColour(wxSYS_COLOUR_3DSHADOW);
#else
	wxColor border_colour = DrawingUtils::DarkColour(m_base_colour, 4.0);
#endif

	m_base_colour_pen = wxPen(m_base_colour);
	m_base_colour_brush = wxBrush(m_base_colour);
	m_base_colour_2 = DrawingUtils::LightColour(m_base_colour, 2.0);
	m_base_colour_3 = DrawingUtils::LightColour(m_base_colour, 3.0);
	
	// used for drawing active tab gradient
#ifdef __WXMSW__
	if(wxUxThemeEngine::GetIfActive()) {
		m_border_pen                 = wxPen(wxSystemSettings::GetColour(wxSYS_COLOUR_ACTIVECAPTION));
		m_base_colour_4              = DrawingUtils::LightColour(wxSystemSettings::GetColour(wxSYS_COLOUR_GRADIENTACTIVECAPTION), 4.0);
		m_colour_gradient_active_tab = wxSystemSettings::GetColour(wxSYS_COLOUR_GRADIENTACTIVECAPTION);
		m_shade_colour               = DrawingUtils::LightColour(wxSystemSettings::GetColour(wxSYS_COLOUR_ACTIVECAPTION), 3.0);
		m_bottom_rect_colour         = DrawingUtils::LightColour(m_colour_gradient_active_tab, 2.0);
		m_active_close_bmp           = wxXmlResource::Get()->LoadBitmap(wxT("tab_x_close"));
		m_disabled_close_bmp         = wxXmlResource::Get()->LoadBitmap(wxT("tab_x_close"));
		
	} else {
		m_border_pen                 = wxPen(border_colour);
		m_base_colour_4              = DrawingUtils::LightColour(m_base_colour, 4.0);
		m_colour_gradient_active_tab = m_base_colour;
		m_shade_colour               = DrawingUtils::DarkColour(m_base_colour, 2.0);
		m_bottom_rect_colour         = m_base_colour;
		m_active_close_bmp           = wxXmlResource::Get()->LoadBitmap(wxT("tab_x_close_active"));
		m_disabled_close_bmp         = wxXmlResource::Get()->LoadBitmap(wxT("tab_x_close_red"));
	}
#else
	m_border_pen                 = wxPen(border_colour);
	m_base_colour_4              = DrawingUtils::LightColour(m_base_colour, 4.0);
	m_colour_gradient_active_tab = m_base_colour;
	m_shade_colour               = m_base_colour;
	m_bottom_rect_colour         = m_base_colour;
	m_active_close_bmp           = wxXmlResource::Get()->LoadBitmap(wxT("tab_x_close_active"));
	m_disabled_close_bmp         = wxXmlResource::Get()->LoadBitmap(wxT("tab_x_close_red"));
#endif


	m_active_left_bmp = wxAuiBitmapFromBits(left_bits, 16, 16, *wxBLACK);
	m_disabled_left_bmp = wxAuiBitmapFromBits(left_bits, 16, 16, wxColour(128,128,128));

	m_active_right_bmp = wxAuiBitmapFromBits(right_bits, 16, 16, *wxBLACK);
	m_disabled_right_bmp = wxAuiBitmapFromBits(right_bits, 16, 16, wxColour(128,128,128));

	m_active_windowlist_bmp = wxAuiBitmapFromBits(list_bits, 16, 16, *wxBLACK);
	m_disabled_windowlist_bmp = wxAuiBitmapFromBits(list_bits, 16, 16, wxColour(128,128,128));

	m_flags = 0;
}

clAuiTabArt::~clAuiTabArt()
{
}

wxAuiTabArt* clAuiTabArt::Clone()
{
	clAuiTabArt* art = new clAuiTabArt;
	art->SetNormalFont(m_normal_font);
	art->SetSelectedFont(m_selected_font);
	art->SetMeasuringFont(m_measuring_font);

	return art;
}

void clAuiTabArt::SetFlags(unsigned int flags)
{
	m_flags = flags;
}

void clAuiTabArt::SetSizingInfo(const wxSize& tab_ctrl_size,
                                size_t tab_count)
{
	m_fixed_tab_width = 100;

	int tot_width = (int)tab_ctrl_size.x - GetIndentSize() - 4;

	if (m_flags & wxAUI_NB_CLOSE_BUTTON)
		tot_width -= m_active_close_bmp.GetWidth();
	if (m_flags & wxAUI_NB_WINDOWLIST_BUTTON)
		tot_width -= m_active_windowlist_bmp.GetWidth();

	if (tab_count > 0) {
		m_fixed_tab_width = tot_width/(int)tab_count;
	}


	if (m_fixed_tab_width < 100)
		m_fixed_tab_width = 100;

	if (m_fixed_tab_width > tot_width/2)
		m_fixed_tab_width = tot_width/2;

	if (m_fixed_tab_width > 220)
		m_fixed_tab_width = 220;

	m_tab_ctrl_height = tab_ctrl_size.y;
}


void clAuiTabArt::DrawBackground(wxDC& dc,
                                 wxWindow* WXUNUSED(wnd),
                                 const wxRect& rect)
{
	// draw background
	wxColor top_color       = m_base_colour;
	wxColor bottom_color    = m_base_colour;

	wxRect r;

	if (m_flags &wxAUI_NB_BOTTOM)
		r = wxRect(rect.x, rect.y, rect.width+2, rect.height);
	// TODO: else if (m_flags &wxAUI_NB_LEFT) {}
	// TODO: else if (m_flags &wxAUI_NB_RIGHT) {}
	else //for wxAUI_NB_TOP
		r = wxRect(rect.x, rect.y, rect.width+2, rect.height-3);
	dc.GradientFillLinear(r, top_color, bottom_color, wxSOUTH);

	// draw base lines
	dc.SetPen(m_border_pen);
	int y = rect.GetHeight();
	int w = rect.GetWidth();

	if (m_flags &wxAUI_NB_BOTTOM) {
		dc.SetBrush(wxBrush(bottom_color));
		dc.DrawRectangle(-1, 0, w+2, 4);
	}
	// TODO: else if (m_flags &wxAUI_NB_LEFT) {}
	// TODO: else if (m_flags &wxAUI_NB_RIGHT) {}
	else { //for wxAUI_NB_TOP
		dc.SetBrush(m_bottom_rect_colour);
		dc.DrawRectangle(-1, y-4, w+2, 4);
	}
}


// DrawTab() draws an individual tab.
//
// dc       - output dc
// in_rect  - rectangle the tab should be confined to
// caption  - tab's caption
// active   - whether or not the tab is active
// out_rect - actual output rectangle
// x_extent - the advance x; where the next tab should start

void clAuiTabArt::DrawTab(wxDC& dc,
                          wxWindow* wnd,
                          const wxAuiNotebookPage& page,
                          const wxRect& in_rect,
                          int close_button_state,
                          wxRect* out_tab_rect,
                          wxRect* out_button_rect,
                          int* x_extent)
{
	wxCoord normal_textx, normal_texty;
	wxCoord selected_textx, selected_texty;
	wxCoord texty;

	// if the caption is empty, measure some temporary text
	wxString caption = page.caption;
	if (caption.empty())
		caption = wxT("Xj");

	dc.SetFont(m_selected_font);
	dc.GetTextExtent(caption, &selected_textx, &selected_texty);

	dc.SetFont(m_normal_font);
	dc.GetTextExtent(caption, &normal_textx, &normal_texty);

	// figure out the size of the tab
	wxSize tab_size = GetTabSize(dc,
	                             wnd,
	                             page.caption,
	                             page.bitmap,
	                             page.active,
	                             close_button_state,
	                             x_extent);

	wxCoord tab_height = m_tab_ctrl_height - 3;
	wxCoord tab_width = tab_size.x;
	wxCoord tab_x = in_rect.x;
	wxCoord tab_y = in_rect.y + in_rect.height - tab_height;


	caption = page.caption;


	// select pen, brush and font for the tab to be drawn

	if (page.active) {
		dc.SetFont(m_selected_font);
		texty = selected_texty;
	} else {
		dc.SetFont(m_normal_font);
		texty = normal_texty;
	}


	// create points that will make the tab outline

	int clip_width = tab_width;
	if (tab_x + clip_width > in_rect.x + in_rect.width)
		clip_width = (in_rect.x + in_rect.width) - tab_x;

	// since the above code above doesn't play well with WXDFB or WXCOCOA,
	// we'll just use a rectangle for the clipping region for now --
	dc.SetClippingRegion(tab_x, tab_y, clip_width+1, tab_height-3);

	wxPoint border_points[6];
	if (m_flags &wxAUI_NB_BOTTOM) {
		border_points[0] = wxPoint(tab_x,             tab_y);
		border_points[1] = wxPoint(tab_x,             tab_y+tab_height-6);
		border_points[2] = wxPoint(tab_x+2,           tab_y+tab_height-4);
		border_points[3] = wxPoint(tab_x+tab_width-2, tab_y+tab_height-4);
		border_points[4] = wxPoint(tab_x+tab_width,   tab_y+tab_height-6);
		border_points[5] = wxPoint(tab_x+tab_width,   tab_y);

	} else { //if (m_flags & wxAUI_NB_TOP) {}

		if(page.active == false) {
			tab_height -= 2;
			tab_y      += 2;
		}

		border_points[0] = wxPoint(tab_x,             tab_y+tab_height-4);
		border_points[1] = wxPoint(tab_x,             tab_y+2);
		border_points[2] = wxPoint(tab_x+2,           tab_y);
		border_points[3] = wxPoint(tab_x+tab_width-2, tab_y);
		border_points[4] = wxPoint(tab_x+tab_width,   tab_y+2);
		border_points[5] = wxPoint(tab_x+tab_width,   tab_y+tab_height-4);
	}
	// TODO: else if (m_flags &wxAUI_NB_LEFT) {}
	// TODO: else if (m_flags &wxAUI_NB_RIGHT) {}

	int drawn_tab_yoff = border_points[1].y;
	int drawn_tab_height = border_points[0].y - border_points[1].y;


	if (page.active) {
		// draw active tab

		// draw base background color
		wxRect r(tab_x, tab_y, tab_width, tab_height);
		dc.SetPen(m_base_colour_pen);
		dc.SetBrush(m_base_colour_brush);
		dc.DrawRectangle(r.x, r.y+1, r.width-1, r.height-4);

		// this white helps fill out the gradient at the top of the tab

		dc.SetPen(m_base_colour);
		dc.SetBrush(m_base_colour);
		dc.DrawRectangle(r.x, r.y+1, r.width-1, r.height-4);

		// these two points help the rounded corners appear more antialiased
		dc.SetPen(m_base_colour_pen);
		dc.DrawPoint(r.x+2, r.y+1);
		dc.DrawPoint(r.x+r.width-2, r.y+1);

		// set rectangle down a bit for gradient drawing
		r.height -= 1;
		r.y      += 1;
		r.width  -= 2;
		r.x      += 1;
		
		// draw gradient background
		wxColor top_color = m_base_colour_4;
		wxColor bottom_color = m_bottom_rect_colour;
		dc.GradientFillLinear(r, bottom_color, top_color, wxNORTH);
	} else {
		// draw inactive tab
		

		wxRect r(tab_x, tab_y, tab_width, tab_height-2);

		// start the gradent up a bit and leave the inside border inset
		// by a pixel for a 3D look.  Only the top half of the inactive
		// tab will have a slight gradient
		r.x += 1;
		r.y++;
		r.width -= 1;
		r.height /= 2;
//		r.height;

		// -- draw top gradient fill for glossy look

		wxColor top_color    = m_shade_colour;
		wxColor bottom_color = m_shade_colour;
		dc.GradientFillLinear(r, bottom_color, top_color, wxNORTH);

		r.y += r.height;
		r.y--;

		// -- draw bottom fill for glossy look
		top_color    = m_shade_colour;
		bottom_color = m_shade_colour;
		dc.GradientFillLinear(r, top_color, bottom_color, wxSOUTH);
	}

	// draw tab outline
	dc.SetPen(m_border_pen);
	dc.SetBrush(*wxTRANSPARENT_BRUSH);
	dc.DrawPolygon(WXSIZEOF(border_points), border_points);

	// there are two horizontal grey lines at the bottom of the tab control,
	// this gets rid of the top one of those lines in the tab control
	if (page.active) {
		if (m_flags &wxAUI_NB_BOTTOM)
			dc.SetPen(wxPen(wxColour(wxAuiStepColour(m_base_colour, 170))));
		// TODO: else if (m_flags &wxAUI_NB_LEFT) {}
		// TODO: else if (m_flags &wxAUI_NB_RIGHT) {}
		else //for wxAUI_NB_TOP
			dc.SetPen(m_bottom_rect_colour);
		dc.DrawLine(border_points[0].x+1,
		            border_points[0].y,
		            border_points[5].x,
		            border_points[5].y);
	}


	int text_offset = tab_x + 8;
	int close_button_width = 0;
	if (close_button_state != wxAUI_BUTTON_STATE_HIDDEN) {
		close_button_width = m_active_close_bmp.GetWidth() + 4;
	}


	int bitmap_offset = 0;
	if (page.bitmap.IsOk()) {
		bitmap_offset = tab_x + 8;

		// draw bitmap
		dc.DrawBitmap(page.bitmap,
		              bitmap_offset,
		              drawn_tab_yoff + (drawn_tab_height/2) - (page.bitmap.GetHeight()/2),
		              true);

		text_offset = bitmap_offset + page.bitmap.GetWidth();
		text_offset += 3; // bitmap padding
	} else {
		text_offset = tab_x + 8;
	}


	wxString draw_text = wxAuiChopText(dc,
	                                   caption,
	                                   tab_width - (text_offset-tab_x) - close_button_width);

	// draw tab text
	dc.SetTextForeground( wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT) );
	dc.DrawText(draw_text,
	            text_offset,
	            drawn_tab_yoff + (drawn_tab_height)/2 - (texty/2) - 1);

	// draw close button if necessary
	if (close_button_state != wxAUI_BUTTON_STATE_HIDDEN) {
		wxBitmap bmp = m_disabled_close_bmp;

		if (close_button_state == wxAUI_BUTTON_STATE_HOVER ||
		        close_button_state == wxAUI_BUTTON_STATE_PRESSED) {
			bmp = m_active_close_bmp;
		}

		wxRect rect(tab_x + tab_width - close_button_width - 1,
		            tab_y + (tab_height/2) - (bmp.GetHeight()/2),
		            close_button_width,
		            tab_height);
		IndentPressedBitmap(&rect, close_button_state);
		dc.DrawBitmap(bmp, rect.x, rect.y-1, true);

		*out_button_rect = rect;
	}

	*out_tab_rect = wxRect(tab_x, tab_y, tab_width, tab_height);

#ifndef __WXMAC__
	// draw focus rectangle
	if (page.active && (wnd->FindFocus() == wnd)) {
		wxRect focusRectText(text_offset, (drawn_tab_yoff + (drawn_tab_height)/2 - (texty/2) - 1),
		                     selected_textx, selected_texty);

		wxRect focusRect;
		wxRect focusRectBitmap;

		if (page.bitmap.IsOk())
			focusRectBitmap = wxRect(bitmap_offset, drawn_tab_yoff + (drawn_tab_height/2) - (page.bitmap.GetHeight()/2),
			                         page.bitmap.GetWidth(), page.bitmap.GetHeight());

		if (page.bitmap.IsOk() && draw_text.IsEmpty())
			focusRect = focusRectBitmap;
		else if (!page.bitmap.IsOk() && !draw_text.IsEmpty())
			focusRect = focusRectText;
		else if (page.bitmap.IsOk() && !draw_text.IsEmpty())
			focusRect = focusRectText.Union(focusRectBitmap);

		focusRect.Inflate(2, 2);
	}
#endif

	dc.DestroyClippingRegion();
}

int clAuiTabArt::GetIndentSize()
{
	return 5;
}

wxSize clAuiTabArt::GetTabSize(wxDC& dc,
                               wxWindow* WXUNUSED(wnd),
                               const wxString& caption,
                               const wxBitmap& bitmap,
                               bool WXUNUSED(active),
                               int close_button_state,
                               int* x_extent)
{
	static wxCoord measured_texty(wxNOT_FOUND);

	wxCoord measured_textx;
	wxCoord tmp;

	dc.SetFont(m_measuring_font);
	dc.GetTextExtent(caption, &measured_textx, &tmp);

	// do it once
	if(measured_texty == wxNOT_FOUND)
		dc.GetTextExtent(wxT("ABCDEFXj"), &tmp, &measured_texty);

	// add padding around the text
	wxCoord tab_width  = measured_textx;
	wxCoord tab_height = measured_texty;

	// if the close button is showing, add space for it
	if (close_button_state != wxAUI_BUTTON_STATE_HIDDEN)
		tab_width += m_active_close_bmp.GetWidth() + 3;

	// if there's a bitmap, add space for it
	if (bitmap.IsOk()) {
		tab_width += bitmap.GetWidth();
		tab_width += 3; // right side bitmap padding
		tab_height = wxMax(tab_height, bitmap.GetHeight());
	}

	// add padding
#ifdef __WXMAC__
	tab_width += 16;
#else
	tab_width += 16;
#endif
	tab_height += 12;

	if (m_flags & wxAUI_NB_TAB_FIXED_WIDTH) {
		tab_width = m_fixed_tab_width;
	}

	*x_extent = tab_width;

	return wxSize(tab_width, tab_height);
}


void clAuiTabArt::DrawButton(wxDC& dc,
                             wxWindow* WXUNUSED(wnd),
                             const wxRect& in_rect,
                             int bitmap_id,
                             int button_state,
                             int orientation,
                             wxRect* out_rect)
{
	wxBitmap bmp;
	wxRect rect;

	switch (bitmap_id) {
	case wxAUI_BUTTON_CLOSE:
		if (button_state & wxAUI_BUTTON_STATE_DISABLED)
			bmp = m_disabled_close_bmp;
		else
			bmp = m_active_close_bmp;
		break;
	case wxAUI_BUTTON_LEFT:
		if (button_state & wxAUI_BUTTON_STATE_DISABLED)
			bmp = m_disabled_left_bmp;
		else
			bmp = m_active_left_bmp;
		break;
	case wxAUI_BUTTON_RIGHT:
		if (button_state & wxAUI_BUTTON_STATE_DISABLED)
			bmp = m_disabled_right_bmp;
		else
			bmp = m_active_right_bmp;
		break;
	case wxAUI_BUTTON_WINDOWLIST:
		if (button_state & wxAUI_BUTTON_STATE_DISABLED)
			bmp = m_disabled_windowlist_bmp;
		else
			bmp = m_active_windowlist_bmp;
		break;
	}


	if (!bmp.IsOk())
		return;

	rect = in_rect;

	if (orientation == wxLEFT) {
		rect.SetX(in_rect.x);
		rect.SetY(((in_rect.y + in_rect.height)/2) - (bmp.GetHeight()/2));
		rect.SetWidth(bmp.GetWidth());
		rect.SetHeight(bmp.GetHeight());
	} else {
		rect = wxRect(in_rect.x + in_rect.width - bmp.GetWidth(),
		              ((in_rect.y + in_rect.height)/2) - (bmp.GetHeight()/2),
		              bmp.GetWidth()+4, bmp.GetHeight());
	}

	IndentPressedBitmap(&rect, button_state);
	dc.DrawBitmap(bmp, rect.x, rect.y, true);

	*out_rect = rect;
}


int clAuiTabArt::ShowDropDown(wxWindow* wnd,
                              const wxAuiNotebookPageArray& pages,
                              int active_idx)
{
	wxMenu menuPopup;

	size_t i, count = pages.GetCount();
	for (i = 0; i < count; ++i) {
		const wxAuiNotebookPage& page = pages.Item(i);
		wxString caption = page.caption;

		// if there is no caption, make it a space.  This will prevent
		// an assert in the menu code.
		if (caption.IsEmpty())
			caption = wxT(" ");

		menuPopup.AppendCheckItem(1000+i, caption);
	}

	if (active_idx != -1) {
		menuPopup.Check(1000+active_idx, true);
	}

	// find out where to put the popup menu of window items
	wxPoint pt = ::wxGetMousePosition();
	pt = wnd->ScreenToClient(pt);

	// find out the screen coordinate at the bottom of the tab ctrl
	wxRect cli_rect = wnd->GetClientRect();
	pt.y = cli_rect.y + cli_rect.height;

	wxAuiCommandCapture* cc = new wxAuiCommandCapture;
	wnd->PushEventHandler(cc);
	wnd->PopupMenu(&menuPopup, pt);
	int command = cc->GetCommandId();
	wnd->PopEventHandler(true);

	if (command >= 1000)
		return command-1000;

	return -1;
}

int clAuiTabArt::GetBestTabCtrlSize(wxWindow* wnd,
                                    const wxAuiNotebookPageArray& pages,
                                    const wxSize& required_bmp_size)
{
	wxClientDC dc(wnd);
	dc.SetFont(m_measuring_font);

	// sometimes a standard bitmap size needs to be enforced, especially
	// if some tabs have bitmaps and others don't.  This is important because
	// it prevents the tab control from resizing when tabs are added.
	wxBitmap measure_bmp;
	if (required_bmp_size.IsFullySpecified()) {
		measure_bmp.Create(required_bmp_size.x,
		                   required_bmp_size.y);
	}


	int max_y = 0;
	size_t i, page_count = pages.GetCount();
	for (i = 0; i < page_count; ++i) {
		wxAuiNotebookPage& page = pages.Item(i);

		wxBitmap bmp;
		if (measure_bmp.IsOk())
			bmp = measure_bmp;
		else
			bmp = page.bitmap;

		// we don't use the caption text because we don't
		// want tab heights to be different in the case
		// of a very short piece of text on one tab and a very
		// tall piece of text on another tab
		int x_ext = 0;
		wxSize s = GetTabSize(dc,
		                      wnd,
		                      wxT("ABCDEFGHIj"),
		                      bmp,
		                      true,
		                      wxAUI_BUTTON_STATE_HIDDEN,
		                      &x_ext);

		max_y = wxMax(max_y, s.y);
	}

	return max_y+2;
}

void clAuiTabArt::SetNormalFont(const wxFont& font)
{
	m_normal_font = font;
}

void clAuiTabArt::SetSelectedFont(const wxFont& font)
{
}

void clAuiTabArt::SetMeasuringFont(const wxFont& font)
{
//	m_measuring_font = font;
}
