//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : cl_unredo.h
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

#ifndef CLUNREDO_H
#define CLUNREDO_H

#include "unredobase.h"

class clEditor;


class CLTextCommand : public CLCommand
{
public:
    CLTextCommand(CLC_types type, const wxString& name="") : CLCommand(type, name)
    {}
    virtual ~CLTextCommand() {}

    virtual bool GetIsAppendable() const { // We can append to a text command
        return true;
    }

protected:

};

class CLInsertTextCommand : public CLTextCommand
{
public:
    CLInsertTextCommand(const wxString& name="insertion:") : CLTextCommand(CLC_insert, name) {}
    virtual ~CLInsertTextCommand() {}

protected:

};

class CLDeleteTextCommand : public CLTextCommand
{
public:
    CLDeleteTextCommand(const wxString& name="deletion:") : CLTextCommand(CLC_delete, name) {}
    virtual ~CLDeleteTextCommand() {}

protected:

};

class CLCommandProcessor : public CommandProcessorBase
{
public:
    CLCommandProcessor();
    virtual ~CLCommandProcessor() {}

    void StartNewTextCommand(CLC_types type, const wxString& text = "");

    void AppendToTextCommand(const wxString& text, int position);
    
    virtual void ProcessOpenCommand();

    clEditor* GetParent() const {
        return m_parent;
    }

    void SetParent(clEditor* parent) {
        m_parent = parent;
    }

    void Reset() { // Like Clear() but retain m_initialCommand. Used when an editor is reloaded
       m_commands.clear();
    }

    void CloseSciUndoAction() const; // Closes any open undo action at the scintilla level

    virtual bool DoUndo();

    virtual bool DoRedo();

protected:
    clEditor* m_parent;

};

#endif // CLUNREDO_H
