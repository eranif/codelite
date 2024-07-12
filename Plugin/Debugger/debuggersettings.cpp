//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : debuggersettings.cpp
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
#include "debuggersettings.h"

#include "macromanager.h"

DebuggerCmdData::DebuggerCmdData() {}

DebuggerCmdData::~DebuggerCmdData() {}

void DebuggerCmdData::DeSerialize(Archive& arch)
{
    arch.Read(wxT("m_name"), m_name);
    arch.Read(wxT("m_command"), m_command);
    if(arch.Read(wxT("m_dbgCommand"), m_dbgCommand) == false) {
        m_dbgCommand = wxT("print");
    }
}

void DebuggerCmdData::Serialize(Archive& arch)
{
    arch.Write(wxT("m_name"), m_name);
    arch.Write(wxT("m_command"), m_command);
    arch.Write(wxT("m_dbgCommand"), m_dbgCommand);
}

//-----------------------------------------------------------------
//-----------------------------------------------------------------
//-----------------------------------------------------------------

DebuggerPreDefinedTypes::DebuggerPreDefinedTypes()
    : m_name(wxT("Default"))
    , m_active(true)
{
}

DebuggerPreDefinedTypes::~DebuggerPreDefinedTypes() {}

void DebuggerPreDefinedTypes::DeSerialize(Archive& arch)
{
    size_t size(0);
    arch.Read(wxT("m_name"), m_name);
    arch.Read(wxT("m_active"), m_active);
    arch.Read(wxT("size"), size);
    for(size_t i = 0; i < size; i++) {
        wxString cmdname;
        cmdname << wxT("DebuggerCmd") << i;
        DebuggerCmdData cmdData;
        arch.Read(cmdname, &cmdData);
        m_cmds.push_back(cmdData);
    }
}

void DebuggerPreDefinedTypes::Serialize(Archive& arch)
{
    size_t size = m_cmds.size();
    arch.Write(wxT("m_name"), m_name);
    arch.Write(wxT("m_active"), m_active);
    arch.Write(wxT("size"), size);

    for(size_t i = 0; i < m_cmds.size(); i++) {
        wxString cmdname;
        cmdname << wxT("DebuggerCmd") << i;
        arch.Write(cmdname, &m_cmds.at(i));
    }
}

wxString GetRealType(const wxString& gdbType)
{
    wxString realType(gdbType);
    realType.Replace(wxT("*"), wxT(""));
    realType.Replace(wxT("const"), wxT(""));
    realType.Replace(wxT("&"), wxT(""));

    // remove any template initialization:
    int depth(0);
    wxString noTemplateType;
    for(size_t i = 0; i < realType.Length(); i++) {
        switch((wxChar)realType.GetChar(i)) {
        case wxT('<'):
            depth++;
            break;
        case wxT('>'):
            depth--;
            break;
        default:
            if(depth == 0)
                noTemplateType << realType.GetChar(i);
            break;
        }
    }

    noTemplateType.Trim().Trim(false);
    return noTemplateType;
}

wxString DebuggerPreDefinedTypes::GetPreDefinedTypeForTypename(const wxString& expr, const wxString& name)
{
    wxString realType = GetRealType(expr);
    for(size_t i = 0; i < m_cmds.size(); i++) {
        DebuggerCmdData dcd = m_cmds.at(i);
        if(dcd.GetName() == realType) {
            // Create variable object for this variable
            // and display the content
            wxString expression = dcd.GetCommand();
            expression = MacroManager::Instance()->Replace(expression, wxT("variable"), name, true);
            return expression;
        }
    }
    return wxT("");
}

//-----------------------------------------------------------------
//-----------------------------------------------------------------
//-----------------------------------------------------------------

DebuggerSettingsPreDefMap::DebuggerSettingsPreDefMap() {}

DebuggerSettingsPreDefMap::~DebuggerSettingsPreDefMap() {}

void DebuggerSettingsPreDefMap::Serialize(Archive& arch)
{
    arch.Write(wxT("size"), m_cmds.size());
    std::map<wxString, DebuggerPreDefinedTypes>::const_iterator iter = m_cmds.begin();
    size_t i(0);
    for(; iter != m_cmds.end(); iter++, i++) {
        wxString cmdname;
        cmdname << wxT("PreDefinedSet") << i;
        arch.Write(cmdname, (SerializedObject*)&(iter->second));
    }
}

void DebuggerSettingsPreDefMap::DeSerialize(Archive& arch)
{
    size_t count(0);
    arch.Read(wxT("size"), count);
    m_cmds.clear();

    for(size_t i = 0; i < count; i++) {
        wxString cmdname;
        cmdname << wxT("PreDefinedSet") << i;
        DebuggerPreDefinedTypes preDefSet;
        arch.Read(cmdname, &preDefSet);
        m_cmds[preDefSet.GetName()] = preDefSet;
    }
}

DebuggerPreDefinedTypes DebuggerSettingsPreDefMap::GetActiveSet() const
{
    std::map<wxString, DebuggerPreDefinedTypes>::const_iterator iter = m_cmds.begin();
    for(; iter != m_cmds.end(); iter++) {
        if(iter->second.IsActive())
            return iter->second;
    }

    // no match, search for the one with the name 'Default'
    iter = m_cmds.begin();
    for(; iter != m_cmds.end(); iter++) {
        if(iter->second.GetName() == wxT("Default"))
            return iter->second;
    }

    // still no match
    // return the first entry
    if(m_cmds.empty() == false)
        return m_cmds.begin()->second;

    // no entries at all?
    return DebuggerPreDefinedTypes();
}

bool DebuggerSettingsPreDefMap::IsSetExist(const wxString& name) { return m_cmds.find(name) != m_cmds.end(); }

void DebuggerSettingsPreDefMap::SetActive(const wxString& name)
{
    std::map<wxString, DebuggerPreDefinedTypes>::iterator iter = m_cmds.begin();
    for(; iter != m_cmds.end(); iter++) {
        m_cmds[iter->first].SetActive(iter->first == name ? true : false);
    }
}
