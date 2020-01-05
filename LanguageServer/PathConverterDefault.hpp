#ifndef PATHCONVERTERDEFAULT_HPP
#define PATHCONVERTERDEFAULT_HPP

#include "LSP/IPathConverter.hpp"

class PathConverterDefault : public IPathConverter
{
public:
    virtual wxString ConvertFrom(const wxString& path) const;
    virtual wxString ConvertTo(const wxString& path) const;
    PathConverterDefault() {}
    virtual ~PathConverterDefault() {}
};

#endif // PATHCONVERTERDEFAULT_HPP
