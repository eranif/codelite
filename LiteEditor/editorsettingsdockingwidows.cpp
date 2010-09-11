#include "editorsettingsdockingwidows.h"
#include "editor_config.h"

EditorSettingsDockingWindows::EditorSettingsDockingWindows( wxWindow* parent )
		: EditorSettingsDockingWindowsBase( parent )
{
	m_checkBoxHideOutputPaneOnClick->SetValue(EditorConfigST::Get()->GetOptions()->GetHideOutpuPaneOnUserClick());
	m_checkBoxHideOutputPaneNotIfDebug->SetValue(EditorConfigST::Get()->GetOptions()->GetHideOutputPaneNotIfDebug());
	m_checkBoxFindBarAtBottom->SetValue(EditorConfigST::Get()->GetOptions()->GetFindBarAtBottom());
	m_checkBoxOutputPaneCanDock->SetValue(EditorConfigST::Get()->GetOptions()->GetOutputPaneDockable());
	m_checkBoxShowDebugOnRun->SetValue(EditorConfigST::Get()->GetOptions()->GetShowDebugOnRun());
	m_checkBoxHideOutputPaneNotIfDebug->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( EditorSettingsDockingWindows::OnHideOutputPaneNotIfDebugUI ), NULL, this );
}

void EditorSettingsDockingWindows::Save(OptionsConfigPtr options)
{
	options->SetHideOutpuPaneOnUserClick( m_checkBoxHideOutputPaneOnClick->IsChecked() );
	options->SetHideOutputPaneNotIfDebug( m_checkBoxHideOutputPaneNotIfDebug->IsChecked() );
	options->SetFindBarAtBottom( m_checkBoxFindBarAtBottom->IsChecked() );
	options->SetOutputPaneDockable( m_checkBoxOutputPaneCanDock->IsChecked() );
	options->SetShowDebugOnRun( m_checkBoxShowDebugOnRun->IsChecked() );
}

void EditorSettingsDockingWindows::OnHideOutputPaneNotIfDebugUI(wxUpdateUIEvent& event)
{
	event.Enable( m_checkBoxHideOutputPaneOnClick->IsChecked() );
}
