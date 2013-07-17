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
    wxColour bgColour = wxColour(EditorConfigST::Get()->GetCurrentOutputviewBgColour());
    wxColour borderUp, borderDown;

    // Determine the pen colour
    if ( !DrawingUtils::IsDark(bgColour)) {
        bgColour = wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);
        borderUp = *wxWHITE;
        borderDown = wxSystemSettings::GetColour(wxSYS_COLOUR_3DSHADOW);
        
    } else {
        borderUp = DrawingUtils::LightColour(bgColour, 1.0);
        borderDown = DrawingUtils::DarkColour(bgColour, 1.0);
    }

#if wxCHECK_VERSION(2, 9, 5)
    // Now set the bg colour. It must be done after setting
    // the pen colour
    wxColour bgColour2 = bgColour;
    bgColour = DrawingUtils::DarkColour(bgColour, 2.0);
     
    dc.SetPen(bgColour);
    dc.SetBrush(bgColour);
    dc.DrawRectangle(rect);
    dc.GradientFillLinear(rect, bgColour2, bgColour, wxSOUTH);
#else
    wxColour bgColour2 = bgColour;
    bgColour = DrawingUtils::DarkColour(bgColour, 2.0);
     
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
#endif
}

void CLMainAuiTBArt::DrawGripper(wxDC& dc, wxWindow* wnd, const wxRect& rect)
{
    wxColour bgColour = wxColour(EditorConfigST::Get()->GetCurrentOutputviewBgColour());

    if ( !DrawingUtils::IsDark(bgColour)) {
        wxAuiDefaultToolBarArt::DrawGripper(dc, wnd, rect);
        return;
    }

    wxColour gp1, gp2, gp3;
    gp3 = DrawingUtils::LightColour(bgColour, 1.0);
    gp2 = DrawingUtils::LightColour(bgColour, 2.0);
    gp1 = DrawingUtils::LightColour(bgColour, 3.0);

    int i = 0;
    while (1) {
        int x, y;

        if (m_flags & wxAUI_TB_VERTICAL) {
            x = rect.x + (i*4) + 5;
            y = rect.y + 3;
            if (x > rect.GetWidth()-5)
                break;
        } else {
            x = rect.x + 3;
            y = rect.y + (i*4) + 5;
            if (y > rect.GetHeight()-5)
                break;
        }

        dc.SetPen(gp1);
        dc.DrawPoint(x, y);
        dc.SetPen(gp2);
        dc.DrawPoint(x, y+1);
        dc.DrawPoint(x+1, y);
        dc.SetPen(gp3);
        dc.DrawPoint(x+2, y+1);
        dc.DrawPoint(x+2, y+2);
        dc.DrawPoint(x+1, y+2);

        i++;
    }

}

void CLMainAuiTBArt::DrawSeparator(wxDC& dc, wxWindow* wnd, const wxRect& _rect)
{
    wxColour bgColour = wxColour(EditorConfigST::Get()->GetCurrentOutputviewBgColour());

    if ( !DrawingUtils::IsDark(bgColour)) {
        wxAuiDefaultToolBarArt::DrawSeparator(dc, wnd, _rect);
        return;
    }

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

    wxColour startColour = bgColour.ChangeLightness(80);
    wxColour endColour = bgColour.ChangeLightness(80);
    dc.GradientFillLinear(rect, startColour, endColour, horizontal ? wxSOUTH : wxEAST);
}

#endif
