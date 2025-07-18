#include "clRustLocator.hpp"

#include "Platform/Platform.hpp"
#include "fileutils.h"

#include <wx/arrstr.h>
#include <wx/filename.h>

clRustLocator::clRustLocator() {}

clRustLocator::~clRustLocator() {}

bool clRustLocator::Locate()
{
    const auto tool_exe = ThePlatform->Which("cargo");
    if (!tool_exe) {
        return false;
    }
    m_binDir = wxFileName(*tool_exe).GetPath();
    return true;
}

wxString clRustLocator::GetRustTool(const wxString& name) const
{
    if(m_binDir.empty()) {
        return wxEmptyString;
    }

    wxFileName toolpath(m_binDir, name);
#ifdef __WXMSW__
    toolpath.SetExt("exe");
#endif
    if(toolpath.FileExists()) {
        return toolpath.GetFullPath();
    }
    return wxEmptyString;
}
