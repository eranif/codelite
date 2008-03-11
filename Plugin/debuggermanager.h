#ifndef DEBUGGER_MANAGER_H
#define DEBUGGER_MANAGER_H

#include "map"
#include "list"
#include "vector"
#include "wx/string.h"
#include "debugger.h"
#include "wx/arrstr.h"
#include "serialized_object.h"
#include "dynamiclibrary.h"

class EnvironmentConfig;

class DebuggersData : public SerializedObject
{
private:
	std::vector<DebuggerInformation> m_debuggers;

public:
	DebuggersData();
	virtual ~DebuggersData();

	bool GetDebuggerInformation(const wxString &name, DebuggerInformation &info);
	void SetDebuggerInformation(const wxString &name, const DebuggerInformation &info);
	
	void Serialize(Archive &arc);
	void DeSerialize(Archive &arc);
};

class DebuggerMgr
{
	std::map<wxString, IDebugger*> m_debuggers;
	wxString m_baseDir;
	std::vector< clDynamicLibrary* > m_dl;
	wxString m_activeDebuggerName;
	wxEvtHandler *m_parent;
	std::vector<BreakpointInfo> m_bps;
	DebuggersData m_debuggersData;
	EnvironmentConfig *m_env;
	
private:
	DebuggerMgr();
	virtual ~DebuggerMgr();
	static DebuggerMgr *ms_instance;

public:
	/**
	 * Set the base dir for the debugger manager. On Linux this is
	 * equivalent to $(HOME)/.liteeditor/, and on windows it is set
	 * to C:\Program Files\LiteEditor\
	 */
	void Initialize(wxEvtHandler *parent, EnvironmentConfig *env, const wxString &dir) 
	{ 
		m_baseDir = dir; 
		m_parent = parent;
		m_env = env;
	}

	/**
	 * Load all available debuggers. This functions searches for dll/so/sl
	 * which are located udner $(HOME)/.liteeditor/debuggers/ on Linux, and on Windows
	 * under C:\Program Files\LiteEditor\debuggers\
	 */
	bool LoadDebuggers();

	/**
	 * Return list of all available debuggers which were loaded
	 * successfully into the debugger manager
	 */
	wxArrayString GetAvailableDebuggers();

	/**
	 * Set the active debugger to be 'name'. If a debugger with name does not
	 * exist, this function does nothing
	 */
	void SetActiveDebugger(const wxString &name);

	/**
	 * Return the currently selected debugger. The debugger is selected 
	 * based on previous call to SetActiveDebugger(). If no active debugger is
	 * set, this function may return NULL
	 */
	IDebugger *GetActiveDebugger();
	
	/**
	 * Add a break point to the current debugger
	 * all breakpoints will be pass to the debugger
	 * once started
	 */
	bool AddBreakpoint(const BreakpointInfo &bp);

	/**
	 * Delete break point by file and line
	 */
	bool DelBreakpoint(const BreakpointInfo &bp);
	
	/**
	 * return list of breakpoints
	 */
	void GetBreakpoints(std::vector<BreakpointInfo> &li);

	// delete all breakpoint of fileName
	void DelBreakpoints(const wxString &fileName);

	void GetBreakpointsByFileName(const wxString &fileName, std::vector<BreakpointInfo> &li);
	
	/** 
	 * remove all breakpoints
	 */
	void DelAllBreakpoints();

	//get/set debugger information
	void SetDebuggerInformation(const wxString &name, const DebuggerInformation &info);
	bool GetDebuggerInformation(const wxString &name, DebuggerInformation &info);
	
	static DebuggerMgr& Get();
	static void Free();
};
#endif //DEBUGGER_MANAGER_H
