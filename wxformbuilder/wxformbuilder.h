#ifndef __wxFormBuilder__
#define __wxFormBuilder__

#include "wxfbitemdlg.h"
#include "plugin.h"
class wxMenuItem;

class wxFormBuilder : public IPlugin
{
	wxEvtHandler *m_topWin;
	wxMenuItem* m_separatorItem;
	wxMenuItem* m_openWithWxFbItem;
	wxMenuItem* m_openWithWxFbSepItem;
	
public:
	wxFormBuilder(IManager *manager);
	~wxFormBuilder();
	
protected:
	void OnSettings(wxCommandEvent &e);
	void OnNewDialog(wxCommandEvent &e);
	void OnNewFrame(wxCommandEvent &e);
	void OnNewPanel(wxCommandEvent &e);
	void OpenWithWxFb(wxCommandEvent &e);
	
	wxMenu *CreatePopupMenu();
	void DoCreateWxFormBuilderProject(const wxFBItemInfo &data);
	void DoLaunchWxFB(const wxString &file);
	wxString GetWxFBPath();
	
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

