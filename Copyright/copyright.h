#ifndef __Copyright__
#define __Copyright__

#include "plugin.h"
class wxMenuItem;
class Copyright : public IPlugin
{
	wxEvtHandler *m_topWin;
	wxMenuItem* m_sepItem;
public:
	Copyright(IManager *manager);
	~Copyright();
	
	//--------------------------------------------
	//Abstract methods
	//--------------------------------------------
	virtual wxToolBar *CreateToolBar(wxWindow *parent);
	virtual void CreatePluginMenu(wxMenu *pluginsMenu);
	virtual void HookPopupMenu(wxMenu *menu, MenuType type);
	virtual void UnHookPopupMenu(wxMenu *menu, MenuType type);
	virtual void UnPlug();
	
	// event handlers
	void OnOptions(wxCommandEvent &e);
	void OnInsertCopyrights(wxCommandEvent &e);
	void OnBatchInsertCopyrights(wxCommandEvent &e);
};

#endif //Copyright

