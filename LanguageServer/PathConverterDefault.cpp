#include "PathConverterDefault.hpp"
#include <wx/filesys.h>

wxString PathConverterDefault::ConvertFrom(const wxString& path) const
{
    wxFileName fn = wxFileSystem::URLToFileName(path);
    return fn.GetFullPath();
}

wxString PathConverterDefault::ConvertTo(const wxString& path) const
{
    wxFileName fn(path);
    return wxFileSystem::FileNameToURL(fn);
}
