#include "editorsettingsterminal.h"
#include "editor_config.h"

EditorSettingsTerminal::EditorSettingsTerminal( wxWindow* parent )
		: EditorSettingsTerminalBase( parent )
		, TreeBookNode<EditorSettingsTerminal>()

{
	OptionsConfigPtr options = EditorConfigST::Get()->GetOptions();
    m_textCtrlProgramConsoleCmd->SetValue(options->GetProgramConsoleCommand());
}

void EditorSettingsTerminal::Save(OptionsConfigPtr options)
{
	options->SetProgramConsoleCommand (m_textCtrlProgramConsoleCmd->GetValue());
}
