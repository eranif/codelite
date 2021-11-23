#ifndef LSPCTAGSDDETECTOR_HPP
#define LSPCTAGSDDETECTOR_HPP

#include "LSPDetector.hpp"

#include <wx/filename.h>

class LSPCTagsdDetector : public LSPDetector
{
private:
    void ConfigureFile(const wxFileName& clangdExe);

public:
    LSPCTagsdDetector();
    ~LSPCTagsdDetector();

    bool DoLocate() override;
};

#endif // LSPCTAGSDDETECTOR_HPP
