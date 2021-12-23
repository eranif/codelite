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
    wxWindow* window = nullptr;
    int bitmap = wxNOT_FOUND;
    bool isFile = false;
    wxFileName filename;
    bool isModified = false;
    clTab() {}
};
#endif // CLTAB_H
