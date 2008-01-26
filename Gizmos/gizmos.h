#ifndef GIZMOS_H
#define GIZMOS_H

#include "plugin.h"
#include "vector"
#include "newclassdlg.h"
#include "newwxprojectinfo.h"

class GizmosPlugin : public IPlugin
{
	void CreateClass(const NewClassInfo &info);
	wxString DoGetVirtualFuncImpl(const NewClassInfo &info);
	wxString DoGetVirtualFuncDecl(const NewClassInfo &info);
	std::vector<wxMenuItem*> m_vdDynItems;
protected:
	void CreateWxProject(NewWxProjectInfo &info);
	
public:
	GizmosPlugin(IManager *manager);
	~GizmosPlugin();
	
	//--------------------------------------------
	//Abstract methods
	//--------------------------------------------
	virtual wxToolBar *CreateToolBar(wxWindow *parent);
	virtual void CreatePluginMenu(wxMenu *pluginsMenu);
	virtual void HookPopupMenu(wxMenu *menu, MenuType type);
	virtual void UnHookPopupMenu(wxMenu *menu, MenuType type);
	virtual void UnPlug();
	
	//event handlers
	virtual void OnNewPlugin(wxCommandEvent &e);
	virtual void OnNewClass(wxCommandEvent &e);
	virtual void OnNewClassUI(wxUpdateUIEvent &e);
	virtual void OnNewPluginUI(wxUpdateUIEvent &e);
	virtual void OnNewWxProject(wxCommandEvent &e);
	virtual void OnNewWxProjectUI(wxUpdateUIEvent &e);

};

#endif //GIZMOS_H

