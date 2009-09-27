//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : gizmos.h
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
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

	void DoCreateNewPlugin   ();
	void DoCreateNewClass    ();
	void DoCreateNewWxProject();
	//event handlers
	virtual void OnNewPlugin(wxCommandEvent &e);
	virtual void OnNewClass(wxCommandEvent &e);
	virtual void OnNewClassUI(wxUpdateUIEvent &e);
	virtual void OnNewPluginUI(wxUpdateUIEvent &e);
	virtual void OnNewWxProject(wxCommandEvent &e);
	virtual void OnNewWxProjectUI(wxUpdateUIEvent &e);

	//event handlers
	virtual void OnGizmos       (wxCommandEvent  &e);
	virtual void OnGizmosUI     (wxUpdateUIEvent &e);
};

#endif //GIZMOS_H

