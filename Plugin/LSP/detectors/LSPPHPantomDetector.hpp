#pragma once

#include "LSPDetector.hpp"
#include "codelite_exports.h"

#include <wx/filename.h>

class WXDLLIMPEXP_SDK LSPPHPantomDetector : public LSPDetector
{
public:
    LSPPHPantomDetector();
    ~LSPPHPantomDetector() override = default;

    bool DoLocate() override;

private:
    void ConfigureFile(const wxFileName& phpantom_exe);
};
