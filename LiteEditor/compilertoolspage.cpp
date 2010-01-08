#include "compilertoolspage.h"
#include "build_settings_config.h"
#include "compiler.h"

CompilerToolsPage::CompilerToolsPage( wxWindow* parent, const wxString &cmpname )
		: CompilerToolsBase( parent )
		, m_cmpname(cmpname)
{
	CompilerPtr cmp = BuildSettingsConfigST::Get()->GetCompiler(m_cmpname);
	m_textArchiveTool->SetValue(cmp->GetTool(wxT("ArchiveTool")));
	m_textCompilerName->SetValue(cmp->GetTool(wxT("CompilerName")));
	m_textCtrlCCompilerName->SetValue(cmp->GetTool(wxT("C_CompilerName")));
	m_textLinkerName->SetValue(cmp->GetTool(wxT("LinkerName")));
	m_textSOLinker->SetValue(cmp->GetTool(wxT("SharedObjectLinkerName")));
	m_textResourceCmp->SetValue(cmp->GetTool(wxT("ResourceCompiler")));
	m_textCtrlPathVariable->SetValue(cmp->GetPathVariable());
}

void CompilerToolsPage::Save(CompilerPtr cmp)
{
	cmp->SetTool(wxT("ArchiveTool"),            m_textArchiveTool->GetValue());
	cmp->SetTool(wxT("CompilerName"),           m_textCompilerName->GetValue());
	cmp->SetTool(wxT("C_CompilerName"),         m_textCtrlCCompilerName->GetValue());
	cmp->SetTool(wxT("LinkerName"),             m_textLinkerName->GetValue());
	cmp->SetTool(wxT("SharedObjectLinkerName"), m_textSOLinker->GetValue());
	cmp->SetTool(wxT("ResourceCompiler"),       m_textResourceCmp->GetValue());
	cmp->SetPathVariable(m_textCtrlPathVariable->GetValue());
}
