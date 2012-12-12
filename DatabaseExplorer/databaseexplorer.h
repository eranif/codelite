#ifndef __DatabaseExplorer__
#define __DatabaseExplorer__

#include "plugin.h"
#include "DbViewerPanel.h"

class DatabaseExplorer : public IPlugin
{
public:
	DatabaseExplorer(IManager *manager);
	~DatabaseExplorer();
	
	//--------------------------------------------
	//Abstract methods
	//--------------------------------------------
	virtual clToolBar *CreateToolBar(wxWindow *parent);
	virtual void CreatePluginMenu(wxMenu *pluginsMenu);
	virtual void HookPopupMenu(wxMenu *menu, MenuType type);
	virtual void UnHookPopupMenu(wxMenu *menu, MenuType type);
	virtual void UnPlug();
	
	bool IsDbViewDetached();
	
	static IManager* GetManager();
	static DbViewerPanel* GetViewerPanel() {
		return m_dbViewerPanel;
	}
	
protected:
	bool m_addFileMenu;

	void OnAbout(wxCommandEvent &e);
	void OnOpenWithDBE(wxCommandEvent &e);
	void OnUpdateOpenWithDBE(wxUpdateUIEvent &e);

	static DbViewerPanel* m_dbViewerPanel;
};

#endif //DatabaseExplorer

