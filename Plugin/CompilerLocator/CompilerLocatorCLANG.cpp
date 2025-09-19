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

#include "StringUtils.h"
#include "clFilesCollector.h"
#include "file_logger.h"

#include <optional>
#include <wx/filename.h>

#ifdef __WXOSX__
#include <wx/tokenzr.h>
#endif

#ifdef __WXOSX__
bool OSXFindBrewClang(wxFileName& clang)
{
    // We use brew to query the location of clang.
    // Clang is installed as part of the LLVM package
    IProcess::Ptr_t proc(::CreateSyncProcess("brew list llvm", IProcessWrapInShell));
    if (!proc) {
        return false;
    }
    wxString output;
    proc->WaitForTerminate(output);

    wxArrayString lines = ::wxStringTokenize(output, "\r\n", wxTOKEN_STRTOK);
    for (const wxString& line : lines) {
        if (line.Contains("bin/clang")) {
            clang = line;
            clang.SetFullName("clang");
            return true;
        }
    }
    return false;
}
#endif

bool CompilerLocatorCLANG::Locate()
{
    clDEBUG() << "Searching for clang compilers..." << endl;
    m_compilers.clear();
#ifdef __WXOSX__
    {
        wxFileName fnClang;
        if (OSXFindBrewClang(fnClang)) {
            AddCompiler(fnClang.GetPath());
        }
    }
#endif

    // Locate CLANG under all the locations under PATH variable
    // Search the entire path locations and get all files that matches the pattern 'clang*'
    wxArrayString paths = GetPaths();
    clFilesScanner scanner;
    clFilesScanner::EntryData::Vec_t outputFiles, tmpFiles;
    for (const wxString& path : paths) {
        if (scanner.ScanNoRecurse(path, tmpFiles, "clang")) {
            outputFiles.insert(outputFiles.end(), tmpFiles.begin(), tmpFiles.end());
        }
    }

    wxStringSet_t set;
    for (const auto& d : outputFiles) {
        if (d.flags & clFilesScanner::kIsFile) {
            wxFileName clang(d.fullpath);
            if (clang.GetFullName() == "clang") {
                // keep all the paths were we found clang
                set.insert(clang.GetFullPath());
            }
        }
    }

    clDEBUG() << "Scan completed. Found" << set.size() << "compilers" << endl;

    for (const auto& path : set) {
        AddCompiler(path);
    }
    clDEBUG() << "Locate completed!" << endl;
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
    if (!found) {
        // try to see if we have a bin folder here
        clang.AppendDir("bin");
        found = clang.FileExists();
    }

    if (found) {
        return AddCompiler(clang.GetPath());
    }
    return NULL;
}

void CompilerLocatorCLANG::AddTool(CompilerPtr compiler, const wxString& name, const wxFileName& fntool,
                                   const wxString& extraArgs)
{
    wxString tool = fntool.GetFullPath();
    StringUtils::WrapWithQuotes(tool);
    if (!extraArgs.IsEmpty()) {
        tool << " " << extraArgs;
    }
    compiler->SetTool(name, tool);
}

namespace
{
/// Given clang full path, try to locate `toolname` at the same location
/// For example if `clang` is `/usr/bin/clang-17` and `toolname` is `clang++`
/// This method will try to locate `/usr/bin/clang++-17`
std::optional<wxFileName> GetTool(const wxFileName& clang, const wxString& toolname)
{
    wxString fullpath = clang.GetFullPath();
    fullpath.Replace("clang", toolname);
    wxFileName fn(fullpath);
    if (fn.FileExists()) {
        return fn;
    } else {
        return {};
    }
}
} // namespace

void CompilerLocatorCLANG::AddTools(CompilerPtr compiler, const wxFileName& clang)
{
    // add normalize path
    compiler->SetInstallationPath(clang.GetPath());
    AddTool(compiler, "CXX", GetTool(clang, "clang++").value_or(clang));
    AddTool(compiler, "LinkerName", GetTool(clang, "clang++").value_or(clang));

#ifdef __WXMAC__
    AddTool(compiler, "SharedObjectLinkerName", GetTool(clang, "clang++").value_or(clang), "-dynamiclib -fPIC");
#else
    AddTool(compiler, "SharedObjectLinkerName", GetTool(clang, "clang++").value_or(clang), "-shared -fPIC");
#endif

    AddTool(compiler, "CC", clang);

    // Add the archive tool
    AddTool(compiler, "AR", GetTool(clang, "llvm-ar").value_or(wxFileName("/usr/bin/ar")), "rcu");
    AddTool(compiler, "AS", GetTool(clang, "llvm-as").value_or(wxFileName("/usr/bin/as")), wxEmptyString);
    AddTool(compiler, "ResourceCompiler", {});

    wxString makeExtraArgs;
    if (wxThread::GetCPUCount() > 1) {
        makeExtraArgs << "-j" << wxThread::GetCPUCount();
    }
    AddTool(compiler, "MAKE", wxFileName("/usr/bin/make"), makeExtraArgs);
}

wxString CompilerLocatorCLANG::GetCompilerFullName(const wxString& clangBinary) { return clangBinary; }

void CompilerLocatorCLANG::CheckUninstRegKey(const wxString& displayName, const wxString& installFolder,
                                             const wxString& displayVersion)
{
    wxUnusedVar(displayVersion);
    wxUnusedVar(installFolder);
    wxUnusedVar(displayName);
}

CompilerPtr CompilerLocatorCLANG::AddCompiler(const wxFileName& clang)
{
    CompilerPtr compiler(new Compiler(NULL));
    compiler->SetCompilerFamily(COMPILER_FAMILY_CLANG);
    compiler->SetGenerateDependeciesFile(true);
    compiler->SetName(clang.GetFullPath());
    m_compilers.push_back(compiler);
    AddTools(compiler, clang);
    return compiler;
}
