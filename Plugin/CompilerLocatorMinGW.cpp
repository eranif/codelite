//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : CompilerLocatorMinGW.cpp
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

#include "CompilerLocatorMinGW.h"
#include <wx/dir.h>
#include <wx/filefn.h>
#include <wx/tokenzr.h>
#include <globals.h>
#include "file_logger.h"
#include "procutils.h"
#include <wx/wfstream.h>
#include <wx/txtstrm.h>

#ifdef __WXMSW__
#   include <wx/msw/registry.h>
#   include <wx/volume.h>
#endif

CompilerLocatorMinGW::CompilerLocatorMinGW()
{
}

CompilerLocatorMinGW::~CompilerLocatorMinGW()
{
}

CompilerPtr CompilerLocatorMinGW::Locate(const wxString& folder)
{
    m_compilers.clear();
    wxFileName gcc(folder, "gcc");
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
        AddTools(gcc.GetPath(), GetGCCVersion(gcc.GetFullPath() ));
        return *m_compilers.begin();
    }
    return NULL;
}

wxString CompilerLocatorMinGW::GetGCCVersion(const wxString& gccBinary)
{
    wxString command;
    wxArrayString stdoutArr;
    command << gccBinary << " --version";
    ProcUtils::SafeExecuteCommand(command, stdoutArr);
    if ( !stdoutArr.IsEmpty() ) {
        wxString versionString = stdoutArr.Item(0).Trim().Trim(false);
        return versionString;
    }
    return "";
}

bool CompilerLocatorMinGW::Locate()
{
    m_compilers.clear();
    m_locatedFolders.clear();

    // for wxRegKey
#ifdef __WXMSW__

    {
        // HKEY_LOCAL_MACHINE\SOFTWARE\codelite\settings
        wxRegKey regClMinGW(wxRegKey::HKLM, "SOFTWARE\\codelite\\settings");
        wxString clInstallFolder;
        if ( regClMinGW.QueryValue("MinGW", clInstallFolder) && wxDirExists(clInstallFolder)) {
            wxFileName gccExe(clInstallFolder, "gcc.exe");
            wxString ver;
            regClMinGW.QueryValue("MinGW_Version", ver);
            gccExe.AppendDir("bin");
            if ( gccExe.FileExists() ) {
                AddTools(gccExe.GetPath(), "CodeLite-" + ver);
            }
        }
    }

    {
        // HKEY_LOCAL_MACHINE\SOFTWARE\codelite\settings
        wxRegKey regClMinGW(wxRegKey::HKLM, "SOFTWARE\\Wow6432Node\\codelite\\settings");
        wxString clInstallFolder;
        if ( regClMinGW.QueryValue("MinGW", clInstallFolder) && wxDirExists(clInstallFolder)) {
            wxFileName gccExe(clInstallFolder, "gcc.exe");
            wxString ver;
            regClMinGW.QueryValue("MinGW_Version", ver);
            gccExe.AppendDir("bin");
            if ( gccExe.FileExists() ) {
                AddTools(gccExe.GetPath(), "CodeLite-" + ver);
            }
        }
    }

    // locate codeblock's MinGW
    wxRegKey regCB(wxRegKey::HKCU, "SOFTWARE\\CodeBlocks");
    wxString cbInstallPath;
    if ( regCB.QueryValue("Path", cbInstallPath) ) {
        wxFileName mingwBinFolder( cbInstallPath, "" );
        mingwBinFolder.AppendDir("MinGW");
        mingwBinFolder.AppendDir("bin");
        if ( mingwBinFolder.DirExists() && wxFileName(mingwBinFolder.GetFullPath(), "gcc.exe").FileExists() ) {
            AddTools(mingwBinFolder.GetPath(), "Code::Blocks");
        }
    }
    
    // TDM-GCC installations in %ProgramData%/TDM-GCC/installations.txt
    wxString programDataDir;
    wxGetEnv("ProgramData", &programDataDir);
    wxFileName tdmgccInstall(programDataDir, "installations.txt");
    tdmgccInstall.AppendDir("TDM-GCC");
    
    if (tdmgccInstall.FileExists()) {
        wxFileInputStream input(tdmgccInstall.GetFullPath());
        wxTextInputStream text(input);
        while (input.IsOk() && !input.Eof()) {
            wxString instDir = text.ReadLine();
            if (!instDir.IsEmpty() && wxDirExists(instDir)) {
                wxFileName tdmBinDir(instDir, "g++.exe");
                tdmBinDir.AppendDir("bin");
                if (tdmBinDir.FileExists())
                    AddTools(tdmBinDir.GetPath());
            }
        }
    }

    // check uninstall keys
    std::vector<wxString> unInstKey;
    unInstKey.push_back("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall");
    unInstKey.push_back("SOFTWARE\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall");

    std::vector<wxRegKey::StdKey> regBase;
    regBase.push_back(wxRegKey::HKCU);
    regBase.push_back(wxRegKey::HKLM);

    for (size_t i = 0; i < regBase.size(); ++i) {
        for (size_t j = 0; j < unInstKey.size(); ++j) {
            wxRegKey regKey(regBase[i], unInstKey[j]);
            if (!regKey.Exists() || !regKey.Open(wxRegKey::Read))
                continue;

            size_t subkeys = 0;
            regKey.GetKeyInfo(&subkeys, NULL, NULL, NULL);
            wxString keyName;
            long keyIndex = 0;
            regKey.GetFirstKey(keyName, keyIndex);

            for (size_t k = 0; k < subkeys; ++k) {
                wxRegKey subKey(regKey, keyName);
                if (!subKey.Exists() || !subKey.Open(wxRegKey::Read))
                    continue;

                wxString displayName, installFolder;
                if (subKey.HasValue("DisplayName")
                    && subKey.HasValue("InstallLocation")
                    && subKey.QueryValue("DisplayName", displayName)
                    && subKey.QueryValue("InstallLocation", installFolder))
                {
                    CheckRegKey(displayName, installFolder);
                }

                subKey.Close();
                regKey.GetNextKey(keyName, keyIndex);
            }
            regKey.Close();
        }
    }

    // Last: many people install MinGW by simply extracting it into the
    // root folder:
    // C:\MinGW-X.Y.Z
    wxArrayString volumes = wxFSVolume::GetVolumes();
    wxArrayString mingwFolderArr;
    // Get list of folders for the volume only
    for(size_t i=0; i<volumes.GetCount(); ++i) {
        wxDir dir( volumes.Item(i) );
        if ( dir.IsOpened() ) {
            wxString path;
            bool cont = dir.GetFirst(&path, "*mingw*", wxDIR_DIRS);
            while (cont ) {
                wxString fullpath;
                fullpath << volumes.Item(i) << path;
                CL_DEBUG("Found folder containing MinGW: %s", fullpath);
                mingwFolderArr.Add( fullpath );
                cont = dir.GetNext( &path );
            }
        }
    }

    for(size_t i=0; i<mingwFolderArr.GetCount(); ++i) {
        wxString binFolder = FindBinFolder( mingwFolderArr.Item(i) );
        if ( binFolder.IsEmpty() )
            continue;

        wxFileName gcc(binFolder, "gcc.exe");
        if( gcc.FileExists() ) {
            AddTools(gcc.GetPath());
        }
    }
#endif

    // try to find MinGW in environment variable PATH (last)
    wxString pathValues;
    wxGetEnv("PATH", &pathValues);

    if ( !pathValues.IsEmpty() ) {
        wxArrayString pathArray = ::wxStringTokenize(pathValues, wxPATH_SEP, wxTOKEN_STRTOK);
        for (size_t i = 0; i < pathArray.GetCount(); ++i) {
            wxFileName gccComp( pathArray.Item(i), "gcc.exe" );
            if ( gccComp.GetDirs().Last() == "bin" && gccComp.Exists() ) {
                // We found gcc.exe
                wxString pathToGcc = gccComp.GetPath();
                pathToGcc.MakeLower();

                // Don't mix cygwin and mingw
                if ( !pathToGcc.Contains("cygwin") ) {
                    AddTools( gccComp.GetPath() );
                }
            }
        }
    }

    return !m_compilers.empty();
}

void CompilerLocatorMinGW::CheckRegKey(const wxString& displayName, const wxString& installFolder)
{
    if (displayName.StartsWith("TDM-GCC")) {
        wxFileName fnTDMBinFolder(installFolder, "");
        fnTDMBinFolder.AppendDir("bin");
        fnTDMBinFolder.SetFullName("g++.exe");
        if (fnTDMBinFolder.FileExists())
            AddTools(fnTDMBinFolder.GetPath(), displayName);
    }

    else if (displayName.StartsWith("MSYS2")) {
        wxFileName fnMingw32BinFolder(installFolder, "");
        fnMingw32BinFolder.AppendDir("mingw32");
        fnMingw32BinFolder.AppendDir("bin");
        fnMingw32BinFolder.SetFullName("g++.exe");
        if (fnMingw32BinFolder.FileExists())
            AddTools(fnMingw32BinFolder.GetPath(), "MinGW 32bit ( " + displayName + " )");

        wxFileName fnMingw64BinFolder(installFolder, "");
        fnMingw64BinFolder.AppendDir("mingw64");
        fnMingw64BinFolder.AppendDir("bin");
        fnMingw64BinFolder.SetFullName("g++.exe");
        if (fnMingw64BinFolder.FileExists())
            AddTools(fnMingw64BinFolder.GetPath(), "MinGW 64bit ( " + displayName + " )");
    }
}

void CompilerLocatorMinGW::AddTools(const wxString& binFolder, const wxString& name)
{
    wxFileName masterPath(binFolder, "");
    masterPath.RemoveLastDir();
    if ( m_locatedFolders.count(masterPath.GetPath()) ) {
        return;
    }

    // Create an empty compiler
    CompilerPtr compiler( new Compiler(NULL) );
    compiler->SetCompilerFamily(COMPILER_FAMILY_MINGW);
    compiler->SetGenerateDependeciesFile(true);
    m_compilers.push_back( compiler );
    m_locatedFolders.insert( masterPath.GetPath() );

    if ( name.IsEmpty() )
        compiler->SetName("MinGW ( " + masterPath.GetDirs().Last() + " )");
    else if (!name.Lower().Contains("mingw"))
        compiler->SetName("MinGW ( " + name + " )");
    else
        compiler->SetName(name);
    compiler->SetInstallationPath( masterPath.GetPath() );

    CL_DEBUG("Found MinGW compiler under: %s. \"%s\"", masterPath.GetPath(), compiler->GetName());
    wxFileName toolFile(binFolder, "");

    toolFile.SetFullName("g++.exe");
    AddTool(compiler, "CXX", toolFile.GetFullPath());
    AddTool(compiler, "LinkerName", toolFile.GetFullPath());
    AddTool(compiler, "SharedObjectLinkerName", toolFile.GetFullPath(), "-shared -fPIC");

    toolFile.SetFullName("gcc.exe");
    AddTool(compiler, "CC", toolFile.GetFullPath());

    toolFile.SetFullName("ar.exe");
    AddTool(compiler, "AR", toolFile.GetFullPath(), "rcu");

    toolFile.SetFullName("windres.exe");
    AddTool(compiler, "ResourceCompiler", toolFile.GetFullPath());

    toolFile.SetFullName("as.exe");
    AddTool(compiler, "AS", toolFile.GetFullPath());

    toolFile.SetFullName("make.exe");
    wxString makeExtraArgs;
    if ( wxThread::GetCPUCount() > 1 ) {
        makeExtraArgs << "-j" << wxThread::GetCPUCount();
    }

    // This is needed under MinGW
    makeExtraArgs <<  " SHELL=cmd.exe ";

    if ( toolFile.FileExists() ) {
        AddTool(compiler, "MAKE", toolFile.GetFullPath(), makeExtraArgs);

    } else {
        toolFile.SetFullName("mingw32-make.exe");
        if ( toolFile.FileExists() ) {
            AddTool(compiler, "MAKE", toolFile.GetFullPath(), makeExtraArgs);
        }
    }

    toolFile.SetFullName("gdb.exe");
    if(toolFile.Exists()) {
        AddTool(compiler, "Debugger", toolFile.GetFullPath());
    }
}

void CompilerLocatorMinGW::AddTool(CompilerPtr compiler, const wxString& toolname, const wxString& toolpath, const wxString& extraArgs)
{
    wxString tool = toolpath;
    ::WrapWithQuotes(tool);
    if(!extraArgs.IsEmpty()) {
        tool << " " << extraArgs;
    }
    compiler->SetTool(toolname, tool);
}

wxString CompilerLocatorMinGW::FindBinFolder(const wxString& parentPath)
{
    std::queue<wxString> dirs;
    dirs.push( parentPath );

    while ( !dirs.empty() ) {
        wxString curdir = dirs.front();
        dirs.pop();

        wxFileName fn(curdir, "" );
        fn.AppendDir("bin");

        if ( fn.DirExists() && fn.GetDirCount() && fn.GetDirs().Last() == "bin") {
            return fn.GetPath();
        }

        // Check to see if there are more directories to recurse
        wxDir dir;
        if ( dir.Open( curdir ) ) {
            wxString dirname;
            bool cont = dir.GetFirst( &dirname, "", wxDIR_DIRS );
            while ( cont ) {
                wxString new_dir;
                new_dir << curdir << wxFileName::GetPathSeparator() << dirname;
                dirs.push( new_dir );
                dirname.Clear();
                cont = dir.GetNext( &dirname );
            }
        }
    }
    return wxEmptyString;
}
