#ifndef __ContinuousBuild__
#define __ContinuousBuild__

#include "plugin.h"
#include "compiler.h"

class wxEvtHandler;
class ContinousBuildPane;
class ShellCommand;

struct CurrentBuildFile {
	wxString file;
	wxString project;
	wxArrayString output;

	void Clear() {
		output.Clear();
		file.Clear();
		project.Clear();
	}
};

class ContinuousBuild : public IPlugin
{
	ContinousBuildPane *m_view;
	wxEvtHandler *m_topWin;
	ShellCommand *m_shellProcess;
	wxArrayString m_files;
	CurrentBuildFile m_currentBuildInfo;

public:
	void DoBuild(const wxString &fileName);
	void DoReportErrors();
	bool IsCompilable(const wxString &fileName);
	CompilerPtr DoGetCompiler();
	
public:
	ContinuousBuild(IManager *manager);
	~ContinuousBuild();

	//--------------------------------------------
	//Abstract methods
	//--------------------------------------------
	virtual wxToolBar *CreateToolBar(wxWindow *parent);
	virtual void CreatePluginMenu(wxMenu *pluginsMenu);
	virtual void HookPopupMenu(wxMenu *menu, MenuType type);
	virtual void UnHookPopupMenu(wxMenu *menu, MenuType type);
	virtual void UnPlug();

	void StopAll();

	// Event handlers
	DECLARE_EVENT_TABLE()

	void OnFileSaved(wxCommandEvent &e);
	void OnShellAddLine(wxCommandEvent &e);
	void OnShellBuildStarted(wxCommandEvent &e);
	void OnShellProcessEnded(wxCommandEvent &e);
	void OnShellBuildStartedNoClean(wxCommandEvent &e);
};

#endif //ContinuousBuild
