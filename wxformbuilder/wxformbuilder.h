#ifndef __wxFormBuilder__
#define __wxFormBuilder__

#include "plugin.h"

class wxFormBuilder : public IPlugin
{
	wxEvtHandler *m_topWin;
	
public:
	wxFormBuilder(IManager *manager);
	~wxFormBuilder();
	
protected:
	void OnSettings(wxCommandEvent &e);
	
public:
	//--------------------------------------------
	//Abstract methods
	//--------------------------------------------
	virtual wxToolBar *CreateToolBar(wxWindow *parent);
	virtual void CreatePluginMenu(wxMenu *pluginsMenu);
	virtual void HookPopupMenu(wxMenu *menu, MenuType type);
	virtual void UnHookPopupMenu(wxMenu *menu, MenuType type);
	virtual void UnPlug();
};

#endif //wxFormBuilder

