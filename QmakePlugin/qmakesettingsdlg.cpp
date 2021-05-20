//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : qmakesettingsdlg.cpp
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

#include "globals.h"
#include "imanager.h"
#include "qmakeconf.h"
#include "qmakesettingsdlg.h"
#include "qmakesettingstab.h"
#include "windowattrmanager.h"
#include <wx/log.h>
#include <wx/menu.h>
#include <wx/msgdlg.h>
#include <wx/textdlg.h>
#include <wx/xrc/xmlres.h>

QMakeSettingsDlg::QMakeSettingsDlg(wxWindow* parent, IManager* manager, QmakeConf* conf)
    : QMakeSettingsBaseDlg(parent)
    , m_manager(manager)
    , m_conf(conf)
    , m_rightClickTabIdx(wxNOT_FOUND)
{
    Initialize();
    SetName("QMakeSettingsDlg");
    ::clSetSmallDialogBestSizeAndPosition(this);
}

void QMakeSettingsDlg::Initialize()
{
    if(!m_conf) {
        return;
    }

    wxString group;
    long idx;
    bool cont = m_conf->GetFirstGroup(group, idx);
    while(cont) {

        QmakeSettingsTab* p = new QmakeSettingsTab(m_notebook, group, m_conf);
        m_notebook->AddPage(p, group);

        cont = m_conf->GetNextGroup(group, idx);
    }
}

void QMakeSettingsDlg::OnNewQmakeSettings(wxCommandEvent& event)
{
    wxUnusedVar(event);

    wxString name = wxGetTextFromUser(_("New qmake settings name"), _("New qmake settings"));
    if(name.IsEmpty() == false) {
        m_notebook->AddPage(new QmakeSettingsTab(m_notebook, name, m_conf), name, true);
    }
}

void QMakeSettingsDlg::OnOK(wxCommandEvent& event)
{
    // save the data and dismiss the dialog
    m_conf->DeleteAll();
    m_conf->Flush();

    for(size_t i = 0; i < m_notebook->GetPageCount(); i++) {
        QmakeSettingsTab* tab = dynamic_cast<QmakeSettingsTab*>(m_notebook->GetPage(i));
        if(tab) {
            tab->Save(m_conf);
        }
    }
    EndModal(wxID_OK);
}

QMakeSettingsDlg::~QMakeSettingsDlg() {}

void QMakeSettingsDlg::OnRightDown(wxMouseEvent& event)
{
    long flags(0);
    m_rightClickTabIdx = m_notebook->HitTest(event.GetPosition(), &flags);
    if(m_rightClickTabIdx != wxNOT_FOUND && flags & wxNB_HITTEST_ONLABEL) {
        wxMenu menu;

        menu.Append(XRCID("rename_qmake"), _("Rename..."), wxEmptyString);
        menu.Append(XRCID("delete_qmake"), _("Delete"), wxEmptyString);

        menu.Connect(XRCID("rename_qmake"), wxEVT_COMMAND_MENU_SELECTED,
                     wxCommandEventHandler(QMakeSettingsDlg::OnRename), NULL, this);
        menu.Connect(XRCID("delete_qmake"), wxEVT_COMMAND_MENU_SELECTED,
                     wxCommandEventHandler(QMakeSettingsDlg::OnDelete), NULL, this);

        m_notebook->PopupMenu(&menu);
    }
}

void QMakeSettingsDlg::OnDelete(wxCommandEvent& event)
{
    if(m_rightClickTabIdx != wxNOT_FOUND) {
        wxString qmakeSettingsName = m_notebook->GetPageText((size_t)m_rightClickTabIdx);
        int answer = wxMessageBox(
            wxString::Format(_("Are you sure you want to delete qmake settings '%s'?"), qmakeSettingsName.c_str()),
            _("CodeLite"), wxYES_NO | wxCANCEL, this);
        if(answer == wxYES) {
            m_notebook->DeletePage((size_t)m_rightClickTabIdx);
        }
    }
}

void QMakeSettingsDlg::OnRename(wxCommandEvent& event)
{
    if(m_rightClickTabIdx != wxNOT_FOUND) {
        wxString qmakeSettingsName = m_notebook->GetPageText((size_t)m_rightClickTabIdx);
        wxString newName = wxGetTextFromUser(_("New name:"), _("Rename..."));
        if(newName.empty() == false) {
            QmakeSettingsTab* tab = dynamic_cast<QmakeSettingsTab*>(m_notebook->GetPage(m_rightClickTabIdx));
            if(tab) {
                tab->SetTabName(newName);
                m_notebook->SetPageText((size_t)m_rightClickTabIdx, newName);
            }
        }
    }
}
