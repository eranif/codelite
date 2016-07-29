#ifndef CLBITMAP_H
#define CLBITMAP_H

#include <wx/bitmap.h> // Base class: wxBitmap
#include "codelite_exports.h"

class WXDLLIMPEXP_CL clBitmap : public wxBitmap
{
public:
    static bool ShouldLoadHiResImages();
    
public:
    clBitmap();
    clBitmap(const wxImage& img, double scale = 1.0);
    virtual ~clBitmap();

    virtual bool LoadFile(const wxString& name, wxBitmapType type = wxBITMAP_DEFAULT_TYPE);
};

#endif // CLBITMAP_H
