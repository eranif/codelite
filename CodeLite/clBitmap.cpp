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
        // try the environment variable approach first
        wxString dpiscale = "1.0";
        if(wxGetEnv("GDK_DPI_SCALE", &dpiscale)) {
            double scale = 1.0;
            if(dpiscale.ToDouble(&scale)) {
                shouldLoad = (scale >= 1.5);
                return shouldLoad;
            }
        }

        // Try the GTK way
        GdkScreen* screen = gdk_screen_get_default();
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

bool clBitmap::LoadPNGFromMemory(const wxString& name, wxMemoryInputStream& mis,
                                 std::function<bool(const wxString&, void**, size_t&)> fnGetHiResVersion)
{
    void* pData = NULL;
    size_t nLen = 0;

    // we will load the @2x version on demand
    if(name.Contains("@2x")) { return false; }

    if(ShouldLoadHiResImages()) {
        wxString hiresName = name + "@2x";
        if(fnGetHiResVersion(hiresName, &pData, nLen)) {
            wxMemoryInputStream m(pData, nLen);
            wxImage img(m, wxBITMAP_TYPE_PNG);
            if(img.IsOk()) {
                *this = clBitmap(img, 2.0);
                return IsOk();
            }
        }
    }
    wxImage img(mis, wxBITMAP_TYPE_PNG);
    *this = clBitmap(img, 1.0);
    return IsOk();
}

#endif // LIBCODELITE_WITH_UI
