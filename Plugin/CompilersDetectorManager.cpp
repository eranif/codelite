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

#include "CompilersDetectorManager.h"
#include "CompilerLocatorMinGW.h"
#include "CompilerLocatorGCC.h"
#include "CompilerLocatorCrossGCC.h"
#include "CompilerLocatorMSVC.h"
#include "CompilerLocatorCLANG.h"
#include "CompilerLocatorCygwin.h"
#include <wx/utils.h>
#include <wx/msgdlg.h>
#include "cl_config.h"
#include "macros.h"
#include <wx/arrstr.h>
#include <wx/choicdlg.h>
#include "includepathlocator.h"
#include "build_settings_config.h"
#include "JSON.h"
#include <wx/stream.h>
#include <wx/url.h>
#include "environmentconfig.h"
#include "CompilerLocatorEosCDT.h"

CompilersDetectorManager::CompilersDetectorManager()
{
#ifdef __WXMSW__
    m_detectors.push_back(ICompilerLocator::Ptr_t(new CompilerLocatorMinGW()));
    m_detectors.push_back(ICompilerLocator::Ptr_t(new CompilerLocatorCLANG()));
    m_detectors.push_back(ICompilerLocator::Ptr_t(new CompilerLocatorMSVC()));
    m_detectors.push_back(ICompilerLocator::Ptr_t(new CompilerLocatorCygwin()));

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
}

CompilersDetectorManager::~CompilersDetectorManager() {}

bool CompilersDetectorManager::Locate()
{
    // Apply the enviroment before searching for compilers
    // Some of the locators are relying on PATH environment
    // variable (e.g. MinGW)
    EnvSetter env;
    m_compilersFound.clear();
    ICompilerLocator::Vect_t::iterator iter = m_detectors.begin();
    for(; iter != m_detectors.end(); ++iter) {
        if((*iter)->Locate()) {
            m_compilersFound.insert(
                m_compilersFound.end(), (*iter)->GetCompilers().begin(), (*iter)->GetCompilers().end());
        }
    }
    for(size_t i = 0; i < m_compilersFound.size(); ++i) {
        MSWFixClangToolChain(m_compilersFound.at(i), m_compilersFound);
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

#define DLBUFSIZE 1024
void CompilersDetectorManager::MSWSuggestToDownloadMinGW(bool prompt)
{
#ifdef __WXMSW__
    if(!prompt ||
       ::wxMessageBox(_("Could not locate any MinGW compiler installed on your machine, would you like to "
                        "install one now?"),
                      "CodeLite",
                      wxYES_NO | wxCANCEL | wxYES_DEFAULT | wxCENTER | wxICON_QUESTION) == wxYES) {
        // No MinGW compiler detected!, offer the user to download one
        wxStringMap_t mingwCompilers;
        wxArrayString options;

        // Load the compilers list from the website
        wxURL url("http://codelite.org/compilers.json");

        if(url.GetError() == wxURL_NOERR) {

            wxInputStream* in_stream = url.GetInputStream();
            if(!in_stream) {
                return;
            }
            unsigned char buffer[DLBUFSIZE + 1];
            wxString dataRead;
            do {
                in_stream->Read(buffer, DLBUFSIZE);
                size_t bytes_read = in_stream->LastRead();
                if(bytes_read > 0) {
                    buffer[bytes_read] = 0;
                    wxString buffRead((const char*)buffer, wxConvUTF8);
                    dataRead.Append(buffRead);
                }

            } while(!in_stream->Eof());

            JSON root(dataRead);
            JSONItem compilers = root.toElement().namedObject("Compilers");
            JSONItem arr = compilers.namedObject("MinGW");
            int count = arr.arraySize();
            for(int i = 0; i < count; ++i) {
                JSONItem compiler = arr.arrayItem(i);
                mingwCompilers.insert(
                    std::make_pair(compiler.namedObject("Name").toString(), compiler.namedObject("URL").toString()));
                options.Add(compiler.namedObject("Name").toString());
            }

            if(options.IsEmpty()) {
                ::wxMessageBox(_("Unable to fetch compilers list from the website\nhttp://codelite.org/compilers.json"),
                               "CodeLite",
                               wxOK | wxCENTER | wxICON_WARNING);
                return;
            }
            int sel = 0;

            wxString selection =
                ::wxGetSingleChoice(_("Select a compiler to download"), _("Choose compiler"), options, sel);
            if(!selection.IsEmpty()) {
                // Reset the compiler detection flag so next time codelite is restarted, it will
                // rescan the machine
                clConfig::Get().Write(kConfigBootstrapCompleted, false);

                // Open the browser to start downloading the compiler
                ::wxLaunchDefaultBrowser(mingwCompilers.find(selection)->second);
                ::wxMessageBox(_("After install is completed, click the 'Scan' button"),
                               "CodeLite",
                               wxOK | wxCENTER | wxICON_INFORMATION);
            }
        }
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
                IncludePathLocator locator(NULL);
                wxArrayString includePaths, excludePaths;
                locator.Locate(includePaths, excludePaths, false, mingwCmp->GetTool("CXX"));

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
