#include "CompilerLocatorMSYS2.hpp"

#include "GCCMetadata.hpp"
#include "Platform/Platform.hpp"
#include "compiler.h"
#include "file_logger.h"

#include <wx/filename.h>

// --------------------------------------------------
// --------------------------------------------------

namespace
{
std::vector<std::unordered_map<wxString, wxString>> TOOLCHAINS = {
    // GCC
    { { "CC", "gcc" },
      { "CXX", "g++" },
      { "AR", "ar" },
      { "AS", "as" },
      { "LD", "g++" },
      { "MAKE", "mingw32-make" },
      { "WINDRES", "windres" },
      { "MKDIR", "mkdir" },
      { "DEBUGGER", "gdb" } },
    // Clang
    { { "CC", "clang" },
      { "CXX", "clang++" },
      { "AR", "ar" },
      { "AS", "as" },
      { "LD", "clang++" },
      { "MAKE", "mingw32-make" },
      { "WINDRES", "windres" },
      { "MKDIR", "mkdir" },
      { "DEBUGGER", "gdb" } }
};
}

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

CompilerPtr CompilerLocatorMSYS2::TryToolchain(const wxString& folder,
                                               const std::unordered_map<wxString, wxString>& toolchain)
{
    // check for g++
    clDEBUG() << "searching for toolchain:" << toolchain.at("CXX") << "at:" << folder << endl;

    wxFileName gcc = GetFileName(folder, toolchain.at("CC"));
    wxFileName gxx = GetFileName(folder, toolchain.at("CXX"));
    wxFileName ar = GetFileName(folder, toolchain.at("AR"));
    wxFileName as = GetFileName(folder, toolchain.at("AS"));
    wxFileName make = GetFileName(folder, toolchain.at("MAKE"));
    wxFileName windres = GetFileName(folder, toolchain.at("WINDRES"));
    wxFileName mkdir = GetFileName(folder, toolchain.at("MKDIR"));
    wxFileName gdb = GetFileName(folder, toolchain.at("DEBUGGER"));

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

CompilerPtr CompilerLocatorMSYS2::Locate(const wxString& folder)
{
    // check for g++
    for(const auto& toolchain : TOOLCHAINS) {
        auto cmp = TryToolchain(folder, toolchain);
        if(cmp) {
            return cmp;
        }
    }
    return nullptr;
}

void CompilerLocatorMSYS2::AddTool(const wxString& tool_name, const wxString& value) {}

wxFileName CompilerLocatorMSYS2::GetFileName(const wxString& bin_dir, const wxString& fullname) const
{
    wxFileName tool(bin_dir, fullname);
    tool.SetExt("exe");
    return tool;
}
