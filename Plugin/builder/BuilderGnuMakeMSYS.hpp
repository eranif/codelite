#ifndef BUILDERGNUMAKEMSYS_HPP
#define BUILDERGNUMAKEMSYS_HPP

#include "builder_gnumake_default.h"

class WXDLLIMPEXP_SDK BuilderGnuMakeMSYS : public BuilderGnuMake
{
public:
    BuilderGnuMakeMSYS();
    BuilderGnuMakeMSYS(const wxString& name, const wxString& buildTool, const wxString& buildToolOptions);
    virtual ~BuilderGnuMakeMSYS();
};

#endif // BUILDERGNUMAKEMSYS_HPP
