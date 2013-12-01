#include "cl_aui_notebook_art.h"

#include "editor_config.h"
#include <wx/dcgraph.h>
#include "plugin_general_wxcp.h"
#include <wx/dcmemory.h>
#include <editor_config.h>
#include "globals.h"
#include "cl_command_event.h"
#include "plugin.h"
#include "event_notifier.h"

static const wxDouble X_RADIUS = 6.0;
static const wxDouble X_DIAMETER = 2 * X_RADIUS;

#ifdef __WXMAC__
#   include <wx/osx/private.h>
#endif

clAuiGlossyTabArt::clAuiGlossyTabArt()
{
}

clAuiGlossyTabArt::~clAuiGlossyTabArt()
{
}

void clAuiGlossyTabArt::DrawBackground(wxDC& dc, wxWindow* wnd, const wxRect& rect)
{
    wxUnusedVar(wnd);
#ifdef __WXGTK__
    wxDC &gdc = dc;
#else
    wxGCDC gdc;
    if ( !DrawingUtils::GetGCDC(dc, gdc) )
        return;
#endif

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
    gdc.SetPen( penColour );
    
    wxPoint ptBottomLeft  = rect.GetBottomLeft();
    wxPoint ptBottomRight = rect.GetBottomRight();
    gdc.DrawLine(ptBottomLeft, ptBottomRight);
}

void clAuiGlossyTabArt::DrawTab(wxDC& dc,
                          wxWindow* wnd,
                          const wxAuiNotebookPage& page,
                          const wxRect& in_rect,
                          int close_button_state,
                          wxRect* out_tab_rect,
                          wxRect* out_button_rect,
                          int* x_extent)
{
    wxColour bgColour = wxColour(EditorConfigST::Get()->GetCurrentOutputviewBgColour());
    wxColour penColour;
    wxColour textColour;
    bool isBgColourDark = DrawingUtils::IsDark(bgColour);
    if ( isBgColourDark ) {
        penColour = DrawingUtils::LightColour(bgColour, 4.0);
    } else {
        if ( !page.active ) {
            bgColour = DrawingUtils::LightColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE), 2.0);
        } else {
            // active page
#ifdef __WXMAC__
            bgColour = wxColour( wxMacCreateCGColorFromHITheme(kThemeBrushToolbarBackground));
            bgColour = bgColour.ChangeLightness(88);
#else
            bgColour = DrawingUtils::DarkColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE), 0.0);
#endif
        }
        penColour = wxSystemSettings::GetColour(wxSYS_COLOUR_3DSHADOW);
    }
    
    if ( page.active ) {
        textColour = EditorConfigST::Get()->GetCurrentOutputviewFgColour();
    } else {
        textColour = isBgColourDark ? wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT) : EditorConfigST::Get()->GetCurrentOutputviewFgColour();
    }
    
    int curx = 0;
    
    // Allow the plugins to override the default colours
    clColourEvent colourEvent( wxEVT_COLOUR_TAB );
    colourEvent.SetIsActiveTab( page.active );
    colourEvent.SetPage( page.window );
    bool customDrawing = false;
    if ( EventNotifier::Get()->ProcessEvent( colourEvent ) ) {
        bgColour = colourEvent.GetBgColour();
        textColour = colourEvent.GetFgColour();
        customDrawing = true;
    }
    
#ifdef __WXGTK__
    // Under GTK, if _not_ using custom drawing
    // use the default AUI tab drawings (which uses the native GTK theme engines)
    if ( !customDrawing ) {
        wxAuiDefaultTabArt::DrawTab(dc, wnd, page, in_rect, close_button_state, out_tab_rect, out_button_rect, x_extent);
        return;
    }
#endif

    wxGCDC gdc;
    if ( !DrawingUtils::GetGCDC(dc, gdc) )
        return;

    wxGraphicsPath path = gdc.GetGraphicsContext()->CreatePath();
    gdc.SetPen( penColour );
    
    wxSize sz = GetTabSize(gdc, wnd, page.caption, page.bitmap, page.active, close_button_state, x_extent);
    
    wxRect rr (in_rect.GetTopLeft(), sz);
    rr.y += 2;
    rr.width -= 2;
#ifdef __WXGTK__
    rr.height += 12;
#else
    rr.height += 4;
#endif

    /// the tab start position (x)
    curx = rr.x + 8;
    
    // Set clipping region
    int clip_width = rr.width;
    if (rr.x + clip_width > in_rect.x + in_rect.width)
        clip_width = (in_rect.x + in_rect.width) - rr.x;
    
    // since the above code above doesn't play well with WXDFB or WXCOCOA,
    // we'll just use a rectangle for the clipping region for now --
    gdc.SetClippingRegion(rr.x, rr.y, clip_width, rr.height);
    path.AddRoundedRectangle(rr.x, rr.y, rr.width-1, rr.height, 3.0);
    
    gdc.SetBrush( bgColour );
    gdc.GetGraphicsContext()->FillPath( path );
    gdc.GetGraphicsContext()->StrokePath( path );
    
    if ( !page.active ) {
        // Draw a line at the bottom rect
        gdc.SetPen(penColour);
        gdc.DrawLine(in_rect.GetBottomLeft(), in_rect.GetBottomRight());
    }
    
    /// Draw the text
    wxString caption = page.caption;
    if ( caption.IsEmpty() ) {
        caption = "Tp";
    }
    
    wxFont fnt = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
    if ( page.active ) {
        fnt.SetWeight(wxFONTWEIGHT_BOLD);
    }
    gdc.SetFont( fnt );
    wxSize ext = gdc.GetTextExtent( caption );
    if ( caption == "Tp" )
        caption.Clear();
    
    gdc.SetTextForeground( textColour );
    gdc.GetGraphicsContext()->DrawText( page.caption, rr.x + 8, (rr.y + (rr.height - ext.y)/2)-2);
    
    // advance the X offset
    curx += ext.x;
    
    /// Draw the bitmap
    if ( page.bitmap.IsOk() ) {
        curx += 4;
        int bmpy = (rr.y + (rr.height - page.bitmap.GetHeight())/2);
        gdc.GetGraphicsContext()->DrawBitmap( page.bitmap, curx, bmpy, page.bitmap.GetWidth(), page.bitmap.GetHeight());
        curx += 8;
    }
    
    /// Draw the X button on the tab
    if ( close_button_state != wxAUI_BUTTON_STATE_HIDDEN ) {
        curx += 4;
        int btny = (rr.y + (rr.height/2));
        if ( close_button_state == wxAUI_BUTTON_STATE_PRESSED ) {
            curx += 1;
            btny += 1;
        }
        
        /// Defines the rectangle surrounding the X button
        wxRect xRect = wxRect(curx, btny - X_RADIUS, X_DIAMETER, X_DIAMETER);
        *out_button_rect = xRect;
        
        /// Defines the 'x' inside the circle
        wxPoint circleCenter( curx + X_RADIUS, btny);
        wxDouble xx_width = ::sqrt( ::pow(X_DIAMETER, 2.0) /2.0 );
        wxDouble x_square = (circleCenter.x - (xx_width/2.0));
        wxDouble y_square = (circleCenter.y - (xx_width/2.0));
        
        wxPoint2DDouble ptXTopLeft(x_square, y_square);
        wxRect2DDouble insideRect(ptXTopLeft.m_x, ptXTopLeft.m_y, xx_width, xx_width);
        insideRect.Inset(1.0 , 1.0); // Shrink it by 1 pixle
        
        /// Draw the 'x' itself
        wxGraphicsPath xpath = gdc.GetGraphicsContext()->CreatePath();
        xpath.MoveToPoint( insideRect.GetLeftTop() );
        xpath.AddLineToPoint( insideRect.GetRightBottom());
        xpath.MoveToPoint( insideRect.GetRightTop() );
        xpath.AddLineToPoint( insideRect.GetLeftBottom() );
        gdc.SetPen( wxPen(wxSystemSettings::GetColour(wxSYS_COLOUR_3DDKSHADOW), 2) );
        gdc.GetGraphicsContext()->StrokePath( xpath  );
        
        curx += X_DIAMETER;
    }
    *out_tab_rect = rr;
    gdc.DestroyClippingRegion();
}

wxSize clAuiGlossyTabArt::GetTabSize(wxDC& dc,
                               wxWindow* WXUNUSED(wnd),
                               const wxString& caption,
                               const wxBitmap& bitmap,
                               bool active,
                               int close_button_state,
                               int* x_extent)
{
    static wxCoord measured_texty(wxNOT_FOUND);

    wxCoord measured_textx;
    wxCoord tmp;
    
    wxFont f = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
    if ( active ) {
        f.SetWeight(wxFONTWEIGHT_BOLD);
    }
    
    dc.SetFont(f);
    dc.GetTextExtent(caption, &measured_textx, &tmp);

    // do it once
    if(measured_texty == wxNOT_FOUND)
        dc.GetTextExtent(wxT("ABCDEFXj"), &tmp, &measured_texty);

    // add padding around the text
    wxCoord tab_width  = measured_textx;
    wxCoord tab_height = measured_texty;

    // if the close button is showing, add space for it
    if (close_button_state != wxAUI_BUTTON_STATE_HIDDEN)
        tab_width += X_DIAMETER + 3;

    // if there's a bitmap, add space for it
    if (bitmap.IsOk()) {
        tab_width += bitmap.GetWidth();
        tab_width += 3; // right side bitmap padding
        tab_height = wxMax(tab_height, bitmap.GetHeight());
        tab_height += 10;
    } else {
        tab_height += 10;
    }

    // add padding
#ifdef __WXMAC__
    tab_width += 16;
#else
    tab_width += 16;
#endif


    if (m_flags & wxAUI_NB_TAB_FIXED_WIDTH) {
        tab_width = 80;
    }

    *x_extent = tab_width;

    return wxSize(tab_width, tab_height);
}
