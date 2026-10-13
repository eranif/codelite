#pragma once
#include "codelite_exports.h"

#include <wx/string.h>

enum class IconType {
    kNoIcon,
    kQuestion,
    kInfo,
    kError,
    kWarning,
    kSuccess,
    kHouse,
    kNetwork,
};

WXDLLIMPEXP_SDK wxString GetUnicodeSymbol(IconType type);
