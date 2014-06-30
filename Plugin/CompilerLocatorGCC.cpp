//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : CompilerLocatorGCC.cpp
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

#include "CompilerLocatorGCC.h"
#include <wx/filename.h>
#include "file_logger.h"
#include "globals.h"

CompilerLocatorGCC::CompilerLocatorGCC()
{
}

CompilerLocatorGCC::~CompilerLocatorGCC()
{
}

CompilerPtr CompilerLocatorGCC::Locate(const wxString& folder)
{
    m_compilers.clear();
    wxFileName gcc(folder, "gcc");
    wxFileName tmpfn(folder, "");
    
    wxString name;
    if( tmpfn.GetDirCount() > 1 && tmpfn.GetDirs().Last() == "bin" ) {
        tmpfn.RemoveLastDir();
        name = tmpfn.GetDirs().Last();
    }
    
#ifdef __WXMSW__
    gcc.SetExt("exe");
#endif

    bool found = gcc.FileExists();
    if ( ! found ) {
        // try to see if we have a bin folder here
        gcc.AppendDir("bin");
        found = gcc.FileExists();
    }
    
    if ( found ) {
        CompilerPtr compiler( new Compiler(NULL) );
        compiler->SetCompilerFamily(COMPILER_FAMILY_GCC);
        
        // get the compiler version
        compiler->SetName( name.IsEmpty() ? "GCC" : name );
        compiler->SetGenerateDependeciesFile(true);
        m_compilers.push_back( compiler );
        
        // we path the bin folder
        AddTools(compiler, gcc.GetPath());
        return compiler;
    }
    return NULL;
}

bool CompilerLocatorGCC::Locate()
{
    // Locate GCC under /usr/bin
    m_compilers.clear();

    wxArrayString gcc_versions;
    gcc_versions.Add(""); // Default gcc
    gcc_versions.Add("4.2");
    gcc_versions.Add("4.3");
    gcc_versions.Add("4.4");
    gcc_versions.Add("4.5");
    gcc_versions.Add("4.6");
    gcc_versions.Add("4.7");
    gcc_versions.Add("4.8");
    gcc_versions.Add("4.9");

    for(size_t i=0; i<gcc_versions.GetCount(); ++i) {
        wxString suffix = gcc_versions.Item(i);
        if ( !suffix.IsEmpty() ) {
            suffix.Prepend("-");
        }

        wxFileName gccFile("/usr/bin", "gcc" + suffix);
        if ( gccFile.FileExists() ) {
            // add this compiler
            CompilerPtr compiler( new Compiler(NULL) );
            wxString toolchainName;
            toolchainName << "GCC";
            if ( !gcc_versions.Item(i).IsEmpty() ) {
                toolchainName << " ( " << gcc_versions.Item(i) << " )";
            }
            compiler->SetName( toolchainName );
            compiler->SetGenerateDependeciesFile(true);
            compiler->SetCompilerFamily(COMPILER_FAMILY_GCC);
            m_compilers.push_back( compiler );
            AddTools(compiler, "/usr/bin", gcc_versions.Item(i));
        }
    }

    // XCode GCC is installed under /Applications/Xcode.app/Contents/Developer/usr/bin
    wxFileName xcodeGcc("/Applications/Xcode.app/Contents/Developer/usr/bin", "gcc");
    if ( xcodeGcc.FileExists() ) {
        // add this compiler
        CompilerPtr compiler( new Compiler(NULL) );
        compiler->SetCompilerFamily(COMPILER_FAMILY_GCC);
        compiler->SetName("GCC ( XCode )");
        m_compilers.push_back( compiler );
        AddTools(compiler, xcodeGcc.GetPath());
    }

    return !m_compilers.empty();
}


void CompilerLocatorGCC::AddTools(CompilerPtr compiler,
                                  const wxString& binFolder,
                                  const wxString& suffix)
{
    wxFileName masterPath(binFolder, "");
    wxString defaultBinFolder = "/usr/bin";
    compiler->SetCompilerFamily(COMPILER_FAMILY_GCC);
    compiler->SetInstallationPath( binFolder );

    CL_DEBUG("Found GNU GCC compiler under: %s. \"%s\"", masterPath.GetPath(), compiler->GetName());
    wxFileName toolFile(binFolder, "");

    // ++++-----------------------------------------------------------------
    // With XCode installation, only
    // g++, gcc, and make are installed under the Xcode installation folder
    // the rest (mainly ar and as) are taken from /usr/bin
    // ++++-----------------------------------------------------------------

    toolFile.SetFullName("g++");
    AddTool(compiler, "CXX", toolFile.GetFullPath(), suffix);
    AddTool(compiler, "LinkerName", toolFile.GetFullPath(), suffix);
#ifndef __WXMAC__
    AddTool(compiler, "SharedObjectLinkerName", toolFile.GetFullPath(), suffix, "-shared -fPIC");
#else
    AddTool(compiler, "SharedObjectLinkerName", toolFile.GetFullPath(), suffix, "-dynamiclib -fPIC");
#endif
    toolFile.SetFullName("gcc");
    AddTool(compiler, "CC", toolFile.GetFullPath(), suffix);
    toolFile.SetFullName("make");
    wxString makeExtraArgs;
    if ( wxThread::GetCPUCount() > 1 ) {
        makeExtraArgs << "-j" << wxThread::GetCPUCount();
    }
    AddTool(compiler, "MAKE", toolFile.GetFullPath(), "", makeExtraArgs);

    // ++++-----------------------------------------------------------------
    // From this point on, we use /usr/bin only
    // ++++-----------------------------------------------------------------

    toolFile.AssignDir( defaultBinFolder );
    toolFile.SetFullName("ar");
    AddTool(compiler, "AR", toolFile.GetFullPath(), "", "rcu");

    toolFile.SetFullName("windres");
    AddTool(compiler, "ResourceCompiler", "", "");

    toolFile.SetFullName("as");
    AddTool(compiler, "AS", toolFile.GetFullPath(), "");
}

void CompilerLocatorGCC::AddTool(CompilerPtr compiler, const wxString& toolname, const wxString& toolpath, const wxString& suffix, const wxString& extraArgs)
{
    wxString tool = toolpath;
    if ( !suffix.IsEmpty() ) {
        tool << "-" << suffix;
    }

    ::WrapWithQuotes(tool);
    tool << " " << extraArgs;
    compiler->SetTool(toolname, tool);
    CL_DEBUG("Adding tool: %s => %s", toolname, tool);
}

