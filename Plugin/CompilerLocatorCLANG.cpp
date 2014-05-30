#include "CompilerLocatorCLANG.h"
#include <globals.h>
#include "procutils.h"
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
    m_compilers.clear();
    MSWLocate();
    
    // POSIX locate
    wxFileName clang("/usr/bin", "clang");
    if ( clang.FileExists() ) {
        CompilerPtr compiler( new Compiler(NULL) );
        compiler->SetCompilerFamily(COMPILER_FAMILY_CLANG);
        // get the compiler version
        compiler->SetName( GetCompilerFullName(clang.GetFullPath() ) );
        compiler->SetGenerateDependeciesFile(true);
        m_compilers.push_back( compiler );
        clang.RemoveLastDir();
        AddTools(compiler, clang.GetPath());
    }
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
            compiler->SetCompilerFamily(COMPILER_FAMILY_CLANG);
            compiler->SetGenerateDependeciesFile(true);
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
            compiler->SetCompilerFamily(COMPILER_FAMILY_CLANG);
            compiler->SetGenerateDependeciesFile(true);
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
#ifdef __WXMSW__
    AddTool(compiler, "ResourceCompiler", "windres.exe");
#else
    AddTool(compiler, "ResourceCompiler", "");
#endif
    toolFile.SetName("llvm-as");
    AddTool(compiler, "AS", toolFile.GetFullPath());
    
    wxString makeExtraArgs;
    if ( wxThread::GetCPUCount() > 1 ) {
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
    if ( !stdoutArr.IsEmpty() ) {
        wxString versionString = stdoutArr.Item(0);
        versionString = versionString.AfterLast('(');
        versionString = versionString.BeforeLast(')');
        return versionString;
    }
    return "";
}

wxString CompilerLocatorCLANG::GetCompilerFullName(const wxString& clangBinary)
{
    wxString version = GetClangVersion( clangBinary );
    wxString fullname;
    fullname << "clang";
    if ( !version.IsEmpty() ) {
        fullname << "( " << version << " )";
    }
    return fullname;
}
