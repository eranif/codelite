#include "EditorOptionsGeneralEdit.h"

#include "editor_config.h"
#include "globals.h"
#include "optionsconfig.h"

EditorOptionsGeneralEdit::EditorOptionsGeneralEdit(wxWindow* parent, OptionsConfigPtr options)
    : OptionsConfigPage(parent, options)
{
    AddHeader(_("Edit"));
    AddProperty(_("Smart curly brackets"), m_options->GetAutoAddMatchedCurlyBraces(),
                UPDATE_BOOL_CB(SetAutoAddMatchedCurlyBraces));
    AddProperty(_("Smart square brackets and parentheses"), m_options->GetAutoAddMatchedNormalBraces(),
                UPDATE_BOOL_CB(SetAutoAddMatchedNormalBraces));
    AddProperty(_("Auto complete closing bracket and parentheses"), m_options->IsSmartParen(),
                UPDATE_BOOL_CB(SetSmartParen));
    AddProperty(_("Smart quotes"), m_options->GetAutoCompleteDoubleQuotes(),
                UPDATE_BOOL_CB(SetAutoCompleteDoubleQuotes));
    AddProperty(_("Copying empty selection copies caret line"), m_options->GetCopyLineEmptySelection(),
                UPDATE_BOOL_CB(SetCopyLineEmptySelection));
    AddProperty(_("Disable semicolon shift"), m_options->GetDisableSemicolonShift(),
                UPDATE_BOOL_CB(SetDisableSemicolonShift));

    AddHeader(_("Typing in Selection"));
    AddProperty(_("Wrap with quotes"), m_options->IsWrapSelectionWithQuotes(),
                UPDATE_BOOL_CB(SetWrapSelectionWithQuotes));
    AddProperty(_("Wrap with brackets"), m_options->IsWrapSelectionBrackets(),
                UPDATE_BOOL_CB(SetWrapSelectionBrackets));

    AddHeader(_("Other"));
    AddProperty(_("Enable zoom with mouse scroll"), m_options->IsMouseZoomEnabled(),
                UPDATE_BOOL_CB(SetMouseZoomEnabled));
    AddProperty(_("Indent line comments"), m_options->GetIndentedComments(), UPDATE_BOOL_CB(SetIndentedComments));
}

EditorOptionsGeneralEdit::~EditorOptionsGeneralEdit() {}
