#include "ps_compiler_page.h"
#include "compiler.h"
#include "addoptioncheckdlg.h"
#include "project_settings_dlg.h"
#include "build_settings_config.h"
#include "project.h"
#include "ps_general_page.h"
#include "manager.h"

PSCompilerPage::PSCompilerPage( wxWindow* parent, const wxString &projectName, ProjectSettingsDlg *dlg, PSGeneralPage *gp )
    : PSCompilerPageBase( parent )
    , m_dlg(dlg)
    , m_projectName(projectName)
    , m_gp(gp)
{
}

void PSCompilerPage::Load(BuildConfigPtr buildConf)
{
    m_checkCompilerNeeded->SetValue(!buildConf->IsCompilerRequired());
    m_pgPropCppOpts->SetValueFromString(buildConf->GetCompileOptions());
    m_pgPropIncludePaths->SetValueFromString( buildConf->GetIncludePath() );
    m_pgPropPreProcessors->SetValueFromString( buildConf->GetPreprocessor() );
    m_pgPropPreCmpHeaderFile->SetValue( buildConf->GetPrecompiledHeader() );
    m_pgPropCOpts->SetValue(buildConf->GetCCompileOptions());
    SelectChoiceWithGlobalSettings(m_pgPropBehaviorWithGlobalSettings, buildConf->GetBuildCmpWithGlobalSettings());
    m_pgPropIncludePCH->SetValue( buildConf->GetPchInCommandLine() );
    m_pgPropPCHCompileLine->SetValue( buildConf->GetPchCompileFlags() );
    m_pgPropAssembler->SetValue(buildConf->GetAssmeblerOptions());
}

void PSCompilerPage::Save(BuildConfigPtr buildConf, ProjectSettingsPtr projSettingsPtr)
{
    buildConf->SetCompilerRequired(!m_checkCompilerNeeded->IsChecked());
    buildConf->SetCompileOptions(m_pgPropCppOpts->GetValueAsString());
    buildConf->SetIncludePath(m_pgPropIncludePaths->GetValueAsString());
    buildConf->SetPreprocessor(m_pgPropPreProcessors->GetValueAsString());
    buildConf->SetPrecompiledHeader(m_pgPropPreCmpHeaderFile->GetValueAsString());
    buildConf->SetCCompileOptions(m_pgPropCOpts->GetValueAsString());
    buildConf->SetPchInCommandLine(m_pgPropIncludePCH->GetValue().GetBool());
    buildConf->SetUseSeparatePCHFlags(!m_pgPropPCHCompileLine->GetValueAsString().IsEmpty());
    buildConf->SetPchCompileFlags(m_pgPropPCHCompileLine->GetValueAsString());
    buildConf->SetAssmeblerOptions(m_pgPropAssembler->GetValueAsString());
    buildConf->SetBuildCmpWithGlobalSettings( m_pgPropBehaviorWithGlobalSettings->GetValueAsString() );
}

void PSCompilerPage::Clear()
{
    wxPropertyGridIterator iter = m_pgMgr->GetGrid()->GetIterator();
    for( ; !iter.AtEnd(); ++iter ) {
        if ( iter.GetProperty() && !iter.GetProperty()->IsCategory() ) {
            iter.GetProperty()->SetValueToUnspecified();
        }
    }
    m_checkCompilerNeeded->SetValue(false);
}

void PSCompilerPage::OnProjectEnabledUI(wxUpdateUIEvent& event)
{
    event.Enable( m_dlg->IsProjectEnabled() );
}

void PSCompilerPage::OnPropertyChanged(wxPropertyGridEvent& event)
{
    event.Skip();
    m_dlg->SetIsDirty(true);
}

void PSCompilerPage::OnUpdateUI(wxUpdateUIEvent& event)
{
    event.Enable( !m_dlg->IsCustomBuildEnabled() && !m_checkCompilerNeeded->IsChecked());
}

void PSCompilerPage::OnCustomEditorClicked(wxCommandEvent& event)
{
    wxPGProperty* prop = m_pgMgr->GetSelectedProperty();
    CHECK_PTR_RET(prop);
    m_dlg->SetIsDirty(true);
    
    if ( prop == m_pgPropPreProcessors || prop == m_pgPropIncludePaths || prop == m_pgPropAssembler ) {
        wxString value = prop->GetValueAsString();
        if ( PopupAddOptionDlg(value) ) {
            prop->SetValueFromString( value );
        }

    } else if ( prop == m_pgPropCppOpts || prop == m_pgPropCOpts ) {
        wxString value = prop->GetValueAsString();
        wxString cmpName = m_gp->GetCompiler();
        CompilerPtr cmp = BuildSettingsConfigST::Get()->GetCompiler(cmpName);
        if (PopupAddOptionCheckDlg(value, _("Compiler options"), cmp->GetCompilerOptions())) {
            prop->SetValueFromString( value );
        }
    }
}

void PSCompilerPage::OnCompilerNeeded(wxCommandEvent& event)
{
    m_dlg->SetIsDirty(true);
}
