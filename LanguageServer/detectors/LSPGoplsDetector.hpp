#pragma once

#include "LSPDetector.hpp"

#include <wx/filename.h>

class LSPGoplsDetector : public LSPDetector
{
protected:
    virtual bool DoLocate();
    void ConfigureFile(const wxFileName& gopls);

public:
    LSPGoplsDetector();
    virtual ~LSPGoplsDetector();
};
