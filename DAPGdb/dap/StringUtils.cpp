#include "StringUtils.hpp"
#include <codecvt>
#include <locale>

void StringUtils::Rtrim(wstring& str) { str.erase(str.find_last_not_of(L" \n\r\t") + 1); }
void StringUtils::Rtrim(string& str) { str.erase(str.find_last_not_of(" \n\r\t") + 1); }

string StringUtils::BeforeFirst(const string& str, char ch)
{
    size_t where = str.find(ch);
    if(where == string::npos) { return str; }
    return str.substr(0, where);
}

string StringUtils::AfterFirst(const string& str, char ch)
{
    size_t where = str.find(ch);
    if(where == string::npos) { return ""; }
    return str.substr(where + 1);
}
