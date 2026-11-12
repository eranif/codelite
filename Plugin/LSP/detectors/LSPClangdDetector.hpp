#pragma once

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

