#include "CompilerLocatorCLANG.h"
#include "LSPClangdDetector.hpp"
#include "file_logger.h"
#include "globals.h"
#include <wx/filename.h>

LSPClangdDetector::LSPClangdDetector()
    : LSPDetector("clangd")
{
}

LSPClangdDetector::~LSPClangdDetector() {}

bool LSPClangdDetector::DoLocate()
{
    CompilerLocatorCLANG locator;
    wxFileName fnClangd;
#if defined(__WXMSW__)
    fnClangd.SetExt("exe");
#endif

    if(locator.Locate()) {
        const std::vector<wxString> suffixes = { "11", "10", "9", "8", "7", "6", "5", "4", "" };
        const auto& compilers = locator.GetCompilers();
        if(compilers.empty()) { return false; }
        for(const auto& compiler : compilers) {
            wxString cxx = compiler->GetTool("CXX");
            if(cxx.empty()) { continue; }
            fnClangd = cxx;
            // Check for the existence of clangd
            for(const wxString& suffix : suffixes) {
                clDEBUG1() << "Trying to locate clangd LSP v" << suffix;
                fnClangd.SetName("clangd" + (suffix.empty() ? wxString() : wxString("-" + suffix)));
                if(fnClangd.FileExists()) {
                    clDEBUG1() << "==> Found" << fnClangd;
                    wxString command;
                    command << fnClangd.GetFullPath();
                    ::WrapWithQuotes(command);
                    command << " -limit-results=100 -header-insertion-decorators=0";
                    SetCommand(command);
                    // Add support for the languages
                    GetLangugaes().Add("c");
                    GetLangugaes().Add("cpp");
                    SetConnectionString("stdio");
                    SetPriority(90); // clangd should override the default cc engine
                    // Stop at the first match
                    return true;
                }
            }
        }
    }
    return false;
}
