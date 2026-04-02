#pragma once
#include "codelite_exports.h"

#include <wx/string.h>

enum class IconType {
    kNoIcon,
    kQuestion,
    kInfo,
    kError,
    kSuccess,
};

WXDLLIMPEXP_SDK wxString IconType_ToString(IconType type);
