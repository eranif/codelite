#ifndef IPATHCONVERTER_HPP
#define IPATHCONVERTER_HPP

#include <codelite_exports.h>
#include <wx/sharedptr.h>
#include <wx/string.h>

class WXDLLIMPEXP_CL IPathConverter
{
public:
    typedef wxSharedPtr<IPathConverter> Ptr_t;

public:
    IPathConverter() {}
    virtual ~IPathConverter() {}

    /**
     * @brief convert path from a given source to the known path by CodeLite (usually the system default path)
     */
    virtual wxString ConvertFrom(const wxString& path) const = 0;
    
    /**
     * @brief convert path from CodeLite's native path to some other format
     */
    virtual wxString ConvertTo(const wxString& path) const = 0;
};

#endif // IPATHCONVERTER_HPP
