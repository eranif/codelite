#include "cl_aui_dock_art.h"
#include "imanager.h"
#include <wx/dcmemory.h>
#include <wx/settings.h>
#include <editor_config.h>
#include "globals.h"

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
    
    // Hackishly prevent assertions on linux
    if (tmpRect.GetHeight() == 0)
        tmpRect.SetHeight(1);
    if (tmpRect.GetWidth() == 0)
        tmpRect.SetWidth(1);
    
    wxBitmap bmp(tmpRect.GetSize());
    wxMemoryDC memDc;
    memDc.SelectObject(bmp);

    memDc.SetPen(*wxTRANSPARENT_PEN);
    memDc.SetFont(m_captionFont);
    
    wxColour bgColour = wxColour(EditorConfigST::Get()->GetCurrentOutputviewBgColour());
    bool isDarkTheme = DrawingUtils::IsDark(bgColour);
    
    if ( !isDarkTheme ) {
        DrawCaptionBackground(memDc, tmpRect, (pane.state & wxAuiPaneInfo::optionActive) ? true : false);
        
    } else {
        memDc.SetPen( *wxBLACK );
        memDc.SetBrush( bgColour );
        memDc.DrawRectangle( tmpRect );
    }
    
    int caption_offset = 0;
    if ( pane.icon.IsOk() ) {
        DrawIcon(memDc, tmpRect, pane);
        caption_offset += pane.icon.GetWidth() + 3;
    } else {
        caption_offset = 3;
    }
    
    if ( !isDarkTheme ) {
        if (pane.state & wxAuiPaneInfo::optionActive)
            memDc.SetTextForeground(m_activeCaptionTextColour);
        else
            memDc.SetTextForeground(m_inactiveCaptionTextColour);
    } else {
        memDc.SetTextForeground( EditorConfigST::Get()->GetCurrentOutputviewFgColour() );
    }
    
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
    
    wxSize textSize = memDc.GetTextExtent(draw_text);
    
    wxColour borderUp, borderDown;

    // Determine the pen colour
    if ( !isDarkTheme ) {
        borderUp = *wxWHITE;
        borderDown = wxSystemSettings::GetColour(wxSYS_COLOUR_3DSHADOW);
        
    } else {
        borderUp = DrawingUtils::LightColour(bgColour, 1.0);
        borderDown = DrawingUtils::DarkColour(bgColour, 1.0);
    }
    
    //memDc.SetClippingRegion(clip_rect);
    memDc.DrawText(draw_text, tmpRect.x+3 + caption_offset, tmpRect.y+((tmpRect.height - textSize.y)/2));
    memDc.SetPen(borderUp);
    memDc.DrawLine(tmpRect.GetTopLeft(), tmpRect.GetTopRight());
    memDc.DrawLine(tmpRect.GetTopLeft(), tmpRect.GetBottomLeft());
    
    memDc.SetPen( borderDown );
    memDc.DrawLine(tmpRect.GetTopRight(), tmpRect.GetBottomRight());
    memDc.DrawLine(tmpRect.GetBottomLeft(), tmpRect.GetBottomRight());
    
    //memDc.DestroyClippingRegion();
    memDc.SelectObject(wxNullBitmap);
    dc.DrawBitmap( bmp, rect.x, rect.y, true );
}

void clAuiDockArt::DrawBackground(wxDC& dc, wxWindow* window, int orientation, const wxRect& rect)
{
    wxColour bgColour = wxColour(EditorConfigST::Get()->GetCurrentOutputviewBgColour());
    wxColour penColour;
    
    // Determine the pen colour
    if ( DrawingUtils::IsDark(bgColour)) {
        penColour = DrawingUtils::LightColour(bgColour, 4.0);
    } else {
        penColour = wxSystemSettings::GetColour(wxSYS_COLOUR_3DSHADOW);
    }
    
    // Now set the bg colour. It must be done after setting 
    // the pen colour
    if ( !DrawingUtils::IsDark(bgColour) ) {
        bgColour = wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);
    } else {
        bgColour = DrawingUtils::LightColour(bgColour, 3.0);
    }
    
    dc.SetPen(bgColour);
    dc.SetBrush( bgColour );
    dc.DrawRectangle(rect);
}

void clAuiDockArt::DrawBorder(wxDC& dc, wxWindow* window, const wxRect& rect, wxAuiPaneInfo& pane)
{
//    wxUnusedVar(dc);
//    wxUnusedVar(window);
//    wxUnusedVar(rect);
//    wxUnusedVar(pane);
//    return;
    wxColour bgColour = wxColour(EditorConfigST::Get()->GetCurrentOutputviewBgColour());
    wxColour penColour;
    
    // Determine the pen colour
    if ( !DrawingUtils::IsDark(bgColour)) {
        wxAuiDefaultDockArt::DrawBorder(dc, window, rect, pane);
        return;
    }
    
    penColour = DrawingUtils::LightColour(bgColour, 4.0);
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush(*wxTRANSPARENT_BRUSH);
    dc.DrawRectangle(rect);
    
}

void clAuiDockArt::DrawSash(wxDC& dc, wxWindow* window, int orientation, const wxRect& rect)
{
    // dark theme
#if defined(__WXMAC__)
    wxAuiDefaultDockArt::DrawSash(dc, window, orientation, rect);
    return;
#endif
    
    // Prepare a stipple bitmap
    wxColour bgColour = ::GetAUIPaneBGColour();
    
    // MSW
    wxUnusedVar(window);
    wxUnusedVar(orientation);
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush( ::GetAUIStippleBrush() );
    dc.DrawRectangle(rect);
}
