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
#elif defined(__WXGTK__)
    wxFileName clangdExe(clStandardPaths::Get().GetUserDataDir(), "");
    clangdExe.AppendDir("lsp");
    clangdExe.AppendDir("tools");
    clangdExe.SetFullName("clangd");
    if(clangdExe.FileExists()) {
        ConfigureFile(clangdExe);
        return true;
    } else {
        // see if we need to copy it from the installation folder
        wxFileName toolsFile(clStandardPaths::Get().GetDataDir(), "clang-tools.tgz");
        if(toolsFile.FileExists()) {
            // sh -c 'mkdir -p ~/.codelite/lsp/ && tar xvfz /usr/share/codelite/clang-tools.tgz -C ~/.codelite/lsp/'
            clDEBUG() << "Running: sh -c 'mkdir -p ~/.codelite/lsp/ && tar xvfz /usr/share/codelite/clang-tools.tgz -C "
                         "~/.codelite/lsp/'"
                      << clEndl;
            IProcess::Ptr_t process(::CreateSyncProcess("sh -c 'mkdir -p ~/.codelite/lsp/ && tar xvfz "
                                                        "/usr/share/codelite/clang-tools.tgz -C ~/.codelite/lsp/'"));
            if(process) {
                wxString output;
                process->WaitForTerminate(output);
                clDEBUG() << output << clEndl;
            }
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
