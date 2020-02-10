//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : CompilerLocatorCLANG.cpp
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
#include "asyncprocess.h"
#include "build_settings_config.h"
#include "clFilesCollector.h"
#include "file_logger.h"
#include "fileutils.h"
#include "includepathlocator.h"
#include "procutils.h"
#include <globals.h>
#include <wx/regex.h>

#ifdef __WXMSW__
#include <wx/msw/registry.h>
#endif
#include <wx/filename.h>

#ifdef __WXOSX__
bool OSXFindBrewClang(wxFileName& clang)
{
    // We use brew to query the location of clang.
    // Clang is installed as part of the LLVM package
    IProcess::Ptr_t proc(::CreateSyncProcess("brew list llvm"));
    if(!proc) {
        return false;
    }
    wxString output;
    proc->WaitForTerminate(output);

    wxArrayString lines = ::wxStringTokenize(output, "\r\n", wxTOKEN_STRTOK);
    for(const wxString& line : lines) {
        if(line.Contains("bin/clang")) {
            clang = line;
            clang.SetFullName("clang");
            return true;
        }
    }
    return false;
}
#endif

CompilerLocatorCLANG::CompilerLocatorCLANG() {}

CompilerLocatorCLANG::~CompilerLocatorCLANG() {}

bool CompilerLocatorCLANG::Locate()
{
    m_compilers.clear();
    MSWLocate();
#ifdef __WXOSX__
    {
        wxFileName fnClang;
        if(OSXFindBrewClang(fnClang)) {
            AddCompiler(fnClang.GetPath(), "");
        }
    }
#endif

    // POSIX locate
    static wxRegEx reClang("clang([0-9\\-]*)", wxRE_DEFAULT);

    // Locate CLANG under all the locations under PATH variable
    // Search the entire path locations and get all files that matches the pattern 'clang*'
    wxArrayString paths = GetPaths();
    clFilesScanner scanner;
    clFilesScanner::EntryData::Vec_t outputFiles, tmpFiles;
    for(const wxString& path : paths) {
        if(scanner.ScanNoRecurse(path, tmpFiles, "clang*")) {
            outputFiles.insert(outputFiles.end(), tmpFiles.begin(), tmpFiles.end());
        }
    }


    wxStringMap_t map;
    for(const auto& d : outputFiles) {
        if(d.flags & clFilesScanner::kIsFile) {
            wxFileName clang(d.fullpath);
            wxString fullname = clang.GetFullName();
            if(reClang.IsValid() && reClang.Matches(fullname)) {
                wxString acceptableName = "clang" + reClang.GetMatch(fullname, 1);
                if(fullname == acceptableName) {
                    // keep unique paths only + the suffix
                    map.insert({ d.fullpath, reClang.GetMatch(fullname, 1) });
                }
            }
        }
    }

    for(const auto& vt : map) {
        wxFileName clang(vt.first);
        clDEBUG() << "==> Found" << clang.GetFullPath();
        AddCompiler(clang.GetPath(), vt.second);
    }
    return true;
}

CompilerPtr CompilerLocatorCLANG::Locate(const wxString& folder)
{
    m_compilers.clear();
    wxFileName clang(folder, "clang");
#ifdef __WXMSW__
    clang.SetExt("exe");
#endif
    bool found = clang.FileExists();
    if(!found) {
        // try to see if we have a bin folder here
        clang.AppendDir("bin");
        found = clang.FileExists();
    }

    if(found) {
        return AddCompiler(clang.GetPath(), "");
    }
    return NULL;
}

void CompilerLocatorCLANG::MSWLocate()
{
#ifdef __WXMSW__
    wxString llvmInstallPath, llvmVersion;
    wxArrayString regKeys;
    regKeys.Add("Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\LLVM");
    regKeys.Add("Software\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\LLVM");
    for(size_t i = 0; i < regKeys.size(); ++i) {
        if(ReadMSWInstallLocation(regKeys.Item(i), llvmInstallPath, llvmVersion)) {
            CompilerPtr compiler(new Compiler(NULL));
            compiler->SetCompilerFamily(COMPILER_FAMILY_CLANG);
            compiler->SetGenerateDependeciesFile(true);
            compiler->SetName(wxString() << "CLANG ( " << llvmVersion << " )");
            m_compilers.push_back(compiler);
            AddTools(compiler, llvmInstallPath + "\\bin");
            break;
        }
    }
#endif
}

void CompilerLocatorCLANG::AddTool(CompilerPtr compiler, const wxString& toolname, const wxString& toolpath,
                                   const wxString& extraArgs)
{
    wxString tool = toolpath;
    ::WrapWithQuotes(tool);
    if(!extraArgs.IsEmpty()) {
        tool << " " << extraArgs;
    }
    compiler->SetTool(toolname, tool);
}

void CompilerLocatorCLANG::AddTools(CompilerPtr compiler, const wxString& installFolder, const wxString& suffix)
{
    compiler->SetInstallationPath(installFolder);
    wxFileName toolFile(installFolder, "");
#ifdef __WXMSW__
    toolFile.SetExt("exe");
#endif

    toolFile.SetName("clang++" + suffix);
    AddTool(compiler, "CXX", toolFile.GetFullPath());
    AddTool(compiler, "LinkerName", toolFile.GetFullPath());

#ifdef __WXMAC__
    AddTool(compiler, "SharedObjectLinkerName", toolFile.GetFullPath(), "-dynamiclib -fPIC");
#else
    AddTool(compiler, "SharedObjectLinkerName", toolFile.GetFullPath(), "-shared -fPIC");
#endif

    toolFile.SetName("clang" + suffix);
    AddTool(compiler, "CC", toolFile.GetFullPath());

    // Add the archive tool
    toolFile.SetName("llvm-ar" + suffix);
    if(toolFile.FileExists()) {
        AddTool(compiler, "AR", toolFile.GetFullPath(), "rcu");

    } else {
        toolFile.SetName("ar");
        AddTool(compiler, "AR", toolFile.GetFullPath(), "rcu");
    }

#ifdef __WXMSW__
    AddTool(compiler, "ResourceCompiler", "windres.exe");
#else
    AddTool(compiler, "ResourceCompiler", "");
#endif

    // Add the assembler tool
    toolFile.SetName("llvm-as" + suffix);
    if(toolFile.FileExists()) {
        AddTool(compiler, "AS", toolFile.GetFullPath());

    } else {
        toolFile.SetName("as");
        AddTool(compiler, "AS", toolFile.GetFullPath());
    }

    wxString makeExtraArgs;
    if(wxThread::GetCPUCount() > 1) {
        makeExtraArgs << "-j" << wxThread::GetCPUCount();
    }

#ifdef __WXMSW__
    AddTool(compiler, "MAKE", "mingw32-make.exe", makeExtraArgs);
#else
    AddTool(compiler, "MAKE", "make", makeExtraArgs);
#endif
}

wxString CompilerLocatorCLANG::GetClangVersion(const wxString& clangBinary)
{
    wxString command;
    wxArrayString stdoutArr;
    command << clangBinary << " --version";
    ProcUtils::SafeExecuteCommand(command, stdoutArr);
    if(!stdoutArr.IsEmpty()) {
        wxString versionString = stdoutArr.Item(0);
        versionString = versionString.AfterLast('(');
        versionString = versionString.BeforeLast(')');
        return versionString;
    }
    return "";
}

wxString CompilerLocatorCLANG::GetCompilerFullName(const wxString& clangBinary)
{
    wxString fullname;
#ifdef __WXMSW__
    wxString version = GetClangVersion(clangBinary);
    fullname << "clang";
    if(!version.IsEmpty()) {
        fullname << "( " << version << " )";
    }
#else
    fullname = wxFileName(clangBinary).GetFullName().Upper();
#endif
    return fullname;
}

bool CompilerLocatorCLANG::ReadMSWInstallLocation(const wxString& regkey, wxString& installPath, wxString& llvmVersion)
{
#ifdef __WXMSW__
    wxRegKey reg(wxRegKey::HKLM, regkey);
    installPath.Clear();
    llvmVersion.Clear();
    if(reg.Exists()) {
        reg.QueryValue("DisplayIcon", installPath);
        reg.QueryValue("DisplayVersion", llvmVersion);
    }
    return !installPath.IsEmpty() && !llvmVersion.IsEmpty();
#else
    return false;
#endif
}

CompilerPtr CompilerLocatorCLANG::AddCompiler(const wxString& clangFolder, const wxString& suffix)
{
    wxFileName clang(clangFolder, "clang" + suffix);
    CompilerPtr compiler(new Compiler(NULL));
    compiler->SetCompilerFamily(COMPILER_FAMILY_CLANG);
    // get the compiler version
    compiler->SetName(GetCompilerFullName(clang.GetFullPath()));
    compiler->SetGenerateDependeciesFile(true);
    m_compilers.push_back(compiler);
    AddTools(compiler, clangFolder, suffix);
    return compiler;
}
