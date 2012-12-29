#include "cl_aui_dock_art.h"
#include "imanager.h"
#include <wx/dcmemory.h>

// --------------------------------------------

static wxString wxAuiChopText(wxDC& dc, const wxString& text, int max_size)
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

// ------------------------------------------------------------


clAuiDockArt::clAuiDockArt(IManager *manager)
    : m_manager(manager)
{
    BitmapLoader *bl = m_manager->GetStdIcons();
    m_bmpClose   = bl->LoadBitmap("aui/close");
    m_bmpCloseInactive = bl->LoadBitmap("aui/close-inactive");
}

clAuiDockArt::~clAuiDockArt()
{
}

void clAuiDockArt::DrawPaneButton(wxDC& dc, wxWindow *WXUNUSED(window),
                                  int button,
                                  int button_state,
                                  const wxRect& _rect,
                                  wxAuiPaneInfo& pane)
{
    wxBitmap bmp;
    if (!(&pane))
        return;
        
    switch (button) {
    default:
    case wxAUI_BUTTON_CLOSE:
        if (pane.state & wxAuiPaneInfo::optionActive)
            bmp = m_bmpClose;
        else
            bmp = m_bmpCloseInactive;
        break;
    case wxAUI_BUTTON_PIN:
        if (pane.state & wxAuiPaneInfo::optionActive)
            bmp = m_activePinBitmap;
        else
            bmp = m_inactivePinBitmap;
        break;
    case wxAUI_BUTTON_MAXIMIZE_RESTORE:
        if (pane.IsMaximized()) {
            if (pane.state & wxAuiPaneInfo::optionActive)
                bmp = m_activeRestoreBitmap;
            else
                bmp = m_inactiveRestoreBitmap;
        } else {
            if (pane.state & wxAuiPaneInfo::optionActive)
                bmp = m_activeMaximizeBitmap;
            else
                bmp = m_inactiveMaximizeBitmap;
        }
        break;
    }


    wxRect rect = _rect;

    int old_y = rect.y;
    rect.y = rect.y + (rect.height/2) - (bmp.GetHeight()/2);
    rect.height = old_y + rect.height - rect.y - 1;

    if (button_state == wxAUI_BUTTON_STATE_HOVER || button_state == wxAUI_BUTTON_STATE_PRESSED) {
        if (pane.state & wxAuiPaneInfo::optionActive) {
            dc.SetBrush(wxBrush(m_activeCaptionColour.ChangeLightness(120)));
            dc.SetPen(wxPen(m_activeCaptionColour.ChangeLightness(70)));
        } else {
            dc.SetBrush(wxBrush(m_inactiveCaptionColour.ChangeLightness(120)));
            dc.SetPen(wxPen(m_inactiveCaptionColour.ChangeLightness(70)));
        }
    }


    // draw the button itself
    if ( bmp.IsOk() )
        dc.DrawBitmap(bmp, rect.x, rect.y, true);
}

void clAuiDockArt::DrawCaption(wxDC& dc, wxWindow* window, const wxString& text, const wxRect& rect, wxAuiPaneInfo& pane)
{
    wxRect tmpRect(wxPoint(0, 0), rect.GetSize());
    
    if ( tmpRect.GetSize() == wxSize(0, 0) )
        tmpRect.SetSize(wxSize(1, 1)); // or it will assert on linux
    
    wxBitmap bmp(tmpRect.GetSize());
    wxMemoryDC memDc;
    memDc.SelectObject(bmp);

    memDc.SetPen(*wxTRANSPARENT_PEN);
    memDc.SetFont(m_captionFont);

    DrawCaptionBackground(memDc, tmpRect, (pane.state & wxAuiPaneInfo::optionActive) ? true : false);

    int caption_offset = 0;
    if ( pane.icon.IsOk() ) {
        DrawIcon(memDc, tmpRect, pane);
        caption_offset += pane.icon.GetWidth() + 3;
    }

    if (pane.state & wxAuiPaneInfo::optionActive)
        memDc.SetTextForeground(m_activeCaptionTextColour);
    else
        memDc.SetTextForeground(m_inactiveCaptionTextColour);

    wxCoord w,h;
    memDc.GetTextExtent(wxT("ABCDEFHXfgkj"), &w, &h);

    wxRect clip_rect = tmpRect;
    clip_rect.width -= 3; // text offset
    clip_rect.width -= 2; // button padding
    if (pane.HasCloseButton())
        clip_rect.width -= m_buttonSize;
    if (pane.HasPinButton())
        clip_rect.width -= m_buttonSize;
    if (pane.HasMaximizeButton())
        clip_rect.width -= m_buttonSize;

    wxString draw_text = wxAuiChopText(memDc, text, clip_rect.width);

    memDc.SetClippingRegion(clip_rect);
    memDc.DrawText(draw_text, tmpRect.x+3 + caption_offset, tmpRect.y+(tmpRect.height/2)-(h/2)-1);
    memDc.DestroyClippingRegion();
    memDc.SelectObject(wxNullBitmap);
    dc.DrawBitmap( bmp, rect.x, rect.y, true );
}
