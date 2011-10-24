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

// Windows headers define it
#ifdef __WXMSW__
#ifdef Yield
#undef Yield
#endif
#endif

#include <wx/app.h>
#include "imanager.h"
#include "wx/toolbar.h"
#include <wx/pen.h>
#include "cl_aui_tb_are.h"
#include "cl_defs.h"
#if USE_AUI_TOOLBAR
#include <wx/aui/auibar.h>
#endif
#include "wx/event.h"
#include "wx/notebook.h"
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
	// an attempt to open a file using double click / ENTER was made
	// on an item in the 'File Explorer' OR from the 'Workspace' tree
	// clientData is the full path of the file (wxString*)
	wxEVT_TREE_ITEM_FILE_ACTIVATED,

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

	// the following 2 events are used as "transaction"
	// the first event indicates that any "wxEVT_FILE_SAVED" event sent from this point
	// is due to build process which is about to starte
	// the later event, indicates the end of that transaction
	wxEVT_FILE_SAVE_BY_BUILD_END,
	wxEVT_FILE_SAVE_BY_BUILD_START,

	// clientData is fileName (wxString*)
	wxEVT_FILE_SAVED,
	// clientData is list of files which have been retagged (std::vector<wxFileName>*)
	wxEVT_FILE_RETAGGED,
	// clientData is wxArrayString*: Item(0) = oldName
	//                               Item(1) = newName
	wxEVT_FILE_RENAMED,
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
	wxEVT_ALL_EDITORS_CLOSING,
	//clientData is NULL
	wxEVT_ALL_EDITORS_CLOSED,

	// This event is sent when the user clicks inside an editor
	// this event can not be Veto()
	// clientData is NULL. You may query the clicked editor by calling to
	// IManager::GetActiveEditor()
	wxEVT_EDITOR_CLICKED,

	// User dismissed the Editor's settings dialog with
	// Apply or OK (Settings | Editor)
	// clientData is NULL
	wxEVT_EDITOR_SETTINGS_CHANGED,

	// This event is sent from plugins to the application to tell it to reload
	// any open files (and re-tag them as well)
	wxEVT_CMD_RELOAD_EXTERNALLY_MODIFIED,

	// Same as wxEVT_CMD_RELOAD_EXTERNALLY_MODIFIED
	// just without prompting the user
	// this event only reload code files without
	// any reload to the workspace / project
	wxEVT_CMD_RELOAD_EXTERNALLY_MODIFIED_NOPROMPT,

	// Sent by the project settings dialogs to indicate that
	// the project configurations are saved
	// clientData is the project name (wxString*)
	// event.GetString() returns the selected configuration
	wxEVT_CMD_PROJ_SETTINGS_SAVED,

	/**
	 * Build related events
	 */

	// clientData is NULL
	wxEVT_BUILD_STARTED,

	// clientData is NULL
	wxEVT_BUILD_ENDED,

	// sent by the application to the plugins to indicate that a
	// build process is about to start. by handling this event
	// and by avoid calling event.Skip() codelite will NOT start
	// the build process
	// clientData is the builded project name (wxString*)
	// event.GetString() returns the selected configuration
	wxEVT_BUILD_STARTING,

	// return the project clean command
	// clientData is the builded project name (wxString*)
	// event.GetString() returns the selected configuration
	// the returned answer is expected in the event.SetString() and by
	// avoid calling event.Skip()
	wxEVT_GET_PROJECT_CLEAN_CMD,

	// return the project build command
	// clientData is the builded project name (wxString*)
	// event.GetString() returns the selected configuration
	// the returned answer is expected in the event.SetString() and by
	// avoid calling event.Skip()
	wxEVT_GET_PROJECT_BUILD_CMD,

	// the below two events are sent by the application to the plugins to query whether a
	// given project and build configuration are handled by the plugin.
	// the first event is whether the makefile file is generated by the plugin
	// and the second is to know whether the build process is also being
	// handled by the plugin.
	// clientData is the builded project name (wxString*)
	// event.GetString() returns the selected configuration
	// the return answer is done by simply avoid calling event.Skip() (which will result in ProcessEvent() == true)
	wxEVT_GET_IS_PLUGIN_MAKEFILE,
	wxEVT_GET_IS_PLUGIN_BUILD,

	/**
	 * Debug related events
	 */

	// sent when the debugger is about to start
	// clientData is a pointer to a DebuggerStartupInfo structure
	wxEVT_DEBUG_STARTING,

	// sent right after the debugger started; program is not running yet
	// clientData is a pointer to a DebuggerStartupInfo structure
	wxEVT_DEBUG_STARTED,

	// sent just before the debugger stops
	// clientData is NULL
	wxEVT_DEBUG_ENDING,

	// sent after the debugger stopped
	// clientData is NULL
	wxEVT_DEBUG_ENDED,

	// set when the editor gains or loses
	// the control over the debugger
	wxEVT_DEBUG_EDITOR_LOST_CONTROL,
	wxEVT_DEBUG_EDITOR_GOT_CONTROL,

	/**
	 ** Build events (additional)
	 **/
	// These events allows the plugins to concatenate a string
	// to the compilation/link line of the default build system
	// By using the event.SetString()/event.GetString()
	// Note, that the since all multiple plugins
	// might be interesting with this feature, it is recommened
	// to use it like this:
	// wxString content = event.GetString();
	// content << wxT(" -DMYMACRO ");
	// event.SetString( content );
	// event.Skip();
	wxEVT_GET_ADDITIONAL_COMPILEFLAGS,
	wxEVT_GET_ADDITIONAL_LINKFLAGS,

	// Sent to the plugins to request to export the makefile
	// for the project + configuration
	// clientData is the builded project name (wxString*)
	// event.GetString() returns the selected configuration
	wxEVT_PLUGIN_EXPORT_MAKEFILE,
	
	/////////////////////////////////////////////////
	// Code completion events
	/////////////////////////////////////////////////
	
	// User hit Ctrl-Space in the editor
	// let the plugins a chance to handle this
	// event. 
	// Call event.GetInt() to get the position in the current editor
	// Use m_mgr->GetActiveEditor() to get the active editor
	wxEVT_CMD_CODE_COMPLETE,
	
	// A tooltip is requested for the selected entry in the completion box
	// clientData is set to the client data set by the user
	// the plugin returns the tooltip to the IDE using the:
	// evt.SetString(..) method
	// Use evt.GetClientData() to retrieve the client data associated with this tag
	wxEVT_CMD_CODE_COMPLETE_TAG_COMMENT,
	
	// A function calltip is requesed
	// clientData is set to the client data set by the user
	// the plugin returns the tooltip to the IDE using the:
	// evt.SetString(..) method
	wxEVT_CMD_CODE_COMPLETE_FUNCTION_CALLTIP,
	
	// The code completion box has been dismissed
	wxEVT_CMD_CODE_COMPLETE_BOX_DISMISSED,
	
	// User has requested to display the current files' outline
	// Use m_mgr->GetActiveEditor() to get the active editor
	wxEVT_CMD_SHOW_QUICK_OUTLINE,
	
	// User is hovering a text, display the typeinfo
	// User m_mgr->GetActiveEditor() to get the current editor Or
	// IEditor* editor = dynamic_cast<IEditor*>(evt.GetEventObject());
	// Hover position is set in the evt.GetInt()
	wxEVT_CMD_TYPEINFO_TIP,
	
	// codelite is about to display a code-completion
	// box with the language specific keywords
	// User m_mgr->GetActiveEditor() to get the current editor Or
	// IEditor* editor = dynamic_cast<IEditor*>(evt.GetEventObject());
	wxEVT_CMD_DISPLAY_LANGUAGE_KEYWORDS_CC_BOX,
	
	/////////////////////////////////////////////////
	// Project management events
	/////////////////////////////////////////////////
	
	// User selected an option to create a new workspace
	wxEVT_CMD_CREATE_NEW_WORKSPACE,
	// User selected an option to create a new project
	wxEVT_CMD_CREATE_NEW_PROJECT,
	
	// User requested to open a workspace
	// By default codelite will open a dialog to open a workspace with the '.workspace' suffix.
	// If a plugin wishes to offer a user a dialog with other extension (for example: .myworkspace)
	// it should handle this event and open the dialog itself and then based on the selected
	// file extension it should decide what to do: pass the selected file to codelite (assuming user selected the
	// standard .workspace file, or handle it by itself).
	// to pass the selection to codelite, simply set it in the evt.SetString(...) function
	// If the plugin wishes to handle the file by itself, it should avoid calling evt.Skip()
	wxEVT_CMD_OPEN_WORKSPACE,
	
	// User requested to close the workspace.
	wxEVT_CMD_CLOSE_WORKSPACE,
	
	// This event is sent by codelite to the plugins to query whether a 
	// a custom workspace is opened (i.e. a worksapce which is completely managed
	// by the plugin) this allows codelite to enable menu items which otherwise
	// will be left disabled
	// to return a true or false reply to codelite, use
	// evt.SetInt(0) or evt.SetInt(1) 
	wxEVT_CMD_IS_WORKSPACE_OPEN,
	
	// User has requested a retagging for the workspace
	wxEVT_CMD_RETAG_WORKSPACE,
	wxEVT_CMD_RETAG_WORKSPACE_FULL,
	
	// codelite has requested a complete list of the workspace files.
	// The result should be wxArrayString that contains a list of files
	// in their ABSOLUTE path.
	// The wxArrayString IS OWNED by codelite 
	// and the plugin should NOT attempt to delete it
	// <code>
	// if(I_want_to_handle_this_event) {
	// 		wxArrayString *files = (wxArrayString *) event.GetClientData();
	// 		<fill the files* array ...>
	// } else {
	// 		event.Skip();
	// }
	// </code>
	wxEVT_CMD_GET_WORKSPACE_FILES,
	// Same as the above event, however you should return 
	// a list of the current active project files
	wxEVT_CMD_GET_ACTIVE_PROJECT_FILES,
	// Same as the above event, however you should return 
	// a list of the current files' project files list
	wxEVT_CMD_GET_CURRENT_FILE_PROJECT_FILES,
	
	/////////////////////////////////////////////////
	// Search events
	/////////////////////////////////////////////////
	
	// User requested to open the resource dialog with the filter set to 'workspace file'
	wxEVT_CMD_OPEN_RESOURCE,
	// User requested to open the resource dialog with the filter set to 'function'
	wxEVT_CMD_OPEN_RESOURCE_FUNCTION,
	// User requested to open the resource dialog with the filter set to 'Type (class)'
	wxEVT_CMD_OPEN_RESOURCE_TYPE,
	// User requested to open the resource dialog with the filter set to 'macros'
	wxEVT_CMD_OPEN_RESOURCE_MACRO,
	// User requested to open the resource dialog with the filter set to 'typedef'
	wxEVT_CMD_OPEN_RESOURCE_TYPEDEF,
	
	// User requested to perform a raw search for a symbol
	// Use evt.GetString() to get the searched string
	wxEVT_CMD_FIND_SYMBOL,
	
	// codelite is about to display the editor's context menu
	// A plugin can override the default menu display by catching this event and
	// handling it differently
	// event.GetEventObject() holds a pointer to the editor triggered 
	// the menu
	wxEVT_CMD_EDITOR_CONTEXT_MENU,
	
	// codelite is about to display the editor's *left margin* context menu
	// A plugin can override the default menu display by catching this event and
	// handling it differently
	// event.GetEventObject() holds a pointer to the editor triggered 
	// the menu
	wxEVT_CMD_EDITOR_MARGIN_CONTEXT_MENU,
	
	// codelite is requesting for the find-in-files file masking.
	// the format should be:
	// *.a;*.b
	// and should be placed at:
	// event.SetString("*.a;*.b");
	// The plugin can also control what codelite will do with its masking by 
	// setting the event.SetInt() with the following values (the plugin can use
	// bitwise OR):
	// 0x00000001 -> append the plugins' masking to the default masking
	// 0x00000002 -> prepend the plugins' masking to default masking
	// 0x00000004 -> replace default masking with the one provided by the plugin
	// 0x00000008 -> select the plugin's masking by default
	wxEVT_CMD_GET_FIND_IN_FILES_MASK
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
	virtual clToolBar *CreateToolBar(wxWindow *parent) = 0;

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
	 * @brief load image file from /path/to/install/plugins/resources/
	 * @param name file name (name+extension)
	 * @return Bitmap of wxNullBitmap if no match was found
	 */
	virtual wxBitmap LoadBitmapFile(const wxString &name, wxBitmapType type = wxBITMAP_TYPE_PNG) {
		wxBitmap bmp;
#ifdef __WXGTK__
		wxString pluginsDir(PLUGINS_DIR, wxConvUTF8);
#else
		wxString pluginsDir(m_mgr->GetInstallDirectory() + wxT( "/plugins" ));
#endif
		wxString basePath(pluginsDir + wxT("/resources/"));

		bmp.LoadFile(basePath + name, type);
		if (bmp.IsOk()) {
			return bmp;
		}
		return wxNullBitmap;
	}


	/**
	 * @brief allow the plugins to hook a tab in the project settings
	 * @param notebook the parent
	 * @param configName the associated configuration name
	 */
	virtual void HookProjectSettingsTab(wxBookCtrlBase *notebook, const wxString &projectName, const wxString &configName) {
		wxUnusedVar( notebook );
		wxUnusedVar( projectName );
		wxUnusedVar( configName );
	}

	/**
	 * @brief Unhook any tab from the project settings dialog
	 * @param notebook the parent
	 * @param configName the associated configuration name
	 */
	virtual void UnHookProjectSettingsTab(wxBookCtrlBase *notebook, const wxString &projectName, const wxString &configName) {
		wxUnusedVar( notebook );
		wxUnusedVar( projectName );
		wxUnusedVar( configName );
	}
};

#define CHECK_CL_SHUTDOWN(){\
	if(m_mgr->IsShutdownInProgress())\
		return;\
}

//Every dll must contain at least this function
typedef IPlugin*    (*GET_PLUGIN_CREATE_FUNC)            (IManager*);
typedef PluginInfo  (*GET_PLUGIN_INFO_FUNC)              ();
typedef int         (*GET_PLUGIN_INTERFACE_VERSION_FUNC) ();
#endif //PLUGIN_H
