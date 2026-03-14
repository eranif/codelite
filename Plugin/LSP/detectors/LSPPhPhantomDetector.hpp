#pragma once

#include "LSPDetector.hpp"
#include "codelite_exports.h"

#include <wx/filename.h>

class WXDLLIMPEXP_SDK LSPPhPhantomDetector : public LSPDetector
{
public:
    LSPPhPhantomDetector();
    virtual ~LSPPhPhantomDetector() = default;

    bool DoLocate() override;

private:
    void ConfigureFile(const wxFileName& cmakelsp);
};
