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

    AddCompilerOptions(compiler);
    AddLinkerOptions(compiler);

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

void CompilerLocatorMSVC::AddCompilerOptions(CompilerPtr compiler)
{
    compiler->AddCompilerOption("/c", "Compiles without linking.");
    compiler->AddCompilerOption("/MD", "Causes the application to use the multithread-specific and DLL-specific version of the run-time library.");
    compiler->AddCompilerOption("/MDd", "Causes the application to use the debug multithread-specific and DLL-specific version of the run-time library.");
    compiler->AddCompilerOption("/MT", "Causes the application to use the multithread, static version of the run-time library.");
    compiler->AddCompilerOption("/MTd", "Causes the application to use the debug multithread, static version of the run-time library.");

    compiler->AddCompilerOption("/O1", "Creates small code.");
    compiler->AddCompilerOption("/O2", "Creates fast code.");
    compiler->AddCompilerOption("/Od", "Disables optimization.");
    compiler->AddCompilerOption("/Ox", "Uses maximum optimization.");
    compiler->AddCompilerOption("/Oi", "Generates intrinsic functions.");

    compiler->AddCompilerOption("/MP", "Compiles multiple source files by using multiple processes.");
    compiler->AddCompilerOption("/sdl", "Enables additional security features and warnings.");
    compiler->AddCompilerOption("/errorReport:none", "Reports about internal compiler errors will not be collected or sent to Microsoft.");
    compiler->AddCompilerOption("/errorReport:prompt", "Prompts you to send a report when you receive an internal compiler error.");
    compiler->AddCompilerOption("/FS", "Forces writes to the program database (PDB) file to be serialized through MSPDBSRV.EXE.");
    compiler->AddCompilerOption("/Zs", "Checks syntax only.");
    compiler->AddCompilerOption("/GA", "Optimizes code for Windows application.");
    compiler->AddCompilerOption("/GL", "Enables whole program optimization.");
    compiler->AddCompilerOption("/Gm", "Enables minimal rebuild.");
    compiler->AddCompilerOption("/Gy", "Enables function-level linking.");
    compiler->AddCompilerOption("/EHa", "Enable C++ Exceptions with SEH exceptions");
    compiler->AddCompilerOption("/EHs", "Enable C++ Exceptions with Extern C functions.");
    compiler->AddCompilerOption("/EHsc", "Enable C++ Exceptions with SEH and Extern C functions.");

    compiler->AddCompilerOption("/Z7", "Produces an .obj file containing full symbolic debugging information for use with the debugger.");
    compiler->AddCompilerOption("/Zi", "Produces a program database (PDB) that contains type information and symbolic debugging information for use with the debugger.");
    compiler->AddCompilerOption("/ZI", "Produces a program database, as described above, in a format that supports the Edit and Continue feature.");

    compiler->AddCompilerOption("/w", "Disables all compiler warnings.");
    compiler->AddCompilerOption("/W0", "Disables all warnings.");
    compiler->AddCompilerOption("/W1", "Displays level 1 (severe) warnings.");
    compiler->AddCompilerOption("/W2", "Displays level 1 and level 2 (significant) warnings.");
    compiler->AddCompilerOption("/W3", "Displays level 1, level 2 and level 3 (production quality) warnings.");
    compiler->AddCompilerOption("/W4", "Displays level 1, level 2, and level 3 warnings, and all level 4 (informational) warnings that are not turned off by default.");
    compiler->AddCompilerOption("/Wall", "Displays all warnings displayed by /W4 and all other warnings that /W4 does not include.");
    compiler->AddCompilerOption("/WX", "Treats all compiler warnings as errors.");
}

void CompilerLocatorMSVC::AddLinkerOptions(CompilerPtr compiler)
{
    compiler->AddLinkerOption("/DEBUG", "Creates debugging information.");
    compiler->AddLinkerOption("/DYNAMICBASE", "Use address space layout randomization.");
    compiler->AddLinkerOption("/DYNAMICBASE:NO", "Don't use address space layout randomization");
    compiler->AddLinkerOption("/ERRORREPORT:NONE", "Reports about internal compiler errors will not be collected or sent to Microsoft.");
    compiler->AddLinkerOption("/ERRORREPORT:PROMPT", "Prompts you to send a report when you receive an internal compiler error.");
    compiler->AddLinkerOption("/INCREMENTAL", "Enables incremental linking.");
    compiler->AddLinkerOption("/INCREMENTAL:NO", "Disables incremental linking.");
    compiler->AddLinkerOption("/LARGEADDRESSAWARE", "Tells the compiler that the application supports addresses larger than two gigabytes.");
    compiler->AddLinkerOption("/LARGEADDRESSAWARE:NO", "Tells the compiler that the application does not support addresses larger than two gigabytes.");

    compiler->AddLinkerOption("/LTCG:INCREMENTAL", "Specifies link-time code generation.");
    compiler->AddLinkerOption("/LTCG:STATUS", "Specifies link-time code generation.");
    compiler->AddLinkerOption("/LTCG:NOSTATUS", "Specifies link-time code generation.");
    compiler->AddLinkerOption("/LTCG:OFF", "Specifies link-time code generation.");
    compiler->AddLinkerOption("/MACHINE:X64", "Specifies the target platform.");
    compiler->AddLinkerOption("/MACHINE:X86", "Specifies the target platform.");
    compiler->AddLinkerOption("/NOENTRY", "Creates a resource-only DLL.");
    compiler->AddLinkerOption("/NXCOMPAT", "Specify Compatibility with Data Execution Prevention.");
    compiler->AddLinkerOption("/NXCOMPAT:NO", "Specify Compatibility with Data Execution Prevention.");

    compiler->AddLinkerOption("/OPT:REF", "Controls LINK optimizations.");
    compiler->AddLinkerOption("/OPT:NOREF", "Controls LINK optimizations.");
    compiler->AddLinkerOption("/OPT:ICF", "Controls LINK optimizations.");
    compiler->AddLinkerOption("/OPT:NOICF", "Controls LINK optimizations.");
    compiler->AddLinkerOption("/OPT:LBR", "Controls LINK optimizations.");
    compiler->AddLinkerOption("/OPT:NOLBR", "Controls LINK optimizations.");

    compiler->AddLinkerOption("/PROFILE", "Produces an output file that can be used with the Performance Tools profiler.");
    compiler->AddLinkerOption("/SAFESEH", "Image has Safe Exception Handlers.");
    compiler->AddLinkerOption("/SAFESEH:NO", "Image does not have Safe Exception Handlers");
    compiler->AddLinkerOption("/SUBSYSTEM:CONSOLE", "Tells the operating system how to run the .exe file.");
    compiler->AddLinkerOption("/SUBSYSTEM:WINDOWS", "Tells the operating system how to run the .exe file.");
    compiler->AddLinkerOption("/VERBOSE", "Prints linker progress messages.");
    compiler->AddLinkerOption("/WX", "Treats linker warnings as errors.");
    compiler->AddLinkerOption("/WX:NO", "Do not treats linker warnings as errors.");
}
