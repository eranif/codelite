#include "CompilerLocatorEosCDT.h"
#include <wx/filename.h>

CompilerLocatorEosCDT::CompilerLocatorEosCDT() {}

CompilerLocatorEosCDT::~CompilerLocatorEosCDT() {}

bool CompilerLocatorEosCDT::Locate() { return false; }

CompilerPtr CompilerLocatorEosCDT::Locate(const wxString& folder)
{
    wxString name;
    m_compilers.clear();
    wxFileName eosio_cc(folder, "eosio-cc");
    wxFileName tmpfn(folder, "");
#ifdef __WXMSW__
    eosio_cc.SetExt("exe");
#endif

    bool found = eosio_cc.FileExists();
    if(!found) {
        // try to see if we have a bin folder here
        eosio_cc.AppendDir("bin");
        found = eosio_cc.FileExists();
    }

    if(found) {
        CompilerPtr compiler(new Compiler(NULL));
        compiler->SetCompilerFamily(COMPILER_FAMILY_CLANG);

        // get the compiler version
        compiler->SetName("eosio");
        compiler->SetGenerateDependeciesFile(true);
        m_compilers.push_back(compiler);

        // Add the tools
        AddTool("CC", eosio_cc.GetFullPath());
        return compiler;
    }
    return NULL;
}

void CompilerLocatorEosCDT::AddTool(const wxString& toolname, const wxString& path, const wxString& args)
{
}
