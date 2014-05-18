#include "CompilerLocatorMinGW.h"
#include <wx/filefn.h>
#include <wx/tokenzr.h>
#include <globals.h>
#include "file_logger.h"

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

bool CompilerLocatorMinGW::Locate()
{
    // try to find MinGW in environment variable PATH first
    wxString pathValues;
    wxGetEnv("PATH", &pathValues);

    if ( !pathValues.IsEmpty() ) {
        wxArrayString pathArray = ::wxStringTokenize(pathValues, wxPATH_SEP, wxTOKEN_STRTOK);
        for (size_t i = 0; i < pathArray.GetCount(); ++i) {
            wxFileName gccComp( pathArray.Item(i), "gcc.exe" );
            if ( gccComp.GetDirs().Last() == "bin" && gccComp.Exists() ) {
                // We found gcc.exe
                CompilerPtr compiler( new Compiler(NULL) );
                m_compilers.push_back( compiler );
                AddTools( compiler, gccComp.GetPath() );
            }
        }
    }

#ifdef __WXMSW__ // for wxRegKey
    // Check registry for TDM-GCC
    // HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\TDM-GCC
    wxRegKey regTDM(wxRegKey::HKLM, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\TDM-GCC");
    wxString tdmInstallFolder;
    if ( regTDM.QueryValue("InstallLocation", tdmInstallFolder) ) {
        wxFileName fnTDMBinFolder( tdmInstallFolder, "" );
        fnTDMBinFolder.AppendDir("bin");
        CompilerPtr compiler( new Compiler(NULL) );
        m_compilers.push_back( compiler );
        AddTools(compiler, fnTDMBinFolder.GetPath(), "TDM-GCC");
    }
    
    // 64 bit
    // HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\TDM-GCC
    wxRegKey regTDM_64(wxRegKey::HKLM, "SOFTWARE\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall");
    if ( regTDM_64.QueryValue("InstallLocation", tdmInstallFolder) ) {
        wxFileName fnTDMBinFolder( tdmInstallFolder, "" );
        fnTDMBinFolder.AppendDir("bin");
        CompilerPtr compiler( new Compiler(NULL) );
        m_compilers.push_back( compiler );
        AddTools(compiler, fnTDMBinFolder.GetPath(), "TDM-GCC-64");
    }
    
    // locate codeblock's MinGW
    wxRegKey regCB(wxRegKey::HKCU, "SOFTWARE\\CodeBlocks");
    wxString cbInstallPath;
    if ( regCB.QueryValue("Path", cbInstallPath) ) {
        wxFileName mingwBinFolder( cbInstallPath, "" );
        mingwBinFolder.AppendDir("MinGW");
        mingwBinFolder.AppendDir("bin");
        if ( mingwBinFolder.DirExists() && wxFileName(mingwBinFolder.GetFullPath(), "gcc.exe").FileExists() ) {
            CompilerPtr compiler( new Compiler(NULL) );
            m_compilers.push_back( compiler );
            AddTools(compiler, mingwBinFolder.GetPath(), "Code::Blocks MinGW");
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
        wxFileName gcc(mingwFolderArr.Item(i), "gcc.exe");
        gcc.AppendDir("bin");
        if( gcc.FileExists() ) {
            CompilerPtr compiler( new Compiler(NULL) );
            m_compilers.push_back( compiler );
            AddTools(compiler, gcc.GetPath());
        }
    }
    
    // FIXME: remove duplicate compilers
    // FIXME: search for 'bin' folder recursively inside the folder
#endif
    return !m_compilers.empty();
}

void CompilerLocatorMinGW::AddTools(CompilerPtr compiler, const wxString& binFolder, const wxString& name)
{
    wxFileName masterPath(binFolder, "");
    masterPath.RemoveLastDir();
    if ( name.IsEmpty() ) {
        compiler->SetName("MinGW ( " + masterPath.GetDirs().Last() + " )");

    } else {
        compiler->SetName("MinGW ( " + name + " )");
    }
    
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
    if ( toolFile.FileExists() ) {
        AddTool(compiler, "MAKE", toolFile.GetFullPath());
        
    } else {
        toolFile.SetFullName("mingw32-make.exe");
        if ( toolFile.FileExists() ) {
            AddTool(compiler, "MAKE", toolFile.GetFullPath());
        }
    }
}

void CompilerLocatorMinGW::AddTool(CompilerPtr compiler, const wxString& toolname, const wxString& toolpath, const wxString& extraArgs)
{
    wxString tool = toolpath;
    ::WrapWithQuotes(tool);
    compiler->SetTool(toolname, toolpath + " " + extraArgs);
}
