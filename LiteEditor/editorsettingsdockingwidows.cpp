#include "editorsettingsdockingwidows.h"
#include "editor_config.h"

EditorSettingsDockingWindows::EditorSettingsDockingWindows( wxWindow* parent )
		: EditorSettingsDockingWindowsBase( parent )
{
	m_checkBoxHideOutputPaneOnClick->SetValue(EditorConfigST::Get()->GetOptions()->GetHideOutpuPaneOnUserClick());
	m_checkBoxHideOutputPaneNotIfBuild->SetValue(EditorConfigST::Get()->GetOptions()->GetHideOutputPaneNotIfBuild());
	m_checkBoxHideOutputPaneNotIfErrors->SetValue(EditorConfigST::Get()->GetOptions()->GetHideOutputPaneNotIfErrors());
	m_checkBoxHideOutputPaneNotIfSearch->SetValue(EditorConfigST::Get()->GetOptions()->GetHideOutputPaneNotIfSearch());
	m_checkBoxHideOutputPaneNotIfReplace->SetValue(EditorConfigST::Get()->GetOptions()->GetHideOutputPaneNotIfReplace());
	m_checkBoxHideOutputPaneNotIfReferences->SetValue(EditorConfigST::Get()->GetOptions()->GetHideOutputPaneNotIfReferences());
	m_checkBoxHideOutputPaneNotIfOutput->SetValue(EditorConfigST::Get()->GetOptions()->GetHideOutputPaneNotIfOutput());
	m_checkBoxHideOutputPaneNotIfDebug->SetValue(EditorConfigST::Get()->GetOptions()->GetHideOutputPaneNotIfDebug());
	m_checkBoxHideOutputPaneNotIfTrace->SetValue(EditorConfigST::Get()->GetOptions()->GetHideOutputPaneNotIfTrace());
	m_checkBoxHideOutputPaneNotIfTasks->SetValue(EditorConfigST::Get()->GetOptions()->GetHideOutputPaneNotIfTasks());
	m_checkBoxFindBarAtBottom->SetValue(EditorConfigST::Get()->GetOptions()->GetFindBarAtBottom());
	m_checkBoxOutputPaneCanDock->SetValue(EditorConfigST::Get()->GetOptions()->GetOutputPaneDockable());
	m_checkBoxShowDebugOnRun->SetValue(EditorConfigST::Get()->GetOptions()->GetShowDebugOnRun());
	m_checkBoxHideOutputPaneNotIfDebug->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( EditorSettingsDockingWindows::OnHideOutputPaneNotIfDebugUI ), NULL, this );
}

void EditorSettingsDockingWindows::Save(OptionsConfigPtr options)
{
	options->SetHideOutpuPaneOnUserClick( m_checkBoxHideOutputPaneOnClick->IsChecked() );
	options->SetHideOutputPaneNotIfBuild( m_checkBoxHideOutputPaneNotIfBuild->IsChecked() );
	options->SetHideOutputPaneNotIfErrors( m_checkBoxHideOutputPaneNotIfErrors->IsChecked() );
	options->SetHideOutputPaneNotIfSearch( m_checkBoxHideOutputPaneNotIfSearch->IsChecked() );
	options->SetHideOutputPaneNotIfReplace( m_checkBoxHideOutputPaneNotIfReplace->IsChecked() );
	options->SetHideOutputPaneNotIfReferences( m_checkBoxHideOutputPaneNotIfReferences->IsChecked() );
	options->SetHideOutputPaneNotIfOutput( m_checkBoxHideOutputPaneNotIfOutput->IsChecked() );
	options->SetHideOutputPaneNotIfDebug( m_checkBoxHideOutputPaneNotIfDebug->IsChecked() );
	options->SetHideOutputPaneNotIfTrace( m_checkBoxHideOutputPaneNotIfTrace->IsChecked() );
	options->SetHideOutputPaneNotIfTasks( m_checkBoxHideOutputPaneNotIfTasks->IsChecked() );
	options->SetFindBarAtBottom( m_checkBoxFindBarAtBottom->IsChecked() );
	options->SetOutputPaneDockable( m_checkBoxOutputPaneCanDock->IsChecked() );
	options->SetShowDebugOnRun( m_checkBoxShowDebugOnRun->IsChecked() );
}

void EditorSettingsDockingWindows::OnHideOutputPaneNotIfDebugUI(wxUpdateUIEvent& event)
{
	event.Enable( m_checkBoxHideOutputPaneOnClick->IsChecked() );
}
