#ifndef COMPILERLOCATORMSYS2CLANG_HPP
#define COMPILERLOCATORMSYS2CLANG_HPP

#include "ICompilerLocator.h"
#include "MSYS2.hpp"

#include <wx/filename.h>

/// Locate Clang compilers in the MSYS2 environment
class WXDLLIMPEXP_SDK CompilerLocatorMSYS2Clang : public ICompilerLocator
{
protected:
    MSYS2 m_msys2;
    wxString m_repository;

protected:
    void AddTool(const wxString& tool_name, const wxString& value);
    wxFileName GetFileName(const wxString& bin_dir, const wxString& fullname) const;

    CompilerLocatorMSYS2Clang();
    virtual ~CompilerLocatorMSYS2Clang();

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
    virtual ~CompilerLocatorMSYS2ClangUsr();
};

class WXDLLIMPEXP_SDK CompilerLocatorMSYS2ClangClang64 : public CompilerLocatorMSYS2Clang
{
public:
    CompilerLocatorMSYS2ClangClang64();
    virtual ~CompilerLocatorMSYS2ClangClang64();
};

class WXDLLIMPEXP_SDK CompilerLocatorMSYS2ClangMingw64 : public CompilerLocatorMSYS2Clang
{
public:
    CompilerLocatorMSYS2ClangMingw64();
    virtual ~CompilerLocatorMSYS2ClangMingw64();
};
#endif // CompilerLocatorMSYS2Clang_HPP
