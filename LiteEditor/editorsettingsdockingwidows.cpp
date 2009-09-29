#include "editorsettingsdockingwidows.h"
#include "editor_config.h"

EditorSettingsDockingWindows::EditorSettingsDockingWindows( wxWindow* parent )
		: EditorSettingsDockingWindowsBase( parent )
{
	m_checkBoxHideOutputPaneOnClick->SetValue(EditorConfigST::Get()->GetOptions()->GetHideOutpuPaneOnUserClick());
	m_checkBoxShowQuickFinder->SetValue(EditorConfigST::Get()->GetOptions()->GetShowQuickFinder());
}

void EditorSettingsDockingWindows::Save(OptionsConfigPtr options)
{
	options->SetHideOutpuPaneOnUserClick( m_checkBoxHideOutputPaneOnClick->IsChecked() );
	options->SetShowQuickFinder( m_checkBoxShowQuickFinder->IsChecked() );
}
