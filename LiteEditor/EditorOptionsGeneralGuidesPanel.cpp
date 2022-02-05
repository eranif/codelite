#include "EditorOptionsGeneralGuidesPanel.h"

#include "cl_config.h"
#include "editor_config.h"

EditorOptionsGeneralGuidesPanel::EditorOptionsGeneralGuidesPanel(wxWindow* parent)
    : EditorOptionsGeneralGuidesPanelBase(parent)
{
    OptionsConfigPtr options = EditorConfigST::Get()->GetOptions();
    m_checkBoxLineNumbersShow->SetValue(options->GetDisplayLineNumbers());
    m_checkBoxLineNumbersHighlightCurrent->SetValue(options->IsLineNumberHighlightCurrent());
    m_checkBoxLineNumbersRelative->SetValue(options->GetRelativeLineNumbers());
    m_checkBoxHighlightIndentLines->SetValue(options->GetShowIndentationGuidelines());
    m_checkBoxHighlightBraces->SetValue(options->GetHighlightMatchedBraces());
    m_checkBoxCaretLineEnabeldHighlight->SetValue(options->GetHighlightCaretLine());
    m_colourPickerCaretLineColour->SetColour(options->GetCaretLineColour());
    m_spinCtrlCaretLineAlpha->SetValue(options->GetCaretLineAlpha());
    m_checkBoxHighlightModifiedLines->SetValue(options->IsTrackChanges());
    m_checkBoxDebuggerLineEnabled->SetValue(options->HasOption(OptionsConfig::Opt_Mark_Debugger_Line));
    m_colourPickerDebuggerLineColour->SetColour(options->GetDebuggerMarkerLine());
    m_choiceWhitespaceVisibility->SetSelection(options->GetShowWhitspaces());
    m_choiceWhitespaceEOLMode->SetStringSelection(options->GetEolMode());
    m_spinCtrlWhitespaceLineSpacing->SetValue(clConfig::Get().Read("extra_line_spacing", (int)0));
}

EditorOptionsGeneralGuidesPanel::~EditorOptionsGeneralGuidesPanel() {}

void EditorOptionsGeneralGuidesPanel::OnDisplayLineNumbersUI(wxUpdateUIEvent& event)
{
    event.Enable(m_checkBoxLineNumbersShow->IsChecked());
}

void EditorOptionsGeneralGuidesPanel::OnHighlightCaretLineUI(wxUpdateUIEvent& event)
{
    event.Enable(m_checkBoxCaretLineEnabeldHighlight->IsChecked());
}

void EditorOptionsGeneralGuidesPanel::OnHighlightDebuggerLineUI(wxUpdateUIEvent& event)
{
    event.Enable(m_checkBoxDebuggerLineEnabled->IsChecked());
}

void EditorOptionsGeneralGuidesPanel::Save(OptionsConfigPtr options)
{
    options->SetDisplayLineNumbers(m_checkBoxLineNumbersShow->IsChecked());
    options->SetRelativeLineNumbers(m_checkBoxLineNumbersRelative->IsChecked());
    options->SetHighlightMatchedBraces(m_checkBoxHighlightBraces->IsChecked());
    options->SetShowIndentationGuidelines(m_checkBoxHighlightIndentLines->IsChecked());
    options->SetHighlightCaretLine(m_checkBoxCaretLineEnabeldHighlight->IsChecked());

    options->SetCaretLineColour(m_colourPickerCaretLineColour->GetColour());
    options->SetCaretLineAlpha(m_spinCtrlCaretLineAlpha->GetValue());

    options->SetEolMode(m_choiceWhitespaceEOLMode->GetStringSelection());
    options->SetTrackChanges(m_checkBoxHighlightModifiedLines->IsChecked());
    options->SetDebuggerMarkerLine(m_colourPickerDebuggerLineColour->GetColour());
    options->EnableOption(OptionsConfig::Opt_Mark_Debugger_Line, m_checkBoxDebuggerLineEnabled->IsChecked());
    options->SetShowWhitspaces(m_choiceWhitespaceVisibility->GetSelection());
    options->SetLineNumberHighlightCurrent(m_checkBoxLineNumbersHighlightCurrent->IsChecked());
    clConfig::Get().Write("extra_line_spacing", (int)m_spinCtrlWhitespaceLineSpacing->GetValue());
}

void EditorOptionsGeneralGuidesPanel::OnUseRelativeLineNumbersUI(wxUpdateUIEvent& event)
{
    event.Enable(m_checkBoxLineNumbersShow->IsChecked() && m_checkBoxLineNumbersHighlightCurrent->IsChecked());
}
