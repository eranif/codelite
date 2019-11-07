#ifndef LSPPYTHONDETECTOR_HPP
#define LSPPYTHONDETECTOR_HPP

#include "LSPDetector.hpp" // Base class: LSPDetector

class LSPPythonDetector : public LSPDetector
{
public:
    LSPPythonDetector();
    virtual ~LSPPythonDetector();

public:
    virtual bool Locate();
};

#endif // LSPPYTHONDETECTOR_HPP
