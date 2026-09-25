#pragma once

#include "LSPDetector.hpp"
#include "codelite_exports.h"

#include <wx/filename.h>

class WXDLLIMPEXP_SDK LSPGoplsDetector : public LSPDetector
{
protected:
    bool DoLocate() override;
    void ConfigureFile(const wxFileName& gopls);

public:
    LSPGoplsDetector();
    ~LSPGoplsDetector() override = default;
};
