#include "editorsettingsdockingwidows.h"
#include "editor_config.h"

EditorSettingsDockingWindows::EditorSettingsDockingWindows( wxWindow* parent )
		: EditorSettingsDockingWindowsBase( parent )
{
	OptionsConfigPtr options = EditorConfigST::Get()->GetOptions();
	
	m_checkBoxHideOutputPaneOnClick->SetValue        (options->GetHideOutpuPaneOnUserClick());
	m_checkBoxHideOutputPaneNotIfBuild->SetValue     (options->GetHideOutputPaneNotIfBuild());
	m_checkBoxHideOutputPaneNotIfErrors->SetValue    (options->GetHideOutputPaneNotIfErrors());
	m_checkBoxHideOutputPaneNotIfSearch->SetValue    (options->GetHideOutputPaneNotIfSearch());
	m_checkBoxHideOutputPaneNotIfReplace->SetValue   (options->GetHideOutputPaneNotIfReplace());
	m_checkBoxHideOutputPaneNotIfReferences->SetValue(options->GetHideOutputPaneNotIfReferences());
	m_checkBoxHideOutputPaneNotIfOutput->SetValue    (options->GetHideOutputPaneNotIfOutput());
	m_checkBoxHideOutputPaneNotIfDebug->SetValue     (options->GetHideOutputPaneNotIfDebug());
	m_checkBoxHideOutputPaneNotIfTrace->SetValue     (options->GetHideOutputPaneNotIfTrace());
	m_checkBoxHideOutputPaneNotIfTasks->SetValue     (options->GetHideOutputPaneNotIfTasks());
	m_checkBoxFindBarAtBottom->SetValue              (options->GetFindBarAtBottom());
	m_checkBoxOutputPaneCanDock->SetValue            (options->GetOutputPaneDockable());
	m_checkBoxShowDebugOnRun->SetValue               (options->GetShowDebugOnRun());
	m_radioBoxHint->SetSelection                     (options->GetDockingStyle());
	
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
	options->SetDockingStyle( m_radioBoxHint->GetSelection() );
}

void EditorSettingsDockingWindows::OnHideOutputPaneNotIfDebugUI(wxUpdateUIEvent& event)
{
	event.Enable( m_checkBoxHideOutputPaneOnClick->IsChecked() );
}
