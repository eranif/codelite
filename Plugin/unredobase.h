//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : unredobase.h
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

#ifndef UNREDOBASE_H
#define UNREDOBASE_H

#include "clToolBar.h"
#include "codelite_exports.h"

#include <vector>
#include <wx/aui/auibar.h>
#include <wx/bitmap.h>
#include <wx/event.h>
#include <wx/gdicmn.h>
#include <wx/sharedptr.h>

enum CLC_types { CLC_insert, CLC_delete, CLC_unknown };

class WXDLLIMPEXP_SDK CLCommand
{
public:
    typedef wxSharedPtr<CLCommand> Ptr_t;
    typedef std::vector<CLCommand::Ptr_t> Vec_t;

    CLCommand(CLC_types type = CLC_unknown, const wxString& name = "")
        : m_commandType(type)
        , m_commandName(name)
        , m_isOpen(true)
    {
    }
    virtual ~CLCommand() {}

    int GetCommandType() const { return m_commandType; }
    wxString GetName() const { return m_commandName; }
    void SetName(const wxString& name) { m_commandName = name; }
    const wxString& GetText() const { return m_text; }
    void SetText(const wxString& text) { m_text = text; }
    bool IsOpen() const { return m_isOpen; }
    void Close() { m_isOpen = false; }
    wxString GetUserLabel() const { return m_userLabel; }
    void SetUserLabel(const wxString& label) { m_userLabel = label; }
    void AppendText(const wxString& text, int position) { m_text << text; }

    virtual bool GetIsAppendable() const = 0;
    virtual bool Do() { return true; }
    virtual bool Undo() { return true; }

protected:
    const int m_commandType;
    wxString m_commandName;
    wxString m_text;
    wxString m_userLabel;
    bool m_isOpen;
};

class WXDLLIMPEXP_SDK CommandProcessorBase : public wxEvtHandler
{
public:
    CommandProcessorBase();
    virtual ~CommandProcessorBase();

    bool CanAppend(CLC_types type)
    {
        return GetOpenCommand()->GetIsAppendable() && GetOpenCommand()->GetCommandType() == type;
    }

    virtual void ProcessOpenCommand();

    void PopulateUnRedoMenu(clToolBar* tb, wxWindowID toolId);
    virtual void DoPopulateUnRedoMenu(wxMenu& menu, bool undoing);

    void PrepareLabelledStatesMenu(wxMenu* menu);
    void PopulateLabelledStatesMenu(wxMenu* menu);

    void UnBindLabelledStatesMenu(wxMenu* menu);
    void DoUnBindLabelledStatesMenu(wxMenu* menu);

    CLCommand::Ptr_t GetInitialCommand() const { return m_initialCommand; }

    int GetCurrentCommand() const { return m_currentCommand; }

    void IncrementCurrentCommand();
    void DecrementCurrentCommand();

    int GetNextUndoCommand() const { return GetCommands().size() - m_currentCommand - 1; }

    void Add(CLCommand::Ptr_t command)
    {
        m_commands.push_back(command);
        m_currentCommand = m_commands.size() - 1;
    }

    CLCommand::Ptr_t GetOpenCommand();
    bool HasOpenCommand() { return (GetOpenCommand() != NULL); }

    virtual void CloseOpenCommand();

    CLCommand::Vec_t& GetCommands() { return m_commands; }

    const CLCommand::Vec_t& GetCommands() const { return m_commands; }

    void OnTBUnRedo(wxCommandEvent& event);

    virtual bool DoUndo() = 0;

    virtual bool DoRedo() = 0;

    bool CanUndo() const { return (GetCurrentCommand() > -1); }

    bool CanRedo() const
    {
        return (GetCommands().size() > 0) && (GetCurrentCommand() < (int)(GetCommands().size() - 1));
    }

    void ClearRedos()
    { // Remove any redos invalidated by a new addition
        while(GetCurrentCommand() < (int)(GetCommands().size() - 1)) {
            GetCommands().pop_back();
        }
    }

    void SetUserLabel(const wxString& label);

    CLCommand::Ptr_t GetActiveCommand() const; // The command indexed by m_currentCommand

    virtual void OnUndoDropdownItem(wxCommandEvent& event);
    virtual void OnRedoDropdownItem(wxCommandEvent& event);

    void Clear()
    {
        m_commands.clear();
        m_initialCommand = NULL;
    }

protected:
    virtual void OnLabelledStatesMenuItem(wxCommandEvent& event);

    CLCommand::Ptr_t m_initialCommand; // A command to hold any initial-state user-label, and to store any initial state
                                       // if we're state-storing
    CLCommand::Vec_t m_commands;
    int m_currentCommand; // The next one to be undone
};

#endif // UNREDOBASE_H
