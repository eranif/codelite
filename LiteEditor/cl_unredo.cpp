#include "cl_unredo.h"
#include "cl_editor.h"


CLCommandProcessor::CLCommandProcessor() : CommandProcessorBase()
{
    m_initialCommand = new CLTextCommand(CLC_unknown);
    m_initialCommand->Close();
}

void CLCommandProcessor::ProcessOpenCommand()
{
    CommandProcessorBase::ProcessOpenCommand(); // Do the real work
    
    wxCHECK_RET(GetParent(), "A parentless CLCommandProcessor");
    GetParent()->BeginUndoAction(); // Tell scintilla to stop trying to append to the stale 'current' CLCommand. Yes, we *do* need both the Begin and End.
    GetParent()->EndUndoAction();
}

void CLCommandProcessor::StartNewTextCommand(CLC_types type, const wxString& text)
{
    wxCHECK_RET(!GetOpenCommand(), "Trying to start a new command when there's already an existing one");

    if (CanRedo()) {
        ClearRedos(); // Remove any now-stale redoable items
    }

    if (type == CLC_delete) {
        Add( CLCommand::Ptr_t(new CLDeleteTextCommand) );
    } else {
        Add( CLCommand::Ptr_t(new CLInsertTextCommand) );
    }

    GetOpenCommand()->SetText(text);
}

void CLCommandProcessor::AppendToTextCommand(const wxString& text, int WXUNUSED(position))
{
    wxCHECK_RET(GetOpenCommand(), "Trying to add to a non-existent or non-open command");
    CLCommand::Ptr_t command = GetOpenCommand();
    if (command->GetCommandType() == CLC_delete) {
        // Reverse any incrementally-added string here, so that undoing an insertion of "abcd" gets displayed as: delete "abcd", not "dcba"
        command->SetText(text + command->GetText());
    } else {
        command->SetText(command->GetText() + text);
    }
}

bool CLCommandProcessor::DoUndo()
{
    wxCHECK_MSG(CanUndo(), false, "Trying to Undo when you can't");
    wxCHECK_MSG(GetParent(), false, "A parentless CLCommandProcessor");
    GetParent()->Undo();

    return true;
}

bool CLCommandProcessor::DoRedo()
{
    wxCHECK_MSG(CanRedo(), false, "Trying to Redo when you can't");
    wxCHECK_MSG(GetParent(), false, "A parentless CLCommandProcessor");
    GetParent()->Redo();

    return true;
}
