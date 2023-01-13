#ifndef CLBITMAP_H
#define CLBITMAP_H

#include <wx/defs.h>

#if wxUSE_GUI
#include "codelite_exports.h"

#include <functional>
#include <wx/bitmap.h> // Base class: wxBitmap
#include <wx/mstream.h>

class WXDLLIMPEXP_CL clBitmap : public wxBitmap
{
public:
    clBitmap();
    clBitmap(const wxImage& img, double scale = 1.0);
    virtual ~clBitmap();
};
#endif
#endif // CLBITMAP_H
