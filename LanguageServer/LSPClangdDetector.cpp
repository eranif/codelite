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
    wxFileName clangdExe(clStandardPaths::Get().GetUserDataDir(), "");
    clangdExe.AppendDir("lsp");
    clangdExe.AppendDir("clang-tools");
    clangdExe.SetFullName("clangd");
    if(clangdExe.FileExists()) {
        ConfigureFile(clangdExe);
        // this clangd requires LD_LIBRARY_PATH set properly
        clEnvList_t environment;
        environment.push_back({ "LD_LIBRARY_PATH", clangdExe.GetPath() });
        SetEnv(environment);
        return true;
    } else {
        // see if we need to copy it from the installation folder
        wxFileName toolsFile(clStandardPaths::Get().GetDataDir(), "clang-tools.tgz");
        if(toolsFile.FileExists()) {
            wxString clang_tools_tgz = toolsFile.GetFullPath();
            ::WrapWithQuotes(clang_tools_tgz);

            wxFileName fnToolsDir(clStandardPaths::Get().GetUserDataDir(), "");
            fnToolsDir.AppendDir("lsp");
            wxString tools_local_folder = fnToolsDir.GetPath();
            ::WrapWithQuotes(tools_local_folder);

            // sh -c 'mkdir -p ~/.codelite/lsp/ && tar xvfz /usr/share/codelite/clang-tools.tgz -C ~/.codelite/lsp/'
            wxString command;
            command << "mkdir -p " << tools_local_folder << " && tar xvfz " << clang_tools_tgz << " -C "
                    << tools_local_folder;
            ::WrapInShell(command);

            clDEBUG() << "Running:" << command << clEndl;
            IProcess::Ptr_t process(::CreateSyncProcess(command));
            if(process) {
                wxString output;
                process->WaitForTerminate(output);
                clDEBUG() << output << clEndl;

                // Check again if clangd exists
                if(clangdExe.FileExists()) {
                    ConfigureFile(clangdExe);
                    // this clangd requires LD_LIBRARY_PATH set properly
                    clEnvList_t environment;
                    environment.push_back({ "LD_LIBRARY_PATH", clangdExe.GetPath() });
                    SetEnv(environment);
                    return true;
                }
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
