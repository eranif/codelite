#include "CompilerLocatorMSYS2Clang.hpp"

#include "GCCMetadata.hpp"
#include "Platform.hpp"
#include "compiler.h"

#include <wx/filename.h>

// --------------------------------------------------
// --------------------------------------------------

CompilerLocatorMSYS2ClangUsr::CompilerLocatorMSYS2ClangUsr()
{
    m_repository = "";
    m_msys2.SetChroot("\\usr");
}
CompilerLocatorMSYS2ClangUsr::~CompilerLocatorMSYS2ClangUsr() {}

CompilerLocatorMSYS2ClangMingw64::CompilerLocatorMSYS2ClangMingw64()
{
    m_repository = "mingw64";
    m_msys2.SetChroot("\\mingw64");
}

CompilerLocatorMSYS2ClangMingw64::~CompilerLocatorMSYS2ClangMingw64() {}

CompilerLocatorMSYS2ClangClang64::CompilerLocatorMSYS2ClangClang64()
{
    m_repository = "clang64";
    m_msys2.SetChroot("\\clang64");
}
CompilerLocatorMSYS2ClangClang64::~CompilerLocatorMSYS2ClangClang64() {}

// --------------------------------------------------
// --------------------------------------------------

CompilerLocatorMSYS2Clang::CompilerLocatorMSYS2Clang() {}

CompilerLocatorMSYS2Clang::~CompilerLocatorMSYS2Clang() {}

bool CompilerLocatorMSYS2Clang::Locate()
{
    m_compilers.clear();

    // try some defaults
    wxString clang_exe;
    if(!m_msys2.Which("clang", &clang_exe)) {
        return false;
    }

    auto compiler = Locate(wxFileName(clang_exe).GetPath());
    if(compiler) {
        m_compilers.push_back(compiler);
    }
    return !m_compilers.empty();
}

CompilerPtr CompilerLocatorMSYS2Clang::Locate(const wxString& folder)
{
    // check for g++
    wxFileName clang = GetFileName(folder, "clang");
    wxFileName clangxx = GetFileName(folder, "clang++");
    wxFileName ar = GetFileName(folder, "ar");
    wxFileName as = GetFileName(folder, "as");
    wxFileName make = GetFileName(folder, "mingw32-make");
    wxFileName windres = GetFileName(folder, "windres");
    wxFileName mkdir = GetFileName(folder, "mkdir");
    wxFileName gdb = GetFileName(folder, "gdb");

    // make sure that both clang & g++ exist
    if(!(clang.FileExists() && clangxx.FileExists())) {
        return nullptr;
    }

    // define the toolchain name
    wxString basename = m_repository;
    if(!basename.empty()) {
        basename << "/";
    }
    basename << "clang";
    GCCMetadata cmd(basename);

    cmd.Load(clang.GetFullPath(), folder);

    // create new compiler
    CompilerPtr compiler(new Compiler(nullptr));
    compiler->SetName(cmd.GetName());
    compiler->SetCompilerFamily(COMPILER_FAMILY_CLANG);
    compiler->SetInstallationPath(folder);

    // add the tools
    compiler->SetTool("CXX", clangxx.GetFullPath());
    compiler->SetTool("CC", clang.GetFullPath());
    compiler->SetTool("AR", ar.GetFullPath() + " -r");
    compiler->SetTool("LinkerName", clangxx.GetFullPath());
    compiler->SetTool("SharedObjectLinkerName", clangxx.GetFullPath() + " -shared -fPIC");
    compiler->SetTool("AS", as.GetFullPath());

    size_t cpu_count = wxThread::GetCPUCount();
    compiler->SetTool("MAKE", wxString() << make.GetFullPath() << " -j" << cpu_count);
    compiler->SetTool("ResourceCompiler", windres.GetFullPath());
    compiler->SetTool("Debugger", gdb.GetFullPath());
    return compiler;
}

void CompilerLocatorMSYS2Clang::AddTool(const wxString& tool_name, const wxString& value) {}

wxFileName CompilerLocatorMSYS2Clang::GetFileName(const wxString& bin_dir, const wxString& fullname) const
{
    wxFileName tool(bin_dir, fullname);
    tool.SetExt("exe");
    return tool;
}
