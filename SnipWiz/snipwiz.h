/////////////////////////////////////////////////////////////////////////////
// Name:        snipwiz.h
// Purpose:
// Author:      Frank Lichtner
// Modified by:
// Created:     09/02/08
// RCS-ID:
// Copyright:   2008 Frank Lichtner
// Licence:		GNU General Public Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __snipwiz__
#define __snipwiz__

#include "plugin.h"
#include "swStringDb.h"
#include <vector>

class wxMenuItem;
class SnipWiz : public IPlugin
{
	std::vector<wxMenuItem*> m_vdDynItems;
	
public:
	/// returns StringDB
	swStringDb* GetStringDb() {	return &m_StringDb; }
	wxMenu* CreateSubMenu();
	SnipWiz( IManager *manager );
	~SnipWiz();

	//--------------------------------------------
	//Abstract methods
	//--------------------------------------------
	virtual wxToolBar *CreateToolBar( wxWindow *parent );
	virtual void CreatePluginMenu( wxMenu *pluginsMenu );
	virtual void HookPopupMenu( wxMenu *menu, MenuType type );
	virtual void UnHookPopupMenu( wxMenu *menu, MenuType type );
	virtual void UnPlug();
	// event handler
	void OnMenuSnippets( wxCommandEvent &e );
	void OnMenuExpandSwitch( wxCommandEvent &e );
	void OnMenuPaste( wxCommandEvent &e );
	void OnSettings( wxCommandEvent &e );
	void OnClassWizard( wxCommandEvent &e );


	wxMenuItem* m_sepItem;
	wxEvtHandler *m_topWin;
	enum
	{
		IDM_BASE = 20000,
		IDM_SETTINGS,
		IDM_CLASS_WIZ,
		IDM_EXP_SWITCH,
		IDM_PASTE,

		IDM_ADDSTART = 20050

	};
protected:
	/// get editor pointer
	IEditor* GetEditor();
	/// returns string with necessary tabs
	wxString GetTabs( IEditor* pEditor, long pos );
	/// returns number of tabs before pos
	long GetCurrentIndentation( IEditor *pEditor, long pos );
	/// replaces all EOL with EOL + necessary tabs
	wxString FormatOutput( IEditor* pEditor, const wxString& text );
	/// attaches dynamic menus
	void AttachDynMenus();
	/// detaches dynamic menus
	void DetachDynMenus();
	/// if stringDB is not found, init snippets with some defaults
	void IntSnippets();

	wxString	  m_pluginPath;	// plugins path
	wxArrayString m_snippets;	// keys for StringDB
	bool 		  m_modified;	// modified flag
	swStringDb 	  m_StringDb;	// string database with snippets and templates
	wxString	  m_clipboard;	// internal clipboard
};

#endif //snipwiz
