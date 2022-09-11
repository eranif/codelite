#include "LSPClangdDetector.hpp"

#include "CompilerLocatorCLANG.h"
#include "MSYS2.hpp"
#include "asyncprocess.h"
#include "clFilesCollector.h"
#include "file_logger.h"
#include "globals.h"
#include "procutils.h"

#include <wx/filename.h>
#include <wx/regex.h>

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
    wxString clangd_exe = "clangd";

#ifdef __WXMAC__
    wxString fullpath = ProcUtils::GrepCommandOutput({ "brew", "list", "llvm" }, "clangd");
    if(!fullpath.empty()) {
        paths.Add(wxFileName(fullpath).GetPath());
    }
    paths.Add("/opt/homebrew/opt/llvm/bin");
#else
    // add msys2 paths
    wxString msyspath;
    if(MSYS2::FindInstallDir(&msyspath)) {
        clDEBUG() << "searching for clangd in MSYS paths" << endl;
        clDEBUG() << msyspath + R"(\clang64\bin)" << endl;
        clDEBUG() << msyspath + R"(\mingw64\bin)" << endl;

        paths.Add(msyspath + R"(\clang64\bin)");
        paths.Add(msyspath + R"(\mingw64\bin)");
    }

    // on windows, we are also providing our own clangd
    wxFileName install_dir(clStandardPaths::Get().GetExecutablePath());
    install_dir.AppendDir("lsp");
    paths.Add(install_dir.GetPath());
    clangd_exe << ".exe";
#endif

    paths.AddEnvList("PATH");

    wxString path = paths.FindAbsoluteValidPath(clangd_exe);
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

    command << " -limit-results=500 -header-insertion-decorators=0 --compile-commands-dir=$(WorkspacePath)";
    SetCommand(command);
    // Add support for the languages
    GetLangugaes().Add("c");
    GetLangugaes().Add("cpp");
    SetConnectionString("stdio");
    SetPriority(90); // clangd should override the default cc engine
}
