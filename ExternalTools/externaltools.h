#ifndef __ExternalTools__
#define __ExternalTools__

#include "plugin.h"
#include "externaltoolsdata.h"

class wxToolBar;
class AsyncExeCmd;

class ExternalToolsPlugin : public IPlugin
{
	wxToolBar *m_tb;
	wxEvtHandler *topWin;
	AsyncExeCmd *m_pipedProcess;
	DECLARE_EVENT_TABLE()
	
protected:	
	void OnSettings(wxCommandEvent &e);
	void OnLaunchExternalTool(wxCommandEvent &e);
	void OnLaunchExternalToolUI(wxUpdateUIEvent &e);
	void OnStopExternalTool(wxCommandEvent &e);
	void OnStopExternalToolUI(wxUpdateUIEvent &e);
	void DoLaunchTool(const ToolInfo &ti);
	void DoRecreateToolbar();
	bool IsRedirectedToolRunning();
	void OnProcessEnd(wxProcessEvent &event);
	void OnToolProcess(wxCommandEvent &event);
	void SelectOutputTab();
	
public:
	ExternalToolsPlugin(IManager *manager);
	~ExternalToolsPlugin();
	
	//--------------------------------------------
	//Abstract methods
	//--------------------------------------------
	virtual wxToolBar *CreateToolBar(wxWindow *parent);
	virtual void CreatePluginMenu(wxMenu *pluginsMenu);
	virtual void HookPopupMenu(wxMenu *menu, MenuType type);
	virtual void UnHookPopupMenu(wxMenu *menu, MenuType type);
	virtual void UnPlug();
};

#endif //ExternalTools

