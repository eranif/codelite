#pragma once

#include "LSPDetector.hpp"
#include "codelite_exports.h"

#include <wx/filename.h>

class WXDLLIMPEXP_SDK LSPCMakeDetector : public LSPDetector
{
    void ConfigureFile(const wxFileName& cmakelsp);

public:
    LSPCMakeDetector();
    ~LSPCMakeDetector() override = default;

    bool DoLocate() override;
};

