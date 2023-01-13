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
#endif // LIBCODELITE_WITH_UI
