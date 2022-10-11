#include "clVersionString.hpp"

#include <algorithm>
#include <cmath>
#include <vector>
#include <wx/arrstr.h>
#include <wx/tokenzr.h>

clVersionString::clVersionString(const wxString& version_string)
    : m_version_string(version_string)
{
    wxArrayString version_parts_arr = ::wxStringTokenize(version_string, ".", wxTOKEN_STRTOK);
    std::vector<double> numbers;

    // convert to string "1.2.3" into vector of: [1,2,3]
    for(wxString str : version_parts_arr) {
        str.Trim().Trim(false);
        double nNumber = 1;
        if(str.ToCDouble(&nNumber)) {
            numbers.push_back(nNumber);
        }
    }

    if(!numbers.empty()) {
        // conver the vector to number
        // example: [1,2,3] -> 1*10^2 + 2*10^1 + 3*10^0 => 123
        double power = (double)(numbers.size() - 1);
        for(double number : numbers) {
            m_number += number * std::pow(10.0, power);
            power -= 1.0;
        }
    }
}

clVersionString::~clVersionString() {}
