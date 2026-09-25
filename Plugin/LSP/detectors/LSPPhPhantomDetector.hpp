#pragma once

#include "LSPDetector.hpp"
#include "codelite_exports.h"

#include <wx/filename.h>

class WXDLLIMPEXP_SDK LSPPhPhantomDetector : public LSPDetector
{
public:
    LSPPhPhantomDetector();
    ~LSPPhPhantomDetector() override = default;

    bool DoLocate() override;

private:
    void ConfigureFile(const wxFileName& cmakelsp);
};
