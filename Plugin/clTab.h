#pragma once

#include "codelite_exports.h"

#include <vector>
#include <wx/bitmap.h>
#include <wx/filename.h>
#include <wx/window.h>

// A struct representing a tab in the notebook control
struct WXDLLIMPEXP_SDK clTab {
    using Vec_t = std::vector<clTab>;
    wxString text;
    wxWindow* window = nullptr;
    wxBitmap bitmap;
    bool isFile = false;
    wxFileName filename;
    bool isModified = false;
    clTab() = default;
};
