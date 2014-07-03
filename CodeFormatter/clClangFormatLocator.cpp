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
    wxFileName fnClangFormat( clStandardPaths::Get().GetBinaryFullPath("codelite-clang-format") );
    if ( fnClangFormat.FileExists() ) {
        clangFormat = fnClangFormat.GetFullPath();
        return true;
    }
    return false;
}
