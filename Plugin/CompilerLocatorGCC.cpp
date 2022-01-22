//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
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

#include "clFilesCollector.h"
#include "file_logger.h"
#include "fileutils.h"
#include "globals.h"

#include <wx/filename.h>
#include <wx/regex.h>

CompilerLocatorGCC::CompilerLocatorGCC() {}

CompilerLocatorGCC::~CompilerLocatorGCC() {}

CompilerPtr CompilerLocatorGCC::Locate(const wxString& folder)
{
    m_compilers.clear();
    wxFileName gcc(folder, "gcc");
    wxFileName tmpfn(folder, "");

    wxString name;
    if(tmpfn.GetDirCount() > 1 && tmpfn.GetDirs().Last() == "bin") {
        tmpfn.RemoveLastDir();
        name = tmpfn.GetDirs().Last();
    }

#ifdef __WXMSW__
    gcc.SetExt("exe");
#endif

    bool found = gcc.FileExists();
    if(!found) {
        // try to see if we have a bin folder here
        gcc.AppendDir("bin");
        found = gcc.FileExists();
    }

    if(found) {
        CompilerPtr compiler(new Compiler(NULL));
        compiler->SetCompilerFamily(COMPILER_FAMILY_GCC);

        // get the compiler version
        compiler->SetName(name.IsEmpty() ? "GCC" : name);
        compiler->SetGenerateDependeciesFile(true);
        m_compilers.push_back(compiler);

        // we path the bin folder
        AddTools(compiler, gcc.GetPath());
        return compiler;
    }
    return NULL;
}

bool CompilerLocatorGCC::Locate()
{
    static wxRegEx reGcc("gcc([0-9\\-]*)", wxRE_DEFAULT);

    // Locate GCC under all the locations under PATH variable
    // Search the entire path locations and get all files that matches the pattern 'gcc*'
    wxArrayString paths = GetPaths();
    clFilesScanner scanner;
    clFilesScanner::EntryData::Vec_t outputFiles, tmpFiles;
    for(const wxString& path : paths) {
        if(scanner.ScanNoRecurse(path, tmpFiles, "gcc*")) {
            outputFiles.insert(outputFiles.end(), tmpFiles.begin(), tmpFiles.end());
        }
    }

    m_compilers.clear();
    wxStringMap_t map;
    for(const auto& d : outputFiles) {
        if(d.flags & clFilesScanner::kIsFile) {
            wxFileName gcc(d.fullpath);
#ifdef __CYGWIN__
            wxString fullname = gcc.GetName(); // no extension
#else
            wxString fullname = gcc.GetFullName(); // name + extension
#endif
            if(reGcc.IsValid() && reGcc.Matches(fullname)) {
                wxString acceptableName = "gcc" + reGcc.GetMatch(fullname, 1);
                if(fullname == acceptableName) {
                    // keep unique paths only + the suffix
                    map.insert({ d.fullpath, reGcc.GetMatch(fullname, 1) });
                }
            }
        }
    }
    for(const auto& vt : map) {
        // add this compiler
        wxFileName gcc(vt.first);
        CompilerPtr compiler(new Compiler(NULL));
        compiler->SetName(gcc.GetFullName().Upper());
        compiler->SetGenerateDependeciesFile(true);
        compiler->SetCompilerFamily(COMPILER_FAMILY_GCC);
        m_compilers.push_back(compiler);
        AddTools(compiler, "/usr/bin", vt.second);
    }

    // XCode GCC is installed under /Applications/Xcode.app/Contents/Developer/usr/bin
    wxFileName xcodeGcc("/Applications/Xcode.app/Contents/Developer/usr/bin", "gcc");
    if(xcodeGcc.FileExists()) {
        // add this compiler
        CompilerPtr compiler(new Compiler(NULL));
        compiler->SetCompilerFamily(COMPILER_FAMILY_GCC);
        compiler->SetName("GCC ( XCode )");
        m_compilers.push_back(compiler);
        AddTools(compiler, xcodeGcc.GetPath());
    }

    return !m_compilers.empty();
}

void CompilerLocatorGCC::AddTools(CompilerPtr compiler, const wxString& binFolder, const wxString& suffix)
{
    wxFileName masterPath(binFolder, "");
    wxString defaultBinFolder = "/usr/bin";
    compiler->SetCompilerFamily(COMPILER_FAMILY_GCC);
    compiler->SetInstallationPath(binFolder);

    clDEBUG() << "Found GNU GCC compiler under:" << masterPath.GetPath() << compiler->GetName();
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
    if(wxThread::GetCPUCount() > 1) {
        makeExtraArgs << "-j" << wxThread::GetCPUCount();
    }
    AddTool(compiler, "MAKE", toolFile.GetFullPath(), "", makeExtraArgs);

    // ++++-----------------------------------------------------------------
    // From this point on, we use /usr/bin only
    // ++++-----------------------------------------------------------------

    toolFile.AssignDir(defaultBinFolder);
    toolFile.SetFullName("ar");
    AddTool(compiler, "AR", toolFile.GetFullPath(), "", "rcu");

    toolFile.SetFullName("windres");
    AddTool(compiler, "ResourceCompiler", "", "");

    toolFile.SetFullName("as");
    AddTool(compiler, "AS", toolFile.GetFullPath(), "");

    toolFile.SetFullName("gdb");
    if(toolFile.Exists()) {
        AddTool(compiler, "Debugger", toolFile.GetFullPath(), "");
    }
}

void CompilerLocatorGCC::AddTool(CompilerPtr compiler, const wxString& toolname, const wxString& toolpath,
                                 const wxString& suffix, const wxString& extraArgs)
{
    wxString tool = toolpath + suffix;
    ::WrapWithQuotes(tool);
    if(!extraArgs.IsEmpty()) {
        tool << " " << extraArgs;
    }
    compiler->SetTool(toolname, tool);
    clDEBUG() << "Adding tool:" << toolname << "=>" << tool;
}
