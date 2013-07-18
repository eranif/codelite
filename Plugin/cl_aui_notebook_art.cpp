#include "cl_aui_notebook_art.h"

#include "editor_config.h"
#include <wx/dcgraph.h>
#include "plugin_general_wxcp.h"
#include <wx/dcmemory.h>
#include <editor_config.h>
#include "globals.h"

clAuiGlossyTabArt::clAuiGlossyTabArt()
{
}

clAuiGlossyTabArt::~clAuiGlossyTabArt()
{
}

void clAuiGlossyTabArt::DrawBackground(wxDC& dc, wxWindow* wnd, const wxRect& rect)
{
    wxUnusedVar(wnd);
    wxGCDC gdc;
    if ( !DrawingUtils::GetGCDC(dc, gdc) )
        return;
        
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
    bgColour = DrawingUtils::GetAUIPaneBGColour();
    
    gdc.SetPen(bgColour);
    gdc.SetBrush( DrawingUtils::GetStippleBrush() );
    gdc.DrawRectangle(rect);
    gdc.SetPen(penColour);
    gdc.DrawLine(rect.GetBottomLeft(), rect.GetBottomRight());
}
