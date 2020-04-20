#ifndef LSPCLANGDDETECTOR_HPP
#define LSPCLANGDDETECTOR_HPP

#include "LSPDetector.hpp"
#include <wx/filename.h>

class LSPClangdDetector : public LSPDetector
{
protected:
    virtual bool DoLocate();
    void ConfigureFile(const wxFileName& clangdExe);

public:
    LSPClangdDetector();
    virtual ~LSPClangdDetector();
};

#endif // LSPCLANGDDETECTOR_HPP
