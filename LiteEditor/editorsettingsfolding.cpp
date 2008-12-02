#include "editorsettingsfolding.h"

EditorSettingsFolding::EditorSettingsFolding( wxWindow* parent )
: EditorSettingsFoldingBase( parent )
, TreeBookNode<EditorSettingsFolding>()
{
	OptionsConfigPtr options = EditorConfigST::Get()->GetOptions();
	m_displayMargin->SetValue(options->GetDisplayFoldMargin());
	m_underlineFolded->SetValue(options->GetUnderlineFoldLine());
	m_foldPreprocessors->SetValue(options->GetFoldPreprocessor());
	m_foldCompact->SetValue(options->GetFoldCompact());
	m_foldElse->SetValue(options->GetFoldAtElse());
	m_foldStyle->SetStringSelection( options->GetFoldStyle() );
}


void EditorSettingsFolding::Save(OptionsConfigPtr options)
{
	options->SetDisplayFoldMargin(m_displayMargin->GetValue());
	options->SetUnderlineFoldLine(m_underlineFolded->GetValue());
	options->SetFoldPreprocessor(m_foldPreprocessors->GetValue());
	options->SetFoldCompact(m_foldCompact->GetValue());
	options->SetFoldAtElse(m_foldElse->GetValue());
	options->SetFoldStyle(m_foldStyle->GetStringSelection());
}
