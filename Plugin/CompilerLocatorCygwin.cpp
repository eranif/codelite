//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : CompilerLocatorCygwin.cpp
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

#include "CompilerLocatorCygwin.h"

#include "file_logger.h"
#include "globals.h"
#include "procutils.h"

#include <wx/regex.h>

#ifdef __WXMSW__
#include <wx/msw/registry.h>
#endif

CompilerLocatorCygwin::CompilerLocatorCygwin()
    : ICompilerLocator()
{
}

bool CompilerLocatorCygwin::Locate()
{
    m_compilers.clear();
#ifdef __WXMSW__ // for wxRegKey
    {
        wxRegKey regkey(wxRegKey::HKLM, "SOFTWARE\\Cygwin\\setup");
        wxString cygwinInstallDir;
        if(regkey.QueryValue("rootdir", cygwinInstallDir) && wxDirExists(cygwinInstallDir)) {
            Locate(cygwinInstallDir);
        }
    }
    {
        // If we are running a 64 bit version of CodeLite, we should search under the
        // Wow6432Node
        wxRegKey regkey(wxRegKey::HKLM, "SOFTWARE\\Wow6432Node\\Cygwin\\setup");
        wxString cygwinInstallDir;
        if(regkey.QueryValue("rootdir", cygwinInstallDir) && wxDirExists(cygwinInstallDir)) {
            Locate(cygwinInstallDir);
        }
    }

#endif
    return !m_compilers.empty();
}

void CompilerLocatorCygwin::AddTools(const wxString& binFolder, const wxString& name, const wxString& suffix)
{
    wxFileName masterPath(binFolder, "");
    masterPath.RemoveLastDir();

    // Create an empty compiler
    CompilerPtr compiler(new Compiler(NULL));
    compiler->SetCompilerFamily(COMPILER_FAMILY_CYGWIN);
    compiler->SetGenerateDependeciesFile(true);
    compiler->SetName(name);
    compiler->SetInstallationPath(masterPath.GetPath());
    m_compilers.push_back(compiler);

    clDEBUG() << "Found Cygwin compiler under:" << masterPath.GetPath() << ". Name:" << compiler->GetName() << endl;
    wxFileName toolFile(binFolder, "");

    toolFile.SetFullName("g++-" + suffix + ".exe");
    AddTool(compiler, "CXX", toolFile.GetFullPath());
    AddTool(compiler, "LinkerName", toolFile.GetFullPath());
    AddTool(compiler, "SharedObjectLinkerName", toolFile.GetFullPath(), "-shared -fPIC");

    toolFile.SetFullName("gcc-" + suffix + ".exe");
    AddTool(compiler, "CC", toolFile.GetFullPath());

    toolFile.SetFullName("ar.exe");
    AddTool(compiler, "AR", toolFile.GetFullPath(), "rcu");

    toolFile.SetFullName("windres.exe");
    AddTool(compiler, "ResourceCompiler", toolFile.GetFullPath());

    toolFile.SetFullName("as.exe");
    AddTool(compiler, "AS", toolFile.GetFullPath());

    toolFile.SetFullName("make.exe");
    wxString makeExtraArgs;
    if(wxThread::GetCPUCount() > 1) {
        makeExtraArgs << "-j" << wxThread::GetCPUCount();
    }

    if(toolFile.FileExists()) {
        AddTool(compiler, "MAKE", toolFile.GetFullPath(), makeExtraArgs);

    } else {
        toolFile.SetFullName("mingw32-make.exe");
        if(toolFile.FileExists()) {
            AddTool(compiler, "MAKE", toolFile.GetFullPath(), makeExtraArgs);
        }
    }
    AddTool(compiler, "MakeDirCommand", "mkdir", "-p");

    toolFile.SetFullName("gdb.exe");
    if(toolFile.FileExists()) {
        AddTool(compiler, "Debugger", toolFile.GetFullPath());
    }
}

void CompilerLocatorCygwin::AddTool(CompilerPtr compiler, const wxString& toolname, const wxString& toolpath,
                                    const wxString& extraArgs)
{
    wxString tool = toolpath;
    ::WrapWithQuotes(tool);

    // Cygwin does not like backslahes... replace the tools to use /
    tool.Replace("\\", "/");
    if(!extraArgs.IsEmpty()) {
        tool << " " << extraArgs;
    }
    compiler->SetTool(toolname, tool);
}

wxString CompilerLocatorCygwin::GetGCCVersion(const wxString& gccBinary)
{
    static wxRegEx reVersion("([0-9]+\\.[0-9]+\\.[0-9]+)");
    wxString command;
    command << gccBinary << " --version";
    wxString versionString = ProcUtils::SafeExecuteCommand(command);
    if(!versionString.IsEmpty() && reVersion.Matches(versionString)) {
        return reVersion.GetMatch(versionString);
    }
    return wxEmptyString;
}

CompilerPtr CompilerLocatorCygwin::Locate(const wxString& folder)
{
    m_compilers.clear();

    wxString binFolder;
    wxFileName gcc(folder, "gcc.exe");
    if(gcc.FileExists()) {
        binFolder = gcc.GetPath();
    } else {
        gcc.AppendDir("bin");
        if(gcc.FileExists()) {
            binFolder = gcc.GetPath();
        }
    }

    if(binFolder.IsEmpty())
        return NULL;

    wxArrayString suffixes = GetSuffixes(binFolder);
    if(suffixes.IsEmpty())
        return NULL;

    for(size_t i = 0; i < suffixes.GetCount(); ++i) {
        gcc.SetFullName("gcc-" + suffixes.Item(i) + ".exe");
        wxString gccVer = GetGCCVersion(gcc.GetFullPath());

        wxString compilerName;
        compilerName << "Cygwin";
        if(!gccVer.IsEmpty()) {
            compilerName << " - " << gccVer;
        }
        // Add the tools (use the bin folder)
        AddTools(gcc.GetPath(), compilerName, suffixes.Item(i));
    }
    return m_compilers.at(0);
}

wxArrayString CompilerLocatorCygwin::GetSuffixes(const wxString& binFolder)
{
    wxFileName gcc3(binFolder, "gcc-3.exe");
    wxFileName gcc4(binFolder, "gcc-4.exe");
    wxFileName gcc5(binFolder, "gcc-5.exe");

    wxArrayString arr;
    if(gcc3.FileExists())
        arr.Add("3");
    if(gcc4.FileExists())
        arr.Add("4");
    if(gcc5.FileExists())
        arr.Add("5");
    return arr;
}
