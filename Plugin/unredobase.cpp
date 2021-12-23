//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : unredobase.cpp
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

#include "unredobase.h"

#include "clToolBar.h"
#include "macros.h"

#include <algorithm>
#include <wx/app.h>
#include <wx/menu.h>
#include <wx/xrc/xmlres.h>

const int FIRST_MENU_ID = 10000;

wxString GetBestLabel(CLCommand::Ptr_t command)
{
    wxString label, text;
    if(command) {
        if(!command->GetUserLabel().empty()) {
            label = command->GetUserLabel();
        } else {
            label = command->GetName();
            text = command->GetText();
            size_t len = text.Len();
            if(len) {
                text.Replace("\r\n", "\\n"); // Otherwise newlines result in a multiline display!
                text.Replace("\n", "\\n");
                // Truncate long pastes
                if(len > 70) {
                    wxString shorter = text.Left(34);
                    shorter << " ... " << text.Right(34);
                    text = shorter;
                }
                label << " \"" << text << "\"";
            }
        }
    }
    return label;
}

CommandProcessorBase::CommandProcessorBase()
    : m_currentCommand(-1)
{
    Bind(wxEVT_TOOL_DROPDOWN, &CommandProcessorBase::OnTBUnRedo, this, wxID_UNDO);
    Bind(wxEVT_TOOL_DROPDOWN, &CommandProcessorBase::OnTBUnRedo, this, wxID_REDO);
}

CommandProcessorBase::~CommandProcessorBase()
{
    Clear();
    Unbind(wxEVT_TOOL_DROPDOWN, &CommandProcessorBase::OnTBUnRedo, this, wxID_UNDO);
    Unbind(wxEVT_TOOL_DROPDOWN, &CommandProcessorBase::OnTBUnRedo, this, wxID_REDO);
}

void CommandProcessorBase::ProcessOpenCommand()
{
    CLCommand::Ptr_t command = GetOpenCommand();
    wxCHECK_RET(command, "Trying to process a non-existing or non-open command");

    command->SetName(GetBestLabel(command)); // Update the item's label
    CloseOpenCommand();
}

CLCommand::Ptr_t CommandProcessorBase::GetOpenCommand()
{
    CLCommand::Ptr_t command(NULL);

    size_t size = GetCommands().size();
    if(size && GetCommands().at(size - 1)->IsOpen()) {
        command = GetCommands().at(size - 1);
    }

    return command;
}

void CommandProcessorBase::CloseOpenCommand()
{
    CLCommand::Ptr_t command = GetOpenCommand();
    wxCHECK_RET(command, "Trying to close to a non-existent or already-closed command");

    command->Close();
}

void CommandProcessorBase::IncrementCurrentCommand()
{
    wxCHECK_RET((m_currentCommand + 1) < (int)GetCommands().size(), "Can't increment the current command");
    ++m_currentCommand;
}

void CommandProcessorBase::DecrementCurrentCommand()
{
    wxCHECK_RET(m_currentCommand > -1, "Can't decrement the current command");

    // Close any open command. That makes sense anyway if we're undoing and, if we don't, the command doesn't get a
    // sensible label when it's displayed in the dropdown menu
    if(GetOpenCommand()) {
        ProcessOpenCommand();
    }

    --m_currentCommand;
}

void CommandProcessorBase::SetUserLabel(const wxString& label)
{
    if(GetOpenCommand()) {
        ProcessOpenCommand(); // We need to make it non-pending, otherwise it may change after the label is set; which
                              // probably won't be what the user expects
    }

    CLCommand::Ptr_t command = GetActiveCommand();
    if(command) {
        command->SetUserLabel(label);
    }
}

void CommandProcessorBase::OnTBUnRedo(wxCommandEvent& event)
{
    PopulateUnRedoMenu(dynamic_cast<clToolBar*>(event.GetEventObject()), event.GetId());
}

void CommandProcessorBase::PopulateUnRedoMenu(clToolBar* tb, wxWindowID toolId)
{
    CHECK_PTR_RET(tb);
    wxMenu menu;
    DoPopulateUnRedoMenu(menu, (toolId == wxID_UNDO));

    if(toolId == wxID_UNDO) {
        menu.Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(CommandProcessorBase::OnUndoDropdownItem), this);
        tb->ShowMenuForButton(toolId, &menu);
        menu.Unbind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(CommandProcessorBase::OnUndoDropdownItem), this);

    } else {
        menu.Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(CommandProcessorBase::OnRedoDropdownItem), this);
        tb->ShowMenuForButton(toolId, &menu);
        menu.Unbind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(CommandProcessorBase::OnRedoDropdownItem), this);
    }
}

void CommandProcessorBase::DoPopulateUnRedoMenu(wxMenu& menu, bool undoing)
{
    wxString prefix(undoing ? _("Undo ") : _("Redo "));
    int id = FIRST_MENU_ID;
    int count = 0;

    if(undoing) {
        if(GetCommands().size() > 0) {
            for(CLCommand::Vec_t::const_reverse_iterator iter = GetCommands().rbegin() + GetNextUndoCommand();
                iter != GetCommands().rend(); ++iter) {
                CLCommand::Ptr_t command = *iter;
                if(command) {
                    wxString label;
                    if(!command->GetUserLabel().empty()) {
                        if(command->GetName().Contains(":")) {
                            label = command->GetName().BeforeFirst(':') + ": ";
                        }
                        label << command->GetUserLabel();
                    } else {
                        if(command == GetOpenCommand()) {
                            label = GetBestLabel(
                                command); // If the command's still open, there won't otherwise be a name string
                        } else {
                            label = command->GetName();
                        }
                    }
                    menu.Append(id++, wxString::Format("%i ", ++count) + prefix + label);
                }
            }
        }
    } else {
        for(CLCommand::Vec_t::const_iterator iter = GetCommands().begin() + GetCurrentCommand() + 1;
            iter != GetCommands().end(); ++iter) {
            CLCommand::Ptr_t command = *iter;
            if(command) {
                wxString label;
                if(!command->GetUserLabel().empty()) {
                    if(command->GetName().Contains(":")) {
                        label = command->GetName().BeforeFirst(':') + ": ";
                    }
                    label << command->GetUserLabel();
                } else {
                    label = command->GetName();
                }
                menu.Append(id++, wxString::Format("%i ", ++count) + prefix + label);
            }
        }
    }
}

void CommandProcessorBase::PrepareLabelledStatesMenu(wxMenu* editmenu)
{
    // First remove any current labelled-state submenu, which will almost certainly hold stale data
    wxMenuItem* menuitem = editmenu->FindItem(XRCID("goto_labelled_state"));
    if(menuitem) {
        editmenu->Delete(menuitem);
    }

    // Find the item we want to insert _after_
    size_t pos;
    menuitem = editmenu->FindChildItem(XRCID("label_current_state"), &pos);
    wxCHECK_RET(menuitem && (int)pos > wxNOT_FOUND, "Failed to find the 'label_current_state' item");
    int kludge = 0;
    if(pos == 2) { // For some reason, the insertion point of the CL edit menu is otherwise off-by-1. The wxC frame one
                   // isn't :/
        kludge = 1;
    }

    // Get any labelled undo/redo states into the submenu. If none, abort.
    wxMenu* submenu = new wxMenu;
    PopulateLabelledStatesMenu(submenu);
    if(submenu->GetMenuItemCount()) {
        editmenu->Insert(pos + 1 + kludge, XRCID("goto_labelled_state"), _("Undo/Redo to a pre&viously labelled state"),
                         submenu);
    } else {
        delete submenu;
    }
}

void CommandProcessorBase::PopulateLabelledStatesMenu(wxMenu* menu)
{
    wxCHECK_RET(menu, "NULL menu");

    for(size_t n = menu->GetMenuItemCount(); n > 0; --n) { // We need to delete any items left over from a previous call
        wxMenuItem* item = menu->FindItemByPosition(n - 1);
        if(item) {
            menu->Delete(item);
        }
    }

    if(!GetInitialCommand()->GetUserLabel().empty()) { // First any initial label
        menu->Append(FIRST_MENU_ID - 1,
                     GetInitialCommand()->GetUserLabel()); // (Ab)use an out-of-range id to avoid confusion
    }

    int id = FIRST_MENU_ID;
    for(CLCommand::Vec_t::const_iterator iter = GetCommands().begin(); iter != GetCommands().end(); ++iter) {
        CLCommand::Ptr_t command = *iter;
        if(command && !command->GetUserLabel().empty()) {
            menu->Append(id, command->GetUserLabel());
        }
        ++id; // It's easier in the handler if the event id is the GetCommands() index, rather than a menu index
    }

    menu->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(CommandProcessorBase::OnLabelledStatesMenuItem),
               this);
}

void CommandProcessorBase::UnBindLabelledStatesMenu(wxMenu* menu)
{
    wxCHECK_RET(menu, "NULL menu");
    CallAfter(&CommandProcessorBase::DoUnBindLabelledStatesMenu,
              menu); // We can't Unbind yet: the event won't have been caught
}

void CommandProcessorBase::DoUnBindLabelledStatesMenu(wxMenu* menu)
{
    wxCHECK_RET(menu, "NULL menu");
    menu->Unbind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(CommandProcessorBase::OnLabelledStatesMenuItem),
                 this);
}

void CommandProcessorBase::OnLabelledStatesMenuItem(wxCommandEvent& event)
{
    if(GetOpenCommand()) {
        ProcessOpenCommand();
    }

    int index = event.GetId() - FIRST_MENU_ID; // NB index will be -1 if the selection is the initial-command's label.
                                               // This will always mean *un*dos (or nothing)
    wxCHECK_RET(index < (int)GetCommands().size(), "An ID that overruns the command-list");

    if(index < GetCurrentCommand()) {
        const int count = GetCurrentCommand() - index;
        for(int n = 0; n < count; ++n) {
            if(DoUndo()) {
                DecrementCurrentCommand();
            }
        }
    } else {
        const int count = index - GetCurrentCommand();
        for(int n = 0; n < count; ++n) {
            if(DoRedo()) {
                IncrementCurrentCommand();
            }
        }
    }
}

void CommandProcessorBase::OnUndoDropdownItem(wxCommandEvent& event)
{
    if(GetOpenCommand()) {
        ProcessOpenCommand();
    }

    const int count = event.GetId() - FIRST_MENU_ID + 1;
    for(int n = 0; n < count; ++n) {
        if(DoUndo()) {
            DecrementCurrentCommand();
        }
    }
}

void CommandProcessorBase::OnRedoDropdownItem(wxCommandEvent& event)
{
    const int count = event.GetId() - FIRST_MENU_ID + 1;
    for(int n = 0; n < count; ++n) {
        if(DoRedo()) {
            IncrementCurrentCommand();
        }
    }
}

CLCommand::Ptr_t CommandProcessorBase::GetActiveCommand() const
{
    CLCommand::Ptr_t command(NULL);

    if(GetCurrentCommand() == -1) {
        command = GetInitialCommand();

    } else if(GetCommands().size() > 0) {
        command = GetCommands().at(GetCurrentCommand());
    }

    return command;
}
