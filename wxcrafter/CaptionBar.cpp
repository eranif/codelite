#include "CaptionBar.h"
#include "gui.h"
#include "wxguicraft_main_view.h"
#include <event_notifier.h>
#include <wx/dcbuffer.h>
#include <wx/dcmemory.h>
#include <wx/renderer.h>
#include <wx/settings.h>

CaptionBar::CaptionBar(wxWindow* parent, const wxString& caption, const wxString& style, const wxBitmap& icon)
    : CaptionBarBase(parent)
    , m_caption(caption)
    , m_style(style)
    , m_icon(icon)
{
    wxMemoryDC memDC;
    wxBitmap bmp(1, 1);
    memDC.SelectObject(bmp);
    memDC.SetFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));

    wxSize sz = memDC.GetTextExtent("Tp");
    SetSizeHints(-1, (sz.y > 22 ? sz.y : 22));
}

CaptionBar::~CaptionBar() {}
void CaptionBar::OnEraseBG(wxEraseEvent& event) { wxUnusedVar(event); }

void CaptionBar::OnPaint(wxPaintEvent& event)
{
    wxcImages myImages;
    wxUnusedVar(event);
    wxBufferedPaintDC dc(this);
    wxColour penColour = wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);
    wxColour bgColour1 = wxSystemSettings::GetColour(wxSYS_COLOUR_GRADIENTACTIVECAPTION);
    wxColour bgColour2 = wxSystemSettings::GetColour(wxSYS_COLOUR_ACTIVECAPTION);

    PrepareDC(dc);

    dc.SetPen(penColour);
    dc.SetBrush(penColour);
    dc.DrawRectangle(GetClientRect());

// Draw gradient from one side to another
#ifdef __WXMSW__
    dc.GradientFillLinear(GetClientRect(), bgColour2, bgColour1);

#elif defined(__WXMAC__)
    bgColour1 = wxSystemSettings::GetColour(wxSYS_COLOUR_3DDKSHADOW);
    bgColour2 = wxSystemSettings::GetColour(wxSYS_COLOUR_3DSHADOW);
    dc.GradientFillLinear(GetClientRect(), bgColour2, bgColour1, wxSOUTH);

#else
    dc.GradientFillLinear(GetClientRect(), bgColour2, bgColour2);
#endif

    wxCoord curx = 5;
    if(m_icon.IsOk()) {
        // Draw the frame icon
        wxCoord bmpy = (GetClientRect().GetHeight() - m_icon.GetHeight()) / 2;
        dc.DrawBitmap(m_icon, 5, bmpy);
        curx += m_icon.GetWidth();
        curx += 5; // spacer
    }

    // Draw the text
    wxColour textColour = wxSystemSettings::GetColour(wxSYS_COLOUR_CAPTIONTEXT);
    wxFont font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
    dc.SetFont(font);
    dc.SetTextForeground(textColour);

    wxSize textSize = dc.GetTextExtent(m_caption);

    wxRect clientRect = GetClientRect();
    wxCoord texty = (clientRect.GetHeight() - textSize.GetHeight()) / 2;
    dc.DrawText(m_caption, clientRect.GetTopLeft().x + curx, clientRect.GetTopLeft().y + texty);

    wxArrayString styles = ::wxStringTokenize(m_style, "|", wxTOKEN_STRTOK);
    bool draw_minimize_button = false;
    bool draw_maximize_button = false;
    bool draw_close_button = false;

    int bmpSize = 0;

    const wxBitmap& bmpClose = myImages.Bitmap("button_close");
    if(styles.Index("wxCLOSE_BOX") != wxNOT_FOUND || styles.Index("wxDEFAULT_DIALOG_STYLE") != wxNOT_FOUND ||
       styles.Index("wxDEFAULT_FRAME_STYLE") != wxNOT_FOUND) {
        // draw close box
        draw_close_button = true;
        bmpSize += bmpClose.GetWidth();
    }

    if(styles.Index("wxMINIMIZE_BOX") != wxNOT_FOUND || styles.Index("wxDEFAULT_FRAME_STYLE") != wxNOT_FOUND) {
        // draw close box
        draw_minimize_button = true;
        bmpSize += bmpClose.GetWidth();
    }

    if(styles.Index("wxMAXIMIZE_BOX") != wxNOT_FOUND || styles.Index("wxDEFAULT_FRAME_STYLE") != wxNOT_FOUND) {
        // draw close box
        draw_maximize_button = true;
        bmpSize += bmpClose.GetWidth();
    }

    // Draw the buttons

    int button_x = clientRect.GetWidth() - bmpSize - 2;
    int button_y = (clientRect.GetHeight() - bmpClose.GetHeight()) / 2;
    if(draw_minimize_button) {
        dc.DrawBitmap(myImages.Bitmap("button_minimize"), button_x, button_y, true);
        button_x += bmpClose.GetWidth();
    }

    if(draw_maximize_button) {
        dc.DrawBitmap(myImages.Bitmap("button_maximize"), button_x, button_y, true);
        button_x += bmpClose.GetWidth();
    }

    if(draw_close_button) {
        dc.DrawBitmap(myImages.Bitmap("button_close"), button_x, button_y, true);
        button_x += bmpClose.GetWidth();
    }
}

void CaptionBar::OnLeftDown(wxMouseEvent& event)
{
    event.Skip();
    wxCommandEvent evt(wxEVT_WXC_SELECT_TREE_TLW);
    EventNotifier::Get()->AddPendingEvent(evt);
}
