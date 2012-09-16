#include "editorsettingscaret.h"
#include "editor_config.h"
#include "globals.h"

EditorSettingsCaret::EditorSettingsCaret( wxWindow* parent )
    :EditorSettingsCaretBase( parent )
    , TreeBookNode<EditorSettingsCaret>()
{
    OptionsConfigPtr options = EditorConfigST::Get()->GetOptions();
    m_spinCtrlBlinkPeriod->SetValue( ::wxIntToString(options->GetCaretBlinkPeriod()) );
    m_spinCtrlCaretWidth->SetValue( ::wxIntToString(options->GetCaretWidth()) );
    m_checkBoxCaretUseCamelCase->SetValue(options->GetCaretUseCamelCase());
    m_checkBoxScrollBeyondLastLine->SetValue( options->GetScrollBeyondLastLine() );
    m_checkBoxAdjustScrollbarSize->SetValue(options->GetAutoAdjustHScrollBarWidth());
}

void EditorSettingsCaret::Save(OptionsConfigPtr options)
{
    options->SetCaretBlinkPeriod( ::wxStringToInt(m_spinCtrlBlinkPeriod->GetValue(), 500, 0) );
    options->SetCaretWidth( ::wxStringToInt(m_spinCtrlCaretWidth->GetValue(), 1, 1, 10) );
    options->SetCaretUseCamelCase(m_checkBoxCaretUseCamelCase->IsChecked());
    options->SetScrollBeyondLastLine(m_checkBoxScrollBeyondLastLine->IsChecked());
    options->SetAutoAdjustHScrollBarWidth(m_checkBoxAdjustScrollbarSize->IsChecked());
}
