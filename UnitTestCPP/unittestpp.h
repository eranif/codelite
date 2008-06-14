#ifndef __UnitTestPP__
#define __UnitTestPP__

#include "plugin.h"

class wxMenuItem;

class UnitTestPP : public IPlugin
{
	wxEvtHandler *m_topWindow;
	wxMenuItem *m_sepItem;
public:
	UnitTestPP(IManager *manager);
	~UnitTestPP();
	
	//--------------------------------------------
	//Abstract methods
	//--------------------------------------------
	virtual wxToolBar *CreateToolBar(wxWindow *parent);
	virtual void CreatePluginMenu(wxMenu *pluginsMenu);
	virtual void HookPopupMenu(wxMenu *menu, MenuType type);
	virtual void UnHookPopupMenu(wxMenu *menu, MenuType type);
	virtual void UnPlug();
	
protected:
	void OnNewSimpleTest(wxCommandEvent &e);
	void OnNewClassTest(wxCommandEvent &e);
	
	void DoCreateSimpleTest(const wxString &name, IEditor *editor);
	void DoCreateFixtureTest(const wxString &name, const wxString &fixture, IEditor *editor);
	
private:
	wxMenu *CreateEditorPopMenu();
};

#endif //UnitTestPP

