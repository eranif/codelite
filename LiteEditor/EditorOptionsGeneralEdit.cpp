#include "EditorOptionsGeneralEdit.h"
#include "editor_config.h"
#include "globals.h"
#include "optionsconfig.h"

EditorOptionsGeneralEdit::EditorOptionsGeneralEdit(wxWindow* parent)
    : EditorOptionsGeneralEditBase(parent)
{
    ::wxPGPropertyBooleanUseCheckbox(m_pgMgrEdit->GetGrid());
    OptionsConfigPtr options = EditorConfigST::Get()->GetOptions();
    m_pgPropSmartCurly->SetValue(options->GetAutoAddMatchedCurlyBraces());
    // SmartParen: move one char to the right
    m_pgPropSmartParentheses->SetValue(options->IsSmartParen());
    // Auto complete closing paren
    m_pgPropAutoCompleteParen->SetValue(options->GetAutoAddMatchedNormalBraces());
    m_pgPropSmartQuotes->SetValue(options->GetAutoCompleteDoubleQuotes());
    m_pgPropCopyLineEmptySelection->SetValue(options->GetCopyLineEmptySelection());
    m_pgPropWrapBrackets->SetValue(options->IsWrapSelectionBrackets());
    m_pgPropWrapQuotes->SetValue(options->IsWrapSelectionWithQuotes());
    m_pgPropZoomUsingCtrlScroll->SetValue(options->IsMouseZoomEnabled());
    m_pgPropCommentsIndented->SetValue(options->GetIndentedComments());
}

EditorOptionsGeneralEdit::~EditorOptionsGeneralEdit() {}

void EditorOptionsGeneralEdit::OnValueChanged(wxPropertyGridEvent& event)
{
    // event.Skip();
}

void EditorOptionsGeneralEdit::Save(OptionsConfigPtr options)
{
    options->SetAutoAddMatchedCurlyBraces(m_pgPropSmartCurly->GetValue().GetBool());
    options->SetAutoAddMatchedNormalBraces(m_pgPropAutoCompleteParen->GetValue().GetBool());
    options->SetSmartParen(m_pgPropSmartParentheses->GetValue().GetBool());
    options->SetAutoCompleteDoubleQuotes(m_pgPropSmartQuotes->GetValue().GetBool());
    options->SetCopyLineEmptySelection(m_pgPropCopyLineEmptySelection->GetValue().GetBool());
    options->SetWrapSelectionBrackets(m_pgPropWrapBrackets->GetValue().GetBool());
    options->SetWrapSelectionWithQuotes(m_pgPropWrapQuotes->GetValue().GetBool());
    options->SetMouseZoomEnabled(m_pgPropZoomUsingCtrlScroll->GetValue().GetBool());
    options->SetIndentedComments(m_pgPropCommentsIndented->GetValue().GetBool());
}
