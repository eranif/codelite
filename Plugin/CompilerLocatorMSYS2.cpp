#include "CompilerLocatorMSYS2.hpp"

#include "GCCMetadata.hpp"
#include "Platform.hpp"
#include "compiler.h"

#include <wx/filename.h>

// --------------------------------------------------
// --------------------------------------------------

CompilerLocatorMSYS2Usr::CompilerLocatorMSYS2Usr()
{
    m_repository = "";
    m_msys2.SetChroot("\\usr");
}
CompilerLocatorMSYS2Usr::~CompilerLocatorMSYS2Usr() {}

CompilerLocatorMSYS2Mingw64::CompilerLocatorMSYS2Mingw64()
{
    m_repository = "mingw64";
    m_msys2.SetChroot("\\mingw64");
}

CompilerLocatorMSYS2Mingw64::~CompilerLocatorMSYS2Mingw64() {}

CompilerLocatorMSYS2Clang64::CompilerLocatorMSYS2Clang64()
{
    m_repository = "clang64";
    m_msys2.SetChroot("\\clang64");
}
CompilerLocatorMSYS2Clang64::~CompilerLocatorMSYS2Clang64() {}

// --------------------------------------------------
// --------------------------------------------------

CompilerLocatorMSYS2::CompilerLocatorMSYS2() {}

CompilerLocatorMSYS2::~CompilerLocatorMSYS2() {}

bool CompilerLocatorMSYS2::Locate()
{
    m_compilers.clear();

    // try some defaults
    wxString gcc_exe;
    if(!m_msys2.Which("gcc", &gcc_exe)) {
        return false;
    }

    auto compiler = Locate(wxFileName(gcc_exe).GetPath());
    if(compiler) {
        m_compilers.push_back(compiler);
    }
    return !m_compilers.empty();
}

CompilerPtr CompilerLocatorMSYS2::Locate(const wxString& folder)
{
    // check for g++
    wxFileName gcc = GetFileName(folder, "gcc");
    wxFileName gxx = GetFileName(folder, "g++");
    wxFileName ar = GetFileName(folder, "ar");
    wxFileName as = GetFileName(folder, "as");
    wxFileName make = GetFileName(folder, "mingw32-make");
    wxFileName windres = GetFileName(folder, "windres");
    wxFileName mkdir = GetFileName(folder, "mkdir");
    wxFileName gdb = GetFileName(folder, "gdb");

    // make sure that both gcc & g++ exist
    if(!(gcc.FileExists() && gxx.FileExists())) {
        return nullptr;
    }

    // define the toolchain name
    wxString basename = m_repository;
    if(!basename.empty()) {
        basename << "/";
    }
    basename << "gcc";
    GCCMetadata cmd(basename);

    cmd.Load(gcc.GetFullPath(), folder);

    // create new compiler
    CompilerPtr compiler(new Compiler(nullptr));
    compiler->SetName(cmd.GetName());
    compiler->SetCompilerFamily(COMPILER_FAMILY_MSYS2);
    compiler->SetInstallationPath(folder);

    // add the tools
    compiler->SetTool("CXX", gxx.GetFullPath());
    compiler->SetTool("CC", gcc.GetFullPath());
    compiler->SetTool("AR", ar.GetFullPath() + " -r");
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

wxFileName CompilerLocatorMSYS2::GetFileName(const wxString& bin_dir, const wxString& fullname) const
{
    wxFileName tool(bin_dir, fullname);
    tool.SetExt("exe");
    return tool;
}
