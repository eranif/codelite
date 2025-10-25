#ifndef LSPCLANGDDETECTOR_HPP
#define LSPCLANGDDETECTOR_HPP

#include "LSPDetector.hpp"
#include "codelite_exports.h"

#include <wx/filename.h>

class WXDLLIMPEXP_SDK LSPClangdDetector : public LSPDetector
{
protected:
    virtual bool DoLocate();
    void ConfigureFile(const wxFileName& clangdExe);

public:
    LSPClangdDetector();
    virtual ~LSPClangdDetector() = default;
};

#endif // LSPCLANGDDETECTOR_HPP
