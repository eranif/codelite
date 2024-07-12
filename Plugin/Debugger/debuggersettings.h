//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : debuggersettings.h
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
#ifndef DEBUGGER_SETTINGS_H
#define DEBUGGER_SETTINGS_H

#include "codelite_exports.h"
#include "serialized_object.h"
#include "vector"

class WXDLLIMPEXP_SDK DebuggerCmdData : public SerializedObject
{
    wxString m_name;
    wxString m_command;
    wxString m_dbgCommand;

public:
    DebuggerCmdData();
    virtual ~DebuggerCmdData();

    void Serialize(Archive& arhc);
    void DeSerialize(Archive& arhc);

    // Setters
    void SetCommand(const wxString& command) { this->m_command = command; }
    void SetName(const wxString& name) { this->m_name = name; }

    // Getters
    const wxString& GetCommand() const { return m_command; }
    const wxString& GetName() const { return m_name; }

    void SetDbgCommand(const wxString& dbgCommand) { this->m_dbgCommand = dbgCommand; }
    const wxString& GetDbgCommand() const { return m_dbgCommand; }
};

typedef std::vector<DebuggerCmdData> DebuggerCmdDataVec;

class WXDLLIMPEXP_SDK DebuggerPreDefinedTypes : public SerializedObject
{
    DebuggerCmdDataVec m_cmds;
    wxString m_name;
    bool m_active;

public:
    DebuggerPreDefinedTypes();
    virtual ~DebuggerPreDefinedTypes();

    void Serialize(Archive& arch);
    void DeSerialize(Archive& arch);

    void SetCmds(const DebuggerCmdDataVec& cmds) { this->m_cmds = cmds; }

    const DebuggerCmdDataVec& GetCmds() const { return m_cmds; }
    void SetName(const wxString& name) { this->m_name = name; }
    const wxString& GetName() const { return m_name; }

    void SetActive(bool active) { this->m_active = active; }
    bool IsActive() const { return m_active; }

    wxString GetPreDefinedTypeForTypename(const wxString& expr, const wxString& name);
};

class WXDLLIMPEXP_SDK DebuggerSettingsPreDefMap : public SerializedObject
{
    std::map<wxString, DebuggerPreDefinedTypes> m_cmds;

public:
    DebuggerSettingsPreDefMap();
    virtual ~DebuggerSettingsPreDefMap();

    void Serialize(Archive& arch);
    void DeSerialize(Archive& arch);

    void SePreDefinedTypesMap(const std::map<wxString, DebuggerPreDefinedTypes>& cmds) { this->m_cmds = cmds; }
    const std::map<wxString, DebuggerPreDefinedTypes>& GePreDefinedTypesMap() const { return m_cmds; }

    /**
     * @brief return the active set, in this context it means return the one with the name 'Default'
     */
    DebuggerPreDefinedTypes GetActiveSet() const;
    bool IsSetExist(const wxString& name);
    void SetActive(const wxString& name);
};

#endif // DEBUGGER_SETTINGS_H
