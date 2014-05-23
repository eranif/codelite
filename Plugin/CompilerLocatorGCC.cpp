#include "CompilerLocatorGCC.h"
#include <wx/filename.h>
#include "file_logger.h"
#include "globals.h"

CompilerLocatorGCC::CompilerLocatorGCC()
{
}

CompilerLocatorGCC::~CompilerLocatorGCC()
{
}

bool CompilerLocatorGCC::Locate()
{
    // Locate GCC under /usr/bin
    
    wxArrayString gcc_versions;
    gcc_versions.Add(""); // Default gcc
    gcc_versions.Add("4.2");
    gcc_versions.Add("4.3");
    gcc_versions.Add("4.4");
    gcc_versions.Add("4.5");
    gcc_versions.Add("4.6");
    gcc_versions.Add("4.7");
    gcc_versions.Add("4.8");
    gcc_versions.Add("4.9");
    
    for(size_t i=0; i<gcc_versions.GetCount(); ++i) {
        wxString suffix = gcc_versions.Item(i);
        if ( !suffix.IsEmpty() ) {
            suffix.Prepend("-");
        }
        
        wxFileName gccFile("/usr/bin", "gcc" + suffix);
        if ( gccFile.FileExists() ) {
            // add this compiler
            CompilerPtr compiler( new Compiler(NULL) );
            m_compilers.push_back( compiler );
            AddTools(compiler, "/usr/bin", gcc_versions.Item(i));
        }
    }
    return !m_compilers.empty();
}

void CompilerLocatorGCC::AddTools(CompilerPtr compiler, const wxString& binFolder, const wxString& suffix)
{
    wxFileName masterPath(binFolder, "");
    
    wxString toolchainName;
    toolchainName << "GCC";
    if ( !suffix.IsEmpty() ) {
        toolchainName << " ( " << suffix << " )";
    }
    compiler->SetName( toolchainName );
    compiler->SetCompilerFamily("GNU GCC");
    compiler->SetInstallationPath( binFolder );
    
    CL_DEBUG("Found GNU GCC compiler under: %s. \"%s\"", masterPath.GetPath(), compiler->GetName());
    wxFileName toolFile(binFolder, "");
    
    toolFile.SetFullName("g++");
    AddTool(compiler, "CXX", toolFile.GetFullPath(), suffix);
    AddTool(compiler, "LinkerName", toolFile.GetFullPath(), suffix);

#ifdef __WXGTK__
    AddTool(compiler, "SharedObjectLinkerName", toolFile.GetFullPath(), suffix, "-shared -fPIC");
#else
    AddTool(compiler, "SharedObjectLinkerName", toolFile.GetFullPath(), suffix, "-dynamiclib -fPIC");
#endif

    toolFile.SetFullName("gcc");
    AddTool(compiler, "CC", toolFile.GetFullPath(), suffix);

    toolFile.SetFullName("ar");
    AddTool(compiler, "AR", toolFile.GetFullPath(), "", "rcu");

    toolFile.SetFullName("windres");
    AddTool(compiler, "ResourceCompiler", "", "");
    
    toolFile.SetFullName("as");
    AddTool(compiler, "AS", toolFile.GetFullPath(), "");
    
    toolFile.SetFullName("make");
    wxString makeExtraArgs;
    if ( wxThread::GetCPUCount() > 1 ) {
        makeExtraArgs << "-j" << wxThread::GetCPUCount();
    }
    AddTool(compiler, "MAKE", toolFile.GetFullPath(), "", makeExtraArgs);
}

void CompilerLocatorGCC::AddTool(CompilerPtr compiler, const wxString& toolname, const wxString& toolpath, const wxString& suffix, const wxString& extraArgs)
{
    wxString tool = toolpath;
    if ( !suffix.IsEmpty() ) {
        tool << "-" << suffix;
    }

    ::WrapWithQuotes(tool);
    tool << " " << extraArgs;
    compiler->SetTool(toolname, tool);
    CL_DEBUG("Adding tool: %s => %s", toolname, tool);
}
