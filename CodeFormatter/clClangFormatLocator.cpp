#include "clClangFormatLocator.h"
#include "cl_standard_paths.h"
#include <wx/filename.h>

clClangFormatLocator::clClangFormatLocator()
{
}

clClangFormatLocator::~clClangFormatLocator()
{
}

bool clClangFormatLocator::Locate(wxString& clangFormat)
{
#ifdef __WXGTK__
    wxFileName fnClangFormat("/usr/bin", "");
    wxArrayString nameOptions;
    nameOptions.Add("clang-format");
    nameOptions.Add("clang-format-3.4");
    nameOptions.Add("clang-format-3.5");
    nameOptions.Add("clang-format-3.6");

    for(size_t i = 0; i < nameOptions.GetCount(); ++i) {
        fnClangFormat.SetFullName(nameOptions.Item(i));
        if(fnClangFormat.Exists()) {
            clangFormat = fnClangFormat.GetFullPath();
            return true;
        }
    }
    return false;
#else
    wxFileName fnClangFormat(clStandardPaths::Get().GetBinaryFullPath("codelite-clang-format"));
    if(fnClangFormat.FileExists()) {
        clangFormat = fnClangFormat.GetFullPath();
        return true;
    }
    return false;
#endif
}
