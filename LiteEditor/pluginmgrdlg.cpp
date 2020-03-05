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
#include "cl_config.h"
#include "globals.h"
#include "manager.h"
#include "pluginmanager.h"
#include "pluginmgrdlg.h"
#include "windowattrmanager.h"
#include <algorithm>

PluginMgrDlg::PluginMgrDlg(wxWindow* parent)
    : PluginMgrDlgBase(parent)
{
    this->Initialize();
    ::clSetSmallDialogBestSizeAndPosition(this);
}

PluginMgrDlg::~PluginMgrDlg() {}

void PluginMgrDlg::Initialize()
{
    clConfig conf("plugins.conf");
    PluginInfoArray plugins;
    conf.ReadItem(&plugins);

    m_initialEnabledPlugins = plugins.GetEnabledPlugins();
    std::sort(m_initialEnabledPlugins.begin(), m_initialEnabledPlugins.end());

    const PluginInfo::PluginMap_t& pluginsMap = PluginManager::Get()->GetInstalledPlugins();

    // Clear the list
    m_dvListCtrl->DeleteAllItems();
    for(const auto& vt : pluginsMap) {
        const PluginInfo& info = vt.second;
        wxVector<wxVariant> cols;
        cols.push_back(::MakeCheckboxVariant(info.GetName(), plugins.CanLoad(info), wxNOT_FOUND));
        m_dvListCtrl->AppendItem(cols);
    }

    if(!m_dvListCtrl->IsEmpty()) {
        m_dvListCtrl->Select(m_dvListCtrl->RowToItem(0));
        CreateInfoPage(0);
    }
}

void PluginMgrDlg::OnItemSelected(wxDataViewEvent& event)
{
    wxDataViewItem item = event.GetItem();
    CreateInfoPage(m_dvListCtrl->ItemToRow(item));
}

void PluginMgrDlg::OnButtonOK(wxCommandEvent& event)
{
    clConfig conf("plugins.conf");
    PluginInfoArray plugins;
    conf.ReadItem(&plugins);

    wxArrayString enabledPlugins;
    for(size_t i = 0; i < m_dvListCtrl->GetItemCount(); ++i) {
        wxDataViewItem item = m_dvListCtrl->RowToItem(i);
        if(m_dvListCtrl->IsItemChecked(item)) {
            enabledPlugins.Add(m_dvListCtrl->GetItemText(item));
        }
    }

    std::sort(enabledPlugins.begin(), enabledPlugins.end());
    plugins.EnablePlugins(enabledPlugins);
    conf.WriteItem(&plugins);
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
    conf.ReadItem(&plugins);

    m_richTextCtrl->Clear();
    m_richTextCtrl->Freeze();
    m_richTextCtrl->SetEditable(true);
    // get the plugin name
    wxString pluginName = m_dvListCtrl->GetItemText(m_dvListCtrl->RowToItem(index));
    auto iter = PluginManager::Get()->GetInstalledPlugins().find(pluginName);
    if(iter != plugins.GetPlugins().end()) {
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
    for(size_t i = 0; i < m_dvListCtrl->GetItemCount(); ++i) {
        m_dvListCtrl->SetItemChecked(m_dvListCtrl->RowToItem(i), true);
    }
}

void PluginMgrDlg::OnCheckAllUI(wxUpdateUIEvent& event)
{
    bool atLeastOneIsUnChecked = false;
    for(size_t i = 0; i < m_dvListCtrl->GetItemCount(); ++i) {
        wxDataViewItem item = m_dvListCtrl->RowToItem(i);
        if(!m_dvListCtrl->IsItemChecked(item)) {
            atLeastOneIsUnChecked = true;
            break;
        }
    }
    event.Enable(atLeastOneIsUnChecked);
}

void PluginMgrDlg::OnUncheckAll(wxCommandEvent& event)
{
    for(size_t i = 0; i < m_dvListCtrl->GetItemCount(); ++i) {
        m_dvListCtrl->SetItemChecked(m_dvListCtrl->RowToItem(i), false);
    }
}

void PluginMgrDlg::OnUncheckAllUI(wxUpdateUIEvent& event)
{
    bool atLeastOneIsChecked = false;
    for(size_t i = 0; i < m_dvListCtrl->GetItemCount(); ++i) {
        wxDataViewItem item = m_dvListCtrl->RowToItem(i);
        if(m_dvListCtrl->IsItemChecked(item)) {
            atLeastOneIsChecked = true;
            break;
        }
    }
    event.Enable(atLeastOneIsChecked);
}
