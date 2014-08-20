//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : CompilerLocatorCrossGCC.cpp
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

#include "CompilerLocatorCrossGCC.h"
#include <wx/dir.h>
#include <wx/filefn.h>
#include <wx/tokenzr.h>
#include <globals.h>
#include "file_logger.h"
#include "procutils.h"

CompilerLocatorCrossGCC::CompilerLocatorCrossGCC()
{
}

CompilerLocatorCrossGCC::~CompilerLocatorCrossGCC()
{
}

CompilerPtr CompilerLocatorCrossGCC::Locate(const wxString& folder)
{
    return Locate(folder, true);
}

CompilerPtr CompilerLocatorCrossGCC::Locate(const wxString& folder, bool clear)
{
    if (clear)
        m_compilers.clear();

    wxArrayString matches;
    int count;
#ifdef __WXMSW__
    count = wxDir::GetAllFiles(folder, &matches, "*-gcc.exe", wxDIR_FILES);
#else
    count = wxDir::GetAllFiles(folder, &matches, "*-gcc", wxDIR_FILES);
#endif

    if ( count == 0 )
        return NULL;

    for ( int i = 0; i < count; ++i ) {
#ifndef __WXMSW__
        // Check if this is a script
        char sha[2];
        wxFile(matches[i]).Read(sha, 2);
        if (strncmp(sha, "#!", 2) == 0) {
            continue;
        }
#endif
        wxFileName filename(matches.Item(i));
        if(filename.GetName() == "mingw32-gcc" || filename.GetName() == "x86_64-w64-mingw32-gcc") {
            // Don't include standard mingw32-gcc (32 and 64 bit) binaries
            // they will be picked up later by the MinGW locator
            continue;
        }
        
        CompilerPtr compiler( new Compiler(NULL) );
        compiler->SetCompilerFamily(COMPILER_FAMILY_GCC);

        // get the compiler version
        compiler->SetName( filename.GetName() );
        compiler->SetGenerateDependeciesFile(true);
        m_compilers.push_back( compiler );

        // we path the bin folder
        AddTools(compiler, filename.GetPath(),
                 filename.GetName().BeforeLast('-'), filename.GetExt());
    }
    
    if(m_compilers.empty()) {
        return NULL;
    } else {
        return *m_compilers.begin();
    }
}

bool CompilerLocatorCrossGCC::Locate()
{
    m_compilers.clear();

    // try to find a cross GCC in the PATH
    wxString pathValues;
    wxGetEnv("PATH", &pathValues);
    wxStringSet_t tried;

    if ( !pathValues.IsEmpty() ) {
        wxArrayString pathArray = ::wxStringTokenize(pathValues, wxPATH_SEP, wxTOKEN_STRTOK);
        for (size_t i = 0; i < pathArray.GetCount(); ++i) {
            if ( tried.count(pathArray[i]) )
                continue;
            Locate(pathArray[i], false);
            tried.insert( pathArray[i] );
        }
    }

    return !m_compilers.empty();
}

void CompilerLocatorCrossGCC::AddTools(CompilerPtr compiler,
                                       const wxString &binFolder,
                                       const wxString &prefix,
                                       const wxString &suffix)
{
    compiler->SetName("Cross GCC ( " + prefix + " )");
    compiler->SetInstallationPath( binFolder );

    CL_DEBUG("Found CrossGCC compiler under: %s. \"%s\"", binFolder, compiler->GetName());
    wxFileName toolFile(binFolder, "");

    toolFile.SetFullName(prefix + "-g++");
    toolFile.SetExt(suffix);
    AddTool(compiler, "CXX", toolFile.GetFullPath(), suffix);
    AddTool(compiler, "LinkerName", toolFile.GetFullPath());
    AddTool(compiler, "SharedObjectLinkerName", toolFile.GetFullPath(), "-shared -fPIC");

    toolFile.SetFullName(prefix + "-gcc");
    toolFile.SetExt(suffix);
    AddTool(compiler, "CC", toolFile.GetFullPath());

    toolFile.SetFullName(prefix + "-ar");
    toolFile.SetExt(suffix);
    AddTool(compiler, "AR", toolFile.GetFullPath(), "rcu");

    toolFile.SetFullName(prefix + "-windres");
    toolFile.SetExt(suffix);
    if (toolFile.FileExists())
        AddTool(compiler, "ResourceCompiler", toolFile.GetFullPath());

    toolFile.SetFullName(prefix + "-as");
    toolFile.SetExt(suffix);
    AddTool(compiler, "AS", toolFile.GetFullPath());

    toolFile.SetFullName(prefix + "-gdb");
    toolFile.SetExt(suffix);
    AddTool(compiler, "Debugger", toolFile.GetFullPath());

    toolFile.SetFullName("make");
    toolFile.SetExt(suffix);
    wxString makeExtraArgs;
    if ( wxThread::GetCPUCount() > 1 ) {
        makeExtraArgs << "-j" << wxThread::GetCPUCount();
    }

    // XXX Need this on Windows?
    //makeExtraArgs <<  " SHELL=cmd.exe ";

    // What to do if there's no make here? (on Windows)
    if (toolFile.FileExists())
        AddTool(compiler, "MAKE", toolFile.GetFullPath(), makeExtraArgs);
}

void CompilerLocatorCrossGCC::AddTool(CompilerPtr compiler, const wxString& toolname, const wxString& toolpath, const wxString& extraArgs)
{
    wxString tool = toolpath;
    ::WrapWithQuotes(tool);
    compiler->SetTool(toolname, tool + " " + extraArgs);
    CL_DEBUG("Adding tool: %s => %s", toolname, tool);
}

