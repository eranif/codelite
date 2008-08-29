#ifndef __ExternalTools__
#define __ExternalTools__

#include "plugin.h"
#include "externaltoolsdata.h"

class ExternalToolsPlugin : public IPlugin
{
	wxEvtHandler *topWin;
	void OnSettings(wxCommandEvent &e);
	void OnLaunchExternalTool(wxCommandEvent &e);
	void DoLaunchTool(const ToolInfo &ti);
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

