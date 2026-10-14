#include "UnicodeSymbols.hpp"

wxString GetUnicodeSymbol(IconType type)
{
    wxString symbol;
    switch (type) {
    case IconType::kQuestion:
        symbol = wxT("‣");
        break;
    case IconType::kInfo:
        symbol = wxT("⋮");
        break;
    case IconType::kError:
        symbol = wxT("❌");
        break;
    case IconType::kSuccess:
        symbol = wxT("✔");
        break;
    case IconType::kWarning:
        symbol = wxT("⋮");
        break;
    case IconType::kNoIcon:
        symbol = "";
        break;
    case IconType::kHouse:
        symbol = wxT("🏡");
        break;
    case IconType::kNetwork:
        symbol = wxT("🌐");
        break;
    case IconType::kSearch:
        symbol = wxT("🔎");
        break;
    case IconType::kWarningSign:
        symbol = wxT("⚠");
        break;
    }
    return symbol;
}
