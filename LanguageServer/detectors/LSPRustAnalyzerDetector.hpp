#ifndef LSPRUSTANALYZERDETECTOR_HPP
#define LSPRUSTANALYZERDETECTOR_HPP

#include "LSPDetector.hpp"

class LSPRustAnalyzerDetector : public LSPDetector
{
protected:
    bool DoLocate();

public:
    LSPRustAnalyzerDetector();
    ~LSPRustAnalyzerDetector();
};

#endif // LSPRUSTANALYZERDETECTOR_HPP
