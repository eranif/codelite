#include "editorsettingscaret.h"
#include "editor_config.h"

EditorSettingsCaret::EditorSettingsCaret( wxWindow* parent )
		:EditorSettingsCaretBase( parent )
		, TreeBookNode<EditorSettingsCaret>()
{
	OptionsConfigPtr options = EditorConfigST::Get()->GetOptions();
    m_spinCtrlBlinkPeriod->SetValue(options->GetCaretBlinkPeriod());
	m_spinCtrlCaretWidth->SetValue(options->GetCaretWidth());
	m_checkBoxCaretUseCamelCase->SetValue(options->GetCaretUseCamelCase());
	m_checkBoxScrollBeyondLastLine->SetValue( options->GetScrollBeyondLastLine() );
	m_checkBoxAdjustScrollbarSize->SetValue(options->GetAutoAdjustHScrollBarWidth());
}

void EditorSettingsCaret::Save(OptionsConfigPtr options)
{
	options->SetCaretBlinkPeriod(m_spinCtrlBlinkPeriod->GetValue());
	options->SetCaretWidth(m_spinCtrlCaretWidth->GetValue());
	options->SetCaretUseCamelCase(m_checkBoxCaretUseCamelCase->IsChecked());
	options->SetScrollBeyondLastLine(m_checkBoxScrollBeyondLastLine->IsChecked());
	options->SetAutoAdjustHScrollBarWidth(m_checkBoxAdjustScrollbarSize->IsChecked());
}
