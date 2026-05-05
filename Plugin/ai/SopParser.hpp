#pragma once

#include "codelite_exports.h"

#include <optional>
#include <vector>
#include <wx/string.h>

struct WXDLLIMPEXP_SDK SopInfo {
    struct Param {
        wxString name;
        wxString desc;
    };
    wxString title;
    std::vector<Param> params;
    wxString full_sop;
    static std::optional<SopInfo> Parse(const wxString& text);
    wxString to_string() const;
};
