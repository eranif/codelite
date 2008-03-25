#ifndef PLUGIN_H
#define PLUGIN_H

#include "imanager.h"
#include "wx/toolbar.h"
#include "wx/event.h"

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
	//clientData is fileName (wxString)
	wxEVT_FILE_SAVED			 = 3450,
	//clientData is NULL
	wxEVT_FILE_EXP_REFRESHED,	
	//clientData is NULL
	wxEVT_WORKSPACE_LOADED,		
	//clientData is NULL
	wxEVT_FILE_EXP_INIT_DONE,
	//clientData is list of files added to project (wxArrayString)
	wxEVT_PROJ_FILE_ADDED,
	//clientData is the fileName which has been removed (wxString)
	wxEVT_PROJ_FILE_REMOVED,
	//clientData is the project name (wxString)
	wxEVT_PROJ_REMOVED,
	//clientData is the project name (wxString)
	wxEVT_PROJ_ADDED,
	//clientData is NULL
	wxEVT_INIT_DONE
};

//------------------------------------------------------------------
//each plugin must implement this interface
//------------------------------------------------------------------
class IPlugin : public wxEvtHandler {
protected:
	wxString m_shortName;
	wxString m_longName;
	IManager *m_mgr;

public:
	IPlugin(IManager *manager) : m_mgr(manager){}
	virtual ~IPlugin(){}

	//-----------------------------------------------
	//The interface
	//-----------------------------------------------
	virtual const wxString &GetShortName() const{return m_shortName;}
	virtual const wxString &GetLongName() const{return m_longName;}

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
	 * \param type menu type, can be one of:
	 * -# MenuTypeFileExplorer
	 * -# MenuTypeFileView
	 * -# MenuTypeEditor
	 */
	virtual void HookPopupMenu(wxMenu *menu, MenuType type){
		wxUnusedVar(type);
		wxUnusedVar(menu);
	};

	virtual void UnHookPopupMenu(wxMenu *menu, MenuType type){
		wxUnusedVar(type);
		wxUnusedVar(menu);
	};
};

//Every dll must contain at least this function
typedef IPlugin* (*GET_PLUGIN_CREATE_FUNC)(IManager*);
typedef wxString (*GET_PLUGIN_NAME_FUNC)();

#endif //PLUGIN_H
