#ifndef COMPILERLOCATORMSYS2_HPP
#define COMPILERLOCATORMSYS2_HPP

#include "ICompilerLocator.h"
#include "MSYS2.hpp"

#include <wx/filename.h>

/// Locate for GCC compilers
class WXDLLIMPEXP_SDK CompilerLocatorMSYS2 : public ICompilerLocator
{
protected:
    MSYS2 m_msys2;
    wxString m_repository;

protected:
    void AddTool(const wxString& tool_name, const wxString& value);
    wxFileName GetFileName(const wxString& bin_dir, const wxString& fullname) const;

    CompilerLocatorMSYS2();
    virtual ~CompilerLocatorMSYS2();

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
#endif // COMPILERLOCATORMSYS2_HPP
