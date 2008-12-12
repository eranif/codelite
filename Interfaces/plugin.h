//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : plugin.h
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
#ifndef PLUGIN_H
#define PLUGIN_H

#include "imanager.h"
#include "wx/toolbar.h"
#include "wx/event.h"
#include "plugindata.h"
#include "plugin_version.h"

#ifdef _WIN32
#define STDCALL __stdcall
#define EXPORT  __declspec(dllexport)
#else
#define STDCALL
#define EXPORT
#endif

class IManager;

class ParseThreadEventData
{
	wxString m_fileName;
	std::vector<std::pair<wxString, TagEntry> >  m_items;
public:
	ParseThreadEventData() {}
	~ParseThreadEventData() {}

	void SetFileName(const wxString& fileName) {
		this->m_fileName = fileName.c_str();
	}
	void SetItems(const std::vector<std::pair<wxString, TagEntry> >& items) {
		this->m_items = items;
	}
	const wxString& GetFileName() const {
		return m_fileName;
	}
	const std::vector<std::pair<wxString, TagEntry> >& GetItems() const {
		return m_items;
	}
};

/**
 * Possible popup menu
 */
enum MenuType {
	MenuTypeFileExplorer = 0,
	MenuTypeFileView_Workspace,
	MenuTypeFileView_Project,
	MenuTypeFileView_Folder,
	MenuTypeFileView_File,
	MenuTypeEditor
};

//
//Plugins events
//
enum {
	//clientData is NULL
	wxEVT_INIT_DONE = 3450,
    
    //clientData is editor config node name (wxString*)
    wxEVT_EDITOR_CONFIG_CHANGED,
    
	//clientData is NULL
	wxEVT_WORKSPACE_LOADED,
    //clientData is NULL
	wxEVT_WORKSPACE_CONFIG_CHANGED,
	//clientData is NULL
    wxEVT_WORKSPACE_CLOSED,
    
    //clientData is NULL
	wxEVT_FILE_VIEW_INIT_DONE,
	//clientData is NULL
	wxEVT_FILE_VIEW_REFRESHED,
    //clientData is NULL
	wxEVT_FILE_EXP_INIT_DONE,
	//clientData is NULL
	wxEVT_FILE_EXP_REFRESHED,
    
	//clientData is list of files added to project (wxArrayString*)
	wxEVT_PROJ_FILE_ADDED,
	//clientData is list of files which have been removed (wxArrayString*)
	wxEVT_PROJ_FILE_REMOVED,
	//clientData is the project name (wxString*)
	wxEVT_PROJ_REMOVED,
	//clientData is the project name (wxString*)
	wxEVT_PROJ_ADDED,
    
	//clientData is the selected word (wxString*)
	wxEVT_CCBOX_SELECTION_MADE,
    
	//clientData is fileName (wxString*)
	wxEVT_FILE_SAVED,
    //clientData is list of files which have been retagged (std::vector<wxFileName>*)
    wxEVT_FILE_RETAGGED,
	//clientData is ParseThreadEventData*
	wxEVT_SYNBOL_TREE_UPDATE_ITEM,
	//clientData is ParseThreadEventData*
	wxEVT_SYNBOL_TREE_DELETE_ITEM,
	//clientData is ParseThreadEventData*
	wxEVT_SYNBOL_TREE_ADD_ITEM,
    
    //clientData is active editor (IEditor*)
    wxEVT_ACTIVE_EDITOR_CHANGED,
    //clientData is closing editor (IEditor*)
    wxEVT_EDITOR_CLOSING,
    //clientData is NULL
    wxEVT_ALL_EDITORS_CLOSED,
};

//------------------------------------------------------------------
//each plugin must implement this interface
//------------------------------------------------------------------
/**
 * @class IPlugin
 * @author Eran
 * @date 05/07/08
 * @file plugin.h
 * @brief The interface that defines a plugin for CodeLite. each plugin must implement the pure virtual methods of this
 * interface. The plugin gains access to CodeLite data by using the m_mgr member
 * @sa  IManager
 */
class IPlugin : public wxEvtHandler
{
protected:
	wxString m_shortName;
	wxString m_longName;
	IManager *m_mgr;

public:
	IPlugin(IManager *manager) : m_mgr(manager) {}
	virtual ~IPlugin() {}

	//-----------------------------------------------
	//The interface
	//-----------------------------------------------
	/**
	 * @brief return the plugin's short name
	 * @return
	 */
	virtual const wxString &GetShortName() const {
		return m_shortName;
	}
	/**
	 * @brief return the plugin's long and more descriptive name
	 * @return
	 */
	virtual const wxString &GetLongName() const {
		return m_longName;
	}

	/**
	 * When LiteEditor loads all the plugins, this function is called. If toolbar
	 * is provided by the plugin, the Plugin Manager will palce it in the appropriate
	 * place on the toolbar pane.
	 * \param parent toolbar parent, usually this is the main frame
	 * \return toolbar or NULL
	 */
	virtual wxToolBar *CreateToolBar(wxWindow *parent) = 0;

	/**
	 * Every plugin can place a sub menu in the 'Plugins' Menu at the menu bar
	 * \param pluginsMenu
	 */
	virtual void CreatePluginMenu(wxMenu *pluginsMenu) = 0;

	/**
	 * \brief Call the plugin "shutdown" function
	 */
	virtual void UnPlug() = 0;

	/**
	 * Override this method to allow the plugin to
	 * hook the popup menu by adding its entries.
	 * \param menu menu to hook
	 * \param type menu type
	 * \sa  MenuType
	 */
	virtual void HookPopupMenu(wxMenu *menu, MenuType type) {
		wxUnusedVar(type);
		wxUnusedVar(menu);
	};

	/**
	 * @brief un hook previosly hooked popup menu
	 * @param menu the parent of our hooked menu
	 * @param type the parent's type
	 * @sa MenuType
	 */
	virtual void UnHookPopupMenu(wxMenu *menu, MenuType type) {
		wxUnusedVar(type);
		wxUnusedVar(menu);
	};
	
	/**
	 * @brief load image file from /path/to/install/plugins/resources/
	 * @param name file name (name+extension)
	 * @return Bitmap of wxNullBitmap if no match was found
	 */
	virtual wxBitmap LoadBitmapFile(const wxString &name, wxBitmapType type = wxBITMAP_TYPE_PNG) {
		wxBitmap bmp;
		wxString basePath(m_mgr->GetInstallDirectory() + wxT("/plugins/resources/"));
	
		bmp.LoadFile(basePath + name, type);
		if(bmp.IsOk()) {
			return bmp;
		}
		return wxNullBitmap;
	}
};

//Every dll must contain at least this function
typedef IPlugin* (*GET_PLUGIN_CREATE_FUNC)(IManager*);
typedef PluginInfo (*GET_PLUGIN_INFO_FUNC)();
typedef int (*GET_PLUGIN_INTERFACE_VERSION_FUNC)();
#endif //PLUGIN_H
