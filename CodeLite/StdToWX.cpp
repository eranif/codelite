#include "StdToWX.h"

void StdToWX::Remove(std::string& str, size_t from, size_t len) { str.erase(from, len); }

bool StdToWX::EndsWith(const std::string& str, const std::string& what)
{
    if(str.length() >= what.length()) {
        return (0 == str.compare(str.length() - what.length(), what.length(), what));
    } else {
        return false;
    }
}

bool StdToWX::StartsWith(const std::string& str, const std::string& what)
{
    if(str.length() >= what.length()) {
        return (0 == str.compare(0, what.length(), what));
    } else {
        return false;
    }
}

void StdToWX::Trim(std::string& str, bool fromRight)
{
    static std::string trimString = " \t\r\n";
    if(fromRight) {
        str.erase(str.find_last_not_of(trimString) + 1);
    } else {
        str.erase(0, str.find_first_not_of(trimString));
    }
}

void StdToWX::RemoveLast(std::string& str, size_t count)
{
    if(str.length() >= count) {
        str.erase(str.length() - count, count);
    }
}

void StdToWX::ToVector(const wxArrayString& arr, std::vector<wxString>* vec)
{
    vec->reserve(arr.size());
    vec->insert(vec->end(), arr.begin(), arr.end());
}

void StdToWX::ToArrayString(const std::vector<wxString>& vec, wxArrayString* arr)
{
    arr->reserve(vec.size());
    for(const wxString& s : vec) {
        arr->Add(s);
    }
}
