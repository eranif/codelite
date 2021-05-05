//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : tweaks.cpp
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

#include "TweaksSettingsDlg.h"
#include "editor_config.h"
#include "event_notifier.h"
#include "tweaks.h"
#include "workspace.h"
#include <wx/menu.h>
#include <wx/xrc/xmlres.h>

static Tweaks* thePlugin = NULL;

static int ID_TWEAKS_SETTINGS = ::wxNewId();

#define TWEAKS_ENABLED_EVENT_HANDLER()                                       \
    if(!m_settings.IsEnableTweaks() || !clCxxWorkspaceST::Get()->IsOpen()) { \
        e.Skip();                                                            \
        return;                                                              \
    }

// Define the plugin entry point
CL_PLUGIN_API IPlugin* CreatePlugin(IManager* manager)
{
    if(thePlugin == 0) {
        thePlugin = new Tweaks(manager);
    }
    return thePlugin;
}

CL_PLUGIN_API PluginInfo* GetPluginInfo()
{
    static PluginInfo info;
    info.SetAuthor(wxT("eran"));
    info.SetName(wxT("Tweaks"));
    info.SetDescription(_("Tweak codelite"));
    info.SetVersion(wxT("v1.0"));
    return &info;
}

CL_PLUGIN_API int GetPluginInterfaceVersion() { return PLUGIN_INTERFACE_VERSION; }

Tweaks::Tweaks(IManager* manager)
    : IPlugin(manager)
{
    wxPGInitResourceModule(); // Or we crash...

    m_longName = _("Tweak codelite");
    m_shortName = wxT("Tweaks");

    m_mgr->GetTheApp()->Connect(ID_TWEAKS_SETTINGS, wxEVT_COMMAND_MENU_SELECTED,
                                wxCommandEventHandler(Tweaks::OnSettings), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_COLOUR_TAB, clColourEventHandler(Tweaks::OnColourTab), NULL, this);
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_LOADED, &Tweaks::OnWorkspaceLoaded, this);
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_CLOSED, &Tweaks::OnWorkspaceClosed, this);
    EventNotifier::Get()->Connect(wxEVT_WORKSPACE_VIEW_BUILD_STARTING,
                                  clCommandEventHandler(Tweaks::OnFileViewBuildTree), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_WORKSPACE_VIEW_CUSTOMIZE_PROJECT,
                                  clColourEventHandler(Tweaks::OnCustomizeProject), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_GET_TAB_BORDER_COLOUR, clColourEventHandler(Tweaks::OnTabBorderColour), NULL,
                                  this);
}

void Tweaks::UnPlug()
{
    m_mgr->GetTheApp()->Disconnect(ID_TWEAKS_SETTINGS, wxEVT_COMMAND_MENU_SELECTED,
                                   wxCommandEventHandler(Tweaks::OnSettings), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_COLOUR_TAB, clColourEventHandler(Tweaks::OnColourTab), NULL, this);
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_LOADED, &Tweaks::OnWorkspaceLoaded, this);
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_CLOSED, &Tweaks::OnWorkspaceClosed, this);
    EventNotifier::Get()->Disconnect(wxEVT_WORKSPACE_VIEW_BUILD_STARTING,
                                     clCommandEventHandler(Tweaks::OnFileViewBuildTree), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_WORKSPACE_VIEW_CUSTOMIZE_PROJECT,
                                     clColourEventHandler(Tweaks::OnCustomizeProject), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_GET_TAB_BORDER_COLOUR, clColourEventHandler(Tweaks::OnTabBorderColour), NULL,
                                     this);
}

Tweaks::~Tweaks() {}

void Tweaks::CreateToolBar(clToolBar* toolbar) { wxUnusedVar(toolbar); }

void Tweaks::CreatePluginMenu(wxMenu* pluginsMenu)
{
    wxUnusedVar(pluginsMenu);
    wxMenu* menu = new wxMenu;
    menu->Append(ID_TWEAKS_SETTINGS, _("Settings..."));
    pluginsMenu->AppendSubMenu(menu, _("Tweaks Plugin"));
}

void Tweaks::HookPopupMenu(wxMenu* menu, MenuType type)
{
    wxUnusedVar(menu);
    wxUnusedVar(type);
}

void Tweaks::OnSettings(wxCommandEvent& e)
{
    wxUnusedVar(e);
    TweaksSettingsDlg dlg(m_mgr->GetTheApp()->GetTopWindow());
    if(dlg.ShowModal() == wxID_OK) {
        dlg.GetSettings().Save();
    }

    m_settings.Load(); // Refresh our cached settings
    // Refresh the drawings
    m_mgr->GetTheApp()->GetTopWindow()->Refresh();
}

void Tweaks::OnColourTab(clColourEvent& e)
{
    TWEAKS_ENABLED_EVENT_HANDLER();

    IEditor* editor = FindEditorByPage(e.GetPage());
    if(!editor) {
        if(m_settings.GetGlobalFgColour().IsOk() && m_settings.GetGlobalBgColour().IsOk()) {
            // Non editor tab
            e.SetBgColour(m_settings.GetGlobalBgColour());
            e.SetFgColour(m_settings.GetGlobalFgColour());

        } else {
            e.Skip();
        }

    } else {

        const ProjectTweaks& tw = m_settings.GetProjectTweaks(editor->GetProjectName());
        if(tw.IsOk()) {
            e.SetBgColour(tw.GetTabBgColour());
            e.SetFgColour(tw.GetTabFgColour());

        } else if(m_settings.GetGlobalBgColour().IsOk() && m_settings.GetGlobalFgColour().IsOk()) {
            e.SetBgColour(m_settings.GetGlobalBgColour());
            e.SetFgColour(m_settings.GetGlobalFgColour());

        } else {
            e.Skip();
        }
    }
}

IEditor* Tweaks::FindEditorByPage(wxWindow* page)
{
    for(size_t i = 0; i < m_mgr->GetPageCount(); ++i) {
        if(m_mgr->GetPage(i) == page) {
            return dynamic_cast<IEditor*>(m_mgr->GetPage(i));
        }
    }
    return NULL;
}

void Tweaks::OnWorkspaceLoaded(clWorkspaceEvent& e)
{
    e.Skip();
    // Refresh the list with the current workspace setup
    m_settings.Load();
    m_mgr->GetWorkspacePaneNotebook()->Refresh();
    m_mgr->GetOutputPaneNotebook()->Refresh();
}

void Tweaks::OnWorkspaceClosed(clWorkspaceEvent& e)
{
    e.Skip();
    m_settings.Clear();
    m_project2Icon.clear();
}

void Tweaks::OnFileViewBuildTree(clCommandEvent& e)
{
    TWEAKS_ENABLED_EVENT_HANDLER();
    m_project2Icon.clear();
    if(m_settings.GetProjects().empty()) {
        e.Skip();
        return;
    }

    // See if we got a new image for a project
    wxImageList* images = new wxImageList(16, 16);
    wxImageList* old_images = m_mgr->GetTree(TreeFileView)->GetImageList();

    // Copy the old images to the new one
    for(int i = 0; i < old_images->GetImageCount(); ++i) {
        images->Add(old_images->GetIcon(i));
    }

    ProjectTweaks::Map_t::const_iterator iter = m_settings.GetProjects().begin();
    for(; iter != m_settings.GetProjects().end(); ++iter) {
        wxString bmpfile = iter->second.GetBitmapFilename();
        bmpfile.Trim().Trim(false);
        if(bmpfile.IsEmpty()) {
            continue;
        }
        wxBitmap bmp(bmpfile, wxBITMAP_TYPE_ANY);
        if(bmp.IsOk()) {
            wxIcon icn;
            icn.CopyFromBitmap(bmp);
            int index = images->Add(icn);
            m_project2Icon.insert(std::make_pair(iter->first, index));
        }
    }

    if(m_project2Icon.empty()) {
        e.Skip();
        wxDELETE(images);

    } else {
        // send back the new image list
        e.SetClientData(images);
    }
}

void Tweaks::OnCustomizeProject(clColourEvent& e)
{
    TWEAKS_ENABLED_EVENT_HANDLER();
    if(m_project2Icon.count(e.GetString())) {
        // We got a new icon for this project!
        e.SetInt(m_project2Icon.find(e.GetString())->second);

    } else {
        e.Skip();
    }
}

void Tweaks::OnTabBorderColour(clColourEvent& e)
{
#ifdef __WXGTK__
#if CL_USE_NATIVEBOOK
    // Not supported with native notebooks
    e.Skip();
    return;
#endif
#endif
    TWEAKS_ENABLED_EVENT_HANDLER();

    if(m_settings.GetGlobalBgColour().IsOk()) {
        // return the tab colour
        e.SetBorderColour(DrawingUtils::DarkColour(m_settings.GetGlobalBgColour(), 1.5));
    } else {
        e.Skip();
    }
}
