#ifndef LSPJdtlsDetector_HPP
#define LSPJdtlsDetector_HPP

#include "LSPDetector.hpp"
#include "codelite_exports.h"

#include <wx/filename.h>

/// Java jdtls
class WXDLLIMPEXP_SDK LSPJdtlsDetector : public LSPDetector
{
protected:
    virtual bool DoLocate();
    void ConfigureFile(const wxFileName& clangdExe);

public:
    LSPJdtlsDetector();
    virtual ~LSPJdtlsDetector() = default;
};

#endif // LSPJdtlsDetector_HPP
