//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : qmakeplugin.cpp
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

#include "asyncprocess.h"
#include "build_settings_config.h"
#include "build_system.h"
#include "cl_standard_paths.h"
#include "dirsaver.h"
#include "environmentconfig.h"
#include "event_notifier.h"
#include "globals.h"
#include "macros.h"
#include "newqtprojdlg.h"
#include "processreaderthread.h"
#include "procutils.h"
#include "project.h"
#include "qmakeconf.h"
#include "qmakegenerator.h"
#include "qmakeplugin.h"
#include "qmakesettingsdlg.h"
#include "qmaketab.h"
#include "workspace.h"
#include <wx/app.h>
#include <wx/log.h>
#include <wx/menu.h>
#include <wx/mimetype.h>
#include <wx/msgdlg.h>
#include <wx/stdpaths.h>
#include <wx/xrc/xmlres.h>

static QMakePlugin* thePlugin = NULL;

// Define the plugin entry point
CL_PLUGIN_API IPlugin* CreatePlugin(IManager* manager)
{
    if(thePlugin == 0) {
        thePlugin = new QMakePlugin(manager);
    }
    return thePlugin;
}

CL_PLUGIN_API PluginInfo* GetPluginInfo()
{
    static PluginInfo info;
    info.SetAuthor(wxT("Eran Ifrah"));
    info.SetName(wxT("QMakePlugin"));
    info.SetDescription(_("Qt's QMake integration with CodeLite"));
    info.SetVersion(wxT("v1.0"));
    return &info;
}

CL_PLUGIN_API int GetPluginInterfaceVersion() { return PLUGIN_INTERFACE_VERSION; }

QMakePlugin::QMakePlugin(IManager* manager)
    : IPlugin(manager)
    , m_qmakeProcess(NULL)
{
    m_longName = _("Qt's QMake integration with CodeLite");
    m_shortName = wxT("QMakePlugin");

    m_conf = new QmakeConf(clStandardPaths::Get().GetUserDataDir() + wxFileName::GetPathSeparator() +
                           wxT("config/qmake.ini"));
    Bind(wxEVT_ASYNC_PROCESS_OUTPUT, &QMakePlugin::OnQmakeOutput, this);
    Bind(wxEVT_ASYNC_PROCESS_TERMINATED, &QMakePlugin::OnQmakeTerminated, this);
    // Connect items
    EventNotifier::Get()->Connect(wxEVT_CMD_PROJ_SETTINGS_SAVED,
                                  clProjectSettingsEventHandler(QMakePlugin::OnSaveConfig), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_BUILD_STARTING, clBuildEventHandler(QMakePlugin::OnBuildStarting), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_GET_PROJECT_BUILD_CMD, clBuildEventHandler(QMakePlugin::OnGetBuildCommand),
                                  NULL, this);
    EventNotifier::Get()->Connect(wxEVT_GET_PROJECT_CLEAN_CMD, clBuildEventHandler(QMakePlugin::OnGetCleanCommand),
                                  NULL, this);
    EventNotifier::Get()->Connect(wxEVT_GET_IS_PLUGIN_MAKEFILE, clBuildEventHandler(QMakePlugin::OnGetIsPluginMakefile),
                                  NULL, this);
    EventNotifier::Get()->Connect(wxEVT_TREE_ITEM_FILE_ACTIVATED, clCommandEventHandler(QMakePlugin::OnOpenFile), NULL,
                                  this);
}

QMakePlugin::~QMakePlugin() { delete m_conf; }

void QMakePlugin::CreateToolBar(clToolBar* toolbar)
{
    // Support both toolbars icon size
    int size = m_mgr->GetToolbarIconSize();
    auto images = toolbar->GetBitmapsCreateIfNeeded();
    toolbar->AddTool(XRCID("new_qmake_project"), _("Create new qmake based project"), images->Add("qt", size),
                     _("Create new qmake based project"));
}

void QMakePlugin::CreatePluginMenu(wxMenu* pluginsMenu)
{
    // You can use the below code a snippet:
    wxMenu* menu = new wxMenu();
    wxMenuItem* item(NULL);
    item =
        new wxMenuItem(menu, XRCID("new_qmake_project"), _("New qmake based project..."), wxEmptyString, wxITEM_NORMAL);
    menu->Append(item);

    item = new wxMenuItem(menu, XRCID("qmake_settings"), _("Settings..."), wxEmptyString, wxITEM_NORMAL);
    menu->Append(item);

    pluginsMenu->Append(wxID_ANY, _("QMake"), menu);

    wxTheApp->Connect(XRCID("new_qmake_project"), wxEVT_COMMAND_MENU_SELECTED,
                      wxCommandEventHandler(QMakePlugin::OnNewQmakeBasedProject), NULL, (wxEvtHandler*)this);
    wxTheApp->Connect(XRCID("qmake_settings"), wxEVT_COMMAND_MENU_SELECTED,
                      wxCommandEventHandler(QMakePlugin::OnSettings), NULL, (wxEvtHandler*)this);
    wxTheApp->Connect(XRCID("qmake_run_qmake"), wxEVT_COMMAND_MENU_SELECTED,
                      wxCommandEventHandler(QMakePlugin::OnExportMakefile), NULL, this);
}

void QMakePlugin::HookPopupMenu(wxMenu* menu, MenuType type)
{
    if(type == MenuTypeFileView_Project) {
        if(!menu->FindItem(XRCID("qmake_run_qmake"))) {
            menu->PrependSeparator();
            menu->Prepend(XRCID("qmake_run_qmake"), _("Run qmake..."), _("Run qmake..."));
        }
    }
}

void QMakePlugin::UnPlug()
{
    EventNotifier::Get()->Disconnect(wxEVT_CMD_PROJ_SETTINGS_SAVED,
                                     clProjectSettingsEventHandler(QMakePlugin::OnSaveConfig), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_BUILD_STARTING, clBuildEventHandler(QMakePlugin::OnBuildStarting), NULL,
                                     this);
    EventNotifier::Get()->Disconnect(wxEVT_GET_PROJECT_BUILD_CMD, clBuildEventHandler(QMakePlugin::OnGetBuildCommand),
                                     NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_GET_PROJECT_CLEAN_CMD, clBuildEventHandler(QMakePlugin::OnGetCleanCommand),
                                     NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_GET_IS_PLUGIN_MAKEFILE,
                                     clBuildEventHandler(QMakePlugin::OnGetIsPluginMakefile), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_TREE_ITEM_FILE_ACTIVATED, clCommandEventHandler(QMakePlugin::OnOpenFile),
                                     NULL, this);
    wxTheApp->Disconnect(XRCID("new_qmake_project"), wxEVT_COMMAND_MENU_SELECTED,
                         wxCommandEventHandler(QMakePlugin::OnNewQmakeBasedProject), NULL, (wxEvtHandler*)this);
    wxTheApp->Disconnect(XRCID("qmake_settings"), wxEVT_COMMAND_MENU_SELECTED,
                         wxCommandEventHandler(QMakePlugin::OnSettings), NULL, (wxEvtHandler*)this);
    wxTheApp->Disconnect(XRCID("qmake_run_qmake"), wxEVT_COMMAND_MENU_SELECTED,
                         wxCommandEventHandler(QMakePlugin::OnExportMakefile), NULL, this);
}

void QMakePlugin::HookProjectSettingsTab(wxBookCtrlBase* book, const wxString& projectName, const wxString& configName)
{
    if(!book)
        return;

    DoUnHookAllTabs(book);

    QMakeTab* page = DoGetQmakeTab(configName);
    if(!page) {
        page = new QMakeTab(book, m_conf);
        page->Load(m_mgr, projectName, configName);
        m_pages[configName] = page;
    }
    book->AddPage(page, wxT("QMake"), true, wxNOT_FOUND);
}

void QMakePlugin::UnHookProjectSettingsTab(wxBookCtrlBase* book, const wxString& projectName,
                                           const wxString& configName)
{
    wxUnusedVar(configName);
    DoUnHookAllTabs(book);
}

void QMakePlugin::OnSaveConfig(clProjectSettingsEvent& event)
{
    event.Skip();

    wxString conf, project;
    project = event.GetProjectName();
    conf = event.GetConfigName();

    QMakeTab* tab = DoGetQmakeTab(conf);
    if(!tab) {
        return;
    }
    tab->Save(m_mgr, project, conf);
}

QMakeTab* QMakePlugin::DoGetQmakeTab(const wxString& config)
{
    std::map<wxString, QMakeTab*>::iterator iter = m_pages.find(config);
    if(iter == m_pages.end()) {
        return NULL;
    }
    return iter->second;
}

void QMakePlugin::DoUnHookAllTabs(wxBookCtrlBase* book)
{
    if(!book) {
        return;
    }

    for(size_t i = 0; i < book->GetPageCount(); i++) {
        std::map<wxString, QMakeTab*>::iterator iter = m_pages.begin();
        for(; iter != m_pages.end(); iter++) {
            if(book->GetPage(i) == iter->second) {
                book->RemovePage(i);
                iter->second->Destroy();
                m_pages.erase(iter);
                break;
            }
        }
    }
    book->Layout();
}

void QMakePlugin::OnSettings(wxCommandEvent& event)
{
    QMakeSettingsDlg dlg(NULL, m_mgr, m_conf);
    dlg.ShowModal();
}

void QMakePlugin::OnBuildStarting(clBuildEvent& event)
{
    // call Skip() to allow the standard compilation to take place
    event.Skip();

    QmakePluginData::BuildConfPluginData bcpd;
    wxString project = event.GetProjectName();
    wxString config = event.GetConfigurationName();

    if(!DoGetData(project, config, bcpd)) {
        return;
    }

    if(!bcpd.m_enabled) {
        return;
    }

    // OK this is a qmake project
    event.Skip(false);

    wxString errMsg;
    ProjectPtr p = m_mgr->GetWorkspace()->FindProjectByName(project, errMsg);
    if(!p) {
        return;
    }
    QMakeProFileGenerator generator(m_mgr, project, config);
    if(!wxFileName::Exists(generator.GetProFileName())) {
        // alert and return
        ::wxMessageBox(_("Could not locate pro file.\nDid you remember to run qmake? (right click on the project)"),
                       "QMake", wxICON_WARNING | wxCENTER);
        return;
    } else {
        event.Skip();
    }
}

bool QMakePlugin::DoGetData(const wxString& project, const wxString& conf, QmakePluginData::BuildConfPluginData& bcpd)
{
    wxString errMsg;
    ProjectPtr p = m_mgr->GetWorkspace()->FindProjectByName(project, errMsg);
    if(p) {
        wxString rawData = p->GetPluginData(wxT("qmake"));
        QmakePluginData pd(rawData);
        return pd.GetDataForBuildConf(conf, bcpd);
    }
    return false;
}

void QMakePlugin::OnGetCleanCommand(clBuildEvent& event)
{
    QmakePluginData::BuildConfPluginData bcpd;

    wxString project = event.GetProjectName();
    wxString config = event.GetConfigurationName();

    if(!DoGetData(project, config, bcpd)) {
        event.Skip();
        return;
    }

    if(!bcpd.m_enabled) {
        event.Skip();
        return;
    }

    event.SetCommand(DoGetBuildCommand(project, config, event.IsProjectOnly()) + wxT(" clean"));
}

void QMakePlugin::OnGetBuildCommand(clBuildEvent& event)
{
    QmakePluginData::BuildConfPluginData bcpd;

    wxString project = event.GetProjectName();
    wxString config = event.GetConfigurationName();

    if(!DoGetData(project, config, bcpd)) {
        event.Skip();
        return;
    }

    if(!bcpd.m_enabled) {
        event.Skip();
        return;
    }

    // we avoid calling event.Skip() to override the default build system by this one
    event.SetCommand(DoGetBuildCommand(project, config, event.IsProjectOnly()));
}

void QMakePlugin::OnGetIsPluginMakefile(clBuildEvent& event)
{
    QmakePluginData::BuildConfPluginData bcpd;

    wxString project = event.GetProjectName();
    wxString config = event.GetConfigurationName();

    if(!DoGetData(project, config, bcpd)) {
        event.Skip();
        return;
    }

    if(bcpd.m_enabled) {
        // return without calling event.Skip()
        return;
    }
    event.Skip();
}

wxString QMakePlugin::DoGetBuildCommand(const wxString& project, const wxString& config, bool projectOnly)
{
    wxUnusedVar(config);

    wxString errMsg;
    ProjectPtr p = m_mgr->GetWorkspace()->FindProjectByName(project, errMsg);
    if(!p) {
        return wxEmptyString;
    }

    BuildConfigPtr bldConf = clCxxWorkspaceST::Get()->GetProjBuildConf(project, config);

    wxString cmd;

    wxString projectMakefile;
    projectMakefile << p->GetName() << ".mk";
    ::WrapWithQuotes(projectMakefile);
    projectMakefile.Replace("\\", "/");

    if(!projectOnly) {
        // part of a greater makefile, use $(MAKE)
        cmd << wxT("@cd \"") << p->GetFileName().GetPath() << wxT("\" && ");
        cmd << "$(MAKE) -f " << projectMakefile;
    } else {
        // project only
        cmd = bldConf->GetCompiler()->GetTool("MAKE");
        if(!cmd.Contains("-f")) {
            cmd << " -f ";
        }
        cmd << " " << projectMakefile;
    }
    return cmd;
}

void QMakePlugin::OnNewQmakeBasedProject(wxCommandEvent& event)
{
    wxUnusedVar(event);
    if(m_conf->GetAllConfigurations().IsEmpty()) {
        wxMessageBox(_("There is no qmake defined, please define one from 'Plugins -> Qmake -> Settings'"),
                     _("CodeLite"), wxOK | wxICON_WARNING | wxCENTER, m_mgr->GetTheApp()->GetTopWindow());
        return;
    }

    NewQtProjDlg dlg(m_mgr->GetTheApp()->GetTopWindow(), m_conf, m_mgr);
    if(dlg.ShowModal() == wxID_OK) {
        wxString kind = dlg.GetProjectKind();
        wxString name = dlg.GetProjectName();
        wxString path = dlg.GetProjectPath();
        wxString configRelease = wxT("0000");
        wxString config = wxT("0000");
        wxString templateFile = m_mgr->GetStartupDirectory();
        wxString type = wxEmptyString;
        wxString qmakeSettings = dlg.GetQmake();
        wxString qmake;
        wxString content;

        if(kind == wxT("Static Library")) {

            type = PROJECT_TYPE_STATIC_LIBRARY;

        } else if(kind == wxT("Dynamic Library")) {

            type = PROJECT_TYPE_DYNAMIC_LIBRARY;

        } else if(kind == wxT("Console")) {

            type = PROJECT_TYPE_EXECUTABLE;
            configRelease = wxT("0017CONFIG += console");
            config = wxT("0023CONFIG += console debug");

        } else {

            type = PROJECT_TYPE_EXECUTABLE;
            config = wxT("0015CONFIG += debug");
            configRelease = wxT("0000");
        }

        wxString filename(m_mgr->GetStartupDirectory() + wxT("/templates/qmake/qmake.project"));
        if(!ReadFileWithConversion(filename, content)) {
            return;
        }

        // prepend the string lenght
        qmake = wxString::Format(wxT("%04d%s"), qmakeSettings.Length(), qmakeSettings.c_str());

        content.Replace(wxT("$(TYPE)"), type);
        content.Replace(wxT("$(NAME)"), name);
        content.Replace(wxT("$(CONFIG)"), config);
        content.Replace(wxT("$(RELEASE_CONFIG)"), configRelease);
        content.Replace(wxT("$(QMAKE)"), qmake);

        // save the file to the disk
        {
            DirSaver ds;
            if(!wxSetWorkingDirectory(path)) {
                wxMessageBox(_("Invalid project path!"), _("CodeLite"), wxOK | wxCENTER | wxICON_WARNING);
                return;
            }

            if(dlg.GetCreateDirectory()) {
                wxMkdir(name);
                wxSetWorkingDirectory(name);
            }

            if(!WriteFileWithBackup(name + wxT(".project"), content, false)) {
                wxMessageBox(wxString::Format(_("Failed to create .project file '%s'"),
                                              wxString(name + wxT(".project")).c_str()),
                             _("CodeLite"), wxOK | wxCENTER | wxICON_WARNING);
                return;
            }

            path = wxGetCwd();
            path << wxFileName::GetPathSeparator() << name << wxT(".project");

            m_mgr->AddProject(path);
        }
    }
}

void QMakePlugin::OnOpenFile(clCommandEvent& event)
{
    event.Skip();

    // launch it with the default application
    wxFileName fullpath(event.GetFileName());
    if(fullpath.GetExt().MakeLower() != wxT("ui")) {
        return;
    }

    wxMimeTypesManager* mgr = wxTheMimeTypesManager;
    wxFileType* type = mgr->GetFileTypeFromExtension(fullpath.GetExt());
    if(type) {
        wxString cmd = type->GetOpenCommand(fullpath.GetFullPath());
        delete type;

        if(cmd.IsEmpty() == false) {
            event.Skip(false);
            ::wxExecute(cmd);
        }
    }
}

void QMakePlugin::OnExportMakefile(wxCommandEvent& event)
{
    if(m_qmakeProcess)
        return;

    QmakePluginData::BuildConfPluginData bcpd;

    ProjectPtr pProj = m_mgr->GetSelectedProject();
    CHECK_PTR_RET(pProj);

    BuildConfigPtr bldConf = pProj->GetBuildConfiguration();
    CHECK_PTR_RET(bldConf);

    wxString project = pProj->GetName();
    wxString config = bldConf->GetName();

    if(!DoGetData(project, config, bcpd)) {
        event.Skip();
        return;
    }

    if(bcpd.m_enabled) {
        // This project/configuration is qmake project
        QMakeProFileGenerator generator(m_mgr, project, config);

        // Regenerate the .pro file
        generator.Generate();

        // run qmake
        wxString qmake_exe = m_conf->Read(wxString::Format(wxT("%s/qmake"), bcpd.m_qmakeConfig));
        wxString qmakespec = m_conf->Read(wxString::Format(wxT("%s/qmakespec"), bcpd.m_qmakeConfig));
        wxString qtdir = m_conf->Read(wxString::Format(wxT("%s/qtdir"), bcpd.m_qmakeConfig));

        // Create qmake comand
        wxString qmake_exe_line;
        qmake_exe.Trim().Trim(false);
        qmakespec.Trim().Trim(false);

        // Set QTDIR
        DirSaver ds;
        {
            wxString errMsg;
            ProjectPtr p = m_mgr->GetWorkspace()->FindProjectByName(project, errMsg);
            if(!p) {
                return;
            }
            ::WrapWithQuotes(qmake_exe);
            qmake_exe_line << qmake_exe << wxT(" -spec ") << qmakespec << wxT(" ") << generator.GetProFileName();
            wxStringMap_t om;
            om.insert({ "QTDIR", qtdir });

            // Add the bin folder of the compiler (so qmakw will be able to find g++ or whatever compiler)
            wxString cxx = bldConf->GetCompiler()->GetTool("CXX");
            wxFileName fnCxx(cxx);
            wxString curpath;
            wxString updatedPath = fnCxx.GetPath();
            if(::wxGetEnv("PATH", &curpath)) {
                updatedPath << clPATH_SEPARATOR << curpath;
            }
            om.insert({ "PATH", updatedPath });

            EnvSetter envGuard(NULL, &om, project, config);
            m_mgr->ClearOutputTab(kOutputTab_Build);
            m_mgr->ShowOutputPane(_("Build"));
            m_mgr->AppendOutputTabText(kOutputTab_Build, wxString() << "-- " << qmake_exe_line << "\n");
            m_qmakeProcess =
                ::CreateAsyncProcess(this, qmake_exe_line, IProcessCreateDefault, p->GetFileName().GetPath());
        }
    }
    event.Skip();
}

void QMakePlugin::OnQmakeOutput(clProcessEvent& event)
{
    m_mgr->AppendOutputTabText(kOutputTab_Build, event.GetOutput());
}

void QMakePlugin::OnQmakeTerminated(clProcessEvent& event)
{
    wxDELETE(m_qmakeProcess);
    m_mgr->AppendOutputTabText(kOutputTab_Build, "-- done\n");
}
