#include "LSPClangdDetector.hpp"

#include "CompilerLocatorCLANG.h"
#include "asyncprocess.h"
#include "clFilesCollector.h"
#include "file_logger.h"
#include "globals.h"

#include <wx/filename.h>
#include <wx/regex.h>

namespace
{
wxString command_grep(const std::vector<wxString>& cmd, const wxString& find_what)
{
    IProcess::Ptr_t proc(::CreateAsyncProcess(nullptr, cmd, IProcessCreateDefault | IProcessCreateSync));
    if(!proc) {
        return wxEmptyString;
    }

    wxString output;
    proc->WaitForTerminate(output);
    auto lines = ::wxStringTokenize(output, "\n", wxTOKEN_STRTOK);
    for(wxString& line : lines) {
        line.Trim();
        if(line.Contains(find_what)) {
            return line;
        }
    }
    return wxEmptyString;
}
} // namespace

LSPClangdDetector::LSPClangdDetector()
    : LSPDetector("clangd")
{
}

LSPClangdDetector::~LSPClangdDetector() {}

bool LSPClangdDetector::DoLocate()
{
#if defined(__WXGTK__)
    // try the obvious name first: "clangd"
    wxFileName fnClangdExe("/usr/bin", "clangd");
    if(fnClangdExe.FileExists()) {
        clSYSTEM() << "Found clangd ==>" << fnClangdExe << endl;
        ConfigureFile(fnClangdExe);
        return true;
    }
    wxArrayString suffix;
    for(size_t i = 20; i >= 7; --i) {
        fnClangdExe.SetFullName(wxString() << "clangd-" << i);
        if(fnClangdExe.FileExists()) {
            clSYSTEM() << "Found clangd ==>" << fnClangdExe << endl;
            ConfigureFile(fnClangdExe);
            return true;
        }
    }
#else // macOS / Windows
    wxPathList paths;
#ifdef __WXMAC__
    wxString fullpath = command_grep({ "brew", "list", "llvm" }, "clangd");
    if(!fullpath.empty()) {
        paths.Add(wxFileName(fullpath).GetPath());
    }

    paths.Add("/opt/homebrew/opt/llvm/bin");
#else
    // Try the installation folder first
    wxFileName install_dir(clStandardPaths::Get().GetExecutablePath());
    install_dir.AppendDir("lsp");
    paths.Add(install_dir.GetPath());
#endif

    paths.AddEnvList("PATH");
    wxString path = paths.FindAbsoluteValidPath("clangd");
    if(!path.empty()) {
        clSYSTEM() << "Found clangd ==>" << path << endl;
        ConfigureFile(path);
        return true;
    }
#endif
    return false;
}

void LSPClangdDetector::ConfigureFile(const wxFileName& clangdExe)
{
    clDEBUG() << "==> Found" << clangdExe;
    wxString command;
    command << clangdExe.GetFullPath();
    ::WrapWithQuotes(command);

    command << " -limit-results=500 -header-insertion-decorators=0";
    SetCommand(command);
    // Add support for the languages
    GetLangugaes().Add("c");
    GetLangugaes().Add("cpp");
    SetConnectionString("stdio");
    SetPriority(90); // clangd should override the default cc engine
}
