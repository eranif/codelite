#include "UnicodeSymbols.hpp"

wxString IconType_ToString(IconType type)
{
    wxString symbol;
    switch (type) {
    case IconType::kQuestion:
        symbol = wxT("❓");
        break;
    case IconType::kInfo:
        symbol = wxT("ℹ️");
        break;
    case IconType::kError:
        symbol = wxT("❌");
        break;
    case IconType::kSuccess:
        symbol = wxT("✅");
        break;
    }
    return symbol;
}
