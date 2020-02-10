#include "CompilerLocatorCLANG.h"
#include "LSPClangdDetector.hpp"
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
