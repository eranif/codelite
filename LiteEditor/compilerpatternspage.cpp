#include "compilerpatternspage.h"
#include "build_settings_config.h"
#include "compiler.h"

CompilerPatternsPage::CompilerPatternsPage( wxWindow* parent, const wxString &cmpname )
		: CompilerPatternsBase( parent )
		, m_cmpname(cmpname)
{
	CompilerPtr cmp = BuildSettingsConfigST::Get()->GetCompiler(m_cmpname);
	m_textErrorPattern->SetValue(cmp->GetErrPattern());
	m_textErrorFileIndex->SetValue(cmp->GetErrFileNameIndex());
	m_textErrorLineNumber->SetValue(cmp->GetErrLineNumberIndex());
	m_textWarnPattern->SetValue(cmp->GetWarnPattern());
	m_textWarnFileIndex->SetValue(cmp->GetWarnFileNameIndex());
	m_textWarnLineNumber->SetValue(cmp->GetWarnLineNumberIndex());
}
