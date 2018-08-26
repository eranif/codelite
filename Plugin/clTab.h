#ifndef CLTAB_H
#define CLTAB_H

#include "codelite_exports.h"
#include <vector>
#include <wx/bitmap.h>
#include <wx/filename.h>
#include <wx/window.h>

// A struct representing a tab in the notebook control
struct WXDLLIMPEXP_SDK clTab {
    typedef std::vector<clTab> Vec_t;
    wxString text;
    wxWindow* window;
    wxBitmap bitmap;
    bool isFile;
    wxFileName filename;
    bool isModified;
    clTab()
        : window(NULL)
        , isFile(false)
        , isModified(false)
    {
    }
};
#endif // CLTAB_H
