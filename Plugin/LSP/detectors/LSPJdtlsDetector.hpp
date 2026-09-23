#ifndef LSPJdtlsDetector_HPP
#define LSPJdtlsDetector_HPP

#include "LSPDetector.hpp"
#include "codelite_exports.h"

#include <wx/filename.h>

/// Java jdtls
class WXDLLIMPEXP_SDK LSPJdtlsDetector : public LSPDetector
{
protected:
    bool DoLocate() override;
    void ConfigureFile(const wxFileName& clangdExe);

public:
    LSPJdtlsDetector();
    ~LSPJdtlsDetector() override = default;
};

#endif // LSPJdtlsDetector_HPP
