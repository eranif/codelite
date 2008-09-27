//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : debuggermanager.cpp              
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
 #include "debuggermanager.h"
#include "wx/filename.h"
#include <wx/dir.h>
#include <wx/log.h>
#include "editor_config.h"
#include <wx/msgdlg.h>
#include "debuggerconfigtool.h"

//---------------------------------------------------------
DebuggersData::DebuggersData()
{
}
 
DebuggersData::~DebuggersData()
{
}

void DebuggersData::Serialize(Archive &arch)
{
	size_t count = m_debuggers.size();
	arch.Write(wxT("debuggersCount"), count);
	for(size_t i=0; i<count; i++){
		DebuggerInformation info = m_debuggers.at(i);
		arch.Write(wxT("name"), info.name);
		arch.Write(wxT("path"), info.path);
		arch.Write(wxT("enableDebugLog"), info.enableDebugLog);
		arch.Write(wxT("enablePendingBreakpoints"), info.enablePendingBreakpoints);
		arch.Write(wxT("breakAtWinMain"), info.breakAtWinMain);
		arch.Write(wxT("resolveThis"), info.resolveThis);
		arch.Write(wxT("showTerminal"), info.showTerminal);
	}
}

void DebuggersData::DeSerialize(Archive &arch)
{
	size_t count;
	arch.Read(wxT("debuggersCount"), count);
	for(size_t i=0; i<count; i++){
		DebuggerInformation info;
		arch.Read(wxT("name"), info.name);
		arch.Read(wxT("path"), info.path);
		arch.Read(wxT("enableDebugLog"), info.enableDebugLog);
		arch.Read(wxT("enablePendingBreakpoints"), info.enablePendingBreakpoints);
		arch.Read(wxT("breakAtWinMain"), info.breakAtWinMain);
		arch.Read(wxT("resolveThis"), info.resolveThis);
		arch.Read(wxT("showTerminal"), info.showTerminal);
		m_debuggers.push_back(info);
	}
}

bool DebuggersData::GetDebuggerInformation(const wxString &name, DebuggerInformation &info)
{
	for(size_t i=0; i<m_debuggers.size(); i++){
		if(m_debuggers.at(i).name == name){
			info = m_debuggers.at(i);
			return true;
		}
	}
	return false;
}

void DebuggersData::SetDebuggerInformation(const wxString &name, const DebuggerInformation &info)
{
	//remove old entry
	for(size_t i=0; i<m_debuggers.size(); i++){
		if(m_debuggers.at(i).name == name){
			m_debuggers.erase(m_debuggers.begin()+i);
			break;
		}
	}
	m_debuggers.push_back(info);
}

//---------------------------------------------------------
DebuggerMgr *DebuggerMgr::ms_instance = NULL;

DebuggerMgr::DebuggerMgr()
{
	//load debugger data from configuration file
	DebuggerConfigTool::Get()->ReadObject(wxT("DebuggersData"), &m_debuggersData);
}

DebuggerMgr::~DebuggerMgr()
{
	std::vector<clDynamicLibrary*>::iterator iter = m_dl.begin();
	for(; iter != m_dl.end(); iter++){
		(*iter)->Detach();
		delete (*iter);
	}
	m_dl.clear();
	m_debuggers.clear();

	DebuggerConfigTool::Get()->WriteObject(wxT("DebuggersData"), &m_debuggersData);
}

DebuggerMgr& DebuggerMgr::Get()
{
	if(!ms_instance){
		ms_instance = new DebuggerMgr();
	}
	return *ms_instance;
}

void DebuggerMgr::Free()
{
	delete ms_instance;
	ms_instance = NULL;
}

bool DebuggerMgr::LoadDebuggers()
{
	wxString ext;
#if defined (__WXMSW__)
	ext = wxT("dll");
#else
	ext = wxT("so");
#endif
	wxString fileSpec(wxT("*.")+ext);

	//get list of dlls
	wxArrayString files;
	wxDir::GetAllFiles(m_baseDir + wxT("/debuggers"), &files, fileSpec, wxDIR_FILES);

	for(size_t i=0; i<files.GetCount(); i++){
		clDynamicLibrary *dl = new clDynamicLibrary();
		wxString fileName(files.Item(i));
		if(!dl->Load(fileName)){
			wxLogMessage(wxT("Failed to load debugger's dll: ") + fileName);
			delete dl;
			continue;
		}

		bool success(false);
		GET_DBG_INFO_FUNC pfn = (GET_DBG_INFO_FUNC)dl->GetSymbol(wxT("GetDebuggerInfo"), &success);
		if(!success){
			//dl->Unload();
			delete dl;
			continue;
		}

		DebuggerInfo info = pfn();
		//Call the init method to create an instance of the debugger
		success = false;
		GET_DBG_CREATE_FUNC pfnInitDbg = (GET_DBG_CREATE_FUNC)dl->GetSymbol(info.initFuncName, &success);
		if(!success){
			dl->Detach();
			delete dl;
			continue;
		}

		wxLogMessage(wxT("Loaded debugger: ") + info.name + wxT(", Version: ") + info.version);
		IDebugger *dbg = pfnInitDbg();
		
		//set the environment
		dbg->SetEnvironment(m_env);
		
		m_debuggers[info.name] = dbg;

		//keep the dynamic load library
		m_dl.push_back(dl);
	}
	return true;
}

wxArrayString DebuggerMgr::GetAvailableDebuggers()
{
	wxArrayString dbgs;
	std::map<wxString, IDebugger*>::iterator iter = m_debuggers.begin();
	for(; iter != m_debuggers.end(); iter++){
		dbgs.Add(iter->first);
	}
	return dbgs;
}

IDebugger* DebuggerMgr::GetActiveDebugger()
{
	if(m_activeDebuggerName.IsEmpty()){
		//no active debugger is set, use the first one
		std::map<wxString, IDebugger*>::iterator iter = m_debuggers.begin();
		if(iter != m_debuggers.end()){
			SetActiveDebugger( iter->first );
			return iter->second;
		}
		return NULL;
	}

	std::map<wxString, IDebugger*>::iterator iter = m_debuggers.find(m_activeDebuggerName);
	if(iter != m_debuggers.end()){
		return iter->second;
	}
	return NULL;
}

void DebuggerMgr::SetActiveDebugger(const wxString &name)
{
	m_activeDebuggerName = name;
}

bool DebuggerMgr::AddBreakpoint(const BreakpointInfo &bp)
{
	IDebugger *dbgr = GetActiveDebugger();
	if(dbgr && dbgr->IsRunning()){
		dbgr->Break(bp.file, bp.lineno);
	}
	//if a breakpoint with this file and line already exist
	//dont add it
	std::vector<BreakpointInfo>::iterator iter = m_bps.begin();
	for(; iter != m_bps.end(); iter++){
		BreakpointInfo b = *iter;
		if(b.file == bp.file && b.lineno == bp.lineno){
			return true;
		}
	}
	m_bps.push_back(bp);

	return true;
}

bool DebuggerMgr::DelBreakpoint(const BreakpointInfo &bp)
{
	//remove it from the debugger as well
	IDebugger *dbgr = GetActiveDebugger();
	if(dbgr && dbgr->IsRunning()){
		dbgr->RemoveBreak(bp.file, bp.lineno);
	}

	std::vector<BreakpointInfo>::iterator iter = m_bps.begin();
	for(; iter != m_bps.end(); iter++){
		BreakpointInfo b = *iter;
		if(b.file == bp.file && b.lineno == bp.lineno){
			m_bps.erase(iter);
			break;
		}
	}
	return true;
}

void DebuggerMgr::GetBreakpoints(std::vector<BreakpointInfo> &li)
{
	li = m_bps;
}

void DebuggerMgr::DelAllBreakpoints()
{
	IDebugger *dbgr = GetActiveDebugger();
	if(dbgr){
		dbgr->RemoveAllBreaks();
	}
	m_bps.clear();
}

void DebuggerMgr::DelBreakpoints(const wxString &fileName)
{
	int size = (int)m_bps.size();
	for(int i = size-1; i >= 0; i--){
		if(m_bps.at(i).file == fileName){
			m_bps.erase(m_bps.begin()+i);
		}
	}
}

void DebuggerMgr::GetBreakpointsByFileName(const wxString &fileName, std::vector<BreakpointInfo> &li)
{
	std::vector<BreakpointInfo>::iterator iter = m_bps.begin();
	for(; iter != m_bps.end(); iter++){
		BreakpointInfo b = *iter;
		if(b.file == fileName){
			li.push_back(b);
		}
	}
}
void DebuggerMgr::SetDebuggerInformation(const wxString &name, const DebuggerInformation &info)
{
	m_debuggersData.SetDebuggerInformation(name, info);
}

bool DebuggerMgr::GetDebuggerInformation(const wxString &name, DebuggerInformation &info)
{
	return m_debuggersData.GetDebuggerInformation(name, info);
}



