//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : CompilerLocatorMSVC.cpp
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

#include "CompilerLocatorMSVC.h"
#include "compiler.h"
#include <globals.h>

CompilerLocatorMSVC::CompilerLocatorMSVC() {}

CompilerLocatorMSVC::~CompilerLocatorMSVC() {}

bool CompilerLocatorMSVC::Locate()
{
    m_compilers.clear();

    wxArrayString vc_platforms;
    vc_platforms.Add("x86");
    vc_platforms.Add("x64");

    wxEnvVariableHashMap envvars;
    ::wxGetEnvMap(&envvars);

    for (wxEnvVariableHashMap::const_iterator it = envvars.begin(); it != envvars.end(); ++it) {
        wxString const& envvarName = it->first;
        wxString const& envvarPath = it->second;

        if (!envvarName.Matches("VS??*COMNTOOLS") || envvarPath.IsEmpty() || (envvarName.Find('C') < 3)) {
            continue;
        }

        wxString vcVersion = envvarName.Mid(2, envvarName.Find('C') - 3);
        for(size_t j = 0; j < vc_platforms.GetCount(); ++j) {
            wxString compilerName = "Visual C++ " + vcVersion + " (" + vc_platforms[j] + ")";
            AddTools(envvarPath, compilerName, vc_platforms[j]);
        }
    }

    return !m_compilers.empty();
}

void CompilerLocatorMSVC::AddTools(const wxString& masterFolder,
                                   const wxString& name,
                                   const wxString& platform)
{
    wxFileName installPath(masterFolder, "");
    installPath.RemoveLastDir();
    installPath.RemoveLastDir();

    CompilerPtr compiler(new Compiler(NULL, Compiler::kRegexVC));
    compiler->SetCompilerFamily(COMPILER_FAMILY_VC);
    compiler->SetName(name);
    compiler->SetInstallationPath(installPath.GetPath());

    // CXX
    AddTool("cl.exe", "/nologo /TP /FC", "CXX", compiler);

    // CC
    AddTool("cl.exe", "/nologo /TC /FC", "CC", compiler);

    // AR
    AddTool("lib.exe", "/nologo", "AR", compiler);

    // SharedObjectLinkerName
    AddTool("link.exe", "/nologo /DLL", "SharedObjectLinkerName", compiler);

    // LinkerName
    AddTool("link.exe", "/nologo", "LinkerName", compiler);

    // AS
    AddTool("ml.exe", "/nologo", "AS", compiler);

    // Resource
    AddTool("rc.exe", "/nologo", "ResourceCompiler", compiler);
    compiler->AddCmpFileType("rc",
                       Compiler::CmpFileKindResource,
                       "$(RcCompilerName) $(RcCmpOptions) "
                       "$(ObjectSwitch)$(IntermediateDirectory)/"
                       "$(ObjectName)$(ObjectSuffix) $(RcIncludePath) \"$(FileFullPath)\"");

    //Make
    wxFileName fnVCvars(installPath);
    fnVCvars.AppendDir("VC");
    fnVCvars.SetFullName("vcvarsall.bat");
    wxString makeArgs = platform + " > nul";
    AddTool(fnVCvars.GetFullPath(), makeArgs, "MAKE", compiler);

    compiler->SetSwitch("ArchiveOutput", "/OUT:");
    compiler->SetSwitch("Debug", "/Zi ");
    compiler->SetSwitch("Include", "/I");
    compiler->SetSwitch("Library", " ");
    compiler->SetSwitch("LibraryPath", "/LIBPATH:");
    compiler->SetSwitch("Object", "/Fo");
    compiler->SetSwitch("Output", "/OUT:");
    compiler->SetSwitch("PreprocessOnly", "/P");
    compiler->SetSwitch("Preprocessor", "/D");
    compiler->SetSwitch("Source", "");
    compiler->SetObjectSuffix(".obj");

    // IDE path
    wxFileName fnIdeFolder(masterFolder, "");
    fnIdeFolder.RemoveLastDir();
    fnIdeFolder.AppendDir("IDE");
    compiler->SetPathVariable(fnIdeFolder.GetPath() + ";$PATH");

    // include and lib path, check if cl.exe exists
    wxString includePathCmd = "echo \%INCLUDE\%";
    WrapInShell(includePathCmd);
    wxString libPathCmd = "echo \%LIB\%";
    WrapInShell(libPathCmd);
    wxString clCheck = "where cl.exe";
    wxString command = compiler->GetTool("MAKE") + " & " + includePathCmd + " & " + libPathCmd + " & " + clCheck;
    WrapInShell(command);

    wxArrayString output;
    wxArrayString errors;
    wxExecute(command, output, errors);

    if (output.size() >= 2) {
        wxString includePath = output[0];
        if (includePath.Trim().Trim(false) != "\%INCLUDE\%") {
            compiler->SetGlobalIncludePath(includePath);
        }

        wxString libPath = output[1];
        if (libPath.Trim().Trim(false) != "\%LIB\%") {
            compiler->SetGlobalLibPath(libPath);
        }
    }

    // cl.exe exists
    if (errors.IsEmpty()) {
        m_compilers.push_back(compiler);
    }
}

void CompilerLocatorMSVC::AddTool(const wxString& toolpath,
                                      const wxString& extraArgs,
                                      const wxString& toolname,
                                      CompilerPtr compiler)
{
    wxString tool = toolpath;
    ::WrapWithQuotes(tool);

    if(!extraArgs.IsEmpty()) {
        tool << " " << extraArgs;
    }
    compiler->SetTool(toolname, tool);
}
