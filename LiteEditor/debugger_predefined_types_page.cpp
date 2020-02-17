//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : debugger_predefined_types_page.cpp
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

#include "dbgcommanddlg.h"
#include "debugger_predefined_types_page.h"
#include "globals.h"
#include <wx/msgdlg.h>

PreDefinedTypesPage::PreDefinedTypesPage(wxWindow* parent, const DebuggerPreDefinedTypes& preDefTypes)
    : PreDefinedTypesPageBase(parent)
    , m_selectedItem(wxNOT_FOUND)
{
    m_data = preDefTypes;
    m_listCtrl1->InsertColumn(0, _("Type"));
    m_listCtrl1->InsertColumn(1, _("Expression"));
    m_listCtrl1->InsertColumn(2, _("Debugger Command"));

    // Populate the list with the items from the configuration file
    DebuggerCmdDataVec cmds = m_data.GetCmds();
    for(size_t i = 0; i < cmds.size(); i++) {
        DebuggerCmdData cmd = cmds.at(i);

        long item = AppendListCtrlRow(m_listCtrl1);
        SetColumnText(m_listCtrl1, item, 0, cmd.GetName());
        SetColumnText(m_listCtrl1, item, 1, cmd.GetCommand());
        SetColumnText(m_listCtrl1, item, 2, cmd.GetDbgCommand());
    }
    m_listCtrl1->SetColumnWidth(0, 100);
    m_listCtrl1->SetColumnWidth(1, 200);
}

void PreDefinedTypesPage::OnItemActivated(wxListEvent& event)
{
    m_selectedItem = event.m_itemIndex;
    DoEditItem();
}

void PreDefinedTypesPage::OnItemDeselected(wxListEvent& event)
{
    wxUnusedVar(event);
    m_selectedItem = wxNOT_FOUND;
}

void PreDefinedTypesPage::OnItemSelected(wxListEvent& event) { m_selectedItem = event.m_itemIndex; }

void PreDefinedTypesPage::OnNewShortcut(wxCommandEvent& event)
{
    wxUnusedVar(event);
    DbgCommandDlg dlg(this);

    if(dlg.ShowModal() == wxID_OK) {

        // add new command to the table
        wxString name = dlg.GetName();
        wxString expression = dlg.GetExpression();
        wxString dbgCmd = dlg.GetDbgCommand();

        // Make sure that the expression does not exist
        int count = m_listCtrl1->GetItemCount();
        for(int i = 0; i < count; i++) {
            wxString existingName = GetColumnText(m_listCtrl1, i, 0);
            if(name == existingName) {
                wxMessageBox(_("A Debugger type with that name already exists"), _("CodeLite"),
                             wxOK | wxICON_INFORMATION);
                return;
            }
        }

        // Set the item display name
        long item = ::AppendListCtrlRow(m_listCtrl1);

        SetColumnText(m_listCtrl1, item, 0, name);
        SetColumnText(m_listCtrl1, item, 1, expression);
        SetColumnText(m_listCtrl1, item, 2, dbgCmd);

        m_listCtrl1->SetColumnWidth(0, -1);
        m_listCtrl1->SetColumnWidth(1, -1);
        m_listCtrl1->SetColumnWidth(2, -1);
    }
}

void PreDefinedTypesPage::OnEditShortcut(wxCommandEvent& event)
{
    wxUnusedVar(event);
    DoEditItem();
}

void PreDefinedTypesPage::OnDeleteShortcut(wxCommandEvent& event)
{
    wxUnusedVar(event);
    DoDeleteItem();
}

void PreDefinedTypesPage::DoEditItem()
{
    // Edit the selection
    if(m_selectedItem == wxNOT_FOUND) {
        return;
    }

    // popup edit dialog
    DbgCommandDlg dlg(this);

    wxString name = GetColumnText(m_listCtrl1, m_selectedItem, 0);
    wxString expr = GetColumnText(m_listCtrl1, m_selectedItem, 1);
    wxString dbgCmd = GetColumnText(m_listCtrl1, m_selectedItem, 2);

    dlg.SetName(name);
    dlg.SetExpression(expr);
    dlg.SetDbgCommand(dbgCmd);

    if(dlg.ShowModal() == wxID_OK) {
        SetColumnText(m_listCtrl1, m_selectedItem, 0, dlg.GetName());
        SetColumnText(m_listCtrl1, m_selectedItem, 1, dlg.GetExpression());
        SetColumnText(m_listCtrl1, m_selectedItem, 2, dlg.GetDbgCommand());
    }
}

void PreDefinedTypesPage::DoDeleteItem()
{
    if(m_selectedItem == wxNOT_FOUND) {
        return;
    }
    m_listCtrl1->DeleteItem(m_selectedItem);
    m_selectedItem = wxNOT_FOUND;
}

DebuggerPreDefinedTypes PreDefinedTypesPage::GetPreDefinedTypes()
{
    int count = m_listCtrl1->GetItemCount();
    DebuggerCmdDataVec cmdArr;

    for(int i = 0; i < count; i++) {
        DebuggerCmdData cmd;
        cmd.SetName(GetColumnText(m_listCtrl1, i, 0));
        cmd.SetCommand(GetColumnText(m_listCtrl1, i, 1));
        cmd.SetDbgCommand(GetColumnText(m_listCtrl1, i, 2));
        cmdArr.push_back(cmd);
    }

    m_data.SetCmds(cmdArr);
    return m_data;
}
