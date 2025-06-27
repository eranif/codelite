#include "CompilerLocatorMSYS2.hpp"

#include "compiler.h"
#include "file_logger.h"

#include <wx/filename.h>
#include <wx/tokenzr.h>

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

CompilerLocatorMSYS2Usr::CompilerLocatorMSYS2Usr() { m_msys2.SetChroot("\\usr"); }
CompilerLocatorMSYS2Usr::~CompilerLocatorMSYS2Usr() {}

CompilerLocatorMSYS2Mingw64::CompilerLocatorMSYS2Mingw64() { m_msys2.SetChroot("\\mingw64"); }

CompilerLocatorMSYS2Mingw64::~CompilerLocatorMSYS2Mingw64() {}

CompilerLocatorMSYS2Clang64::CompilerLocatorMSYS2Clang64() { m_msys2.SetChroot("\\clang64"); }
CompilerLocatorMSYS2Clang64::~CompilerLocatorMSYS2Clang64() {}

CompilerLocatorMSYS2Env::CompilerLocatorMSYS2Env() { m_cmdShell = true; }
CompilerLocatorMSYS2Env::~CompilerLocatorMSYS2Env() {}

bool CompilerLocatorMSYS2Env::Locate()
{
    clDEBUG() << "Locating compiler based on PATH environment variable" << endl;
    m_compilers.clear();
    wxString path_env;
    if (!::wxGetEnv("PATH", &path_env)) {
        return false;
    }

    wxArrayString paths_to_try = ::wxStringTokenize(path_env, ";", wxTOKEN_STRTOK);
    for (const auto& path : paths_to_try) {
        clDEBUG() << "Trying to locate compiler at:" << path << endl;
        auto cmp = CompilerLocatorMSYS2::Locate(path);
        if (cmp) {
            clDEBUG() << "Found compiler:" << cmp->GetName() << endl;
            m_compilers.push_back(cmp);
        }
    }
    return !m_compilers.empty();
}

// --------------------------------------------------
// --------------------------------------------------

CompilerLocatorMSYS2::CompilerLocatorMSYS2() {}

CompilerLocatorMSYS2::~CompilerLocatorMSYS2() {}

bool CompilerLocatorMSYS2::Locate()
{
    m_compilers.clear();

    // try some defaults
    const auto gcc_exe = m_msys2.Which("gcc");
    if (!gcc_exe) {
        return false;
    }

    auto compiler = Locate(wxFileName(*gcc_exe).GetPath());
    if (compiler) {
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
    if (!(gcc.FileExists() && gxx.FileExists())) {
        return nullptr;
    }

    // create new compiler
    CompilerPtr compiler(new Compiler(nullptr));
    compiler->SetName(gxx.GetFullPath());
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
    wxString make_extra_args;
    make_extra_args << "-j" << cpu_count;
    if (m_cmdShell) {
        make_extra_args << " SHELL=cmd.exe";
    }

    compiler->SetTool("MAKE", wxString() << make.GetFullPath() << " " << make_extra_args);
    compiler->SetTool("ResourceCompiler", windres.GetFullPath());
    compiler->SetTool("Debugger", gdb.GetFullPath());
    return compiler;
}

CompilerPtr CompilerLocatorMSYS2::Locate(const wxString& folder)
{
    // check for g++/clang++
    for (const auto& toolchain : TOOLCHAINS) {
        auto cmp = TryToolchain(folder, toolchain);
        if (cmp) {
            return cmp;
        }
    }
    return nullptr;
}

wxFileName CompilerLocatorMSYS2::GetFileName(const wxString& bin_dir, const wxString& fullname) const
{
    wxFileName tool(bin_dir, fullname);
    tool.SetExt("exe");
    return tool;
}
