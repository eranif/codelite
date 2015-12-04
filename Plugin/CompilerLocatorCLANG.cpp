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
#include <globals.h>
#include "procutils.h"
#include "includepathlocator.h"
#include "build_settings_config.h"

#ifdef __WXMSW__
#include <wx/msw/registry.h>
#endif
#include <wx/filename.h>

CompilerLocatorCLANG::CompilerLocatorCLANG() {}

CompilerLocatorCLANG::~CompilerLocatorCLANG() {}

bool CompilerLocatorCLANG::Locate()
{
    m_compilers.clear();
    MSWLocate();

    // POSIX locate
    wxFileName clang("/usr/bin", "clang");
    wxArrayString suffixes;
    suffixes.Add("");
    suffixes.Add("-3.3");
    suffixes.Add("-3.4");
    suffixes.Add("-3.5");
    suffixes.Add("-3.6");
    for(size_t i = 0; i < suffixes.GetCount(); ++i) {
        clang.SetFullName("clang" + suffixes.Item(i));
        if(clang.FileExists()) {
            CompilerPtr compiler(new Compiler(NULL));
            compiler->SetCompilerFamily(COMPILER_FAMILY_CLANG);
            // get the compiler version
            compiler->SetName(GetCompilerFullName(clang.GetFullPath()));
            compiler->SetGenerateDependeciesFile(true);
            m_compilers.push_back(compiler);
            clang.RemoveLastDir();
            AddTools(compiler, clang.GetPath(), suffixes.Item(i));
        }
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
        CompilerPtr compiler(new Compiler(NULL));
        compiler->SetCompilerFamily(COMPILER_FAMILY_CLANG);
        // get the compiler version
        compiler->SetName(GetCompilerFullName(clang.GetFullPath()));
        compiler->SetGenerateDependeciesFile(true);
        m_compilers.push_back(compiler);
        clang.RemoveLastDir();
        AddTools(compiler, clang.GetPath());
        return compiler;
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
            compiler->SetName(wxString() << "clang ( " << llvmVersion << " )");
            m_compilers.push_back(compiler);
            AddTools(compiler, llvmInstallPath);
            break;
        }
    }
#endif
}

void CompilerLocatorCLANG::AddTool(CompilerPtr compiler,
                                   const wxString& toolname,
                                   const wxString& toolpath,
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
    toolFile.AppendDir("bin");
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
    toolFile.SetName("llvm-ar");
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
    toolFile.SetName("llvm-as");
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
    wxString version = GetClangVersion(clangBinary);
    wxString fullname;
    fullname << "clang";
    if(!version.IsEmpty()) {
        fullname << "( " << version << " )";
    }
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
