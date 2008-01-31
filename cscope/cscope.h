#ifndef __Cscope__
#define __Cscope__

#include "plugin.h"
#include "map"
#include "vector"
#include "cscopeentrydata.h"

class CscopeTab;

class Cscope : public IPlugin
{
	wxEvtHandler *m_topWindow;
	wxMenuItem *m_sepItem;
	CscopeTab *m_cscopeWin;
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
	void DoCscopeCommand(const wxString &command, const wxString &endMsg);
	
	//Event handlers
	//------------------------------------------
	void OnFindSymbol(wxCommandEvent &e);
	void OnFindGlobalDefinition(wxCommandEvent &e);
	void OnFindFunctionsCalledByThisFuncion(wxCommandEvent &e);
	void OnFindFunctionsCallingThisFunction(wxCommandEvent &e);
	void OnCScopeThreadEnded(wxCommandEvent &e);
	void OnCScopeThreadUpdateStatus(wxCommandEvent &e);
	void OnCscopeUI(wxUpdateUIEvent &e);
};
 
#endif //Cscope

