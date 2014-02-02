
#include "ps_general_page.h"
#include "project_settings_dlg.h"
#include "build_settings_config.h"
#include "project.h"
#include "dirsaver.h"
#include "manager.h"

PSGeneralPage::PSGeneralPage( wxWindow* parent, const wxString &projectName, const wxString &conf, ProjectSettingsDlg *dlg )
    : PSGeneralPageBase( parent )
    , m_dlg(dlg)
    , m_projectName(projectName)
    , m_configName(conf)
{
}

void PSGeneralPage::OnProjectCustumBuildUI( wxUpdateUIEvent& event )
{
    // do not disable the General page even when in custom build
    event.Enable( m_checkBoxEnabled->IsChecked() );
}

void PSGeneralPage::Load(BuildConfigPtr buildConf)
{
    Clear();
    m_configName = buildConf->GetName();
    m_checkBoxEnabled->SetValue( buildConf->IsProjectEnabled() );
    m_pgPropArgs->SetValue( buildConf->GetCommandArguments() );
    m_pgPropDebugArgs->SetValueFromString( buildConf->GetDebugArgs() );
    m_pgPropIntermediateFolder->SetValueFromString( buildConf->GetIntermediateDirectory() );
    m_pgPropGUIApp->SetValue( buildConf->IsGUIProgram() );
    m_pgPropOutputFile->SetValueFromString( buildConf->GetOutputFileName() );
    m_pgPropPause->SetValue( buildConf->GetPauseWhenExecEnds() );
    m_pgPropProgram->SetValueFromString( buildConf->GetCommand() );
    m_pgPropWorkingDirectory->SetValue( buildConf->GetWorkingDirectory() );
    // Project type
    wxPGChoices choices;
    choices.Add(Project::STATIC_LIBRARY);
    choices.Add(Project::DYNAMIC_LIBRARY);
    choices.Add(Project::EXECUTABLE);
    m_pgPropProjectType->SetChoices( choices );
    m_pgPropProjectType->SetChoiceSelection( choices.Index(buildConf->GetProjectType() ) );

    // Compilers
    choices.Clear();
    wxString cmpType = buildConf->GetCompilerType();
    BuildSettingsConfigCookie cookie;
    CompilerPtr cmp = BuildSettingsConfigST::Get()->GetFirstCompiler(cookie);
    while (cmp) {
        choices.Add(cmp->GetName());
        cmp = BuildSettingsConfigST::Get()->GetNextCompiler(cookie);
    }
    m_pgPropCompiler->SetChoices( choices );
    m_pgPropCompiler->SetChoiceSelection( choices.Index( buildConf->GetCompiler()->GetName() ) );

    // Debuggers
    choices.Clear();
    wxString dbgType = buildConf->GetDebuggerType();
    wxArrayString dbgs = DebuggerMgr::Get().GetAvailableDebuggers();
    choices.Add(dbgs);
    m_pgPropDebugger->SetChoices( choices );
    m_pgPropDebugger->SetChoiceSelection( choices.Index(buildConf->GetDebuggerType()) );

    m_dlg->SetIsProjectEnabled( buildConf->IsProjectEnabled() );
}

void PSGeneralPage::Save(BuildConfigPtr buildConf, ProjectSettingsPtr projSettingsPtr)
{
    buildConf->SetOutputFileName( GetPropertyAsString(m_pgPropOutputFile) );
    buildConf->SetIntermediateDirectory( GetPropertyAsString(m_pgPropIntermediateFolder) );
    buildConf->SetCommand( GetPropertyAsString(m_pgPropProgram) );
    buildConf->SetCommandArguments(GetPropertyAsString(m_pgPropArgs));
    buildConf->SetWorkingDirectory(GetPropertyAsString(m_pgPropWorkingDirectory));

    // Get the project type selection, unlocalised
    projSettingsPtr->SetProjectType( GetPropertyAsString(m_pgPropProjectType) );
    buildConf->SetCompilerType( GetPropertyAsString(m_pgPropCompiler) );
    buildConf->SetDebuggerType( GetPropertyAsString(m_pgPropDebugger) );
    buildConf->SetPauseWhenExecEnds( GetPropertyAsBool(m_pgPropPause) );
    buildConf->SetProjectType( GetPropertyAsString(m_pgPropProjectType) );
    buildConf->SetDebugArgs( GetPropertyAsString(m_pgPropDebugArgs) );
    buildConf->SetIsGUIProgram( GetPropertyAsBool(m_pgPropGUIApp) );
    buildConf->SetIsProjectEnabled( m_checkBoxEnabled->IsChecked() );
}

void PSGeneralPage::Clear()
{
    wxPropertyGridIterator iter = m_pgMgr136->GetGrid()->GetIterator();
    for( ; !iter.AtEnd(); ++iter ) {
        if ( iter.GetProperty() && !iter.GetProperty()->IsCategory() ) {
            iter.GetProperty()->SetValueToUnspecified();
        }
    }
    m_checkBoxEnabled->SetValue(true);
}

void PSGeneralPage::OnValueChanged(wxPropertyGridEvent& event)
{
    event.Skip();
    m_dlg->SetIsDirty(true);
}

bool PSGeneralPage::GetPropertyAsBool(wxPGProperty* prop) const
{
    wxVariant v = prop->GetValue();
    bool b = v.GetBool();
    return b;
}

wxString PSGeneralPage::GetPropertyAsString(wxPGProperty* prop) const
{
    wxString s = prop->GetValueAsString();
    return s;
}

void PSGeneralPage::OnProjectEnabled(wxCommandEvent& event)
{
    m_dlg->SetIsProjectEnabled( event.IsChecked() );
    m_dlg->SetIsDirty(true);
}
