#include "clBitmap.h"
#if wxUSE_GUI
#include <wx/dcscreen.h>
#include <wx/filename.h>
#include <wx/window.h>
#ifdef __WXGTK20__
#include <gtk/gtk.h>
#endif

clBitmap::clBitmap() {}

clBitmap::clBitmap(const wxImage& img, double scale)
#if wxVERSION_NUMBER >= 3100 
    : wxBitmap(img, -1, scale)
#else
    : wxBitmap(img, -1)
#endif
{
}

clBitmap::~clBitmap() {}

bool clBitmap::LoadFile(const wxString& name, wxBitmapType type)
{
#if defined(__WXMSW__)
    wxFileName filename(name);
    double scale = 1.0;
    if(ShouldLoadHiResImages()) {
        wxFileName hiResFileName = filename;
        wxString hiresName = hiResFileName.GetName();
        hiresName << "@2x";
        hiResFileName.SetName(hiresName);
        if(hiResFileName.Exists()) {
            filename = hiResFileName;
            scale = 2.0;
        }
    }
    wxImage img(filename.GetFullPath(), type);
    *this = clBitmap(img, scale);
    return true;
#else
    return wxBitmap::LoadFile(name, type);
#endif
}

bool clBitmap::ShouldLoadHiResImages()
{
    static bool once = false;
    static bool shouldLoad = false;
    if(!once) {
        once = true;
#ifdef __WXGTK__
        GdkScreen *screen = gdk_screen_get_default();
        if(screen) {
            double res = gdk_screen_get_resolution(screen);
            shouldLoad = ((res / 96.) >= 1.5); 
        }
#else
        shouldLoad = ((wxScreenDC().GetPPI().y / 96.) >= 1.5);
#endif
    }
    return shouldLoad;
}
#endif // LIBCODELITE_WITH_UI
