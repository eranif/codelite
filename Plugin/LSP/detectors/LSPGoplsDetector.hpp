#pragma once

#include "LSPDetector.hpp"
#include "codelite_exports.h"

#include <wx/filename.h>

class WXDLLIMPEXP_SDK LSPGoplsDetector : public LSPDetector
{
protected:
    virtual bool DoLocate();
    void ConfigureFile(const wxFileName& gopls);

public:
    LSPGoplsDetector();
    virtual ~LSPGoplsDetector() = default;
};
