//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : pluginmgrdlg.cpp
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
#include "pluginmgrdlg.h"
#include "windowattrmanager.h"
#include "manager.h"
#include "cl_config.h"
#include <algorithm>

PluginMgrDlg::PluginMgrDlg( wxWindow* parent )
    : PluginMgrDlgBase( parent )
{
    this->Initialize();
    WindowAttrManager::Load(this, wxT("PluginMgrDlgAttr"), NULL);
}

PluginMgrDlg::~PluginMgrDlg()
{
    WindowAttrManager::Save(this, wxT("PluginMgrDlgAttr"), NULL);
}

void PluginMgrDlg::Initialize()
{
    clConfig conf("plugins.conf");
    PluginInfoArray plugins;
    conf.ReadItem(&plugins);

    m_initialDisabledPlugins = plugins.GetDisabledPlugins();
    std::sort(m_initialDisabledPlugins.begin(), m_initialDisabledPlugins.end());

    const PluginInfo::PluginMap_t& pluginsMap = plugins.GetPlugins();

    //Clear the list
    m_checkListPluginsList->Clear();

    PluginInfo::PluginMap_t::const_iterator iter = pluginsMap.begin();
    for ( ; iter != pluginsMap.end(); ++iter ) {
        PluginInfo info = iter->second;

        int item = m_checkListPluginsList->Append(info.GetName());
        if (item != wxNOT_FOUND) {
            m_checkListPluginsList->Check((unsigned int)item, plugins.CanLoad(info.GetName()));
        }
    }

    if (m_checkListPluginsList->IsEmpty() == false) {
        m_checkListPluginsList->Select(0);
        CreateInfoPage(0);
    }
    m_checkListPluginsList->SetFocus();
}

void PluginMgrDlg::OnItemSelected(wxCommandEvent &event)
{
    int item = event.GetSelection();
    CreateInfoPage((unsigned int)item);
}

void PluginMgrDlg::OnButtonOK(wxCommandEvent &event)
{
    clConfig conf("plugins.conf");
    PluginInfoArray plugins;
    conf.ReadItem(&plugins);

    wxArrayString disabledPlugins;
    for (unsigned int i = 0; i<m_checkListPluginsList->GetCount(); i++) {
        if ( m_checkListPluginsList->IsChecked(i) == false ) {
            disabledPlugins.Add( m_checkListPluginsList->GetString(i) );
        }
    }

    std::sort(disabledPlugins.begin(), disabledPlugins.end());
    plugins.DisablePugins( disabledPlugins );
    conf.WriteItem( &plugins );

    EndModal( disabledPlugins == m_initialDisabledPlugins ? wxID_CANCEL : wxID_OK );
}

void PluginMgrDlg::CreateInfoPage(unsigned int index)
{
    clConfig conf("plugins.conf");
    PluginInfoArray plugins;
    conf.ReadItem(&plugins);

    //get the plugin name
    wxString pluginName = m_checkListPluginsList->GetString(index);
    PluginInfo::PluginMap_t::const_iterator iter = plugins.GetPlugins().find(pluginName);
    if (iter != plugins.GetPlugins().end()) {
        PluginInfo info = iter->second;

        wxString content;
        content << wxT("<html><body>");
        content << wxT("<table border=0 width=\"100%\" >");

        //create line with the plugin name
        content << wxT("<tr bgcolor=\"LIGHT GREY\">");
        content << wxT("<td ALIGN=\"LEFT\" WIDTH=30%><font size=\"2\" face=\"Verdana\"><strong>$(PluginName)</strong></font></td>");
        content << wxT("<td ALIGN=\"LEFT\" ><font size=\"2\" face=\"Verdana\">") << info.GetName() << wxT("</font></td>");
        content << wxT("</tr>");

        //plugin author
        content << wxT("<tr bgcolor=\"WHITE\">");
        content << wxT("<td ALIGN=\"LEFT\" WIDTH=30%><font size=\"2\" face=\"Verdana\"><strong>$(Author)</strong></font></td>");
        content << wxT("<td ALIGN=\"LEFT\" ><font size=\"2\" face=\"Verdana\">") << info.GetAuthor() << wxT("</font></td>");
        content << wxT("</tr>");

        //plugin version
        content << wxT("<tr bgcolor=\"LIGHT GREY\">");
        content << wxT("<td ALIGN=\"LEFT\" WIDTH=30%><font size=\"2\" face=\"Verdana\"><strong>$(Version)</strong></font></td>");
        content << wxT("<td ALIGN=\"LEFT\" ><font size=\"2\" face=\"Verdana\">") << info.GetVersion() << wxT("</font></td>");
        content << wxT("</tr>");

        //plugin description
        content << wxT("<tr bgcolor=\"WHITE\">");
        content << wxT("<td ALIGN=\"LEFT\" WIDTH=30%><font size=\"2\" face=\"Verdana\"><strong>$(Description)</strong></font></td>");
        content << wxT("<td ALIGN=\"LEFT\" ><font size=\"2\" face=\"Verdana\">") << info.GetDescription() << wxT("</font></td>");
        content << wxT("</tr>");

        content << wxT("<tr bgcolor=\"LIGHT GREY\">");
        content << wxT("<td ALIGN=\"LEFT\" WIDTH=30%><font size=\"2\" face=\"Verdana\"><strong>$(Status)</strong></font></td>");

        content.Replace(wxT("$(PluginName)"), _("Plugin Name:"));
        content.Replace(wxT("$(Author)"), _("Author:"));
        content.Replace(wxT("$(Version)"), _("Version:"));
        content.Replace(wxT("$(Description)"), _("Description:"));
        content.Replace(wxT("$(Status)"), _("Status:"));

        wxString status;
        if ( plugins.CanLoad(info.GetName()) ) {
            status = wxT("<img src=\"$(InstallPath)/images/plugin_ok.png\" ></img>");
        } else {
            status = wxT("<img src=\"$(InstallPath)/images/plugin_not_ok.png\" > </img>");
        }
        status.Replace(wxT("$(InstallPath)"), ManagerST::Get()->GetStartupDirectory());

        content << wxT("<td ALIGN=\"LEFT\" ><font size=\"2\" face=\"Verdana\">") << status << wxT("</font></td>");
        content << wxT("</tr>");

        content << wxT("</table><html><body>");

        m_htmlWinDesc->SetPage(content);
    }
}
void PluginMgrDlg::OnCheckAll(wxCommandEvent& event)
{
    for(size_t i=0; i<m_checkListPluginsList->GetCount(); ++i) {
        m_checkListPluginsList->Check(i, true);
    }
}

void PluginMgrDlg::OnCheckAllUI(wxUpdateUIEvent& event)
{
    bool atLeastOneIsUnChecked = false;
    for(size_t i=0; i<m_checkListPluginsList->GetCount(); ++i) {
        if ( !m_checkListPluginsList->IsChecked(i) ) {
            atLeastOneIsUnChecked = true;
            break;
        }
    }
    event.Enable( atLeastOneIsUnChecked );
}

void PluginMgrDlg::OnUncheckAll(wxCommandEvent& event)
{
    for(size_t i=0; i<m_checkListPluginsList->GetCount(); ++i) {
        m_checkListPluginsList->Check(i, false);
    }
}

void PluginMgrDlg::OnUncheckAllUI(wxUpdateUIEvent& event)
{
    bool atLeastOneIsChecked = false;
    for(size_t i=0; i<m_checkListPluginsList->GetCount(); ++i) {
        if ( m_checkListPluginsList->IsChecked(i) ) {
            atLeastOneIsChecked = true;
            break;
        }
    }
    event.Enable( atLeastOneIsChecked );
}
