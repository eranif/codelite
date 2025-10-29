#ifndef IPATHCONVERTER_HPP
#define IPATHCONVERTER_HPP

#include "LSP/FilePath.hpp"
#include "codelite_exports.h"

#include <memory>
#include <wx/string.h>

class WXDLLIMPEXP_CL IPathConverter
{
public:
    using Ptr_t = std::shared_ptr<IPathConverter>;

public:
    IPathConverter() = default;
    virtual ~IPathConverter() = default;

    /**
     * @brief convert path from a given source to the known path by CodeLite (usually the system default path)
     */
    virtual LSP::FilePath ConvertFrom(const wxString& path) const = 0;

    /**
     * @brief convert path from CodeLite's native path to some other format
     */
    virtual LSP::FilePath ConvertTo(const wxString& path) const = 0;
};

#endif // IPATHCONVERTER_HPP
