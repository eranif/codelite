#include "CompilerLocatorEosCDT.h"
#include <wx/filename.h>
#include "globals.h"
#include <algorithm>
#include <wx/thread.h>

CompilerLocatorEosCDT::CompilerLocatorEosCDT() {}

CompilerLocatorEosCDT::~CompilerLocatorEosCDT() {}

bool CompilerLocatorEosCDT::Locate()
{
    std::vector<wxString> possiblePaths{ "/usr/bin", "/usr/local/bin" };
    std::for_each(possiblePaths.begin(), possiblePaths.end(), [&](const wxString& path) {
        wxString foundPath;
        if(CheckExists(path, foundPath)) { m_compilers.push_back(CreateCompiler(foundPath)); }
    });
    return !m_compilers.empty();
}

CompilerPtr CompilerLocatorEosCDT::Locate(const wxString& folder)
{
    m_compilers.clear();
    wxString foundPath;
    if(!CheckExists(folder, foundPath)) { return NULL; }

    m_compilers.push_back(CreateCompiler(foundPath));
    return m_compilers[0];
}

void CompilerLocatorEosCDT::AddTool(CompilerPtr compiler, const wxString& toolname, const wxString& path,
                                    const wxString& args) const
{
    wxString tool = path;
    ::WrapWithQuotes(tool);
    if(!args.IsEmpty()) { tool << " " << args; }
    compiler->SetTool(toolname, tool);
}

bool CompilerLocatorEosCDT::CheckExists(const wxString& path, wxString& foundPath) const
{
    wxFileName eosio_tool(path, "eosio-cc");
    wxFileName tmpfn(path, "");
#ifdef __WXMSW__
    eosio_tool.SetExt("exe");
#endif

    bool found = eosio_tool.FileExists();
    if(!found) {
        // try to see if we have a bin folder here
        eosio_tool.AppendDir("bin");
        found = eosio_tool.FileExists();
        if(found) { foundPath = eosio_tool.GetPath(); }
    } else {
        foundPath = eosio_tool.GetPath();
    }
    return found;
}

CompilerPtr CompilerLocatorEosCDT::CreateCompiler(const wxString& path) const
{

    CompilerPtr compiler(new Compiler(NULL));
    compiler->SetCompilerFamily(COMPILER_FAMILY_CLANG);

    // get the compiler version
    compiler->SetName("eosio");
    compiler->SetGenerateDependeciesFile(true);
    compiler->SetInstallationPath(path);
    
    // Add the tools
    wxFileName eosio_tool(path, "eosio-cc");
#ifdef __WXMSW__
    eosio_tool.SetExt("exe");
#endif
    AddTool(compiler, "CC", eosio_tool.GetFullPath());

    eosio_tool.SetName("eosio-cpp");
    AddTool(compiler, "CXX", eosio_tool.GetFullPath());
    AddTool(compiler, "LinkerName", eosio_tool.GetFullPath());
#ifdef __WXMAC__
    AddTool(compiler, "SharedObjectLinkerName", eosio_tool.GetFullPath(), "-dynamiclib -fPIC");
#else
    AddTool(compiler, "SharedObjectLinkerName", eosio_tool.GetFullPath(), "-shared -fPIC");
#endif
    eosio_tool.SetName("ar");
    AddTool(compiler, "AR", eosio_tool.GetFullPath(), "rcu");
    wxString makeExtraArgs;
    if(wxThread::GetCPUCount() > 1) {
        makeExtraArgs << "-j" << wxThread::GetCPUCount();
    }

#ifdef __WXMSW__
    AddTool(compiler, "MAKE", "mingw32-make.exe", makeExtraArgs);
#else
    AddTool(compiler, "MAKE", "make", makeExtraArgs);
#endif
    return compiler;
}
