//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : svndriver.h              
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
 #ifndef SVNDRIVER_H
#define SVNDRIVER_H

#include "wx/event.h"
#include "wx/filename.h"
#include "wx/dir.h"
#include "wx/process.h"
#include "async_executable_cmd.h"
#include "imanager.h"
#include "vector"
#include <wx/arrstr.h>

class SvnCmdHandler;
class SubversionPlugin;
class SvnPostCmdAction;

class SvnDriver : public wxEvtHandler 
{
	AsyncExeCmd *m_cmd;
	IManager *m_manager;
	SvnCmdHandler *m_curHandler;
	std::vector<SvnCmdHandler*> m_statusCmdQueue;
	SubversionPlugin *m_plugin;
	bool m_commitWithPass;
	wxArrayString m_modifiedFiles;
	
protected:
	void OnSvnProcessTerminated(wxProcessEvent &event);
	void OnSvnProcess(wxCommandEvent &event);
	void ExecCommand(const wxString &cmd);
	bool GetFilesList(const wxArrayString& output, wxArrayString &files);
	void SelectSvnTab();
	void CommitWithAuth(const wxString &cmd, const TreeItemInfo &item);

public:
	SvnDriver(SubversionPlugin *plugin, IManager *mgr);
	virtual ~SvnDriver();
	void PrintMessage(const wxString &text);
	void PrintMessage(const wxArrayString &textArr);
	void DisplayDiffFile(const wxString &fileName, const wxString &content);
	void DisplayLog(const wxString &outputFile, const wxString &content);
	void Shutdown();
	void ExecStatusCommand(const wxString &path, wxString &output);
	void SetCommitWithPassword(bool need);
	bool IsRunning() {return m_cmd != NULL;}
	AsyncExeCmd *Svn() {return m_cmd;}

	///////////////////////////////
	//Operations:
	///////////////////////////////

	//operations on a single file 
	void UpdateFile(const wxString &fileName, SvnPostCmdAction *handler = NULL);
	void CommitFile(const wxString &fileName, SvnPostCmdAction *handler = NULL);
	void DiffFile(const wxFileName &fileName);
	void RevertFile(const wxFileName &fileName, SvnPostCmdAction *handler = NULL);

	//operations taken from the file explorer tree
	void Abort();
	void Update(SvnPostCmdAction *handler);
	void Commit();
	void Diff();
	void Add();
	void Add(const wxFileName &filename, SvnPostCmdAction *handler = NULL);
	void Cleanup();
	void ChangeLog();
	void Delete();
	void Revert();
	
	DECLARE_EVENT_TABLE()
};

#endif //SVNDRIVER_H

