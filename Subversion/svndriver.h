#ifndef SVNDRIVER_H
#define SVNDRIVER_H

#include "wx/event.h"
#include "wx/filename.h"
#include "wx/dir.h"
#include "wx/process.h"
#include "async_executable_cmd.h"
#include "imanager.h"
#include "vector"

class SvnCmdHandler;
class SubversionPlugin;

class SvnDriver : public wxEvtHandler 
{
	AsyncExeCmd *m_cmd;
	IManager *m_manager;
	SvnCmdHandler *m_curHandler;
	std::vector<SvnCmdHandler*> m_statusCmdQueue;
	SubversionPlugin *m_plugin;
	bool m_commitWithPass;

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
	void DisplayDiffFile(const wxString &content);
	void DisplayLog(const wxString &outputFile, const wxString &content);
	void Shutdown();
	void ExecStatusCommand(const wxString &path, wxString &output);
	void SetCommitWithPassword(bool need);

	AsyncExeCmd *Svn() {return m_cmd;}

	///////////////////////////////
	//Operations:
	///////////////////////////////

	//operations on a single file 
	void UpdateFile(const wxString &fileName);
	void CommitFile(const wxString &fileName);
	void DiffFile(const wxFileName &fileName);
	void RevertFile(const wxFileName &fileName);

	//operations taken from the file explorer tree
	void Abort();
	void Update();
	void Commit();
	void Diff();
	void Add();
	void Add(const wxFileName &filename);
	void Cleanup();
	void ChangeLog();
	void Delete();
	void Revert();
	
	DECLARE_EVENT_TABLE()
};

#endif //SVNDRIVER_H

