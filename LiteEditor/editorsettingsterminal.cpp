#include "editorsettingsterminal.h"
#include "editor_config.h"

EditorSettingsTerminal::EditorSettingsTerminal( wxWindow* parent )
    : EditorSettingsTerminalBase( parent )
    , TreeBookNode<EditorSettingsTerminal>()

{
    OptionsConfigPtr options = EditorConfigST::Get()->GetOptions();
    m_textCtrlProgramConsoleCmd->SetValue(options->GetProgramConsoleCommand());
    m_checkBoxUseCodeLiteTerminal->SetValue( options->HasOption( OptionsConfig::Opt_Use_CodeLite_Terminal) );
}

void EditorSettingsTerminal::Save(OptionsConfigPtr options)
{
    options->SetProgramConsoleCommand (m_textCtrlProgramConsoleCmd->GetValue());
    options->EnableOption( OptionsConfig::Opt_Use_CodeLite_Terminal, m_checkBoxUseCodeLiteTerminal->IsChecked() );
}

void EditorSettingsTerminal::OnUseCodeLiteTerminalUI(wxUpdateUIEvent& event)
{
#ifdef __WXMSW__
    event.Enable( false );
#else
    event.Enable( !m_checkBoxUseCodeLiteTerminal->IsChecked() );
#endif
}
