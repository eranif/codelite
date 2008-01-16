#ifndef DBGINTERFACE_H
#define DBGINTERFACE_H

#include "wx/string.h"
#include "wx/event.h"
#include "interactiveprocess.h"
#include "list"
#include "debugger.h"
#include <wx/hashmap.h>
#include "consolefinder.h"

#ifdef MSVC_VER
//declare the debugger function creation
extern "C++" IDebugger *CreateDebuggerGDB();
//declare the function that will be called by host application
//to retrieve the debugger initialization function
extern "C++" DebuggerInfo GetDebuggerInfo();
#else
//declare the debugger function creation
extern "C" IDebugger *CreateDebuggerGDB();
//declare the function that will be called by host application
//to retrieve the debugger initialization function
extern "C" DebuggerInfo GetDebuggerInfo();
#endif 

class DbgCmdHandler;
WX_DECLARE_STRING_HASH_MAP(DbgCmdHandler*, HandlersMap);

class DbgGdb : public InteractiveProcess, public IDebugger
{
	HandlersMap m_handlers;
	long m_debuggeePid;
	ConsoleFinder m_consoleFinder;
	std::vector<BreakpointInfo> m_bpList;
	
protected:
	void RegisterHandler(const wxString &id, DbgCmdHandler *cmd);
	DbgCmdHandler *PopHandler(const wxString &id);
	void EmptyQueue();
	bool FilterMessage(const wxString &msg);

	//wrapper for convinience
	bool WriteCommand(const wxString &command, DbgCmdHandler *cmd);
	int StartConsole();
	void DoProcessAsyncCommand(wxString &line, wxString &id);
	void SetBreakpoints();
	
public:
	DbgGdb();
	virtual ~DbgGdb();

	virtual bool Start(const wxString &exeName, const wxString &cwd, const std::vector<BreakpointInfo> &bpList);
	virtual bool ExecSyncCmd(const wxString &command, wxString &output);
	virtual bool Start(const wxString &debuggerPath, const wxString &exeName, const wxString &cwd, const std::vector<BreakpointInfo> &bpList);
	//start debugger, but this time, attach to process
	virtual bool Start(const wxString &debuggerPath, const wxString &exeName, int pid, const std::vector<BreakpointInfo> &bpList);
	virtual bool Run(const wxString &args);
	virtual bool Stop();
	virtual bool Break(const wxString &fileName, long lineno);
	virtual bool RemoveBreak(int bid);
	virtual bool RemoveBreak(const wxString &fileName, long lineno);
	virtual bool RemoveAllBreaks();
	virtual bool StepIn();
	virtual bool StepOut();
	virtual bool Next();
	virtual bool Continue();
	virtual bool QueryFileLine();
	virtual bool Interrupt();
	virtual bool IsRunning();
	virtual bool ExecuteCmd(const wxString &cmd);
	virtual bool EvaluateExpressionToTree(const wxString &expression);
	virtual bool EvaluateExpressionToString(const wxString &expression);
	virtual bool QueryLocals();
	virtual bool ListFrames();
	virtual bool ListThreads(ThreadEntryArray &threads);
	virtual bool SelectThread(long threadId);
	virtual bool SetFrame(int frame);
	virtual void Poke();
};
#endif //DBGINTERFACE_H


