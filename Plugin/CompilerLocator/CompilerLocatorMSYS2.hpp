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

    CompilerLocatorMSYS2() = default;
    ~CompilerLocatorMSYS2() override = default;

    CompilerPtr TryToolchain(const wxString& folder, const std::unordered_map<wxString, wxString>& toolchain);

    MSYS2 m_msys2;
    bool m_useSystemPath{true};
    bool m_cmdShell{false};
};

class WXDLLIMPEXP_SDK CompilerLocatorMSYS2Usr : public CompilerLocatorMSYS2
{
public:
    CompilerLocatorMSYS2Usr();
    ~CompilerLocatorMSYS2Usr() override = default;
};

class WXDLLIMPEXP_SDK CompilerLocatorMSYS2Clang64 : public CompilerLocatorMSYS2
{
public:
    CompilerLocatorMSYS2Clang64();
    ~CompilerLocatorMSYS2Clang64() override = default;
};

class WXDLLIMPEXP_SDK CompilerLocatorMSYS2Mingw64 : public CompilerLocatorMSYS2
{
public:
    CompilerLocatorMSYS2Mingw64();
    ~CompilerLocatorMSYS2Mingw64() override = default;
};

class WXDLLIMPEXP_SDK CompilerLocatorMSYS2Env : public CompilerLocatorMSYS2
{
public:
    CompilerLocatorMSYS2Env();
    ~CompilerLocatorMSYS2Env() override = default;

    /**
     * @brief locate all compilers based on the PATH env variable
     */
    bool Locate() override;
};
