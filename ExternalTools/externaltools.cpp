//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : externaltools.cpp
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

#include "environmentconfig.h"
#include <wx/aui/framemanager.h>
#include "macromanager.h"
#include "async_executable_cmd.h"
#include <wx/bitmap.h>
#include "dirsaver.h"
#include "cl_aui_tb_are.h"
#include <wx/app.h>
#include "workspace.h"
#include "globals.h"
#include "externaltooldlg.h"
#include <wx/menu.h>
#include "externaltools.h"
#include <wx/xrc/xmlres.h>
#include <wx/log.h>
#include <algorithm>
#include "plugin_version.h"
#include "clKeyboardManager.h"
#include "event_notifier.h"
#include "workspace.h"
#include "processreaderthread.h"
#include <wx/msgdlg.h>

static ExternalToolsPlugin* thePlugin = NULL;

struct DecSort {
    bool operator()(const ToolInfo& t1, const ToolInfo& t2) { return t1.GetName().CmpNoCase(t2.GetName()) > 0; }
};

// Define the plugin entry point
extern "C" EXPORT IPlugin* CreatePlugin(IManager* manager)
{
    if(thePlugin == 0) {
        thePlugin = new ExternalToolsPlugin(manager);
    }
    return thePlugin;
}

extern "C" EXPORT PluginInfo GetPluginInfo()
{
    PluginInfo info;
    info.SetAuthor(wxT("Eran Ifrah"));
    info.SetName(wxT("ExternalTools"));
    info.SetDescription(_("A plugin that allows user to launch external tools from within CodeLite"));
    info.SetVersion(wxT("v1.0"));
    return info;
}

extern "C" EXPORT int GetPluginInterfaceVersion() { return PLUGIN_INTERFACE_VERSION; }

ExternalToolsPlugin::ExternalToolsPlugin(IManager* manager)
    : IPlugin(manager)
    , topWin(NULL)
    , m_process(NULL)
    , m_parentMenu(NULL)
{
    m_longName = _("A plugin that allows user to launch external tools from within CodeLite");
    m_shortName = wxT("ExternalTools");
    topWin = m_mgr->GetTheApp();

    topWin->Connect(XRCID("stop_external_tool"),
                    wxEVT_COMMAND_MENU_SELECTED,
                    wxCommandEventHandler(ExternalToolsPlugin::OnStopExternalTool),
                    NULL,
                    (wxEvtHandler*)this);
    topWin->Connect(34733,
                    wxEVT_COMMAND_MENU_SELECTED,
                    wxCommandEventHandler(ExternalToolsPlugin::OnRecreateTB),
                    NULL,
                    (wxEvtHandler*)this);
    topWin->Connect(XRCID("stop_external_tool"),
                    wxEVT_UPDATE_UI,
                    wxUpdateUIEventHandler(ExternalToolsPlugin::OnStopExternalToolUI),
                    NULL,
                    (wxEvtHandler*)this);

    for(int i = 0; i < MAX_TOOLS; i++) {
        wxString winid = wxString::Format(wxT("external_tool_%d"), i);
        topWin->Connect(wxXmlResource::GetXRCID(winid.c_str()),
                        wxEVT_COMMAND_MENU_SELECTED,
                        wxCommandEventHandler(ExternalToolsPlugin::OnLaunchExternalTool),
                        NULL,
                        this);
        topWin->Connect(wxXmlResource::GetXRCID(winid.c_str()),
                        wxEVT_UPDATE_UI,
                        wxUpdateUIEventHandler(ExternalToolsPlugin::OnLaunchExternalToolUI),
                        NULL,
                        this);
    }

    // Register keyboard shortcuts
    for(int i = 0; i < MAX_TOOLS; i++) {
        clKeyboardManager::Get()->AddGlobalAccelerator(
            wxString::Format("external_tool_%d", i),
            wxString::Format("Ctrl-Shift-%d", i),
            wxString::Format("Plugins::External Tools::External Tool %d", i));
    }

    Bind(wxEVT_ASYNC_PROCESS_OUTPUT, &ExternalToolsPlugin::OnProcessOutput, this);
    Bind(wxEVT_ASYNC_PROCESS_TERMINATED, &ExternalToolsPlugin::OnProcessEnd, this);
}

ExternalToolsPlugin::~ExternalToolsPlugin()
{
    topWin->Disconnect(XRCID("external_tools_settings"),
                       wxEVT_COMMAND_MENU_SELECTED,
                       wxCommandEventHandler(ExternalToolsPlugin::OnSettings),
                       NULL,
                       (wxEvtHandler*)this);
    topWin->Disconnect(XRCID("stop_external_tool"),
                       wxEVT_COMMAND_MENU_SELECTED,
                       wxCommandEventHandler(ExternalToolsPlugin::OnStopExternalTool),
                       NULL,
                       (wxEvtHandler*)this);
    topWin->Disconnect(34733,
                       wxEVT_COMMAND_MENU_SELECTED,
                       wxCommandEventHandler(ExternalToolsPlugin::OnRecreateTB),
                       NULL,
                       (wxEvtHandler*)this);
    topWin->Disconnect(XRCID("stop_external_tool"),
                       wxEVT_UPDATE_UI,
                       wxUpdateUIEventHandler(ExternalToolsPlugin::OnStopExternalToolUI),
                       NULL,
                       (wxEvtHandler*)this);

    for(int i = 0; i < MAX_TOOLS; i++) {
        wxString winid = wxString::Format(wxT("external_tool_%d"), i);
        topWin->Disconnect(wxXmlResource::GetXRCID(winid.c_str()),
                           wxEVT_COMMAND_MENU_SELECTED,
                           wxCommandEventHandler(ExternalToolsPlugin::OnLaunchExternalTool),
                           NULL,
                           this);
        topWin->Disconnect(wxXmlResource::GetXRCID(winid.c_str()),
                           wxEVT_UPDATE_UI,
                           wxUpdateUIEventHandler(ExternalToolsPlugin::OnLaunchExternalToolUI),
                           NULL,
                           this);
    }
}

clToolBar* ExternalToolsPlugin::CreateToolBar(wxWindow* parent)
{
    // support both toolbars icon size
    m_tb = NULL;
    ExternalToolsData inData;
    m_mgr->GetConfigTool()->ReadObject(wxT("ExternalTools"), &inData);
    int size = m_mgr->GetToolbarIconSize();

    if(m_mgr->AllowToolbar()) {

        m_tb = new clToolBar(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, clTB_DEFAULT_STYLE);
        m_tb->SetToolBitmapSize(wxSize(size, size));

        if(size == 24) {
            m_tb->AddTool(XRCID("external_tools_settings"),
                          _("Configure external tools..."),
                          m_mgr->GetStdIcons()->LoadBitmap(wxT("toolbars/24/external-tools/configure")),
                          _("Configure external tools..."));
            m_tb->AddTool(XRCID("stop_external_tool"),
                          _("Stop external tool"),
                          m_mgr->GetStdIcons()->LoadBitmap(wxT("toolbars/24/build/stop")),
                          _("Stop external tool"));
        } else {
            m_tb->AddTool(XRCID("external_tools_settings"),
                          _("Configure external tools..."),
                          m_mgr->GetStdIcons()->LoadBitmap(wxT("toolbars/16/external-tools/configure")),
                          _("Configure external tools..."));
            m_tb->AddTool(XRCID("stop_external_tool"),
                          _("Stop external tool"),
                          m_mgr->GetStdIcons()->LoadBitmap(wxT("toolbars/16/build/stop")),
                          _("Stop external tool"));
        }

#if USE_AUI_TOOLBAR
        m_tb->SetArtProvider(new CLMainAuiTBArt());
#endif
        std::vector<ToolInfo> tools = inData.GetTools();
        std::sort(tools.begin(), tools.end(), DecSort());

        for(size_t i = 0; i < tools.size(); i++) {
            ToolInfo ti = tools.at(i);

            wxFileName icon24(ti.GetIcon24());
            wxFileName icon16(ti.GetIcon16());

            if(size == 24) {
                wxBitmap bmp(m_mgr->GetStdIcons()->LoadBitmap(wxT("toolbars/24/external-tools/cog")));
                if(icon24.FileExists()) {
                    bmp.LoadFile(icon24.GetFullPath(), wxBITMAP_TYPE_PNG);
                    if(bmp.IsOk() == false) {
                        bmp = m_mgr->GetStdIcons()->LoadBitmap(wxT("toolbars/24/external-tools/cog"));
                    }
                }
                m_tb->AddTool(wxXmlResource::GetXRCID(ti.GetId()), ti.GetName(), bmp, ti.GetName());

            } else if(size == 16) {
                wxBitmap bmp(m_mgr->GetStdIcons()->LoadBitmap(wxT("toolbars/16/external-tools/cog")));
                if(icon16.FileExists()) {
                    bmp.LoadFile(icon16.GetFullPath(), wxBITMAP_TYPE_PNG);
                    if(bmp.IsOk() == false) {
                        bmp = m_mgr->GetStdIcons()->LoadBitmap(wxT("toolbars/16/external-tools/cog"));
                    }
                }

                m_tb->AddTool(wxXmlResource::GetXRCID(ti.GetId()), ti.GetName(), bmp, ti.GetName());
            }
        }

        m_tb->Realize();
    } else {
        // Using native toolbar
        wxFrame* frame = dynamic_cast<wxFrame*>(parent);
        CHECK_PTR_RET_NULL(frame);

        wxToolBar* toolbar = frame->GetToolBar();
        CHECK_PTR_RET_NULL(toolbar);

        // Add the static tools
        toolbar->AddSeparator();
        if(size == 24) {
            toolbar->AddTool(XRCID("external_tools_settings"),
                             _("Configure external tools..."),
                             m_mgr->GetStdIcons()->LoadBitmap(wxT("toolbars/24/external-tools/configure")),
                             _("Configure external tools..."));
            toolbar->AddTool(XRCID("stop_external_tool"),
                             _("Stop external tool"),
                             m_mgr->GetStdIcons()->LoadBitmap(wxT("toolbars/24/build/stop")),
                             _("Stop external tool"));
        } else {
            toolbar->AddTool(XRCID("external_tools_settings"),
                             _("Configure external tools..."),
                             m_mgr->GetStdIcons()->LoadBitmap(wxT("toolbars/16/external-tools/configure")),
                             _("Configure external tools..."));
            toolbar->AddTool(XRCID("stop_external_tool"),
                             _("Stop external tool"),
                             m_mgr->GetStdIcons()->LoadBitmap(wxT("toolbars/16/build/stop")),
                             _("Stop external tool"));
        }

        DoAppendToolsToNativeToolbar(toolbar);
    }
    return m_tb;
}

void ExternalToolsPlugin::CreatePluginMenu(wxMenu* pluginsMenu)
{
    m_parentMenu = pluginsMenu;
    DoCreatePluginMenu();
}

void ExternalToolsPlugin::HookPopupMenu(wxMenu* menu, MenuType type)
{
    wxUnusedVar(menu);
    wxUnusedVar(type);
}

void ExternalToolsPlugin::UnPlug() {}

void ExternalToolsPlugin::OnSettings(wxCommandEvent& e)
{
    ExternalToolsData inData;
    m_mgr->GetConfigTool()->ReadObject(wxT("ExternalTools"), &inData);
    ExternalToolDlg dlg(m_mgr->GetTheApp()->GetTopWindow(), m_mgr);
    dlg.SetTools(inData.GetTools());

    if(dlg.ShowModal() == wxID_OK) {
        // Do something here
        ExternalToolsData data;
        data.SetTools(dlg.GetTools());
        m_mgr->GetConfigTool()->WriteObject(wxT("ExternalTools"), &data);

        wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, 34733);
        wxPostEvent(this, evt);
    }
}

void ExternalToolsPlugin::OnRecreateTB(wxCommandEvent& e)
{
    wxUnusedVar(e);
    DoRecreateToolbar();
    DoCreatePluginMenu();
}

void ExternalToolsPlugin::OnLaunchExternalTool(wxCommandEvent& e)
{
    ExternalToolsData inData;
    m_mgr->GetConfigTool()->ReadObject(wxT("ExternalTools"), &inData);

    for(size_t i = 0; i < inData.GetTools().size(); i++) {
        ToolInfo ti = inData.GetTools().at(i);
        if(wxXmlResource::GetXRCID(ti.GetId().c_str()) == e.GetId()) {
            DoLaunchTool(ti);
        }
    }
}

void ExternalToolsPlugin::DoLaunchTool(const ToolInfo& ti)
{
    if(m_process) {
        ::wxMessageBox(_("Another tool is currently running"), "CodeLite", wxOK | wxICON_ERROR | wxCENTER);
        return;
    }
    
    wxString command, working_dir;
    command << ti.GetPath();
    ::WrapWithQuotes(command);

    command << " " << ti.GetArguments();
    working_dir = ti.GetWd();

    command = MacroManager::Instance()->Expand(
        command, m_mgr, (m_mgr->GetWorkspace() ? m_mgr->GetWorkspace()->GetActiveProjectName() : ""));
    working_dir = MacroManager::Instance()->Expand(
        working_dir, m_mgr, (m_mgr->GetWorkspace() ? m_mgr->GetWorkspace()->GetActiveProjectName() : ""));

    wxString projectName;
    if(clCxxWorkspaceST::Get()->IsOpen()) {
        projectName = clCxxWorkspaceST::Get()->GetActiveProjectName();
    }

    EnvSetter envGuard(m_mgr->GetEnv(), NULL, projectName);
    m_process = ::CreateAsyncProcess(this, command, IProcessCreateDefault, working_dir);
    m_mgr->AppendOutputTabText(kOutputTab_Output, command + "\n");
}

void ExternalToolsPlugin::DoRecreateToolbar()
{
    if(m_mgr->AllowToolbar()) {
        wxWindow* parent(NULL);
        if(m_tb) {
            // we have a toolbar, remove it from the docking manager
            m_mgr->GetDockingManager()->DetachPane(m_tb);
            parent = m_tb->GetParent();
            m_tb->Destroy();
        } else {
            parent = m_mgr->GetTheApp()->GetTopWindow();
        }

        m_tb = CreateToolBar(parent);
        if(m_tb) {
            m_mgr->GetDockingManager()->AddPane(m_tb,
                                                wxAuiPaneInfo()
                                                    .Name(GetShortName())
                                                    .LeftDockable(true)
                                                    .RightDockable(true)
                                                    .Caption(GetShortName())
                                                    .ToolbarPane()
                                                    .Top());

            // Apply changes
            m_mgr->GetDockingManager()->Update();
        }

    } else {
        // Native toolbar
        wxFrame* frame = EventNotifier::Get()->TopFrame();
        wxToolBar* toolbar = frame->GetToolBar();
        CHECK_PTR_RET(toolbar);
        DoClearNativeToolbarItems(toolbar);
        DoAppendToolsToNativeToolbar(toolbar);
    }
}

bool ExternalToolsPlugin::IsRedirectedToolRunning() { return (m_process != NULL); }

void ExternalToolsPlugin::OnLaunchExternalToolUI(wxUpdateUIEvent& e) { e.Enable(!IsRedirectedToolRunning()); }

void ExternalToolsPlugin::OnStopExternalTool(wxCommandEvent& e)
{
    if(m_process) {
        m_process->Terminate();
    }
}

void ExternalToolsPlugin::OnStopExternalToolUI(wxUpdateUIEvent& e) { e.Enable(this->IsRedirectedToolRunning()); }

void ExternalToolsPlugin::DoCreatePluginMenu()
{
    const int MENU_ID = 28374;
    if(m_parentMenu) {
        // destroy the old menu entries
        if(m_parentMenu->FindItem(MENU_ID)) {
            m_parentMenu->Destroy(MENU_ID);
        }

        wxMenu* menu = new wxMenu();
        wxMenuItem* item(NULL);
        item = new wxMenuItem(
            menu, XRCID("external_tools_settings"), _("Configure external tools..."), wxEmptyString, wxITEM_NORMAL);
        menu->Append(item);
        menu->AppendSeparator();

        // Loop and append the tools already defined
        ExternalToolsData inData;
        m_mgr->GetConfigTool()->ReadObject(wxT("ExternalTools"), &inData);

        for(size_t i = 0; i < inData.GetTools().size(); i++) {
            ToolInfo ti = inData.GetTools().at(i);
            item = new wxMenuItem(
                menu, wxXmlResource::GetXRCID(ti.GetId().c_str()), ti.GetName(), wxEmptyString, wxITEM_NORMAL);
            menu->Append(item);
        }

        m_parentMenu->Append(MENU_ID, _("External Tools"), menu);
        topWin->Connect(XRCID("external_tools_settings"),
                        wxEVT_COMMAND_MENU_SELECTED,
                        wxCommandEventHandler(ExternalToolsPlugin::OnSettings),
                        NULL,
                        (wxEvtHandler*)this);
    }
}

void ExternalToolsPlugin::DoClearNativeToolbarItems(wxToolBar* toolbar)
{
    CHECK_PTR_RET(toolbar);
    ExternalToolsData inData;
    m_mgr->GetConfigTool()->ReadObject(wxT("ExternalTools"), &inData);

    std::vector<ToolInfo> tools = inData.GetTools();
    for(size_t i = 0; i < tools.size(); ++i) {
        toolbar->DeleteTool(wxXmlResource::GetXRCID(tools.at(i).GetId()));
    }
    toolbar->Realize();
}

void ExternalToolsPlugin::DoAppendToolsToNativeToolbar(wxToolBar* toolbar)
{
    int pos = toolbar->GetToolPos(XRCID("external_tools_settings"));
    CHECK_COND_RET((pos != wxNOT_FOUND));

    ExternalToolsData inData;
    m_mgr->GetConfigTool()->ReadObject(wxT("ExternalTools"), &inData);
    int size = m_mgr->GetToolbarIconSize();

    std::vector<ToolInfo> tools = inData.GetTools();
    for(size_t i = 0; i < tools.size(); i++) {
        ToolInfo ti = tools.at(i);

        wxFileName icon24(ti.GetIcon24());
        wxFileName icon16(ti.GetIcon16());

        if(size == 24) {
            wxBitmap bmp(m_mgr->GetStdIcons()->LoadBitmap(wxT("toolbars/24/external-tools/cog")));
            if(icon24.FileExists()) {
                bmp.LoadFile(icon24.GetFullPath(), wxBITMAP_TYPE_PNG);
                if(bmp.IsOk() == false) {
                    bmp = m_mgr->GetStdIcons()->LoadBitmap(wxT("toolbars/24/external-tools/cog"));
                }
            }
            toolbar->InsertTool(
                pos, wxXmlResource::GetXRCID(ti.GetId()), ti.GetName(), bmp, wxNullBitmap, wxITEM_NORMAL, ti.GetName());

        } else if(size == 16) {
            wxBitmap bmp(m_mgr->GetStdIcons()->LoadBitmap(wxT("toolbars/16/external-tools/cog")));
            if(icon16.FileExists()) {
                bmp.LoadFile(icon16.GetFullPath(), wxBITMAP_TYPE_PNG);
                if(bmp.IsOk() == false) {
                    bmp = m_mgr->GetStdIcons()->LoadBitmap(wxT("toolbars/16/external-tools/cog"));
                }
            }

            toolbar->InsertTool(
                pos, wxXmlResource::GetXRCID(ti.GetId()), ti.GetName(), bmp, wxNullBitmap, wxITEM_NORMAL, ti.GetName());
        }
    }
    toolbar->Realize();
}

void ExternalToolsPlugin::OnProcessEnd(clProcessEvent& event)
{
    m_mgr->AppendOutputTabText(kOutputTab_Output, event.GetOutput());
    m_mgr->AppendOutputTabText(kOutputTab_Output, "\n");
    wxDELETE(m_process);

    // Notify codelite to test for any modified bufferes
    EventNotifier::Get()->PostReloadExternallyModifiedEvent();
}

void ExternalToolsPlugin::OnProcessOutput(clProcessEvent& event)
{
    m_mgr->AppendOutputTabText(kOutputTab_Output, event.GetOutput());
}
