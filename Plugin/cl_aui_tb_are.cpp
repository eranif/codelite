#include "cl_aui_tb_are.h"
#include <wx/settings.h>
#include "editor_config.h"

#if USE_AUI_TOOLBAR

CLMainAuiTBArt::CLMainAuiTBArt()
{
}

CLMainAuiTBArt::~CLMainAuiTBArt()
{
}

void CLMainAuiTBArt::DrawBackground(wxDC& dc, wxWindow* wnd, const wxRect& rect)
{
    wxColour bgColour = wxColour(EditorConfigST::Get()->GetCurrentOutputviewBgColour());
    wxColour borderUp, borderDown;

    // Determine the pen colour
    if ( !DrawingUtils::IsDark(bgColour)) {
        wxAuiDefaultToolBarArt::DrawBackground(dc, wnd, rect);
        return;
    }
    
    // Dark theme
    borderUp   = bgColour;
    borderDown = bgColour;
    wxColour bgColour2 = bgColour;
    dc.SetPen(bgColour);
    dc.SetBrush(bgColour);
    dc.DrawRectangle(rect);
    dc.GradientFillLinear(rect, bgColour2, bgColour, wxSOUTH);
    
    dc.SetPen(borderUp);
    dc.DrawLine(rect.GetLeftBottom(), rect.GetLeftTop());
    dc.DrawLine(rect.GetTopLeft(), rect.GetTopRight());
    
    dc.SetPen(borderDown);
    dc.DrawLine(rect.GetTopRight(), rect.GetBottomRight());
    dc.DrawLine(rect.GetBottomLeft(), rect.GetBottomRight());
}
#endif
