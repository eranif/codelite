#ifndef COMPILERLOCATORMSYS2CLANG_HPP
#define COMPILERLOCATORMSYS2CLANG_HPP

#include "ICompilerLocator.h"
#include "Platform/MSYS2.hpp"

#include <wx/filename.h>

/// Locate Clang compilers in the MSYS2 environment
class WXDLLIMPEXP_SDK CompilerLocatorMSYS2Clang : public ICompilerLocator
{
protected:
    MSYS2 m_msys2;
    wxString m_repository;
    bool m_useSystemPath{true};

protected:
    wxFileName GetFileName(const wxString& bin_dir, const wxString& fullname) const;

    CompilerLocatorMSYS2Clang() = default;
    ~CompilerLocatorMSYS2Clang() override = default;

public:
    /**
     * @brief locate the compiler
     */
    bool Locate() override;

    /**
     * @brief locate a compiler in a directory
     */
    CompilerPtr Locate(const wxString& folder) override;
};

class WXDLLIMPEXP_SDK CompilerLocatorMSYS2ClangUsr : public CompilerLocatorMSYS2Clang
{
public:
    CompilerLocatorMSYS2ClangUsr();
    ~CompilerLocatorMSYS2ClangUsr() override = default;
};

class WXDLLIMPEXP_SDK CompilerLocatorMSYS2ClangClang64 : public CompilerLocatorMSYS2Clang
{
public:
    CompilerLocatorMSYS2ClangClang64();
    ~CompilerLocatorMSYS2ClangClang64() override = default;
};

class WXDLLIMPEXP_SDK CompilerLocatorMSYS2ClangMingw64 : public CompilerLocatorMSYS2Clang
{
public:
    CompilerLocatorMSYS2ClangMingw64();
    ~CompilerLocatorMSYS2ClangMingw64() override = default;
};
#endif // CompilerLocatorMSYS2Clang_HPP
