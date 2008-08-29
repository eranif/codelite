#ifndef __ExternalTools__
#define __ExternalTools__

#include "plugin.h"
#include "externaltoolsdata.h"
class wxToolBar;

class ExternalToolsPlugin : public IPlugin
{
	wxToolBar *m_tb;
	wxEvtHandler *topWin;
	
protected:	
	void OnSettings(wxCommandEvent &e);
	void OnLaunchExternalTool(wxCommandEvent &e);
	void DoLaunchTool(const ToolInfo &ti);
	void DoRecreateToolbar();
	
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

