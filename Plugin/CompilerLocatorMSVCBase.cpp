//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : CompilerLocatorMSVCBase.cpp
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

#include "CompilerLocatorMSVCBase.h"
#include "compiler.h"
#include <globals.h>
#include <wx/tokenzr.h>
#ifdef __WXMSW__
#   include <wx/msw/registry.h>
#endif

CompilerLocatorMSVCBase::CompilerLocatorMSVCBase()
{
}

CompilerLocatorMSVCBase::~CompilerLocatorMSVCBase()
{
}

void CompilerLocatorMSVCBase::AddTools(const wxString& masterFolder, const wxString& name)
{
    wxFileName fnBinFolder(masterFolder, "");
    fnBinFolder.RemoveLastDir();
    fnBinFolder.RemoveLastDir();
    fnBinFolder.AppendDir("VC");
    fnBinFolder.AppendDir("bin");
    wxString binFolder = fnBinFolder.GetPath();

    wxFileName fnIdeFolder(masterFolder, "");
    fnIdeFolder.RemoveLastDir();
    fnIdeFolder.AppendDir("IDE");

    wxFileName installPath(masterFolder, "");
    installPath.RemoveLastDir();
    installPath.RemoveLastDir();

    CompilerPtr compiler( new Compiler(NULL, Compiler::kRegexVC) );
    compiler->SetCompilerFamily(COMPILER_FAMILY_VC);
    compiler->SetName( name );
    compiler->SetInstallationPath(installPath.GetPath());
    m_compilers.push_back( compiler );

    wxFileName cxx(binFolder, "cl.exe");

    // CXX
    AddTool(cxx.GetFullPath(), "-nologo -FC -EHs", "CXX", compiler);

    // CC
    AddTool(cxx.GetFullPath(), "-nologo -FC", "CC", compiler);

    // AR
    cxx.SetFullName("lib.exe");
    AddTool(cxx.GetFullPath(), "-nologo", "AR", compiler);

    // SharedObjectLinkerName
    cxx.SetFullName("link.exe");
    AddTool(cxx.GetFullPath(), "-DLL -nologo", "SharedObjectLinkerName", compiler);

    // LinkerName
    AddTool(cxx.GetFullPath(), "-nologo", "LinkerName", compiler);

    compiler->SetSwitch("ArchiveOutput", "/OUT:");
    compiler->SetSwitch("Debug", "/Zi ");
    compiler->SetSwitch("Include", "/I");
    compiler->SetSwitch("Library", " ");
    compiler->SetSwitch("LibraryPath", "/LIBPATH:");
    compiler->SetSwitch("Object", "/Fo");
    compiler->SetSwitch("Output", "/OUT:");
    compiler->SetSwitch("Preprocessor", "/D");
    compiler->SetSwitch("Source", "-c ");
    compiler->SetObjectSuffix(".obj");

    // Add SDK paths
    FindSDKs(compiler);
    
    // Add the global include path
    wxFileName includePath(binFolder, "");
    includePath.RemoveLastDir();
    includePath.AppendDir("include");

    wxString globalIncPath = compiler->GetGlobalIncludePath();
    AddIncOrLibPath(includePath.GetPath(), globalIncPath);
    compiler->SetGlobalIncludePath( globalIncPath );

    // Add the global include path
    wxFileName libpath(binFolder, "");
    libpath.RemoveLastDir();
    libpath.AppendDir("lib");
    
    wxString globalLibPath = compiler->GetGlobalLibPath();
    AddIncOrLibPath(libpath.GetPath(), globalLibPath);
    compiler->SetGlobalLibPath( globalLibPath );

    // IDE path
    compiler->SetPathVariable(fnIdeFolder.GetPath() + ";$PATH");
}

void CompilerLocatorMSVCBase::AddTool(const wxString& toolpath, const wxString& extraArgs, const wxString& toolname, CompilerPtr compiler)
{
    wxString tool = toolpath;
    ::WrapWithQuotes( tool );

    if ( !extraArgs.IsEmpty() ) {
        tool << " " << extraArgs;
    }
    compiler->SetTool(toolname, tool);
}

void CompilerLocatorMSVCBase::FindSDKs(CompilerPtr compiler)
{
#ifdef __WXMSW__
    wxString sdkDir;
    bool sdkfound = false;
    wxRegKey key; // defaults to HKCR
    // try to detect Platform SDK (old versions)
    key.SetName(_T("HKEY_CURRENT_USER\\Software\\Microsoft\\Win32SDK\\Directories"));
    if (key.Exists() && key.Open(wxRegKey::Read)) {
        key.QueryValue(_T("Install Dir"), sdkDir);
        if (!sdkDir.IsEmpty() && wxDirExists(sdkDir))
            sdkfound = true;
        key.Close();
    }

    // try to detect Platform SDK (newer versions)
    wxString msPsdkKeyName[2] = { _T("HKEY_CURRENT_USER\\Software\\Microsoft\\MicrosoftSDK\\InstalledSDKs"),
                                  _T("HKEY_CURRENT_USER\\Software\\Microsoft\\Microsoft SDKs\\Windows")
                                };
    wxString msPsdkKeyValue[2] = { _T("Install Dir"), _T("InstallationFolder") };
    for (int i = 0; i < 2; ++i) {
        key.SetName(msPsdkKeyName[i]);
        if (!sdkfound && key.Exists() && key.Open(wxRegKey::Read)) {
            wxString name;
            long idx;
            bool cont = key.GetFirstKey(name, idx);

            while(cont) {
                wxRegKey subkey(key.GetName(), name);

                if (subkey.Open(wxRegKey::Read) &&
                    (subkey.QueryValue(msPsdkKeyValue[i], sdkDir), !sdkDir.IsEmpty()) &&
                    wxDirExists(sdkDir)) {
                    sdkfound = true;
                    cont = false;
                } else
                    cont = key.GetNextKey(name, idx);

                subkey.Close();
            }
            key.Close();
        }

        if (sdkfound)
            break;
    }
    
    if ( !sdkfound ) {
        // Guess...
        sdkDir = "C:\\Program Files"; // Default
        wxGetEnv("ProgramFiles", &sdkDir);
        sdkDir +=  wxT("\\Microsoft SDKs\\Windows\\v");
        wxArrayString vers = ::wxStringTokenize("8.1A;8.1;8.0;7.1A;7.1;7.0A;7.0;6.1;6.0A;6.0", ";", wxTOKEN_STRTOK);
        for (size_t i = 0; i < vers.GetCount(); ++i) {
            wxString sdkVerDir;
            sdkVerDir << sdkDir << vers.Item(i);
            if ( ::wxDirExists( sdkVerDir ) && ::wxDirExists(sdkVerDir + "\\Include") ) {
                sdkDir.swap( sdkVerDir );
                sdkfound = true;
                break;
            }
        }
    }
    
    if (sdkfound) {
        if (sdkDir.GetChar(sdkDir.Length() - 1) != '\\')
            sdkDir += wxFILE_SEP_PATH;
        
        wxString incPath = compiler->GetGlobalIncludePath();
        AddIncOrLibPath(sdkDir + _T("include"), incPath);
        compiler->SetGlobalIncludePath(incPath);
        
        wxString libPath = compiler->GetGlobalLibPath();
        AddIncOrLibPath(sdkDir + _T("lib"), libPath);
        compiler->SetGlobalLibPath(libPath);
    }
    
#endif // __WXMSW__
}

void CompilerLocatorMSVCBase::AddIncOrLibPath(const wxString& path_to_add, wxString& add_to_me)
{
    wxArrayString paths = wxStringTokenize(add_to_me, ";", wxTOKEN_STRTOK);
    paths.Add( path_to_add );
    
    wxString joinedPath;
    for(size_t i=0; i<paths.GetCount(); ++i) {
        joinedPath << paths.Item(i) << ";";
    }
    
    if ( !joinedPath.IsEmpty() ) {
        joinedPath.RemoveLast();
    }
    
    joinedPath.swap( add_to_me );
}
