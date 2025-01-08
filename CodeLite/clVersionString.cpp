#include "clVersionString.hpp"

#include <algorithm>
#include <cmath>
#include <vector>
#include <wx/arrstr.h>
#include <wx/tokenzr.h>

clVersionString::clVersionString(const wxString& version_string)
{
    auto parts = ::wxStringTokenize(version_string, ".", wxTOKEN_STRTOK);
    for (const auto& str : parts) {
        long n = 0;
        str.ToCLong(&n);
        m_numbers.push_back(n);
    }
}

clVersionString::~clVersionString() {}

int clVersionString::Compare(const wxString& other) const
{
    clVersionString ver_other(other);
    size_t elements_count = std::max(ver_other.m_numbers.size(), m_numbers.size());
    for (size_t i = 0; i < elements_count; ++i) {
        if (number_at(i) > ver_other.number_at(i)) {
            return 1;
        } else if (number_at(i) < ver_other.number_at(i)) {
            return -1;
        }
    }
    return 0;
}

long clVersionString::number_at(size_t index) const
{
    if (index >= m_numbers.size()) {
        return 0;
    }
    return m_numbers[index];
}
