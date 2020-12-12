#ifndef PATHCONVERTERDEFAULT_HPP
#define PATHCONVERTERDEFAULT_HPP

#include "LSP/IPathConverter.hpp"

class PathConverterDefault : public IPathConverter
{
public:
    virtual LSP::FilePath ConvertFrom(const wxString& path) const;
    virtual LSP::FilePath ConvertTo(const wxString& path) const;
    PathConverterDefault() {}
    virtual ~PathConverterDefault() {}
};

#endif // PATHCONVERTERDEFAULT_HPP
