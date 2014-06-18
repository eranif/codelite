#include "editorsettingsdockingwidows.h"
#include "editor_config.h"

EditorSettingsDockingWindows::EditorSettingsDockingWindows( wxWindow* parent )
    : EditorSettingsDockingWindowsBase( parent )
{
    OptionsConfigPtr options = EditorConfigST::Get()->GetOptions();

    m_checkBoxHideOutputPaneOnClick->SetValue        (options->GetHideOutpuPaneOnUserClick());
    m_checkBoxHideOutputPaneNotIfBuild->SetValue     (options->GetHideOutputPaneNotIfBuild());
    m_checkBoxHideOutputPaneNotIfSearch->SetValue    (options->GetHideOutputPaneNotIfSearch());
    m_checkBoxHideOutputPaneNotIfReplace->SetValue   (options->GetHideOutputPaneNotIfReplace());
    m_checkBoxHideOutputPaneNotIfReferences->SetValue(options->GetHideOutputPaneNotIfReferences());
    m_checkBoxHideOutputPaneNotIfOutput->SetValue    (options->GetHideOutputPaneNotIfOutput());
    m_checkBoxHideOutputPaneNotIfTrace->SetValue     (options->GetHideOutputPaneNotIfTrace());
    m_checkBoxHideOutputPaneNotIfTasks->SetValue     (options->GetHideOutputPaneNotIfTasks());
    m_checkBoxHideOutputPaneNotIfBuildQ->SetValue    (options->GetHideOutputPaneNotIfBuildQ());
    m_checkBoxHideOutputPaneNotIfCppCheck->SetValue  (options->GetHideOutputPaneNotIfCppCheck());
    m_checkBoxHideOutputPaneNotIfSvn->SetValue       (options->GetHideOutputPaneNotIfSvn());
    m_checkBoxHideOutputPaneNotIfCscope->SetValue    (options->GetHideOutputPaneNotIfCscope());
    m_checkBoxHideOutputPaneNotIfGit->SetValue       (options->GetHideOutputPaneNotIfGit());
    m_checkBoxHideOutputPaneNotIfDebug->SetValue     (options->GetHideOutputPaneNotIfDebug());
    m_checkBoxFindBarAtBottom->SetValue              (options->GetFindBarAtBottom());
    m_checkBoxDontFoldSearchResults->SetValue        (options->GetDontAutoFoldResults());
    m_checkBoxShowDebugOnRun->SetValue               (options->GetShowDebugOnRun());
    m_radioBoxHint->SetSelection                     (options->GetDockingStyle());
    m_checkBoxHideCaptions->SetValue                 (!options->IsShowDockingWindowCaption());

    int tabStyle (0); // Glossy
    m_startingFlags = OptionsConfig::TabGlossy;
    if(options->GetOptions() & OptionsConfig::TabCurved) {
        tabStyle = 1;
        m_startingFlags = OptionsConfig::TabCurved;
    }

    m_endFlags = m_startingFlags;
    m_radioBoxTabControlStyle->SetSelection(tabStyle);
#if CL_USE_NATIVEBOOK
    m_radioBoxTabControlStyle->Enable(false);
#endif
    m_checkBoxHideOutputPaneNotIfDebug->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( EditorSettingsDockingWindows::OnHideOutputPaneNotIfDebugUI ), NULL, this );
}

void EditorSettingsDockingWindows::Save(OptionsConfigPtr options)
{
    options->SetHideOutpuPaneOnUserClick( m_checkBoxHideOutputPaneOnClick->IsChecked() );
    options->SetHideOutputPaneNotIfBuild( m_checkBoxHideOutputPaneNotIfBuild->IsChecked() );
    options->SetHideOutputPaneNotIfSearch( m_checkBoxHideOutputPaneNotIfSearch->IsChecked() );
    options->SetHideOutputPaneNotIfReplace( m_checkBoxHideOutputPaneNotIfReplace->IsChecked() );
    options->SetHideOutputPaneNotIfReferences( m_checkBoxHideOutputPaneNotIfReferences->IsChecked() );
    options->SetHideOutputPaneNotIfOutput( m_checkBoxHideOutputPaneNotIfOutput->IsChecked() );
    options->SetHideOutputPaneNotIfTrace( m_checkBoxHideOutputPaneNotIfTrace->IsChecked() );
    options->SetHideOutputPaneNotIfTasks( m_checkBoxHideOutputPaneNotIfTasks->IsChecked() );
    options->SetHideOutputPaneNotIfBuildQ( m_checkBoxHideOutputPaneNotIfBuildQ->IsChecked() );
    options->SetHideOutputPaneNotIfCppCheck( m_checkBoxHideOutputPaneNotIfCppCheck->IsChecked() );
    options->SetHideOutputPaneNotIfSvn( m_checkBoxHideOutputPaneNotIfSvn->IsChecked() );
    options->SetHideOutputPaneNotIfCscope( m_checkBoxHideOutputPaneNotIfCscope->IsChecked() );
    options->SetHideOutputPaneNotIfGit( m_checkBoxHideOutputPaneNotIfGit->IsChecked() );
    options->SetHideOutputPaneNotIfDebug( m_checkBoxHideOutputPaneNotIfDebug->IsChecked() );
    options->SetFindBarAtBottom( m_checkBoxFindBarAtBottom->IsChecked() );
    options->SetDontAutoFoldResults( m_checkBoxDontFoldSearchResults->IsChecked() );
    options->SetShowDebugOnRun( m_checkBoxShowDebugOnRun->IsChecked() );
    options->SetDockingStyle( m_radioBoxHint->GetSelection() );
    options->SetShowDockingWindowCaption( !m_checkBoxHideCaptions->IsChecked() );
    
    size_t flags(options->GetOptions());
    m_endFlags = 0;
    // set the tab control options:
    ////////////////////////////////////

    // Clear the current tab control style
    flags &= ~OptionsConfig::TabAll;

    switch(m_radioBoxTabControlStyle->GetSelection()) {
    case 0: // glossy
        flags |= OptionsConfig::TabGlossy;
        m_endFlags |= OptionsConfig::TabGlossy;
        break;
    case 1: // curved
    default:
        flags |= OptionsConfig::TabCurved;
        m_endFlags |= OptionsConfig::TabCurved;
        break;
    }
    options->SetOptions(flags);
}

void EditorSettingsDockingWindows::OnHideOutputPaneNotIfDebugUI(wxUpdateUIEvent& event)
{
    event.Enable( m_checkBoxHideOutputPaneOnClick->IsChecked() );
}

bool EditorSettingsDockingWindows::IsRestartRequired()
{
    return m_startingFlags != m_endFlags;
}
