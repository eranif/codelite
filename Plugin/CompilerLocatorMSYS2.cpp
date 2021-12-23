#include "CompilerLocatorMSYS2.hpp"

#include "GCCMetadata.hpp"
#include "compiler.h"

#include <wx/filename.h>

CompilerLocatorMSYS2::CompilerLocatorMSYS2() {}

CompilerLocatorMSYS2::~CompilerLocatorMSYS2() {}

bool CompilerLocatorMSYS2::Locate()
{
    m_compilers.clear();

    // try some defaults
    std::vector<wxString> paths = { "C:\\msys64", "C:\\msys2" };
    for(const auto& root_dir : paths) {
        auto compiler = Locate(root_dir);
        if(compiler) {
            m_compilers.push_back(compiler);
            return true;
        }
    }
    return false;
}

CompilerPtr CompilerLocatorMSYS2::Locate(const wxString& folder)
{
    // check for g++
    wxFileName gcc = GetFileName(folder, "gcc.exe");
    wxFileName gxx = GetFileName(folder, "g++.exe");
    wxFileName ar = GetFileName(folder, "ar.exe");
    wxFileName as = GetFileName(folder, "as.exe");
    wxFileName make = GetFileName(folder, "make.exe");
    wxFileName windres = GetFileName(folder, "windres.exe");
    wxFileName mkdir = GetFileName(folder, "mkdir.exe");
    wxFileName gdb = GetFileName(folder, "gdb.exe");

    // make sure that both gcc & g++ exist
    if(!(gcc.FileExists() && gxx.FileExists())) {
        return nullptr;
    }

    GCCMetadata cmd("MSYS2");
    cmd.Load(gcc.GetFullPath(), folder);

    // create new compiler
    CompilerPtr compiler(new Compiler(nullptr));
    compiler->SetName(cmd.GetName());
    compiler->SetCompilerFamily(COMPILER_FAMILY_MSYS2);
    compiler->SetInstallationPath(folder);

    // add the tools
    compiler->SetTool("CXX", gxx.GetFullPath());
    compiler->SetTool("CC", gcc.GetFullPath());
    compiler->SetTool("AR", ar.GetFullPath());
    compiler->SetTool("LinkerName", gxx.GetFullPath());
    compiler->SetTool("SharedObjectLinkerName", gxx.GetFullPath() + " -shared -fPIC");
    compiler->SetTool("AS", as.GetFullPath());

    size_t cpu_count = wxThread::GetCPUCount();
    compiler->SetTool("MAKE", wxString() << make.GetFullPath() << " -j" << cpu_count);
    compiler->SetTool("ResourceCompiler", windres.GetFullPath());
    compiler->SetTool("Debugger", gdb.GetFullPath());
    return compiler;
}

void CompilerLocatorMSYS2::AddTool(const wxString& tool_name, const wxString& value) {}

wxFileName CompilerLocatorMSYS2::GetFileName(const wxString& installDir, const wxString& fullname) const
{
    wxFileName tool(installDir, fullname);
    tool.AppendDir("usr");
    tool.AppendDir("bin");
    return tool;
}
