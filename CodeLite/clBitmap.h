#ifndef CLBITMAP_H
#define CLBITMAP_H

#include <wx/defs.h>

#if wxUSE_GUI
#include <wx/bitmap.h> // Base class: wxBitmap
#include "codelite_exports.h"
#include <wx/mstream.h>
#include <functional>

class WXDLLIMPEXP_CL clBitmap : public wxBitmap
{
public:
    static bool ShouldLoadHiResImages();

public:
    clBitmap();
    clBitmap(const wxImage& img, double scale = 1.0);
    virtual ~clBitmap();

    virtual bool LoadFile(const wxString& name, wxBitmapType type = wxBITMAP_DEFAULT_TYPE);
    virtual bool LoadPNGFromMemory(const wxString& name, wxMemoryInputStream& mis,
                                   std::function<bool(const wxString&, void**, size_t&)> fnGetHiResVersion);
};
#endif
#endif // CLBITMAP_H
