#ifndef DEBUGGER_SETTINGS_H
#define DEBUGGER_SETTINGS_H

#include "serialized_object.h"
#include "vector"

class DebuggerCmdData : public SerializedObject 
{ 
	wxString m_name;
	wxString m_command;
	bool m_isSubMenu;
	
public:
	DebuggerCmdData();
	virtual ~DebuggerCmdData();
	
	void Serialize(Archive &arhc);
	void DeSerialize(Archive &arhc);
	
	//Setters
	void SetCommand(const wxString& command) {this->m_command = command;}
	void SetIsSubMenu(const bool& isSubMenu) {this->m_isSubMenu = isSubMenu;}
	void SetName(const wxString& name) {this->m_name = name;}
	//Getters
	const wxString& GetCommand() const {return m_command;}
	const bool& GetIsSubMenu() const {return m_isSubMenu;}
	const wxString& GetName() const {return m_name;}
	
};

class DebuggerSettingsData : public SerializedObject
{
	std::vector<DebuggerCmdData> m_cmds;
public:
	DebuggerSettingsData();
	virtual ~DebuggerSettingsData();
	
	void Serialize(Archive &arhc);
	void DeSerialize(Archive &arhc);
	
	//Setters
	void SetCmds(const std::vector<DebuggerCmdData>& cmds) {this->m_cmds = cmds;}

	//Getters
	const std::vector<DebuggerCmdData>& GetCmds() const {return m_cmds;}
};

#endif //DEBUGGER_SETTINGS_H

