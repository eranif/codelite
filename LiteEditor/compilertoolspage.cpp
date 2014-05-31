#include "compilertoolspage.h"
#include "build_settings_config.h"
#include "compiler.h"

CompilerToolsPage::CompilerToolsPage( wxWindow* parent, const wxString &cmpname )
    : CompilerToolsBase( parent )
    , m_cmpname(cmpname)
{
    CompilerPtr cmp = BuildSettingsConfigST::Get()->GetCompiler(m_cmpname);
    m_pgPropCXX->SetValue( cmp->GetTool(wxT("CXX")) );
    m_pgPropCC->SetValue( cmp->GetTool(wxT("CC")) );
    m_pgPropAR->SetValue( cmp->GetTool(wxT("AR")) );
    m_pgPropLD->SetValue( cmp->GetTool("LinkerName") );
    m_pgPropSharedObjectLD->SetValue( cmp->GetTool("SharedObjectLinkerName"));
    m_pgPropAS->SetValue(cmp->GetTool("AS"));
    m_pgPropMAKE->SetValue(cmp->GetTool("MAKE"));
    m_pgPropResourceCompiler->SetValue(cmp->GetTool("ResourceCompiler"));
}

void CompilerToolsPage::Save(CompilerPtr cmp)
{
    cmp->SetTool(wxT("CXX"),                    m_pgPropCXX->GetValueAsString());
    cmp->SetTool(wxT("CC"),                     m_pgPropCC->GetValueAsString());
    cmp->SetTool(wxT("AR"),                     m_pgPropAR->GetValueAsString());
    cmp->SetTool(wxT("LinkerName"),             m_pgPropLD->GetValueAsString());
    cmp->SetTool(wxT("SharedObjectLinkerName"), m_pgPropSharedObjectLD->GetValueAsString());
    cmp->SetTool(wxT("ResourceCompiler"),       m_pgPropResourceCompiler->GetValueAsString());
    cmp->SetTool("MAKE",                        m_pgPropMAKE->GetValueAsString());
    cmp->SetTool("AS",                          m_pgPropAS->GetValueAsString());
}
