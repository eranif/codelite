#include "EditorOptionsGeneralEdit.h"

#include "editor_config.h"
#include "globals.h"
#include "optionsconfig.h"

EditorOptionsGeneralEdit::EditorOptionsGeneralEdit(wxWindow* parent)
    : EditorOptionsGeneralEditBase(parent)
{
    OptionsConfigPtr options = EditorConfigST::Get()->GetOptions();
    m_checkBoxEditSmartCurlyBrackets->SetValue(options->GetAutoAddMatchedCurlyBraces());
    m_checkBoxAutoCompleteParen->SetValue(options->GetAutoAddMatchedNormalBraces());
    m_checkBoxEditSmartParenthesis->SetValue(options->IsSmartParen());
    m_checkBoxSmartQuotes->SetValue(options->GetAutoCompleteDoubleQuotes());
    m_checkBoxCopyEntireLine->SetValue(options->GetCopyLineEmptySelection());
    m_checkBoxWrapWithBrackets->SetValue(options->IsWrapSelectionBrackets());
    m_checkBoxWrapWithQuotes->SetValue(options->IsWrapSelectionWithQuotes());
    m_checkBoxEnableMouseZoom->SetValue(options->IsMouseZoomEnabled());
    m_checkBoxIdentLineComments->SetValue(options->GetIndentedComments());
    m_checkBoxDisableSemicolonShift->SetValue(options->GetDisableSemicolonShift());
}

EditorOptionsGeneralEdit::~EditorOptionsGeneralEdit() {}

void EditorOptionsGeneralEdit::Save(OptionsConfigPtr options)
{
    options->SetAutoAddMatchedCurlyBraces(m_checkBoxEditSmartCurlyBrackets->IsChecked());
    options->SetAutoAddMatchedNormalBraces(m_checkBoxAutoCompleteParen->IsChecked());
    options->SetSmartParen(m_checkBoxEditSmartParenthesis->IsChecked());
    options->SetAutoCompleteDoubleQuotes(m_checkBoxSmartQuotes->IsChecked());
    options->SetCopyLineEmptySelection(m_checkBoxCopyEntireLine->IsChecked());
    options->SetWrapSelectionBrackets(m_checkBoxWrapWithBrackets->IsChecked());
    options->SetWrapSelectionWithQuotes(m_checkBoxWrapWithQuotes->IsChecked());
    options->SetMouseZoomEnabled(m_checkBoxEnableMouseZoom->IsChecked());
    options->SetIndentedComments(m_checkBoxIdentLineComments->IsChecked());
    options->SetDisableSemicolonShift(m_checkBoxDisableSemicolonShift->IsChecked());
}
