#ifndef LSPRUSTANALYZERDETECTOR_HPP
#define LSPRUSTANALYZERDETECTOR_HPP

#include "LSPDetector.hpp"
#include "codelite_exports.h"

class WXDLLIMPEXP_SDK LSPRustAnalyzerDetector : public LSPDetector
{
protected:
    bool DoLocate();

public:
    LSPRustAnalyzerDetector();
    ~LSPRustAnalyzerDetector() = default;
};

#endif // LSPRUSTANALYZERDETECTOR_HPP
