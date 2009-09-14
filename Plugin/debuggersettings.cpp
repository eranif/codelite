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

DebuggerCmdData::DebuggerCmdData()
{
}

DebuggerCmdData::~DebuggerCmdData()
{
}

void DebuggerCmdData::DeSerialize(Archive &arch)
{
	arch.Read(wxT("m_name"), m_name);
	arch.Read(wxT("m_command"), m_command);
	if( arch.Read(wxT("m_dbgCommand"), m_dbgCommand) == false ) {
		m_dbgCommand = wxT("print");
	}
}

void DebuggerCmdData::Serialize(Archive &arch)
{
	arch.Write(wxT("m_name"), m_name);
	arch.Write(wxT("m_command"), m_command);
	arch.Write(wxT("m_dbgCommand"), m_dbgCommand);
}

//-----------------------------------------------------------------
//-----------------------------------------------------------------
//-----------------------------------------------------------------

DebuggerSettingsData::DebuggerSettingsData()
{
}

DebuggerSettingsData::~DebuggerSettingsData()
{
}

void DebuggerSettingsData::DeSerialize(Archive &arch)
{
	size_t size(0);
	arch.Read(wxT("size"), size);

	for(size_t i=0; i<size; i++){
		wxString cmdname;
		cmdname << wxT("DebuggerCmd") << i;
		DebuggerCmdData cmdData;
		arch.Read(cmdname, &cmdData);
		m_cmds.push_back(cmdData);
	}
}

void DebuggerSettingsData::Serialize(Archive &arch)
{
	size_t size = m_cmds.size();
	arch.Write(wxT("size"), size);
	for(size_t i=0; i<m_cmds.size(); i++){
		wxString cmdname;
		cmdname << wxT("DebuggerCmd") << i;
		arch.Write(cmdname, &m_cmds.at(i));
	}
}
