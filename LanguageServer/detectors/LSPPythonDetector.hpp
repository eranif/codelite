#ifndef LSPPYTHONDETECTOR_HPP
#define LSPPYTHONDETECTOR_HPP

#include "LSPDetector.hpp" // Base class: LSPDetector

class LSPPythonDetector : public LSPDetector
{
public:
    LSPPythonDetector();
    virtual ~LSPPythonDetector();

protected:
    virtual bool DoLocate();

    void ConfigurePylsp(const wxString& pylsp);
};

#endif // LSPPYTHONDETECTOR_HPP
