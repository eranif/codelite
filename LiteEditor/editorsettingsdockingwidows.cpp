#include "editorsettingsdockingwidows.h"
#include "editor_config.h"

EditorSettingsDockingWindows::EditorSettingsDockingWindows( wxWindow* parent )
		: EditorSettingsDockingWindowsBase( parent )
{
	m_checkBoxHideOutputPaneOnClick->SetValue(EditorConfigST::Get()->GetOptions()->GetHideOutpuPaneOnUserClick());
	m_checkBoxHideOutputPaneNotIfDebug->SetValue(EditorConfigST::Get()->GetOptions()->GetHideOutputPaneNotIfDebug());
	m_checkBoxShowQuickFinder->SetValue(EditorConfigST::Get()->GetOptions()->GetShowQuickFinder());
	
	m_checkBoxHideOutputPaneNotIfDebug->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( EditorSettingsDockingWindows::OnHideOutputPaneNotIfDebugUI ), NULL, this );
}

void EditorSettingsDockingWindows::Save(OptionsConfigPtr options)
{
	options->SetHideOutpuPaneOnUserClick( m_checkBoxHideOutputPaneOnClick->IsChecked() );
	options->SetHideOutputPaneNotIfDebug( m_checkBoxHideOutputPaneNotIfDebug->IsChecked() );
	options->SetShowQuickFinder( m_checkBoxShowQuickFinder->IsChecked() );
}

void EditorSettingsDockingWindows::OnHideOutputPaneNotIfDebugUI(wxUpdateUIEvent& event)
{
	event.Enable( m_checkBoxHideOutputPaneOnClick->IsChecked() );
}

