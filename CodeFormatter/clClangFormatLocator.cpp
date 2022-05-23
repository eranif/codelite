#include "clClangFormatLocator.h"

#include "asyncprocess.h"
#include "cl_standard_paths.h"
#include "file_logger.h"
#include "globals.h"
#include "procutils.h"

#include <wx/filefn.h>
#include <wx/filename.h>
#include <wx/log.h>
#include <wx/regex.h>
#include <wx/tokenzr.h>

clClangFormatLocator::clClangFormatLocator() {}

clClangFormatLocator::~clClangFormatLocator() {}

bool clClangFormatLocator::Locate(wxString& clangFormat)
{
#ifdef __WXGTK__
    wxPathList paths;
    paths.AddEnvList("PATH");

    wxArrayString clangFormatNames;
    clangFormatNames.Add("clang-format");
    for(size_t i = 20; i >= 7; --i) {
        clangFormatNames.Add(wxString() << "clang-format-" << i);
    }

    for(wxString cfName : clangFormatNames) {
        wxString fnClangFormat = paths.FindAbsoluteValidPath(cfName);
        if(!fnClangFormat.empty()) {
            clSYSTEM() << "Found clang-format ==>" << fnClangFormat << endl;
            clangFormat = fnClangFormat;
            return true;
        }
    }
#elif defined(__WXMSW__)
    wxFileName fnClangFormat(clStandardPaths::Get().GetBinaryFullPath("clang-format"));
    if(fnClangFormat.FileExists()) {
        clangFormat = fnClangFormat.GetFullPath();
        return true;
    }
#else
    // macOS
    wxFileName fnClangFormat(clStandardPaths::Get().GetBinaryFullPath("clang-format"));
    if(fnClangFormat.FileExists()) {
        clangFormat = fnClangFormat.GetFullPath();
        return true;
    }
#endif
    return false;
}

double clClangFormatLocator::GetVersion(const wxString& clangFormat) const
{
    double double_version = 3.3;
#ifdef __WXGTK__
    //    Ubuntu clang-format version 3.5.0-4ubuntu2~trusty2 (tags/RELEASE_350/final) (based on LLVM 3.5.0) // Linux
    //    LLVM version 3.3 // Linux, old format
    //    clang-format version 3.6.0 (217570) // Windows
    double_version = 3.3;

    static wxRegEx reClangFormatVersion("version ([0-9]+\\.[0-9]+)");
    wxString command;
    command << clangFormat;
    ::WrapWithQuotes(command);
    command << " --version";
    wxString output = ProcUtils::SafeExecuteCommand(command);

    wxArrayString lines = ::wxStringTokenize(output, "\n", wxTOKEN_STRTOK);
    for(size_t i = 0; i < lines.GetCount(); ++i) {
        if(reClangFormatVersion.Matches(lines.Item(i))) {
            wxString version = reClangFormatVersion.GetMatch(lines.Item(i), 1);
            // clLogMessage("clang-format version is %s", version);
            version.ToCDouble(&double_version);
            return double_version;
        }
    }
#elif defined(__WXMSW__)
    double_version = 3.6;
#else
    double_version = 3.5;
#endif
    return double_version; // Default
}
