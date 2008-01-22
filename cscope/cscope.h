#ifndef __Cscope__
#define __Cscope__

#include "plugin.h"
#include "cscopedbbuilderthread.h"

class Cscope : public IPlugin
{
	wxEvtHandler *m_topWindow;
	wxMenuItem *m_sepItem;
	CscopeDbBuilderThread *m_thread;
	
public:
	Cscope(IManager *manager);
	~Cscope();
	
	//--------------------------------------------
	//Abstract methods
	//--------------------------------------------
	virtual wxToolBar *CreateToolBar(wxWindow *parent);
	virtual void CreatePluginMenu(wxMenu *pluginsMenu);
	virtual void HookPopupMenu(wxMenu *menu, MenuType type);
	virtual void UnHookPopupMenu(wxMenu *menu, MenuType type);
	virtual void UnPlug();
	
protected:
	//Helper
	//------------------------------------------
	wxMenu *CreateEditorPopMenu();
	wxString GetCscopeExeName();
	wxString DoCreateListFile();
	
	//Event handlers
	//------------------------------------------
	void OnFindSymbol(wxCommandEvent &e);
	void OnFindGlobalDefinition(wxCommandEvent &e);
	void OnFindFunctionsCalledByThisFuncion(wxCommandEvent &e);
	void OnFindFunctionsCallingThisFunction(wxCommandEvent &e);
	void OnDbBuilderThreadEnded(wxCommandEvent &e);
};

#endif //Cscope

