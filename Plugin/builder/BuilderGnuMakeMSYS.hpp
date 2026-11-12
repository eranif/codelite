#pragma once

#include "builder_gnumake_default.h"

class WXDLLIMPEXP_SDK BuilderGnuMakeMSYS : public BuilderGnuMake
{
public:
    BuilderGnuMakeMSYS();
    BuilderGnuMakeMSYS(const wxString& name, const wxString& buildTool, const wxString& buildToolOptions);
    ~BuilderGnuMakeMSYS() override = default;
};

