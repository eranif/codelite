#include "CompilerLocatorCLANG.h"
#include <globals.h>
#ifdef __WXMSW__
#   include <wx/msw/registry.h>
#endif
#include <wx/filename.h>

CompilerLocatorCLANG::CompilerLocatorCLANG()
{
}

CompilerLocatorCLANG::~CompilerLocatorCLANG()
{
}

bool CompilerLocatorCLANG::Locate()
{
    MSWLocate();
    return true;
}

void CompilerLocatorCLANG::MSWLocate()
{
#ifdef __WXMSW__
    bool found = false;
    {
        wxRegKey reg(wxRegKey::HKCU, "Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\LLVM");
        wxString llvmInstallPath;
        wxString llvmVersion;
        if ( reg.Exists() ) {
            found = true;
            reg.QueryValue("DisplayIcon",    llvmInstallPath);
            reg.QueryValue("DisplayVersion", llvmVersion);
            
            CompilerPtr compiler( new Compiler(NULL) );
            compiler->SetCompilerFamily("LLVM/Clang");
            compiler->SetName( wxString() << "clang ( " << llvmVersion << " )");
            m_compilers.push_back( compiler );
            AddTools(compiler, llvmInstallPath);
        }
    }
    if ( !found ) {
        wxRegKey reg(wxRegKey::HKLM, "Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\LLVM");
        wxString llvmInstallPath;
        wxString llvmVersion;
        if ( reg.Exists() ) {
            reg.QueryValue("DisplayIcon",    llvmInstallPath);
            reg.QueryValue("DisplayVersion", llvmVersion);
            
            CompilerPtr compiler( new Compiler(NULL) );
            compiler->SetCompilerFamily("LLVM/Clang");
            compiler->SetName( wxString() << "clang ( " << llvmVersion << " )");
            m_compilers.push_back( compiler );
            AddTools(compiler, llvmInstallPath);
        }
    }
#endif
}

void CompilerLocatorCLANG::AddTool(CompilerPtr compiler, const wxString& toolname, const wxString& toolpath, const wxString& extraArgs)
{
    wxString tool = toolpath;
    ::WrapWithQuotes(tool);
    compiler->SetTool(toolname, tool + " " + extraArgs);
}

void CompilerLocatorCLANG::AddTools(CompilerPtr compiler, const wxString &installFolder)
{
    compiler->SetInstallationPath( installFolder );
    wxFileName toolFile(installFolder, "");
    toolFile.AppendDir("bin");
#ifdef __WXMSW__
    toolFile.SetExt("exe");
#endif

    toolFile.SetName("clang++");
    AddTool(compiler, "CXX", toolFile.GetFullPath());
    AddTool(compiler, "LinkerName", toolFile.GetFullPath());

#ifdef __WXMAC__
    AddTool(compiler, "SharedObjectLinkerName", toolFile.GetFullPath(), "-dynamiclib -fPIC");
#else
    AddTool(compiler, "SharedObjectLinkerName", toolFile.GetFullPath(), "-shared -fPIC");
#endif

    toolFile.SetName("clang");
    AddTool(compiler, "CC", toolFile.GetFullPath());

    toolFile.SetName("llvm-ar");
    AddTool(compiler, "AR", toolFile.GetFullPath(), "rcu");
    AddTool(compiler, "ResourceCompiler", "windres.exe");
    
    toolFile.SetName("llvm-as");
    AddTool(compiler, "AS", toolFile.GetFullPath());
    
    wxString makeExtraArgs;
    if ( wxThread::GetCPUCount() > 1 ) {
        makeExtraArgs << "-j" << wxThread::GetCPUCount();
    }

    // FIXME: make this fullpath to the MinGW installation
    AddTool(compiler, "MAKE", "mingw32-make.exe", makeExtraArgs);
}
