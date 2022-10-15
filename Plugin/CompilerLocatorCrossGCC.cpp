//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
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

#include "clFilesCollector.h"
#include "file_logger.h"
#include "procutils.h"

#include <globals.h>
#include <wx/dir.h>
#include <wx/filefn.h>
#include <wx/tokenzr.h>

CompilerLocatorCrossGCC::CompilerLocatorCrossGCC() {}

CompilerLocatorCrossGCC::~CompilerLocatorCrossGCC() {}

CompilerPtr CompilerLocatorCrossGCC::Locate(const wxString& folder) { return Locate(folder, true); }

CompilerPtr CompilerLocatorCrossGCC::Locate(const wxString& folder, bool clear)
{
    if(clear) {
        m_compilers.clear();
    }

    wxFileName fnFolder(folder, "");

    // We collect "*-gcc" files
    wxString pattern = "*-gcc";
#if defined(__WXMSW__) || defined(__CYGWIN__)
    pattern << ".exe";
#endif

    clFilesScanner scanner;
    clFilesScanner::EntryData::Vec_t results;
    size_t count = scanner.ScanNoRecurse(fnFolder.GetPath(), results, pattern);
    if(count == 0) {
        // try to see if we have a 'bin' folder under 'folder'
        fnFolder.AppendDir("bin");
        if(wxFileName::DirExists(fnFolder.GetPath())) {
            count = scanner.ScanNoRecurse(fnFolder.GetPath(), results, pattern);
        }
    }

    if(count == 0)
        return nullptr;

    wxArrayString matches;
    matches.reserve(results.size());
    for(const auto& entry : results) {
        matches.Add(entry.fullpath);
    }

    for(int i = 0; i < count; ++i) {
#ifndef __WXMSW__
        // Check if this is a script
        char sha[2];
        wxFile(matches[i]).Read(sha, 2);
        if(strncmp(sha, "#!", 2) == 0) {
            continue;
        }
#endif
        wxFileName filename(matches.Item(i));
        if(!IsCrossGCC(filename.GetName())) {
            continue;
        }

        CompilerPtr compiler(new Compiler(NULL));
        compiler->SetCompilerFamily(COMPILER_FAMILY_GCC);

        // get the compiler version
        compiler->SetName(filename.GetName());
        compiler->SetGenerateDependeciesFile(true);
        m_compilers.push_back(compiler);

        // we path the bin folder
        AddTools(compiler, filename.GetPath(), filename.GetName().BeforeLast('-'), filename.GetExt());
    }

    if(m_compilers.empty()) {
        return NULL;
    } else {
        return *m_compilers.begin();
    }
}

bool CompilerLocatorCrossGCC::IsCrossGCC(const wxString& name) const
{
#ifdef __WXMSW__
    if(name == "mingw32-gcc" || name == "i686-w64-mingw32-gcc" || name == "x86_64-w64-mingw32-gcc") {
        // Don't include standard mingw32-gcc (32 and 64 bit) binaries
        // they will be picked up later by the MinGW locator
        return false;
    }
#elif defined(__WXGTK__)
    if(name == "i686-linux-gnu-gcc" || name == "x86_64-linux-gnu-gcc" || name == "i686-pc-linux-gnu-gcc" ||
       name == "x86_64-pc-linux-gnu-gcc" || name == "i686-redhat-linux-gcc" || name == "x86_64-redhat-linux-gcc") {
        // Standard gcc will be picked up later by the GCC locator
        return false;
    }
#ifdef __CYGWIN__
#ifdef __i386__
    if(name == "i686-pc-cygwin-gcc")
        // Standard gcc will be picked up later by the GCC locator
        return false;
#elif defined(__x86_64__)
    if(name == "x86_64-pc-cygwin-gcc")
        // Standard gcc will be picked up later by the GCC locator
        return false;
#endif // __x86_64__
#endif // __CYGWIN__
#endif // __WXGTK__

    return true;
}

bool CompilerLocatorCrossGCC::Locate()
{
    m_compilers.clear();

    // try to find a cross GCC in the PATH
    wxString pathValues;
    wxGetEnv("PATH", &pathValues);
    wxStringSet_t tried;

    if(!pathValues.IsEmpty()) {
        wxArrayString pathArray = ::wxStringTokenize(pathValues, wxPATH_SEP, wxTOKEN_STRTOK);
        for(size_t i = 0; i < pathArray.GetCount(); ++i) {
            if(tried.count(pathArray[i]))
                continue;
            Locate(pathArray[i], false);
            tried.insert(pathArray[i]);
        }
    }

    return !m_compilers.empty();
}

void CompilerLocatorCrossGCC::AddTools(CompilerPtr compiler, const wxString& binFolder, const wxString& prefix,
                                       const wxString& suffix)
{
    compiler->SetName("Cross GCC ( " + prefix + " )");
    compiler->SetInstallationPath(binFolder);

    clDEBUG() << "Found CrossGCC compiler under:" << binFolder << "Name:" << compiler->GetName() << endl;
    wxFileName toolFile(binFolder, "");

    toolFile.SetFullName(prefix + "-g++");
    toolFile.SetExt(suffix);
    AddTool(compiler, "CXX", toolFile.GetFullPath());
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
    if(toolFile.FileExists())
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
    if(wxThread::GetCPUCount() > 1) {
        makeExtraArgs << "-j" << wxThread::GetCPUCount();
    }

    // XXX Need this on Windows?
    // makeExtraArgs <<  " SHELL=cmd.exe ";

    // What to do if there's no make here? (on Windows)
    if(toolFile.FileExists())
        AddTool(compiler, "MAKE", toolFile.GetFullPath(), makeExtraArgs);
}

void CompilerLocatorCrossGCC::AddTool(CompilerPtr compiler, const wxString& toolname, const wxString& toolpath,
                                      const wxString& extraArgs)
{
    wxString tool = toolpath;
    ::WrapWithQuotes(tool);
    if(!extraArgs.IsEmpty()) {
        tool << " " << extraArgs;
    }
    compiler->SetTool(toolname, tool);
}
