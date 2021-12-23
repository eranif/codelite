#ifndef CLRUSTLOCATOR_HPP
#define CLRUSTLOCATOR_HPP

#include <codelite_exports.h>
#include <wx/string.h>

class WXDLLIMPEXP_SDK clRustLocator
{
    wxString m_binDir;

public:
    clRustLocator();
    ~clRustLocator();

    /**
     * @brief locate rust installation folder
     */
    bool Locate();

    wxString GetRustTool(const wxString& name) const;
};

#endif // CLRUSTLOCATOR_HPP
