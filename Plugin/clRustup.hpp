#ifndef CLRUSTUP_HPP
#define CLRUSTUP_HPP

#include "codelite_exports.h"

#include <wx/filename.h>

class WXDLLIMPEXP_SDK clRustup
{
public:
    clRustup();
    ~clRustup();

    bool FindExecutable(const wxString& name, wxString* opath) const;
};

#endif // CLRUSTUP_HPP
