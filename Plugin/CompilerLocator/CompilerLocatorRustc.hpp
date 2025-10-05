#ifndef COMPILERLOCATORRUSTC_HPP
#define COMPILERLOCATORRUSTC_HPP

#include "ICompilerLocator.h"
#include "codelite_exports.h"

class WXDLLIMPEXP_SDK CompilerLocatorRustc : public ICompilerLocator
{
public:
    CompilerLocatorRustc() = default;
    ~CompilerLocatorRustc() override = default;

    bool Locate() override;
    CompilerPtr Locate(const wxString& folder) override;
};

#endif // COMPILERLOCATORRUSTC_HPP
