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

#include "cl_config.h"
#include "globals.h"
#include "manager.h"
#include "pluginmanager.h"
#include "windowattrmanager.h"

#include <algorithm>

namespace
{
// The columns of the plugins list: a checkbox and the plugin name
constexpr unsigned kColCheck = 0;
constexpr unsigned kColName = 1;
} // namespace

PluginMgrDlg::PluginMgrDlg(wxWindow* parent)
    : PluginMgrDlgBase(parent)
{
    this->Initialize();
    m_typeHelper = std::make_unique<DataViewTypeHelper>(m_dvListCtrl);
    ::clSetSmallDialogBestSizeAndPosition(*this);
    ::AdjustDataViewAlternateColour(m_dvListCtrl);
    m_splitter->SetSashPosition(350);
}

void PluginMgrDlg::Initialize()
{
    clConfig conf("plugins.conf");
    PluginInfoArray plugins;
    conf.ReadItem(plugins);

    m_initialEnabledPlugins = plugins.GetEnabledPlugins();
    std::sort(m_initialEnabledPlugins.begin(), m_initialEnabledPlugins.end());

    const PluginInfo::PluginMap_t& pluginsMap = PluginManager::Get()->GetInstalledPlugins();

    // Columns: a checkbox and the plugin name (add them only if the base class did not create them)
    if (m_dvListCtrl->GetColumnCount() == 0) {
        m_dvListCtrl->AppendToggleColumn(
            _("Enabled"), wxDATAVIEW_CELL_ACTIVATABLE, wxCOL_WIDTH_AUTOSIZE, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE);
        m_dvListCtrl->AppendTextColumn(
            _("Plugins"), wxDATAVIEW_CELL_INERT, wxCOL_WIDTH_AUTOSIZE, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE);
    }

    // Clear the list
    m_dvListCtrl->DeleteAllItems();
    for (const auto& vt : pluginsMap) {
        const PluginInfo& info = vt.second;
        wxVector<wxVariant> cols;
        cols.push_back(wxVariant(plugins.CanLoad(info)));
        cols.push_back(wxVariant(info.GetName()));
        m_dvListCtrl->AppendItem(cols);
    }

    if (m_dvListCtrl->GetItemCount() > 0) {
        m_dvListCtrl->SelectRow(0);
        CreateInfoPage(0);
    }
}

void PluginMgrDlg::OnItemSelected(wxDataViewEvent& event)
{
    const int row = m_dvListCtrl->ItemToRow(event.GetItem());
    if (row != wxNOT_FOUND) {
        CreateInfoPage(row);
    }
}

void PluginMgrDlg::OnButtonOK(wxCommandEvent& event)
{
    clConfig conf("plugins.conf");
    PluginInfoArray plugins;
    conf.ReadItem(plugins);

    wxArrayString enabledPlugins;
    for (int i = 0; i < m_dvListCtrl->GetItemCount(); ++i) {
        if (m_dvListCtrl->GetToggleValue(i, kColCheck)) {
            enabledPlugins.Add(m_dvListCtrl->GetTextValue(i, kColName));
        }
    }

    std::sort(enabledPlugins.begin(), enabledPlugins.end());
    plugins.EnablePlugins(enabledPlugins);
    conf.WriteItem(plugins);
    EndModal(enabledPlugins == m_initialEnabledPlugins ? wxID_CANCEL : wxID_OK);
}

void PluginMgrDlg::WritePropertyLine(const wxString& label, const wxString& text)
{
    m_richTextCtrl->BeginBold();
    m_richTextCtrl->WriteText(label + " : ");
    m_richTextCtrl->EndBold();
    m_richTextCtrl->WriteText(text);
}

void PluginMgrDlg::CreateInfoPage(unsigned int index)
{
    clConfig conf("plugins.conf");
    PluginInfoArray plugins;
    conf.ReadItem(plugins);

    m_richTextCtrl->Clear();
    m_richTextCtrl->Freeze();
    m_richTextCtrl->SetEditable(true);
    // get the plugin name
    wxString pluginName = m_dvListCtrl->GetTextValue(index, kColName);
    auto iter = PluginManager::Get()->GetInstalledPlugins().find(pluginName);
    if (iter != plugins.GetPlugins().end()) {
        const PluginInfo& info = iter->second;
        m_richTextCtrl->BeginBold();
        m_richTextCtrl->WriteText(info.GetName());
        m_richTextCtrl->EndBold();
        m_richTextCtrl->Newline();

        WritePropertyLine(_("Version"), info.GetVersion());
        m_richTextCtrl->Newline();

        WritePropertyLine(_("Author"), info.GetAuthor());
        m_richTextCtrl->Newline();

        WritePropertyLine(_("Is Loaded?"), plugins.CanLoad(info) ? _("Yes") : _("No"));
        m_richTextCtrl->Newline();
        m_richTextCtrl->Newline();

        m_richTextCtrl->BeginBold();
        m_richTextCtrl->WriteText(_("Description:"));
        m_richTextCtrl->EndBold();
        m_richTextCtrl->Newline();
        m_richTextCtrl->WriteText(info.GetDescription());
    }
    m_richTextCtrl->SetEditable(false);
    m_richTextCtrl->Thaw();
}

void PluginMgrDlg::OnCheckAll(wxCommandEvent& event)
{
    for (int i = 0; i < m_dvListCtrl->GetItemCount(); ++i) {
        m_dvListCtrl->SetToggleValue(true, i, kColCheck);
    }
}

void PluginMgrDlg::OnCheckAllUI(wxUpdateUIEvent& event)
{
    bool atLeastOneIsUnChecked = false;
    for (int i = 0; i < m_dvListCtrl->GetItemCount(); ++i) {
        if (!m_dvListCtrl->GetToggleValue(i, kColCheck)) {
            atLeastOneIsUnChecked = true;
            break;
        }
    }
    event.Enable(atLeastOneIsUnChecked);
}

void PluginMgrDlg::OnUncheckAll(wxCommandEvent& event)
{
    for (int i = 0; i < m_dvListCtrl->GetItemCount(); ++i) {
        m_dvListCtrl->SetToggleValue(false, i, kColCheck);
    }
}

void PluginMgrDlg::OnUncheckAllUI(wxUpdateUIEvent& event)
{
    bool atLeastOneIsChecked = false;
    for (int i = 0; i < m_dvListCtrl->GetItemCount(); ++i) {
        if (m_dvListCtrl->GetToggleValue(i, kColCheck)) {
            atLeastOneIsChecked = true;
            break;
        }
    }
    event.Enable(atLeastOneIsChecked);
}
