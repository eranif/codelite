//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : project_settings_dlg.cpp
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
#include "precompiled_header.h"
#include "dirsaver.h"
#include "ps_custom_makefile_rules_page.h"
#include "ps_custom_build_page.h"
#include "ps_build_events_page.h"
#include "ps_environment_page.h"
#include "ps_resources_page.h"
#include "ps_debugger_page.h"
#include "ps_linker_page.h"
#include "ps_compiler_page.h"
#include "ps_completion_page.h"
#include "pluginmanager.h"
#include "windowattrmanager.h"
#include "debuggerconfigtool.h"
#include "project_settings_dlg.h"
#include "debuggersettings.h"
#include "globals.h"
#include "environmentconfig.h"
#include "macrosdlg.h"
#include "add_option_dialog.h"
#include "free_text_dialog.h"
#include "manager.h"
#include "configuration_manager_dlg.h"
#include "macros.h"
#include "editor_config.h"
#include "build_settings_config.h"
#include "debuggermanager.h"
#include "addoptioncheckdlg.h"
#include "ps_general_page.h"
#include "plugin.h"
#include "event_notifier.h"
#include "workspacetab.h"

BEGIN_EVENT_TABLE(ProjectSettingsDlg, ProjectSettingsBaseDlg)
END_EVENT_TABLE()

ProjectSettingsDlg::ProjectSettingsDlg( wxWindow* parent, WorkspaceTab* workspaceTab, const wxString &configName, const wxString &projectName, const wxString &title )
    : ProjectSettingsBaseDlg( parent, wxID_ANY, title, wxDefaultPosition, wxSize( 782,502 ), wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER )
    , m_projectName(projectName)
    , m_configName(configName)
    , m_isDirty(false)
    , m_isCustomBuild(false)
    , m_isProjectEnabled(true)
    , m_workspaceTab(workspaceTab)
{
    DoGetAllBuildConfigs();
    MSWSetNativeTheme( m_treebook->GetTreeCtrl() );
    BuildTree();
    LoadValues(m_configName);

    m_treebook->SetFocus();
    GetSizer()->Fit(this);

    wxSize sz = GetSize();
    Centre();
    WindowAttrManager::Load(this, wxT("ProjectSettingsDlg"), NULL);

    // Make sure that all the controls are visible
    wxSize newSize = GetSize();
    if(newSize.x <= sz.x && newSize.y <= sz.y) {
        GetSizer()->Fit(this);
    }

    EventNotifier::Get()->Connect(wxEVT_PROJECT_TREEITEM_CLICKED, wxCommandEventHandler(ProjectSettingsDlg::OnProjectSelected), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_WORKSPACE_CLOSED, wxCommandEventHandler(ProjectSettingsDlg::OnWorkspaceClosed), NULL, this);
}

void ProjectSettingsDlg::DoClearDialog()
{
    m_choiceConfig->Clear();
    ClearValues();
}

void ProjectSettingsDlg::BuildTree()
{
    wxString selectedPage = EditorConfigST::Get()->GetStringValue(wxT("PSSelectedPage"));
    if(selectedPage.IsEmpty()) {
        selectedPage = _("General");
    }

    PSGeneralPage *gp = new PSGeneralPage(m_treebook, m_projectName, m_choiceConfig->GetStringSelection(), this);
    m_treebook->AddPage(0, _("Common Settings"));
    m_treebook->AddSubPage(gp,                                                       _("General"),               selectedPage == _("General"));
    m_treebook->AddSubPage(new PSCompilerPage(m_treebook, m_projectName, this, gp),  _("Compiler"),              selectedPage == _("Compiler"));
    m_treebook->AddSubPage(new PSLinkerPage(m_treebook, this, gp),                   _("Linker"),                selectedPage == _("Linker"));
    m_treebook->AddSubPage(new PSEnvironmentPage(m_treebook, this),                  _("Environment"),           selectedPage == _("Environment"));
    m_treebook->AddSubPage(new PSDebuggerPage(m_treebook, this),                     _("Debugger"),              selectedPage == _("Debugger"));
    m_treebook->AddSubPage(new PSResourcesPage(m_treebook, this),                    _("Resources"),             selectedPage == _("Resources"));

    m_treebook->AddPage(0, _("Pre / Post Build Commands"));
    m_treebook->AddSubPage(new PSBuildEventsPage(m_treebook, true, this),            _("Pre Build"),             selectedPage == _("Pre Build"));
    m_treebook->AddSubPage(new PSBuildEventsPage(m_treebook, false, this),           _("Post Build"),            selectedPage == _("Post Build"));

    m_treebook->AddPage(0, _("Customize"));
    m_treebook->AddSubPage(new PSCustomBuildPage(m_treebook, m_projectName, this),   _("Custom Build"),          selectedPage == _("Custom Build"));
    m_treebook->AddSubPage(new PSCustomMakefileRulesPage(m_treebook, this),          _("Custom Makefile Rules"), selectedPage == _("Custom Makefile Rules"));

    m_treebook->AddPage(new PSCompletionPage(m_treebook, this), _("Code Completion"), selectedPage == _("Code Completion"));
    m_treebook->AddPage(new GlobalSettingsPanel(m_treebook, m_projectName, this, gp), _("Global Settings"),      selectedPage == _("Global Settings"));
}

ProjectSettingsDlg::~ProjectSettingsDlg()
{
    m_workspaceTab->ProjectSettingsDlgClosed();
    
    EventNotifier::Get()->Disconnect(wxEVT_PROJECT_TREEITEM_CLICKED, wxCommandEventHandler(ProjectSettingsDlg::OnProjectSelected), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_WORKSPACE_CLOSED, wxCommandEventHandler(ProjectSettingsDlg::OnWorkspaceClosed), NULL, this);

    PluginManager::Get()->UnHookProjectSettingsTab(m_treebook, m_projectName, wxEmptyString /* all tabs */);
    EditorConfigST::Get()->SaveStringValue(wxT("PSSelectedPage"), m_treebook->GetPageText(m_treebook->GetSelection()));
    WindowAttrManager::Save(this, wxT("ProjectSettingsDlg"), NULL);
}

void ProjectSettingsDlg::OnButtonOK(wxCommandEvent &event)
{
    event.Skip();
    OnButtonApply(event);
    ManagerST::Get()->UpdateParserPaths(true);
    Destroy();
}

void ProjectSettingsDlg::OnButtonApply(wxCommandEvent &event)
{
    wxUnusedVar(event);
    SaveValues();
    ManagerST::Get()->UpdateParserPaths();
    SetIsDirty(false);
}

void ProjectSettingsDlg::SaveValues()
{

    ProjectSettingsPtr projSettingsPtr = ManagerST::Get()->GetProjectSettings(m_projectName);
    BuildConfigPtr buildConf = projSettingsPtr->GetBuildConfiguration(m_configName);
    if (!buildConf) {
        return;
    }

    size_t pageCount = m_treebook->GetPageCount();
    for(size_t i=0; i<pageCount; i++) {
        wxWindow *page = m_treebook->GetPage(i);
        if(!page) continue;
        IProjectSettingsPage *p = dynamic_cast<IProjectSettingsPage*>(page);
        if(p) {
            p->Save(buildConf, projSettingsPtr);
        }
    }

    //save settings
    ManagerST::Get()->SetProjectSettings(m_projectName, projSettingsPtr);

    // Notify the plugins to save their data
    SendCmdEvent(wxEVT_CMD_PROJ_SETTINGS_SAVED, (void*)&m_projectName, m_configName);
}

void ProjectSettingsDlg::LoadValues(const wxString& configName)
{
    int sel = m_treebook->GetSelection();
    // Load the new tab for the new configuration
    PluginManager::Get()->HookProjectSettingsTab(m_treebook, m_projectName, configName);
    BuildConfigPtr buildConf;
    ProjectSettingsPtr projSettingsPtr = ManagerST::Get()->GetProjectSettings(m_projectName);
    buildConf = projSettingsPtr->GetBuildConfiguration(configName);
    if (!buildConf) {
        return;
    }
    size_t pageCount = m_treebook->GetPageCount();
    for(size_t i=0; i<pageCount; i++) {
        wxWindow *page = m_treebook->GetPage(i);
        if (!page)                                       continue; // NULL page ...
        IProjectSettingsPage *p = dynamic_cast<IProjectSettingsPage*>(page);
        if(p) {
            p->Load(buildConf);
        }
    }

    if( sel != wxNOT_FOUND ) {
        m_treebook->SetSelection( sel );
    }
    m_isDirty = false;
}

void ProjectSettingsDlg::ClearValues()
{
    size_t pageCount = m_treebook->GetPageCount();
    for(size_t i=0; i<pageCount; i++) {
        wxWindow *page = m_treebook->GetPage(i);
        if(!page) continue;

        IProjectSettingsPage *p = dynamic_cast<IProjectSettingsPage*>(page);
        if(p) {
            p->Clear();
        }
    }
}

void ProjectSettingsDlg::OnButtonHelp(wxCommandEvent& e)
{
    wxUnusedVar(e);

    ProjectPtr project = ManagerST::Get()->GetProject(m_projectName);
    IEditor* editor = PluginManager::Get()->GetActiveEditor();

    MacrosDlg dlg(this, MacrosDlg::MacrosProject, project, editor);
    dlg.ShowModal();
#ifdef __WXMAC__
    Raise();
#endif
}

void ProjectSettingsDlg::OnButtonApplyUI(wxUpdateUIEvent& event)
{
    event.Enable( GetIsDirty() );
}

void ProjectSettingsDlg::OnConfigurationChanged(wxCommandEvent& event)
{
    event.Skip();
    if(m_isDirty) {
        if(wxMessageBox(_("Save changes before loading new configuration?"), _("Save Changes"), wxICON_QUESTION|wxYES_NO|wxCENTER) == wxYES) {
            SaveValues();
        } else {
            ClearValues();
        }
    }

    m_configName = event.GetString();

    clWindowUpdateLocker locker(this);
    LoadValues(m_configName);

    m_treebook->SetFocus();
}

void ProjectSettingsDlg::OnProjectSelected(wxCommandEvent& e)
{
    e.Skip();
    
    // Make sure we know which configuration to load for the new project
    
    BuildConfigPtr bldConf = WorkspaceST::Get()->GetProjBuildConf(e.GetString(), "");
    CHECK_PTR_RET(bldConf);
    
    if(m_isDirty) {
        int answer = ::wxMessageBox(_("Save changes before loading new configuration?"), _("Save Changes"), wxICON_QUESTION|wxYES_NO|wxCANCEL|wxCENTER);
        switch ( answer ) {
        case wxYES:
            SaveValues();
            break;
        case wxNO:
            break;
        default:
            // abort
            return;
        }
    }
    
    ClearValues();
    
    // another project was selected in the tree view
    m_projectName = e.GetString();
    m_configName  = bldConf->GetName();
    
    // determine the correct configuration to load
    SetTitle( wxString() << m_projectName << " Project Settings" );
    
    DoGetAllBuildConfigs();
    LoadValues(m_configName);
}

void ProjectSettingsDlg::DoGetAllBuildConfigs()
{
    m_choiceConfig->Clear();
    wxArrayString configs;
    ProjectSettingsPtr projSettingsPtr = ManagerST::Get()->GetProjectSettings(m_projectName);
    ProjectSettingsCookie cookie;
    BuildConfigPtr conf = projSettingsPtr->GetFirstBuildConfiguration(cookie);
    while (conf) {
        configs.Add(conf->GetName());
        conf = projSettingsPtr->GetNextBuildConfiguration(cookie);
    }

    m_choiceConfig->Append(configs);
    int where = m_choiceConfig->FindString(m_configName);
    if(where != wxNOT_FOUND) {

        m_choiceConfig->SetSelection(where);

    } else if ( !m_choiceConfig->IsEmpty() ) {

        m_configName = m_choiceConfig->GetString(0);
        m_choiceConfig->SetSelection(0);

    } else {
        m_configName.clear();
    }
}

void ProjectSettingsDlg::OnWorkspaceClosed(wxCommandEvent& e)
{
    e.Skip();
    Destroy();
}

void ProjectSettingsDlg::OnButtonCancel(wxCommandEvent& event)
{
    event.Skip();
    Destroy();
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

GlobalSettingsPanel::GlobalSettingsPanel(wxWindow* parent, const wxString &projectName, ProjectSettingsDlg *dlg, PSGeneralPage* gp)
    : GlobalSettingsBasePanel(parent)
    , m_projectName(projectName)
    , m_dlg(dlg)
    , m_gp(gp)
{
    GetSizer()->Fit(this);
    Centre();
}

void GlobalSettingsPanel::OnButtonAddCompilerOptions(wxCommandEvent &event)
{
    wxString cmpName = m_gp->GetCompiler();
    CompilerPtr cmp = BuildSettingsConfigST::Get()->GetCompiler(cmpName);
    if (PopupAddOptionCheckDlg(m_textCompilerOptions, _("Compiler options"), cmp->GetCompilerOptions())) {
        m_dlg->SetIsDirty(true);
    }
    event.Skip();
}

void GlobalSettingsPanel::OnButtonAddCCompilerOptions(wxCommandEvent& event)
{
    wxString cmpName = m_gp->GetCompiler();
    CompilerPtr cmp = BuildSettingsConfigST::Get()->GetCompiler(cmpName);
    if (PopupAddOptionCheckDlg(m_textCtrlCCompileOptions, _("C Compiler options"), cmp->GetCompilerOptions())) {
        m_dlg->SetIsDirty(true);
    }
    event.Skip();
}


void GlobalSettingsPanel::OnAddSearchPath(wxCommandEvent &event)
{
    if (PopupAddOptionDlg(m_textAdditionalSearchPath)) {
        m_dlg->SetIsDirty(true);
    }
    event.Skip();
}

void GlobalSettingsPanel::OnButtonAddPreprocessor(wxCommandEvent &event)
{
    if (PopupAddOptionDlg(m_textPreprocessor)) {
        m_dlg->SetIsDirty(true);
    }
    event.Skip();
}

void GlobalSettingsPanel::OnAddLibrary(wxCommandEvent &event)
{
    if (PopupAddOptionDlg(m_textLibraries)) {
        m_dlg->SetIsDirty(true);
    }
    event.Skip();
}

void GlobalSettingsPanel::OnAddLibraryPath(wxCommandEvent &event)
{
    if (PopupAddOptionDlg(m_textLibraryPath)) {
        m_dlg->SetIsDirty(true);
    }
    event.Skip();
}

void GlobalSettingsPanel::OnButtonAddLinkerOptions(wxCommandEvent &event)
{
    wxString cmpName = m_gp->GetCompiler();
    CompilerPtr cmp = BuildSettingsConfigST::Get()->GetCompiler(cmpName);
    if (PopupAddOptionCheckDlg(m_textLinkerOptions, _("Linker options"), cmp->GetLinkerOptions())) {
        m_dlg->SetIsDirty(true);
    }
    event.Skip();
}

void GlobalSettingsPanel::OnResourceCmpAddOption(wxCommandEvent &event)
{
    if (PopupAddOptionDlg(m_textAddResCmpOptions)) {
        m_dlg->SetIsDirty(true);
    }
    event.Skip();
}

void GlobalSettingsPanel::OnResourceCmpAddPath(wxCommandEvent &event)
{
    if (PopupAddOptionDlg(m_textAddResCmpPath)) {
        m_dlg->SetIsDirty(true);
    }
    event.Skip();
}

void GlobalSettingsPanel::OnCmdEvtVModified(wxCommandEvent& event)
{
    wxUnusedVar(event);
    m_dlg->SetIsDirty(true);
}

void GlobalSettingsPanel::Clear()
{
    m_textCompilerOptions->SetValue(wxEmptyString);
    m_textPreprocessor->SetValue(wxEmptyString);
    m_textAdditionalSearchPath->SetValue(wxEmptyString);

    m_textLinkerOptions->SetValue(wxEmptyString);
    m_textLibraries->SetValue(wxEmptyString);
    m_textLibraryPath->SetValue(wxEmptyString);

    m_textAddResCmpOptions->SetValue(wxEmptyString);
    m_textAddResCmpPath->SetValue(wxEmptyString);
}

void GlobalSettingsPanel::Load(BuildConfigPtr buildConf)
{
    ProjectSettingsPtr projSettingsPtr = ManagerST::Get()->GetProjectSettings(m_projectName);
    BuildConfigCommonPtr globalSettings = projSettingsPtr->GetGlobalSettings();
    if (!globalSettings) {
        Clear();
        return;
    }

    m_textCompilerOptions->SetValue(globalSettings->GetCompileOptions());
    m_textCtrlCCompileOptions->SetValue(globalSettings->GetCCompileOptions());
    m_textPreprocessor->SetValue(globalSettings->GetPreprocessor());
    m_textAdditionalSearchPath->SetValue(globalSettings->GetIncludePath());

    m_textLinkerOptions->SetValue(globalSettings->GetLinkOptions());
    m_textLibraries->SetValue(globalSettings->GetLibraries());
    m_textLibraryPath->SetValue(globalSettings->GetLibPath());

    m_textAddResCmpOptions->SetValue(globalSettings->GetResCompileOptions());
    m_textAddResCmpPath->SetValue(globalSettings->GetResCmpIncludePath());
}

void GlobalSettingsPanel::Save(BuildConfigPtr buildConf, ProjectSettingsPtr projSettingsPtr)
{
    wxUnusedVar(buildConf);
    wxUnusedVar(projSettingsPtr);

    BuildConfigCommonPtr globalSettings = projSettingsPtr->GetGlobalSettings();
    if (!globalSettings) {
        return;
    }

    globalSettings->SetCompileOptions(m_textCompilerOptions->GetValue());
    globalSettings->SetCCompileOptions(m_textCtrlCCompileOptions->GetValue());
    globalSettings->SetIncludePath(m_textAdditionalSearchPath->GetValue());
    globalSettings->SetPreprocessor(m_textPreprocessor->GetValue());

    globalSettings->SetLibPath(m_textLibraryPath->GetValue());
    globalSettings->SetLibraries(m_textLibraries->GetValue());
    globalSettings->SetLinkOptions(m_textLinkerOptions->GetValue());

    globalSettings->SetResCmpIncludePath(m_textAddResCmpPath->GetValue());
    globalSettings->SetResCmpOptions(m_textAddResCmpOptions->GetValue());

    //save settings
    ManagerST::Get()->SetProjectGlobalSettings(m_projectName, globalSettings);
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

bool IProjectSettingsPage::PopupAddOptionDlg(wxTextCtrl* ctrl)
{
    AddOptionDlg dlg(NULL, ctrl->GetValue());
    if (dlg.ShowModal() == wxID_OK) {
        ctrl->SetValue(dlg.GetValue());
        return true;
    }
    return false;
}

bool IProjectSettingsPage::PopupAddOptionDlg(wxString& value)
{
    AddOptionDlg dlg(NULL, value);
    if (dlg.ShowModal() == wxID_OK) {
        value.Clear();
        value << dlg.GetValue();
        return true;
    }
    return false;
}

bool IProjectSettingsPage::SelectChoiceWithGlobalSettings(wxChoice* c, const wxString& text)
{
    if (text == BuildConfig::APPEND_TO_GLOBAL_SETTINGS) {
        c->Select(c->FindString(BuildConfig::APPEND_TO_GLOBAL_SETTINGS));

    } else if (text == BuildConfig::OVERWRITE_GLOBAL_SETTINGS) {
        c->Select(c->FindString(BuildConfig::OVERWRITE_GLOBAL_SETTINGS));

    } else if (text == BuildConfig::PREPEND_GLOBAL_SETTINGS) {
        c->Select(c->FindString(BuildConfig::PREPEND_GLOBAL_SETTINGS));

    } else {
        c->Select(c->FindString(BuildConfig::APPEND_TO_GLOBAL_SETTINGS));
        return false;
    }
    return true;
}

bool IProjectSettingsPage::PopupAddOptionCheckDlg(wxTextCtrl *ctrl, const wxString& title, const Compiler::CmpCmdLineOptions& options)
{
    AddOptionCheckDlg dlg(NULL, title, options, ctrl->GetValue());
    if (dlg.ShowModal() == wxID_OK) {
        ctrl->SetValue(dlg.GetValue());
        return true;
    }
    return false;
}

bool IProjectSettingsPage::PopupAddOptionCheckDlg(wxString& v, const wxString& title, const Compiler::CmpCmdLineOptions& options)
{
    AddOptionCheckDlg dlg(NULL, title, options, v);
    if (dlg.ShowModal() == wxID_OK) {
        v = dlg.GetValue();
        return true;
    }
    return false;
}

void IProjectSettingsPage::SelectChoiceWithGlobalSettings(wxPGProperty* p, const wxString& text)
{
    wxPGChoices choices;
    choices.Add(BuildConfig::APPEND_TO_GLOBAL_SETTINGS);
    choices.Add(BuildConfig::OVERWRITE_GLOBAL_SETTINGS);
    choices.Add(BuildConfig::PREPEND_GLOBAL_SETTINGS);
    p->SetChoices(choices);
    p->SetChoiceSelection(choices.Index(text));
}
