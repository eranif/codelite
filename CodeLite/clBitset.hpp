#ifndef CLBITSET_HPP
#define CLBITSET_HPP

#include "codelite_exports.h"

#include <string.h>
#include <wx/string.h>

#define BITS_SIZE 64

class WXDLLIMPEXP_CL clBitset
{
    unsigned char bits[BITS_SIZE];

public:
    clBitset();
    ~clBitset();

    void set(size_t pos, bool b = true);
    bool test(size_t pos) const;
    void rese();
    wxString to_string() const;
    void from_string(const wxString& str);
};

#endif // CLBITSET_HPP
