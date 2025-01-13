#pragma once

#include "ICompilerLocator.h"
#include "Platform/MSYS2.hpp"

#include <unordered_map>
#include <wx/filename.h>

/// Locate for GCC compilers
class WXDLLIMPEXP_SDK CompilerLocatorMSYS2 : public ICompilerLocator
{
public:
    /**
     * @brief locate the compiler
     */
    bool Locate() override;

    /**
     * @brief locate a compiler in a directory
     */
    CompilerPtr Locate(const wxString& folder) override;

protected:
    wxFileName GetFileName(const wxString& bin_dir, const wxString& fullname) const;

    CompilerLocatorMSYS2();
    virtual ~CompilerLocatorMSYS2();

    CompilerPtr TryToolchain(const wxString& folder, const std::unordered_map<wxString, wxString>& toolchain);

    MSYS2 m_msys2;
    bool m_cmdShell = false;
};

class WXDLLIMPEXP_SDK CompilerLocatorMSYS2Usr : public CompilerLocatorMSYS2
{
public:
    CompilerLocatorMSYS2Usr();
    virtual ~CompilerLocatorMSYS2Usr();
};

class WXDLLIMPEXP_SDK CompilerLocatorMSYS2Clang64 : public CompilerLocatorMSYS2
{
public:
    CompilerLocatorMSYS2Clang64();
    virtual ~CompilerLocatorMSYS2Clang64();
};

class WXDLLIMPEXP_SDK CompilerLocatorMSYS2Mingw64 : public CompilerLocatorMSYS2
{
public:
    CompilerLocatorMSYS2Mingw64();
    virtual ~CompilerLocatorMSYS2Mingw64();
};

class WXDLLIMPEXP_SDK CompilerLocatorMSYS2Env : public CompilerLocatorMSYS2
{
public:
    CompilerLocatorMSYS2Env();
    virtual ~CompilerLocatorMSYS2Env();

    /**
     * @brief locate all compilers based on the PATH env variable
     */
    bool Locate() override;
};
