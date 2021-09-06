//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : CompilersDetectorManager.cpp
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#include "CompilerLocatorCLANG.h"
#include "CompilerLocatorCrossGCC.h"
#include "CompilerLocatorCygwin.h"
#include "CompilerLocatorEosCDT.h"
#include "CompilerLocatorGCC.h"
#include "CompilerLocatorMSVC.h"
#include "CompilerLocatorMSYS2.hpp"
#include "CompilerLocatorMinGW.h"
#include "CompilerLocatorRustc.hpp"
#include "CompilersDetectorManager.h"
#include "GCCMetadata.hpp"
#include "JSON.h"
#include "build_settings_config.h"
#include "cl_config.h"
#include "environmentconfig.h"
#include "macros.h"
#include <wx/arrstr.h>
#include <wx/choicdlg.h>
#include <wx/msgdlg.h>
#include <wx/stream.h>
#include <wx/url.h>
#include <wx/utils.h>

CompilersDetectorManager::CompilersDetectorManager()
{
#ifdef __WXMSW__
    m_detectors.push_back(ICompilerLocator::Ptr_t(new CompilerLocatorMinGW()));
    m_detectors.push_back(ICompilerLocator::Ptr_t(new CompilerLocatorCLANG()));
    m_detectors.push_back(ICompilerLocator::Ptr_t(new CompilerLocatorMSVC()));
    m_detectors.push_back(ICompilerLocator::Ptr_t(new CompilerLocatorCygwin()));
    m_detectors.push_back(ICompilerLocator::Ptr_t(new CompilerLocatorMSYS2()));

#elif defined(__WXGTK__)
    m_detectors.push_back(ICompilerLocator::Ptr_t(new CompilerLocatorGCC()));
    m_detectors.push_back(ICompilerLocator::Ptr_t(new CompilerLocatorCLANG()));
    m_detectors.push_back(ICompilerLocator::Ptr_t(new CompilerLocatorEosCDT()));

#elif defined(__WXMAC__)
    m_detectors.push_back(ICompilerLocator::Ptr_t(new CompilerLocatorGCC()));
    m_detectors.push_back(ICompilerLocator::Ptr_t(new CompilerLocatorCLANG()));
    m_detectors.push_back(ICompilerLocator::Ptr_t(new CompilerLocatorEosCDT()));

#endif
    m_detectors.push_back(ICompilerLocator::Ptr_t(new CompilerLocatorCrossGCC()));
    m_detectors.push_back(ICompilerLocator::Ptr_t(new CompilerLocatorRustc()));
}

CompilersDetectorManager::~CompilersDetectorManager() {}

bool CompilersDetectorManager::Locate()
{
    // Apply the enviroment before searching for compilers
    // Some of the locators are relying on PATH environment
    // variable (e.g. MinGW)
    EnvSetter env;
    m_compilersFound.clear();
    wxStringSet_t S;
    for(auto locator : m_detectors) {
        if(locator->Locate()) {
            for(auto compiler : locator->GetCompilers()) {
                // We'll detect the duplicated compilers by:
                // [1] Recursively resolving the symbolic links, e.g.
                //       /usr/bin/g++ (GCC)
                //         -> /usr/bin/g++-9 (GCC-9)
                //           -> /usr/bin/x86_64-linux-gnu-g++-9
                // [2] Checking whether an identical compiler has already been found
                //     (some executables are same, but not symlinked; i.e. a distinct copy or hard-linked),
                //     e.g.
                //       /bin/clang++-12 (CLANG-12)
                //         -> /lib/llvm-12/bin/clang++
                //           -> /lib/llvm-12/bin/clang
                //     and
                //       /usr/bin/clang++-12 (CLANG-12)
                //         -> /usr/lib/llvm-12/bin/clang++
                //           -> /usr/lib/llvm-12/bin/clang
                wxString cxxPath = GetRealCXXPath(compiler);
                if(S.count(compiler->GetName()) == 0 && !cxxPath.IsEmpty() && S.count(cxxPath) == 0) {
                    S.insert(compiler->GetName());
                    S.insert(cxxPath);
                    m_compilersFound.push_back(compiler);
                }
            }
        }
    }

    for(auto compiler : m_compilersFound) {
        MSWFixClangToolChain(compiler, m_compilersFound);
    }
    return !m_compilersFound.empty();
}

CompilerPtr CompilersDetectorManager::Locate(const wxString& folder)
{
    m_compilersFound.clear();
    ICompilerLocator::Vect_t::iterator iter = m_detectors.begin();
    for(; iter != m_detectors.end(); ++iter) {
        CompilerPtr comp = (*iter)->Locate(folder);
        if(comp) {
            MSWFixClangToolChain(comp);
            return comp;
        }
    }
    return NULL;
}

bool CompilersDetectorManager::FoundMinGWCompiler() const
{
    for(size_t i = 0; i < m_compilersFound.size(); ++i) {
        CompilerPtr compiler = m_compilersFound.at(i);
        if(compiler->GetCompilerFamily() == COMPILER_FAMILY_MINGW) {
            // we found at least one MinGW compiler
            return true;
        }
    }
    return false;
}

void CompilersDetectorManager::MSWSuggestToDownloadMinGW(bool prompt)
{
#ifdef __WXMSW__
    if(!prompt ||
       ::wxMessageBox(_("Could not locate any MinGW compiler installed on your machine, would you like to "
                        "install one now?"),
                      "CodeLite", wxYES_NO | wxCANCEL | wxYES_DEFAULT | wxCENTER | wxICON_QUESTION) == wxYES) {

        // open the install compiler page
        ::wxLaunchDefaultBrowser("https://docs.codelite.org/build/mingw_builds/#prepare-a-working-environment");

        // Prompt the user on how to proceed
        ::wxMessageBox(_("After the installation process is done\nClick the 'Scan' button"), "CodeLite",
                       wxOK | wxCENTER | wxICON_INFORMATION);
    }
#endif // __WXMSW__
}

void CompilersDetectorManager::MSWFixClangToolChain(CompilerPtr compiler,
                                                    const ICompilerLocator::CompilerVec_t& allCompilers)
{
// Update the toolchain (if Windows)
#ifdef __WXMSW__
    ICompilerLocator::CompilerVec_t compilers;
    if(allCompilers.empty()) {
        BuildSettingsConfigCookie cookie;
        CompilerPtr cmp = BuildSettingsConfigST::Get()->GetFirstCompiler(cookie);
        while(cmp) {
            compilers.push_back(cmp);
            cmp = BuildSettingsConfigST::Get()->GetNextCompiler(cookie);
        }
    } else {
        compilers.insert(compilers.end(), allCompilers.begin(), allCompilers.end());
    }

    if(compiler->GetCompilerFamily() == COMPILER_FAMILY_CLANG) {
        for(size_t i = 0; i < compilers.size(); ++i) {
            CompilerPtr mingwCmp = compilers.at(i);
            if(mingwCmp->GetCompilerFamily() == COMPILER_FAMILY_MINGW) {
                compiler->SetTool("MAKE", mingwCmp->GetTool("MAKE"));
                compiler->SetTool("ResourceCompiler", mingwCmp->GetTool("ResourceCompiler"));

                // Update the include paths
                GCCMetadata compiler_md("MinGW");
                wxArrayString includePaths;
                compiler_md.Load(mingwCmp->GetTool("CXX"), mingwCmp->GetInstallationPath());
                includePaths = compiler_md.GetSearchPaths();

                // Convert the include paths to semi colon separated list
                wxString mingwIncludePaths = wxJoin(includePaths, ';');
                compiler->SetGlobalIncludePath(mingwIncludePaths);

                // Keep the mingw's bin path
                wxFileName mingwGCC(mingwCmp->GetTool("CXX"));
                compiler->SetPathVariable(mingwGCC.GetPath());
                break;
            }
        }
    }
#endif
}

wxString CompilersDetectorManager::GetRealCXXPath(const CompilerPtr compiler) const
{
    if(compiler->GetName() == "rustc") {
        // rustc is a dummy compiler, do not touch it
        return compiler->GetTool("CXX");
    }
    return ResolveLink(compiler->GetTool("CXX"));
}

wxString CompilersDetectorManager::ResolveLink(const wxString& path) const
{
#if !wxCHECK_VERSION(3, 1, 5)
    // Try old 'directory-only' comparison
    return wxFileName(path).GetPath();
#elif defined(__WXMSW__)
    // There is no symlink support in Windows
    return path;
#else
    wxString actualPath, symlinkPath = path;
    wxStringSet_t pathStack;
    while(true) {
        actualPath = wxFileName(symlinkPath).ResolveLink().GetFullPath();
        if(actualPath.IsEmpty()) {
            // The link is no longer alive
            return wxEmptyString;
        }
        if(actualPath == symlinkPath) {
            // This is the real file we're looking for
            break;
        }
        if(pathStack.count(actualPath) > 0) {
            // Infinite-recursion link detected
            return wxEmptyString;
        }
        symlinkPath = actualPath;
        pathStack.insert(actualPath);
    }
    return actualPath;
#endif
}
