
#include "unredobase.h"
#include <wxmd5.h>
#include <wx/menu.h>


const int FIRST_MENU_ID = 10000;


wxString GetBestLabel(CLCommand* command)
{
    wxString label;
    if (command) {
        if (!command->GetUserLabel().empty()) {
            label = command->GetUserLabel();
        } else {
            label = command->GetName();
            size_t len = command->GetText().Len();
            if (len) {
                if (len < 6) {
                    label << " \"" << command->GetText() << "\"";
                } else {
                    label << " (" << command->GetText().Len() << "chars)";
                }
            }
        }
    }
    return label;
}

void CommandProcessorBase::ProcessCurrentCommand()
{
    CLCommand* command = GetOpenCommand();
    wxCHECK_RET(command, "Trying to process a non-existing or non-open command");

    command->SetName( GetBestLabel(command) );  // Update the item's label
    CloseOpenCommand();
}

CLCommand* CommandProcessorBase::GetOpenCommand()
{
    CLCommand* command = NULL;

    size_t size = GetCommands().size();
    if (size && GetCommands().at(size-1)->IsOpen()) {
        command = GetCommands().at(size-1);
    }

    return command;
}

void CommandProcessorBase::CloseOpenCommand()
{
    CLCommand* command = GetOpenCommand();
    wxCHECK_RET(command, "Trying to close to a non-existent or already-closed command");

    command->Close();
}

void CommandProcessorBase::SetUserLabel(const wxString& label)
{
    if (GetOpenCommand()) {
        ProcessCurrentCommand(); // We need to make it non-pending, otherwise it may change after the label is set; which probably won't be what the user expects
    }

    CLCommand* command = GetActiveCommand();
    if (command) {
        command->SetUserLabel(label);
    }
}

void CommandProcessorBase::PopulateUnRedoMenu(wxWindow* win, wxPoint& pt, bool undoing)
{
    wxMenu menu;
    wxString prefix(undoing ? "Undo " : "Redo ");
    int id = FIRST_MENU_ID;
//wxLogDebug("::PopulateUnRedoMenu: GetNextUndoCommand %i current %i  size %i hasopencommand=%s",  GetNextUndoCommand(), GetCurrentCommand(), (int)GetCommands().size(), GetOpenCommand() ? "true":"false");
    if (undoing) {
        if (GetCommands().size() > 0) {
            for (vCLCommands::const_reverse_iterator iter = GetCommands().rbegin() + GetNextUndoCommand(); iter != GetCommands().rend(); ++iter) {
                CLCommand* command = (CLCommand*)*iter;
                if (command) {
                    if (!command->GetUserLabel().empty()) {
                        menu.Append(id++, prefix + command->GetUserLabel());
                    } else {
                        wxString label;
                        if (command == GetOpenCommand()) {
                            label = GetBestLabel(command); // If the command's still open, there won't otherwise be a name string
                        } else {
                            label = command->GetName();
                        }
                        menu.Append(id++, prefix + label);
                    }
                }
            }
        }

        menu.Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(CommandProcessorBase::OnUndoDropdownItem), this, FIRST_MENU_ID, FIRST_MENU_ID + (id-1));
        win->PopupMenu(&menu, pt);
        menu.Unbind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(CommandProcessorBase::OnUndoDropdownItem), this, FIRST_MENU_ID, FIRST_MENU_ID + (id-1));
        
    } else {
        for (vCLCommands::const_iterator iter = GetCommands().begin() + GetCurrentCommand() + 1; iter != GetCommands().end(); ++iter) {
            CLCommand* command = (CLCommand*)*iter;
            if (command) {
                if (!command->GetUserLabel().empty()) {
                    menu.Append(id++, prefix + command->GetUserLabel());
                } else {
                    menu.Append(id++, prefix + command->GetName());
                }
            }
        }
        
        menu.Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(CommandProcessorBase::OnRedoDropdownItem), this, FIRST_MENU_ID, FIRST_MENU_ID + (id-1));
        win->PopupMenu(&menu, pt);
        menu.Unbind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(CommandProcessorBase::OnRedoDropdownItem), this, FIRST_MENU_ID, FIRST_MENU_ID + (id-1));
    }
}

void CommandProcessorBase::PopulateLabelledStatesMenu(wxMenu* menu)
{
    wxCHECK_RET(menu, "NULL menu");
    
    for (size_t n = menu->GetMenuItemCount(); n > 0; --n) { // We need to delete any items left over from a previous call
        wxMenuItem* item = menu->FindItemByPosition(n-1);
        if (item) {
            menu->Delete(item);
        }
    }
    

    if (!GetInitialCommand()->GetUserLabel().empty()) { // First any initial label
        menu->Append(FIRST_MENU_ID - 1, GetInitialCommand()->GetUserLabel()); // (Ab)use an out-of-range id to avoid confusion
    }

    int id = FIRST_MENU_ID;
    for (vCLCommands::const_iterator iter = GetCommands().begin(); iter != GetCommands().end(); ++iter) {
        CLCommand* command = (CLCommand*)*iter;
        if (command && !command->GetUserLabel().empty()) {
            menu->Append(id, command->GetUserLabel());
        }
        ++id; // It's easier in the handler if the event id is the GetCommands() index, rather than a menu index
    }

    menu->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(CommandProcessorBase::OnLabelledStatesMenuItem), this);
}


void CommandProcessorBase::UnBindLabelledStatesMenu(wxMenu* menu)
{
    wxCHECK_RET(menu, "NULL menu");
    CallAfter(&CommandProcessorBase::DoUnBindLabelledStatesMenu, menu); // We can't Unbind yet: the event won't have been caught
}

void CommandProcessorBase::DoUnBindLabelledStatesMenu(wxMenu* menu)
{
    wxCHECK_RET(menu, "NULL menu");
    menu->Unbind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(CommandProcessorBase::OnLabelledStatesMenuItem), this);
}

void CommandProcessorBase::OnLabelledStatesMenuItem(wxCommandEvent& event)
{
    if (GetOpenCommand()) {
        ProcessCurrentCommand();
    }

    int index = event.GetId() - FIRST_MENU_ID;  // NB index will be -1 if the selection is the initial-command's label. This will always mean *un*dos (or nothing)
    wxCHECK_RET(index < (int)GetCommands().size(), "An ID that overruns the command-list");

    if (index < GetCurrentCommand()) {
        const int count = GetCurrentCommand() - index;
        for (int n=0; n < count; ++n) {
            if (DoUndo()) {
                --m_currentCommand;
            }
        }
    } else {
        const int count = index - GetCurrentCommand();
        for (int n=0; n < count; ++n) {
            if (DoRedo()) {
                ++m_currentCommand;
            }
        }
    }
}

void CommandProcessorBase::OnUndoDropdownItem(wxCommandEvent& event)
{//wxLogDebug("::OnUndoDropdownItem: event.GetId() %i  GetNextUndoCommand %i current %i  size %i hasopencommand=%s",event.GetId(),  GetNextUndoCommand(), GetCurrentCommand(), (int)GetCommands().size(), GetOpenCommand() ? "true":"false");
    if (GetOpenCommand()) {
        ProcessCurrentCommand();
    }

    const int count = event.GetId() - FIRST_MENU_ID + 1;
    for (int n=0; n < count; ++n) {
        if (DoUndo()) {
            --m_currentCommand;
        }
    }
}

void CommandProcessorBase::OnRedoDropdownItem(wxCommandEvent& event)
{
    const int count = event.GetId() - FIRST_MENU_ID + 1;
    for (int n=0; n < count; ++n) {
        if (DoRedo()) {
            ++m_currentCommand;
        }
    }
}
    
CLCommand* CommandProcessorBase::GetActiveCommand() const
{
    CLCommand* command = NULL;

    if (GetCurrentCommand() == -1) {
        command = GetInitialCommand();
    } else if (GetCommands().size() > 0) {
        command = GetCommands().at(GetCurrentCommand());
    } 

    return command;
}
    
bool CommandProcessorBase::GetIsCurrentStateSaved() const
{
    CLCommand* command = GetActiveCommand();
    return (command && !command->IsOpen() && (GetSavedMd5Sum() == command->GetMd5Sum()));
}

void CommandProcessorBase::SetCurrentStateIsSaved(bool saved/*=true*/)
{
    if (GetOpenCommand()) {
        ProcessCurrentCommand(); // There's no sense setting a 'saved' state on a moving target
    }

    if (GetActiveCommand()) {
        SetSavedMd5Sum(GetActiveCommand()->GetMd5Sum());
    }
}

wxString CommandProcessorBase::CalculateMd5sum(const wxString& text) const
{
    if (text.empty()) {
        return text; // The md5 implementation returns garbage when passed thin air
    }
    
    wxString md5 = wxMD5::GetDigest(text);
    return md5;
}
