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
#include "AddOptionsDialog.h"
#include "addoptioncheckdlg.h"
#include "build_settings_config.h"
#include "configuration_manager_dlg.h"
#include "debuggerconfigtool.h"
#include "debuggermanager.h"
#include "debuggersettings.h"
#include "dirsaver.h"
#include "editor_config.h"
#include "environmentconfig.h"
#include "event_notifier.h"
#include "free_text_dialog.h"
#include "globals.h"
#include "macros.h"
#include "macrosdlg.h"
#include "manager.h"
#include "plugin.h"
#include "pluginmanager.h"
#include "precompiled_header.h"
#include "project_settings_dlg.h"
#include "ps_build_events_page.h"
#include "ps_compiler_page.h"
#include "ps_completion_page.h"
#include "ps_custom_build_page.h"
#include "ps_custom_makefile_rules_page.h"
#include "ps_debugger_page.h"
#include "ps_environment_page.h"
#include "ps_general_page.h"
#include "ps_linker_page.h"
#include "ps_resources_page.h"
#include "windowattrmanager.h"
#include "workspacetab.h"

BEGIN_EVENT_TABLE(ProjectSettingsDlg, ProjectSettingsBaseDlg)
END_EVENT_TABLE()

ProjectSettingsDlg::ProjectSettingsDlg(wxWindow* parent, WorkspaceTab* workspaceTab, const wxString& configName,
                                       const wxString& projectName, const wxString& title)
#ifdef __WXMAC__
    : ProjectSettingsBaseDlg(parent, wxID_ANY, title, wxDefaultPosition, wxDefaultSize,
                             wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
#else
    : ProjectSettingsBaseDlg(parent, wxID_ANY, title)
#endif
    , m_projectName(projectName)
    , m_configName(configName)
    , m_isDirty(false)
    , m_isCustomBuild(false)
    , m_isProjectEnabled(true)
    , m_workspaceTab(workspaceTab)
{
    DoGetAllBuildConfigs();
    MSWSetNativeTheme(m_treebook->GetTreeCtrl());
    BuildTree();
    LoadValues(m_configName);

    m_treebook->SetFocus();

    CentreOnParent();
    SetName("ProjectSettingsDlg");

    EventNotifier::Get()->Connect(wxEVT_PROJECT_TREEITEM_CLICKED,
                                  wxCommandEventHandler(ProjectSettingsDlg::OnProjectSelected), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_WORKSPACE_CLOSED, wxCommandEventHandler(ProjectSettingsDlg::OnWorkspaceClosed),
                                  NULL, this);

    // No effects plz
    m_infobar->SetShowHideEffects(wxSHOW_EFFECT_NONE, wxSHOW_EFFECT_NONE);
    ShowHideDisabledMessage();
    ::clSetDialogBestSizeAndPosition(this);
}

void ProjectSettingsDlg::DoClearDialog()
{
    m_choiceConfig->Clear();
    ClearValues();
}

void ProjectSettingsDlg::BuildTree()
{
    PSGeneralPage* gp = new PSGeneralPage(m_treebook, m_projectName, m_choiceConfig->GetStringSelection(), this);
    m_treebook->AddPage(gp, _("General"), true);
    m_treebook->AddPage(new PSCompilerPage(m_treebook, m_projectName, this, gp), _("Compiler"));
    m_treebook->AddPage(new PSLinkerPage(m_treebook, this, gp), _("Linker"));
    m_treebook->AddPage(new PSEnvironmentPage(m_treebook, this), _("Environment"));
    m_treebook->AddPage(new PSDebuggerPage(m_treebook, this), _("Debugger"));
    m_treebook->AddPage(new PSResourcesPage(m_treebook, this), _("Resources"));

    m_treebook->AddPage(0, _("Pre / Post Build Commands"));
    m_treebook->AddSubPage(new PSBuildEventsPage(m_treebook, true, this), _("Pre Build"));
    m_treebook->AddSubPage(new PSBuildEventsPage(m_treebook, false, this), _("Post Build"));

    m_treebook->AddPage(0, _("Customize"));
    m_treebook->AddSubPage(new PSCustomBuildPage(m_treebook, m_projectName, this), _("Custom Build"));
    m_treebook->AddSubPage(new PSCustomMakefileRulesPage(m_treebook, this), _("Custom Makefile Rules"));

    m_treebook->AddPage(new PSCompletionPage(m_treebook, this), _("Code Completion"));
    m_treebook->AddPage(new GlobalSettingsPanel(m_treebook, m_projectName, this, gp), _("Global Settings"));
}

ProjectSettingsDlg::~ProjectSettingsDlg()
{
    m_workspaceTab->ProjectSettingsDlgClosed();

    EventNotifier::Get()->Disconnect(wxEVT_PROJECT_TREEITEM_CLICKED,
                                     wxCommandEventHandler(ProjectSettingsDlg::OnProjectSelected), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_WORKSPACE_CLOSED,
                                     wxCommandEventHandler(ProjectSettingsDlg::OnWorkspaceClosed), NULL, this);

    PluginManager::Get()->UnHookProjectSettingsTab(m_treebook, m_projectName, wxEmptyString /* all tabs */);
}

void ProjectSettingsDlg::OnButtonOK(wxCommandEvent& event)
{
    event.Skip();
    SaveValues();

    // Clear the project include paths cache
    clCxxWorkspaceST::Get()->ClearIncludePathCache();
    ManagerST::Get()->CallAfter(&Manager::UpdateParserPaths, true);

#ifndef __WXMAC__
    Destroy();
#endif
}

void ProjectSettingsDlg::OnButtonApply(wxCommandEvent& event)
{
    wxUnusedVar(event);
    SaveValues();

    // Clear the project include paths cache
    clCxxWorkspaceST::Get()->ClearIncludePathCache();
    ManagerST::Get()->CallAfter(&Manager::UpdateParserPaths, false);
    // Clear the dirty flag to (this disables the "Apply" button in the UI)
    SetIsDirty(false);
}

void ProjectSettingsDlg::SaveValues()
{
    ProjectSettingsPtr projSettingsPtr = ManagerST::Get()->GetProjectSettings(m_projectName);
    wxCHECK_RET(projSettingsPtr, "Project settings not found");
    BuildConfigPtr buildConf = projSettingsPtr->GetBuildConfiguration(m_configName);
    if(!buildConf) { return; }

    size_t pageCount = m_treebook->GetPageCount();
    for(size_t i = 0; i < pageCount; i++) {
        wxWindow* page = m_treebook->GetPage(i);
        if(!page) continue;
        IProjectSettingsPage* p = dynamic_cast<IProjectSettingsPage*>(page);
        if(p) { p->Save(buildConf, projSettingsPtr); }
    }

    // save settings
    ManagerST::Get()->SetProjectSettings(m_projectName, projSettingsPtr);

    // Notify the plugins to save their data
    clProjectSettingsEvent eventProjSettingsSaved(wxEVT_CMD_PROJ_SETTINGS_SAVED);
    eventProjSettingsSaved.SetProjectName(m_projectName);
    eventProjSettingsSaved.SetConfigName(m_configName);
    EventNotifier::Get()->AddPendingEvent(eventProjSettingsSaved);
}

void ProjectSettingsDlg::LoadValues(const wxString& configName)
{
    int sel = m_treebook->GetSelection();
    // Load the new tab for the new configuration
    PluginManager::Get()->HookProjectSettingsTab(m_treebook, m_projectName, configName);
    BuildConfigPtr buildConf;
    ProjectSettingsPtr projSettingsPtr = ManagerST::Get()->GetProjectSettings(m_projectName);
    wxCHECK_RET(projSettingsPtr, "Project settings not found");
    buildConf = projSettingsPtr->GetBuildConfiguration(configName);
    if(!buildConf) { return; }

    if(buildConf->GetProjectType().IsEmpty()) {
        // no project type is defined by the
        buildConf->SetProjectType(projSettingsPtr->GetProjectType(wxEmptyString));
    }
    size_t pageCount = m_treebook->GetPageCount();
    for(size_t i = 0; i < pageCount; i++) {
        wxWindow* page = m_treebook->GetPage(i);
        if(!page) continue; // NULL page ...
        IProjectSettingsPage* p = dynamic_cast<IProjectSettingsPage*>(page);
        if(p) {
            GlobalSettingsPanel* globalPage = dynamic_cast<GlobalSettingsPanel*>(page);
            if(globalPage) {
                // update the project name
                // globalPage->SetProjectName(m_projectName);
            }
            p->Load(buildConf);
        }
    }

    if(sel != wxNOT_FOUND) { m_treebook->SetSelection(sel); }
    m_isDirty = false;
}

void ProjectSettingsDlg::ClearValues()
{
    size_t pageCount = m_treebook->GetPageCount();
    for(size_t i = 0; i < pageCount; i++) {
        wxWindow* page = m_treebook->GetPage(i);
        if(!page) continue;

        IProjectSettingsPage* p = dynamic_cast<IProjectSettingsPage*>(page);
        if(p) { p->Clear(); }
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

void ProjectSettingsDlg::OnButtonApplyUI(wxUpdateUIEvent& event) { event.Enable(GetIsDirty()); }

void ProjectSettingsDlg::OnConfigurationChanged(wxCommandEvent& event)
{
    event.Skip();
    if(m_isDirty) {
        if(wxMessageBox(_("Save changes before loading new configuration?"), _("Save Changes"),
                        wxICON_QUESTION | wxYES_NO | wxCENTER) == wxYES) {
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

    BuildConfigPtr bldConf = clCxxWorkspaceST::Get()->GetProjBuildConf(e.GetString(), "");
    CHECK_PTR_RET(bldConf);

    if(m_isDirty) {
        int answer = ::wxMessageBox(_("Save changes before loading new configuration?"), _("Save Changes"),
                                    wxICON_QUESTION | wxYES_NO | wxCANCEL | wxCENTER);
        switch(answer) {
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
    m_configName = bldConf->GetName();

    // determine the correct configuration to load
    SetTitle(wxString() << m_projectName << " Project Settings");

    DoGetAllBuildConfigs();
    LoadValues(m_configName);
}

void ProjectSettingsDlg::DoGetAllBuildConfigs()
{
    m_choiceConfig->Clear();
    wxArrayString configs;
    ProjectSettingsPtr projSettingsPtr = ManagerST::Get()->GetProjectSettings(m_projectName);
    wxCHECK_RET(projSettingsPtr, "Project settings not found");
    ProjectSettingsCookie cookie;
    BuildConfigPtr conf = projSettingsPtr->GetFirstBuildConfiguration(cookie);
    while(conf) {
        configs.Add(conf->GetName());
        conf = projSettingsPtr->GetNextBuildConfiguration(cookie);
    }

    m_choiceConfig->Append(configs);
    int where = m_choiceConfig->FindString(m_configName);
    if(where != wxNOT_FOUND) {

        m_choiceConfig->SetSelection(where);

    } else if(!m_choiceConfig->IsEmpty()) {

        m_configName = m_choiceConfig->GetString(0);
        m_choiceConfig->SetSelection(0);

    } else {
        m_configName.clear();
    }
}

void ProjectSettingsDlg::OnWorkspaceClosed(wxCommandEvent& e)
{
    e.Skip();
#ifndef __WXMAC__
    Destroy();
#endif
}

void ProjectSettingsDlg::OnButtonCancel(wxCommandEvent& event)
{
    event.Skip();
#ifndef __WXMAC__
    Destroy();
#endif
}

void ProjectSettingsDlg::ShowHideDisabledMessage()
{
    if(!m_isProjectEnabled) {
        m_infobar->ShowMessage(_("This project is disabled"), wxICON_WARNING);
    } else {
        m_infobar->Dismiss();
    }
}

void ProjectSettingsDlg::ShowCustomProjectMessage(bool show)
{
    if(show) {
        m_infobar->ShowMessage(_("The settings on this page are ignored during build"), wxICON_INFORMATION);
    } else {
        m_infobar->Dismiss();
    }
}

void ProjectSettingsDlg::OnPageChanged(wxTreebookEvent& event)
{
    event.Skip();

    // Do nothing if the project is disabled
    if(!IsProjectEnabled()) return;

    int sel = m_treebook->GetSelection();
    if(sel != wxNOT_FOUND && IsCustomBuildEnabled()) {
        wxWindow* page = m_treebook->GetPage(sel);

        if(!page || dynamic_cast<PSCustomBuildPage*>(page) || dynamic_cast<PSGeneralPage*>(page) ||
           dynamic_cast<PSBuildEventsPage*>(page) || dynamic_cast<PSEnvironmentPage*>(page) ||
           dynamic_cast<PSDebuggerPage*>(page) || dynamic_cast<PSCompletionPage*>(page)) {
            ShowCustomProjectMessage(false);

        } else {
            ShowCustomProjectMessage(true);
        }
    }
}

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

GlobalSettingsPanel::GlobalSettingsPanel(wxWindow* parent, const wxString& projectName, ProjectSettingsDlg* dlg,
                                         PSGeneralPage* gp)
    : GlobalSettingsBasePanel(parent)
    , m_projectName(projectName)
    , m_dlg(dlg)
    , m_gp(gp)
{
    GetSizer()->Fit(this);
    Centre();
    ::wxPGPropertyBooleanUseCheckbox(m_pgMgr->GetGrid());
}

void GlobalSettingsPanel::Clear()
{
    wxPropertyGridIterator iter = m_pgMgr->GetGrid()->GetIterator();
    for(; !iter.AtEnd(); ++iter) {
        if(iter.GetProperty() && !iter.GetProperty()->IsCategory()) { iter.GetProperty()->SetValueToUnspecified(); }
    }
}

void GlobalSettingsPanel::Load(BuildConfigPtr buildConf)
{
    ProjectSettingsPtr projSettingsPtr = ManagerST::Get()->GetProjectSettings(m_projectName);
    wxCHECK_RET(projSettingsPtr, "Project settings not found");
    BuildConfigCommonPtr globalSettings = projSettingsPtr->GetGlobalSettings();
    if(!globalSettings) {
        Clear();
        return;
    }

    m_pgPropCCmpOpts->SetValue(globalSettings->GetCCompileOptions());
    m_pgPropCppCmpOpts->SetValue(globalSettings->GetCompileOptions());
    m_pgPropPreProcessors->SetValue(globalSettings->GetPreprocessor());
    m_pgPropIncludePaths->SetValue(globalSettings->GetIncludePath());
    m_pgPropOptions->SetValue(globalSettings->GetLinkOptions());
    m_pgPropLIbs->SetValue(globalSettings->GetLibraries());
    m_pgPropLibPath->SetValue(globalSettings->GetLibPath());
    m_pgPropResCmpOptions->SetValue(globalSettings->GetResCompileOptions());
    m_pgPropResCmpSearchPath->SetValue(globalSettings->GetResCmpIncludePath());
}

void GlobalSettingsPanel::Save(BuildConfigPtr buildConf, ProjectSettingsPtr projSettingsPtr)
{
    wxUnusedVar(buildConf);
    wxUnusedVar(projSettingsPtr);

    BuildConfigCommonPtr globalSettings = projSettingsPtr->GetGlobalSettings();
    if(!globalSettings) { return; }

    globalSettings->SetCompileOptions(m_pgPropCppCmpOpts->GetValueAsString());
    globalSettings->SetCCompileOptions(m_pgPropCCmpOpts->GetValueAsString());
    globalSettings->SetIncludePath(m_pgPropIncludePaths->GetValueAsString());
    globalSettings->SetPreprocessor(m_pgPropPreProcessors->GetValueAsString());

    globalSettings->SetLibPath(m_pgPropLibPath->GetValueAsString());
    globalSettings->SetLibraries(m_pgPropLIbs->GetValueAsString());
    globalSettings->SetLinkOptions(m_pgPropOptions->GetValueAsString());

    globalSettings->SetResCmpIncludePath(m_pgPropResCmpSearchPath->GetValueAsString());
    globalSettings->SetResCmpOptions(m_pgPropResCmpOptions->GetValueAsString());

    // save settings
    ManagerST::Get()->SetProjectGlobalSettings(m_projectName, globalSettings);
}

void GlobalSettingsPanel::OnCustomEditorClicked(wxCommandEvent& event)
{
    m_dlg->SetIsDirty(true);
    wxPGProperty* prop = m_pgMgr->GetSelectedProperty();
    CHECK_PTR_RET(prop);

    wxString cmpName = m_gp->GetCompiler();
    CompilerPtr cmp = BuildSettingsConfigST::Get()->GetCompiler(cmpName);

    if(prop == m_pgPropCCmpOpts || prop == m_pgPropCppCmpOpts) {
        wxString v = prop->GetValueAsString();
        if(PopupAddOptionCheckDlg(v, _("Compiler Options"), cmp->GetCompilerOptions())) { prop->SetValueFromString(v); }
    } else if(prop == m_pgPropIncludePaths || prop == m_pgPropPreProcessors || prop == m_pgPropLibPath ||
              prop == m_pgPropLIbs || prop == m_pgPropResCmpOptions || prop == m_pgPropResCmpSearchPath) {
        wxString v = prop->GetValueAsString();
        if(PopupAddOptionDlg(v)) { prop->SetValueFromString(v); }
    } else if(prop == m_pgPropOptions) {
        wxString v = prop->GetValueAsString();
        if(PopupAddOptionCheckDlg(v, _("Linker Options"), cmp->GetLinkerOptions())) { prop->SetValueFromString(v); }
    }
}

void GlobalSettingsPanel::OnValueChanged(wxPropertyGridEvent& event) { m_dlg->SetIsDirty(true); }

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

bool IProjectSettingsPage::PopupAddOptionDlg(wxTextCtrl* ctrl)
{
    AddOptionsDialog dlg(EventNotifier::Get()->TopFrame(), ctrl->GetValue());
    if(dlg.ShowModal() == wxID_OK) {
        ctrl->SetValue(dlg.GetValue());
        return true;
    }
    return false;
}

bool IProjectSettingsPage::PopupAddOptionDlg(wxString& value)
{
    AddOptionsDialog dlg(EventNotifier::Get()->TopFrame(), value);
    if(dlg.ShowModal() == wxID_OK) {
        value.Clear();
        value << dlg.GetValue();
        return true;
    }
    return false;
}

bool IProjectSettingsPage::SelectChoiceWithGlobalSettings(wxChoice* c, const wxString& text)
{
    if(text == BuildConfig::APPEND_TO_GLOBAL_SETTINGS) {
        c->Select(c->FindString(BuildConfig::APPEND_TO_GLOBAL_SETTINGS));

    } else if(text == BuildConfig::OVERWRITE_GLOBAL_SETTINGS) {
        c->Select(c->FindString(BuildConfig::OVERWRITE_GLOBAL_SETTINGS));

    } else if(text == BuildConfig::PREPEND_GLOBAL_SETTINGS) {
        c->Select(c->FindString(BuildConfig::PREPEND_GLOBAL_SETTINGS));

    } else {
        c->Select(c->FindString(BuildConfig::APPEND_TO_GLOBAL_SETTINGS));
        return false;
    }
    return true;
}

bool IProjectSettingsPage::PopupAddOptionCheckDlg(wxTextCtrl* ctrl, const wxString& title,
                                                  const Compiler::CmpCmdLineOptions& options)
{
    AddOptionCheckDlg dlg(NULL, title, options, ctrl->GetValue());
    if(dlg.ShowModal() == wxID_OK) {
        ctrl->SetValue(dlg.GetValue());
        return true;
    }
    return false;
}

bool IProjectSettingsPage::PopupAddOptionCheckDlg(wxString& v, const wxString& title,
                                                  const Compiler::CmpCmdLineOptions& options)
{
    AddOptionCheckDlg dlg(NULL, title, options, v);
    if(dlg.ShowModal() == wxID_OK) {
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
