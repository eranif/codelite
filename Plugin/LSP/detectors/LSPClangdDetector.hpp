#ifndef LSPCLANGDDETECTOR_HPP
#define LSPCLANGDDETECTOR_HPP

#include "LSPDetector.hpp"
#include "codelite_exports.h"

#include <wx/filename.h>

class WXDLLIMPEXP_SDK LSPClangdDetector : public LSPDetector
{
protected:
    bool DoLocate() override;
    void ConfigureFile(const wxFileName& clangdExe);

public:
    LSPClangdDetector();
    ~LSPClangdDetector() override = default;
};

#endif // LSPCLANGDDETECTOR_HPP
