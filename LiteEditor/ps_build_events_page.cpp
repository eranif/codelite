//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : ps_build_events_page.cpp
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

#include "free_text_dialog.h"
#include "globals.h"
#include "macros.h"
#include "ps_build_events_page.h"
#include <wx/tokenzr.h>

PSBuildEventsPage::PSBuildEventsPage(wxWindow* parent, bool preEvents, ProjectSettingsDlg* dlg)
    : PSBuildEventsBasePage(parent)
    , m_isPreEvents(preEvents)
    , m_dlg(dlg)
{
}

void PSBuildEventsPage::OnCmdEvtVModified(wxCommandEvent& event)
{
    wxUnusedVar(event);
    m_dlg->SetIsDirty(true);
}

void PSBuildEventsPage::Load(BuildConfigPtr buildConf)
{
    Clear();

    BuildCommandList buildCmds;
    wxString text;
    if(m_isPreEvents) {
        buildConf->GetPreBuildCommands(buildCmds);
        text = _("Set the commands to run in the pre build stage");

    } else {
        buildConf->GetPostBuildCommands(buildCmds);
        text = _("Set the commands to run in the post build stage");
    }
    text << _("\nCommands starting with the hash sign ('#'), will not be executed");
    m_staticText11->SetLabel(text);
    BuildCommandList::const_iterator iter = buildCmds.begin();
    m_textCtrlBuildEvents->ClearAll();
    for(; iter != buildCmds.end(); iter++) {
        wxString cmdText = iter->GetCommand();
        cmdText.Trim().Trim(false);
        if(iter->GetEnabled() == false && !cmdText.StartsWith(wxT("#"))) { cmdText.Prepend(wxT("#")); }
        cmdText.Append(wxT("\n"));
        m_textCtrlBuildEvents->AppendText(cmdText);
    }

    m_textCtrlBuildEvents->Connect(wxEVT_COMMAND_TEXT_UPDATED,
                                   wxCommandEventHandler(PSBuildEventsPage::OnCmdEvtVModified), NULL, this);
}

void PSBuildEventsPage::Save(BuildConfigPtr buildConf, ProjectSettingsPtr projSettingsPtr)
{
    BuildCommandList cmds;
    wxArrayString commands = ::SplitString(m_textCtrlBuildEvents->GetValue(), true);
    for(size_t i = 0; i < commands.GetCount(); i++) {
        wxString command = commands.Item(i).Trim().Trim(false);
        bool enabled = !command.StartsWith(wxT("#"));
        BuildCommand cmd(command, enabled);
        cmds.push_back(cmd);
    }

    if(m_isPreEvents) {
        buildConf->SetPreBuildCommands(cmds);

    } else {
        buildConf->SetPostBuildCommands(cmds);
    }
}

void PSBuildEventsPage::Clear() { m_textCtrlBuildEvents->Clear(); }
void PSBuildEventsPage::OnProjectEnabledUI(wxUpdateUIEvent& event) { event.Enable(m_dlg->IsProjectEnabled()); }
void PSBuildEventsPage::OnBuildEventCharAdded(wxStyledTextEvent& event)
{
    event.Skip();
    m_dlg->SetIsDirty(true);
}
