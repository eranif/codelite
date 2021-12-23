#ifndef COMPILERLOCATORMSYS2_HPP
#define COMPILERLOCATORMSYS2_HPP

#include "ICompilerLocator.h"

#include <wx/filename.h>
class WXDLLIMPEXP_SDK CompilerLocatorMSYS2 : public ICompilerLocator
{
private:
    void AddTool(const wxString& tool_name, const wxString& value);
    wxFileName GetFileName(const wxString& installDir, const wxString& fullname) const;

public:
    CompilerLocatorMSYS2();
    ~CompilerLocatorMSYS2();

    /**
     * @brief locate the compiler
     */
    bool Locate() override;

    /**
     * @brief locate a compiler in a directory
     */
    CompilerPtr Locate(const wxString& folder) override;
};

#endif // COMPILERLOCATORMSYS2_HPP
