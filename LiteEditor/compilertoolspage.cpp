#include "compilertoolspage.h"
#include "build_settings_config.h"
#include "compiler.h"

CompilerToolsPage::CompilerToolsPage( wxWindow* parent, const wxString &cmpname )
    : CompilerToolsBase( parent )
    , m_cmpname(cmpname)
{
    CompilerPtr cmp = BuildSettingsConfigST::Get()->GetCompiler(m_cmpname);
    m_textArchiveTool->ChangeValue(cmp->GetTool(wxT("AR")));
    m_textCompilerName->ChangeValue(cmp->GetTool(wxT("CXX")));
    m_textCtrlCCompilerName->ChangeValue(cmp->GetTool(wxT("CC")));
    m_textLinkerName->ChangeValue(cmp->GetTool(wxT("LinkerName")));
    m_textSOLinker->ChangeValue(cmp->GetTool(wxT("SharedObjectLinkerName")));
    m_textResourceCmp->ChangeValue(cmp->GetTool(wxT("ResourceCompiler")));
    m_textCtrlPathVariable->ChangeValue(cmp->GetPathVariable());
    m_textCtrlMake->ChangeValue(cmp->GetTool("MAKE"));
}

void CompilerToolsPage::Save(CompilerPtr cmp)
{
    cmp->SetTool(wxT("AR"),            m_textArchiveTool->GetValue());
    cmp->SetTool(wxT("CXX"),           m_textCompilerName->GetValue());
    cmp->SetTool(wxT("CC"),         m_textCtrlCCompilerName->GetValue());
    cmp->SetTool(wxT("LinkerName"),             m_textLinkerName->GetValue());
    cmp->SetTool(wxT("SharedObjectLinkerName"), m_textSOLinker->GetValue());
    cmp->SetTool(wxT("ResourceCompiler"),       m_textResourceCmp->GetValue());
    cmp->SetPathVariable(m_textCtrlPathVariable->GetValue());
    cmp->SetTool("MAKE", m_textCtrlMake->GetValue());
}
