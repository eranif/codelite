#include "compileradvancepage.h"
#include "build_settings_config.h"
#include "compiler.h"

CompilerAdvancePage::CompilerAdvancePage( wxWindow* parent, const wxString &cmpname )
: CompilerAdvanceBase( parent )
, m_cmpname(cmpname)
{
	CompilerPtr cmp = BuildSettingsConfigST::Get()->GetCompiler(m_cmpname);
	m_textObjectExtension->SetValue(cmp->GetObjectSuffix());
    m_textDependExtension->SetValue(cmp->GetDependSuffix());
    m_textPreprocessExtension->SetValue(cmp->GetPreprocessSuffix());
	m_checkBoxGenerateDependenciesFiles->SetValue(cmp->GetGenerateDependeciesFile());
	m_textCtrlGlobalIncludePath->SetValue(cmp->GetGlobalIncludePath());
	m_textCtrlGlobalLibPath->SetValue(cmp->GetGlobalLibPath());
}

void CompilerAdvancePage::Save(CompilerPtr cmp)
{
	cmp->SetGenerateDependeciesFile(m_checkBoxGenerateDependenciesFiles->IsChecked());
	cmp->SetGlobalIncludePath(m_textCtrlGlobalIncludePath->GetValue());
	cmp->SetGlobalLibPath(m_textCtrlGlobalLibPath->GetValue());
	cmp->SetObjectSuffix(m_textObjectExtension->GetValue());
    cmp->SetDependSuffix(m_textDependExtension->GetValue());
    cmp->SetPreprocessSuffix(m_textPreprocessExtension->GetValue());
}
