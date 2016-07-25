#include "clBitmap.h"
#include <wx/dcscreen.h>
#include <wx/filename.h>
#include <wx/window.h>
#ifdef __WXGTK20__
#include <gtk/gtk.h>
#endif

clBitmap::clBitmap() {}

clBitmap::~clBitmap() {}

bool clBitmap::LoadFile(const wxString& name, wxBitmapType type)
{
    wxFileName filename(name);
#ifndef __WXOSX__
    if(ShouldLoadHiResImages()) {
        wxFileName hiResFileName = filename;
        wxString hiresName = hiResFileName.GetName();
        hiresName << "@2x";
        hiResFileName.SetName(hiresName);
        if(hiResFileName.Exists()) {
            filename = hiResFileName;
        }
    }
#endif
    return wxBitmap::LoadFile(filename.GetFullPath(), type);
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
