#pragma once

#include "LSPDetector.hpp"
#include "codelite_exports.h"

#include <wx/filename.h>

class WXDLLIMPEXP_SDK LSPPyRightDetector : public LSPDetector
{
public:
    LSPPyRightDetector();
    virtual ~LSPPyRightDetector() = default;

protected:
    virtual bool DoLocate();
    void ConfigureFile(const wxFileName& tool_path);
};
