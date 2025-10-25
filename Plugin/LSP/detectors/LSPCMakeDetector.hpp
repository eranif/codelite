#ifndef LSPCMAKEDETECTOR_HPP
#define LSPCMAKEDETECTOR_HPP

#include "LSPDetector.hpp"
#include "codelite_exports.h"

#include <wx/filename.h>

class WXDLLIMPEXP_SDK LSPCMakeDetector : public LSPDetector
{
    void ConfigureFile(const wxFileName& cmakelsp);

public:
    LSPCMakeDetector();
    virtual ~LSPCMakeDetector() = default;

    bool DoLocate() override;
};

#endif // LSPCMAKEDETECTOR_HPP
