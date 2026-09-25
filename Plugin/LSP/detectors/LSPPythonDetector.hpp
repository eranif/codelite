#ifndef LSPPYTHONDETECTOR_HPP
#define LSPPYTHONDETECTOR_HPP

#include "LSPDetector.hpp" // Base class: LSPDetector
#include "codelite_exports.h"

class WXDLLIMPEXP_SDK LSPPythonDetector : public LSPDetector
{
public:
    LSPPythonDetector();
    ~LSPPythonDetector() override = default;

protected:
    bool DoLocate() override;

    void ConfigurePylsp(const wxString& pylsp);
};

#endif // LSPPYTHONDETECTOR_HPP
