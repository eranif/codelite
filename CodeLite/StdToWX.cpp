#include "StdToWX.h"

void StdToWX::Trim(std::string& str, bool fromRight)
{
    static const std::string trimString = " \t\r\n";
    if(fromRight) {
        str.erase(str.find_last_not_of(trimString) + 1);
    } else {
        str.erase(0, str.find_first_not_of(trimString));
    }
}

wxArrayString StdToWX::ToArrayString(const std::vector<wxString>& vec)
{
    return { vec.size(), vec.data() };
}
