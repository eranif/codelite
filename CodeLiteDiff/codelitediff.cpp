//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : codelitediff.cpp
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

#include "codelitediff.h"
#include <wx/xrc/xmlres.h>
#include "DiffSideBySidePanel.h"
#include "wx/menu.h"
#include "clKeyboardManager.h"

static CodeLiteDiff* thePlugin = NULL;

// Define the plugin entry point
CL_PLUGIN_API IPlugin* CreatePlugin(IManager* manager)
{
    if(thePlugin == 0) {
        thePlugin = new CodeLiteDiff(manager);
    }
    return thePlugin;
}

CL_PLUGIN_API PluginInfo* GetPluginInfo()
{
    static PluginInfo info;
    info.SetAuthor(wxT("Eran Ifrah"));
    info.SetName(wxT("Diff Plugin"));
    info.SetDescription(_("CodeLite Diff Plugin"));
    info.SetVersion(wxT("v1.0"));
    return &info;
}

CL_PLUGIN_API int GetPluginInterfaceVersion() { return PLUGIN_INTERFACE_VERSION; }

CodeLiteDiff::CodeLiteDiff(IManager* manager)
    : IPlugin(manager)
{
    m_longName = _("CodeLite Diff Plugin");
    m_shortName = wxT("Diff Plugin");

    clKeyboardManager::Get()->AddGlobalAccelerator(
        "diff_new_comparison", "Ctrl-Shift-C", "Plugins::Diff Tool::New File Comparison");
}

CodeLiteDiff::~CodeLiteDiff() {}

clToolBar* CodeLiteDiff::CreateToolBar(wxWindow* parent)
{
    // Create the toolbar to be used by the plugin
    clToolBar* tb(NULL);
    return tb;
}

void CodeLiteDiff::CreatePluginMenu(wxMenu* pluginsMenu)
{
    wxMenu* menu = new wxMenu;
    menu->Append(XRCID("diff_new_comparison"), _("New Diff.."), _("Start new diff"));
    menu->Bind(wxEVT_COMMAND_MENU_SELECTED, &CodeLiteDiff::OnNewDiff, this, XRCID("diff_new_comparison"));
    pluginsMenu->Append(wxID_ANY, _("Diff Tool"), menu);
}

void CodeLiteDiff::UnPlug() {}

void CodeLiteDiff::OnNewDiff(wxCommandEvent& e)
{
    DiffSideBySidePanel* diff = new DiffSideBySidePanel(m_mgr->GetEditorPaneNotebook());
    diff->DiffNew(); // Indicate that we want a clean diff, not from a source control
    m_mgr->AddPage(diff, _("Diff"), wxEmptyString, wxNullBitmap, true);
}
