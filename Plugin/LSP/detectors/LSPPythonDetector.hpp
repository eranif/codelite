#ifndef LSPPYTHONDETECTOR_HPP
#define LSPPYTHONDETECTOR_HPP

#include "LSPDetector.hpp" // Base class: LSPDetector
#include "codelite_exports.h"

class WXDLLIMPEXP_SDK LSPPythonDetector : public LSPDetector
{
public:
    LSPPythonDetector();
    virtual ~LSPPythonDetector() = default;

protected:
    virtual bool DoLocate();

    void ConfigurePylsp(const wxString& pylsp);
};

#endif // LSPPYTHONDETECTOR_HPP
