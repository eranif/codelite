#pragma once

#include "LSPDetector.hpp"
#include "codelite_exports.h"

class WXDLLIMPEXP_SDK LSPRustAnalyzerDetector : public LSPDetector
{
protected:
    bool DoLocate() override;

public:
    LSPRustAnalyzerDetector();
    ~LSPRustAnalyzerDetector() override = default;
};

