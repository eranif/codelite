#include "StringUtils.hpp"
#include <codecvt>
#include <locale>

void StringUtils::Rtrim(wstring& str) { str.erase(str.find_last_not_of(L" \n\r\t") + 1); }
void StringUtils::Rtrim(string& str) { str.erase(str.find_last_not_of(" \n\r\t") + 1); }
