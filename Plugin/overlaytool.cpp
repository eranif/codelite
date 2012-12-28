#include <wx/dcmemory.h>
#include "overlaytool.h"
#include <wx/xrc/xmlres.h>
#include <wx/settings.h>
#include "bitmap_loader.h"

wxBitmap OverlayTool::ms_bmpOK;
wxBitmap OverlayTool::ms_bmpConflict;
wxBitmap OverlayTool::ms_bmpModified;

OverlayTool::OverlayTool()
{
    BitmapLoader bl;
    ms_bmpOK       = bl.LoadBitmap("overlay/16/ok");
    ms_bmpModified = bl.LoadBitmap("overlay/16/modified");
    ms_bmpConflict = bl.LoadBitmap("overlay/16/conflicted");
} 

OverlayTool::~OverlayTool()
{
}

wxBitmap OverlayTool::DoAddBitmap(const wxBitmap& bmp, const wxBitmap& overlayBmp) const
{
    wxMemoryDC dcMem;
#ifdef __WXMSW__
    wxBitmap bitmap = bmp;
    dcMem.SelectObject(bitmap);
#else
    wxColour col = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);
    wxBitmap bitmap(16, 16);
    dcMem.SelectObject(bitmap);
    dcMem.SetPen( wxPen(col) );
    dcMem.SetBrush( wxBrush(col) );
    dcMem.DrawRectangle(wxPoint(0, 0), wxSize(16, 16));
    dcMem.DrawBitmap(bmp, wxPoint(0, 0));
#endif
    dcMem.DrawBitmap(overlayBmp, wxPoint(0, 0), true);
    dcMem.SelectObject(wxNullBitmap);

    return bitmap;
}

OverlayTool& OverlayTool::Get()
{
    static OverlayTool theTool;
    return theTool;
}

wxBitmap OverlayTool::CreateBitmap(const wxBitmap& orig, OverlayTool::BmpType type) const
{
    switch( type ) {
    case Bmp_Conflict:
        return DoAddBitmap(orig, ms_bmpConflict);
    case Bmp_OK:
        return DoAddBitmap(orig, ms_bmpOK);
    case Bmp_Modified:
        return DoAddBitmap(orig, ms_bmpModified);
    default:
        return orig;
    }
}

