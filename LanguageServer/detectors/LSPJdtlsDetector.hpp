#ifndef LSPJdtlsDetector_HPP
#define LSPJdtlsDetector_HPP

#include "LSPDetector.hpp"

#include <wx/filename.h>

/// Java jdtls
class LSPJdtlsDetector : public LSPDetector
{
protected:
    virtual bool DoLocate();
    void ConfigureFile(const wxFileName& clangdExe);

public:
    LSPJdtlsDetector();
    virtual ~LSPJdtlsDetector();
};

#endif // LSPJdtlsDetector_HPP
