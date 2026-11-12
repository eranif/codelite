#pragma once

#include "LSPDetector.hpp" // Base class: LSPDetector
#include "codelite_exports.h"

class WXDLLIMPEXP_SDK LSPTypeScriptDetector : public LSPDetector
{
public:
    LSPTypeScriptDetector();
    ~LSPTypeScriptDetector() override = default;

    bool DoLocate() override;
};

