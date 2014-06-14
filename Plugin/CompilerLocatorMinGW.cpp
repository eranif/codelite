#include "CompilerLocatorMinGW.h"
#include <wx/dir.h>
#include <wx/filefn.h>
#include <wx/tokenzr.h>
#include <globals.h>
#include "file_logger.h"
#include "procutils.h"

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
    
#ifdef __WXMSW__ // for wxRegKey

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

    // Check registry for TDM-GCC
    // HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\TDM-GCC
    wxRegKey regTDM(wxRegKey::HKLM, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\TDM-GCC");
    wxString tdmInstallFolder;
    if ( regTDM.QueryValue("InstallLocation", tdmInstallFolder) ) {
        wxFileName fnTDMBinFolder( tdmInstallFolder, "" );
        fnTDMBinFolder.AppendDir("bin");
        AddTools(fnTDMBinFolder.GetPath(), "TDM-GCC");
    }
    
    // 64 bit
    // HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\TDM-GCC
    wxRegKey regTDM_64(wxRegKey::HKLM, "SOFTWARE\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall");
    if ( regTDM_64.QueryValue("InstallLocation", tdmInstallFolder) ) {
        wxFileName fnTDMBinFolder( tdmInstallFolder, "" );
        fnTDMBinFolder.AppendDir("bin");
        AddTools(fnTDMBinFolder.GetPath(), "TDM-GCC-64");
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
                AddTools( gccComp.GetPath() );
            }
        }
    }

    return !m_compilers.empty();
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
    
    if ( name.IsEmpty() ) {
        compiler->SetName("MinGW ( " + masterPath.GetDirs().Last() + " )");

    } else {
        compiler->SetName("MinGW ( " + name + " )");
    }
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
    
    if ( toolFile.FileExists() ) {
        AddTool(compiler, "MAKE", toolFile.GetFullPath(), makeExtraArgs);
        
    } else {
        toolFile.SetFullName("mingw32-make.exe");
        if ( toolFile.FileExists() ) {
            AddTool(compiler, "MAKE", toolFile.GetFullPath(), makeExtraArgs);
        }
    }
}

void CompilerLocatorMinGW::AddTool(CompilerPtr compiler, const wxString& toolname, const wxString& toolpath, const wxString& extraArgs)
{
    wxString tool = toolpath;
    ::WrapWithQuotes(tool);
    compiler->SetTool(toolname, tool + " " + extraArgs);
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
