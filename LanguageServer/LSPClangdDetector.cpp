#include "CompilerLocatorCLANG.h"
#include "LSPClangdDetector.hpp"
#include "asyncprocess.h"
#include "clFilesCollector.h"
#include "file_logger.h"
#include "globals.h"
#include <wx/filename.h>
#include <wx/regex.h>

LSPClangdDetector::LSPClangdDetector()
    : LSPDetector("clangd")
{
}

LSPClangdDetector::~LSPClangdDetector() {}

bool LSPClangdDetector::DoLocate()
{
#if defined(__WXMSW__)
    // Try the installation folder first
    wxFileName clangdExe(clStandardPaths::Get().GetExecutablePath());
    clangdExe.AppendDir("lsp");
    clangdExe.SetName("clangd");
    if(clangdExe.FileExists() && clangdExe.IsFileExecutable()) {
        ConfigureFile(clangdExe);
        return true;
    }

#elif defined(__WXOSX__)
    // Try the installation folder first
    wxFileName clangdExe(clStandardPaths::Get().GetBinaryFullPath("clangd"));
    if(clangdExe.FileExists()) {
        ConfigureFile(clangdExe);
        return true;
    }
#elif defined(__WXGTK__)
    // try the obvious name first: "clangd"
    wxFileName fnClangdExe("/usr/bin", "clangd");
    if(fnClangdExe.FileExists()) {
        clSYSTEM() << "Fonund clangd ==>" << fnClangdExe << endl;
        ConfigureFile(fnClangdExe);
        return true;
    }
    wxArrayString suffix;
    for(size_t i = 20; i >= 7; --i) {
        fnClangdExe.SetFullName(wxString() << "clangd-" << i);
        if(fnClangdExe.FileExists()) {
            clSYSTEM() << "Fonund clangd ==>" << fnClangdExe << endl;
            ConfigureFile(fnClangdExe);
            return true;
        }
    }
#endif
    CompilerLocatorCLANG locator;
    wxFileName fnClangd;
#if defined(__WXMSW__)
    fnClangd.SetExt("exe");
#endif

    if(locator.Locate()) {
        static wxRegEx reClangd("clangd([0-9\\-]*)", wxRE_DEFAULT);
        const auto& compilers = locator.GetCompilers();
        if(compilers.empty()) {
            return false;
        }

        for(const auto& compiler : compilers) {
            wxFileName cxx = compiler->GetTool("CXX");
            if(!cxx.IsOk()) {
                continue;
            }
            clFilesScanner scanner;
            clFilesScanner::EntryData::Vec_t files;
            if(scanner.ScanNoRecurse(cxx.GetPath(), files, "clangd*")) {
                // Check for the existence of clangd
                for(const auto& d : files) {
                    if(!(d.flags & clFilesScanner::kIsFile)) {
                        continue;
                    }
                    fnClangd = d.fullpath;
                    clDEBUG() << "==> Found" << fnClangd;
                    ConfigureFile(fnClangd);
                    // Stop at the first match
                    return true;
                }
            }
        }
    }
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
