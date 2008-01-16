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
	arch.Read(wxT("m_isSubMenu"), m_isSubMenu);
}

void DebuggerCmdData::Serialize(Archive &arch)
{
	arch.Write(wxT("m_name"), m_name);
	arch.Write(wxT("m_command"), m_command);
	arch.Write(wxT("m_isSubMenu"), m_isSubMenu);
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
