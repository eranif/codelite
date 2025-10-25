#ifndef LSPCTAGSDDETECTOR_HPP
#define LSPCTAGSDDETECTOR_HPP

#include "LSPDetector.hpp"
#include "codelite_exports.h"

#include <wx/filename.h>

class WXDLLIMPEXP_SDK LSPCTagsdDetector : public LSPDetector
{
private:
    void ConfigureFile(const wxFileName& clangdExe);

public:
    LSPCTagsdDetector();
    ~LSPCTagsdDetector() = default;

    bool DoLocate() override;
};

#endif // LSPCTAGSDDETECTOR_HPP
