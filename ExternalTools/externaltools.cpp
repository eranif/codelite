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

#include "ExternalToolsManager.h"
#include "ExternalToolsProcessManager.h"
#include "async_executable_cmd.h"
#include "clKeyboardManager.h"
#include "clToolBarButton.h"
#include "cl_aui_tb_are.h"
#include "codelite_events.h"
#include "dirsaver.h"
#include "environmentconfig.h"
#include "event_notifier.h"
#include "externaltooldlg.h"
#include "externaltools.h"
#include "globals.h"
#include "macromanager.h"
#include "plugin_version.h"
#include "processreaderthread.h"
#include "workspace.h"
#include <algorithm>
#include <wx/app.h>
#include <wx/aui/framemanager.h>
#include <wx/bitmap.h>
#include <wx/log.h>
#include <wx/menu.h>
#include <wx/msgdlg.h>
#include <wx/xrc/xmlres.h>

static ExternalToolsPlugin* thePlugin = NULL;

struct DecSort {
    bool operator()(const ToolInfo& t1, const ToolInfo& t2) { return t1.GetName().CmpNoCase(t2.GetName()) < 0; }
};

// Define the plugin entry point
CL_PLUGIN_API IPlugin* CreatePlugin(IManager* manager)
{
    if(thePlugin == 0) { thePlugin = new ExternalToolsPlugin(manager); }
    return thePlugin;
}

CL_PLUGIN_API PluginInfo* GetPluginInfo()
{
    static PluginInfo info;
    info.SetAuthor(wxT("Eran Ifrah"));
    info.SetName(wxT("ExternalTools"));
    info.SetDescription(_("A plugin that allows user to launch external tools from within CodeLite"));
    info.SetVersion(wxT("v1.0"));
    return &info;
}

CL_PLUGIN_API int GetPluginInterfaceVersion() { return PLUGIN_INTERFACE_VERSION; }

ExternalToolsPlugin::ExternalToolsPlugin(IManager* manager)
    : IPlugin(manager)
    , topWin(NULL)
    , m_parentMenu(NULL)
{
    ToolsTaskManager::Instance(); // Ensure that we allocate the process manager
    m_longName = _("A plugin that allows user to launch external tools from within CodeLite");
    m_shortName = wxT("ExternalTools");
    topWin = m_mgr->GetTheApp();

    topWin->Bind(wxEVT_MENU, &ExternalToolsPlugin::OnSettings, this, XRCID("external_tools_settings"));
    topWin->Bind(wxEVT_MENU, &ExternalToolsPlugin::OnShowRunningTools, this, XRCID("external_tools_monitor"));

    for(int i = 0; i < MAX_TOOLS; i++) {
        wxString winid = wxString::Format(wxT("external_tool_%d"), i);
        topWin->Connect(wxXmlResource::GetXRCID(winid.c_str()), wxEVT_COMMAND_MENU_SELECTED,
                        wxCommandEventHandler(ExternalToolsPlugin::OnLaunchExternalTool), NULL, this);
    }

    // Register keyboard shortcuts
    for(int i = 0; i < MAX_TOOLS; i++) {
        clKeyboardManager::Get()->AddGlobalAccelerator(
            wxString::Format("external_tool_%d", i), wxString::Format("Ctrl-Shift-%d", i),
            wxString::Format("Plugins::External Tools::External Tool %d", i));
    }

    // Bind the "OnFileSave" event
    EventNotifier::Get()->Bind(wxEVT_FILE_SAVED, &ExternalToolsPlugin::OnFileSave, this);

    // Load the tools from the configuraton file
    m_mgr->GetConfigTool()->ReadObject(wxT("ExternalTools"), &m_externalTools);
}

ExternalToolsPlugin::~ExternalToolsPlugin() {}

void ExternalToolsPlugin::CreateToolBar(clToolBar* toolbar)
{
    // support both toolbars icon size
    int size = m_mgr->GetToolbarIconSize();
    toolbar->AddSpacer();
    toolbar->AddButton(XRCID("external_tools_settings"), m_mgr->GetStdIcons()->LoadBitmap("tools", size),
                       _("Configure external tools..."));
    toolbar->AddButton(XRCID("external_tools_monitor"), m_mgr->GetStdIcons()->LoadBitmap("monitor", size),
                       _("Show Running Tools..."));
    DoRecreateToolbar();
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

void ExternalToolsPlugin::UnPlug()
{
    EventNotifier::Get()->Unbind(wxEVT_FILE_SAVED, &ExternalToolsPlugin::OnFileSave, this);
    topWin->Unbind(wxEVT_MENU, &ExternalToolsPlugin::OnSettings, this, XRCID("external_tools_settings"));
    topWin->Unbind(wxEVT_MENU, &ExternalToolsPlugin::OnShowRunningTools, this, XRCID("external_tools_monitor"));

    for(int i = 0; i < MAX_TOOLS; ++i) {
        wxString winid = wxString::Format(wxT("external_tool_%d"), i);
        topWin->Disconnect(wxXmlResource::GetXRCID(winid.c_str()), wxEVT_COMMAND_MENU_SELECTED,
                           wxCommandEventHandler(ExternalToolsPlugin::OnLaunchExternalTool), NULL, this);
    }
    // now that all the event handlers have been disconneted, kill all the running tools
    ToolsTaskManager::Release();
}

void ExternalToolsPlugin::OnSettings(wxCommandEvent& e)
{
    ExternalToolDlg dlg(m_mgr->GetTheApp()->GetTopWindow(), m_mgr);
    dlg.SetTools(m_externalTools.GetTools());

    if(dlg.ShowModal() == wxID_OK) {
        // Update the cache
        m_externalTools.SetTools(dlg.GetTools());

        // and the file system as well
        m_mgr->GetConfigTool()->WriteObject(wxT("ExternalTools"), &m_externalTools);
        CallAfter(&ExternalToolsPlugin::OnRecreateTB);
    }
}

void ExternalToolsPlugin::OnRecreateTB()
{
    DoRecreateToolbar();
    DoCreatePluginMenu();
}

void ExternalToolsPlugin::OnLaunchExternalTool(wxCommandEvent& e)
{
    for(size_t i = 0; i < m_externalTools.GetTools().size(); i++) {
        const ToolInfo& ti = m_externalTools.GetTools()[i];
        if(wxXmlResource::GetXRCID(ti.GetId().c_str()) == e.GetId()) { ToolsTaskManager::Instance()->StartTool(ti); }
    }
}

void ExternalToolsPlugin::DoRecreateToolbar()
{
    int size = m_mgr->GetToolbarIconSize();

    std::vector<ToolInfo> tools = m_externalTools.GetTools();
    std::sort(tools.begin(), tools.end(), DecSort());
    clToolBar* toolbar = clGetManager()->GetToolBar();

    // Remove the old tools
    for(size_t i = 0; i < 10; ++i) {
        wxString xrcid;
        xrcid << "external_tool_" << i;
        toolbar->DeleteById(wxXmlResource::GetXRCID(xrcid));
    }

    // Add the tools
    int where = XRCID("external_tools_monitor");
    for(size_t i = 0; i < tools.size(); i++) {
        const ToolInfo& ti = tools[i];

        wxFileName icon24(ti.GetIcon24());
        wxFileName icon16(ti.GetIcon16());

        wxBitmap bmp(m_mgr->GetStdIcons()->LoadBitmap("cog", size));
        wxBitmap defaultBitmap(m_mgr->GetStdIcons()->LoadBitmap("cog", size));
        if(size == 24) {
            if(icon24.FileExists()) {
                bmp.LoadFile(icon24.GetFullPath(), wxBITMAP_TYPE_PNG);
                if(!bmp.IsOk()) { bmp = defaultBitmap; }
            }

        } else {
            if(icon16.FileExists()) {
                bmp.LoadFile(icon16.GetFullPath(), wxBITMAP_TYPE_PNG);
                if(!bmp.IsOk()) { bmp = defaultBitmap; }
            }
        }
        clToolBarButton* button = new clToolBarButton(toolbar, wxXmlResource::GetXRCID(ti.GetId()), bmp, ti.GetName());
        toolbar->InsertAfter(where, button);
        where = button->GetId();
    }
    toolbar->Realize();
}

void ExternalToolsPlugin::DoCreatePluginMenu()
{
    const int MENU_ID = 28374;
    if(m_parentMenu) {
        // destroy the old menu entries
        if(m_parentMenu->FindItem(MENU_ID)) { m_parentMenu->Destroy(MENU_ID); }

        wxMenu* menu = new wxMenu();
        wxMenuItem* item(NULL);
        item = new wxMenuItem(menu, XRCID("external_tools_settings"), _("Configure external tools..."), wxEmptyString,
                              wxITEM_NORMAL);
        menu->Append(item);
        item = new wxMenuItem(menu, XRCID("external_tools_monitor"), _("Show Running Tools..."), wxEmptyString,
                              wxITEM_NORMAL);
        menu->Append(item);
        menu->AppendSeparator();

        // Loop and append the tools already defined
        std::vector<ToolInfo> tools = m_externalTools.GetTools();
        std::sort(tools.begin(), tools.end(), DecSort());

        for(size_t i = 0; i < tools.size(); i++) {
            ToolInfo ti = tools.at(i);
            item = new wxMenuItem(menu, wxXmlResource::GetXRCID(ti.GetId().c_str()), ti.GetName(), wxEmptyString,
                                  wxITEM_NORMAL);
            menu->Append(item);
        }
        m_parentMenu->Append(MENU_ID, _("External Tools"), menu);
    }
}

void ExternalToolsPlugin::OnShowRunningTools(wxCommandEvent& e)
{
    wxUnusedVar(e);
    ExternalToolsManager dlg(EventNotifier::Get()->TopFrame());
    dlg.ShowModal();
}

void ExternalToolsPlugin::OnFileSave(clCommandEvent& event)
{
    event.Skip(); // always skip

    // Get list of tools that should be triggered when a file is being saved
    const std::vector<ToolInfo>& tools = m_externalTools.GetTools();
    for(const ToolInfo& tool : tools) {
        if(tool.IsCallOnFileSave()) {
            // Run this tool
            ToolInfo ti = tool;
            wxString filename = event.GetFileName();
            ::WrapWithQuotes(filename);
            ToolsTaskManager::Instance()->StartTool(ti, filename);
        }
    }
}
