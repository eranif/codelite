#pragma once

#include "LSPDetector.hpp"
#include "codelite_exports.h"

#include <wx/filename.h>

class WXDLLIMPEXP_SDK LSPPyRightDetector : public LSPDetector
{
public:
    LSPPyRightDetector();
    ~LSPPyRightDetector() override = default;

protected:
    bool DoLocate() override;
    void ConfigureFile(const wxFileName& tool_path);
};
