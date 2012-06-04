
/***************************************************************
 * Name:      callgraph.h
 * Purpose:   Header to create plugin
 * Author:    Vaclav Sprucek
 * Created:   2012-03-04
 * Copyright: Vaclav Sprucek 
 * License:   wxWidgets license (www.wxwidgets.org)
 * Notes:
 **************************************************************/

#ifndef __CallGraph__
#define __CallGraph__

#include "plugin.h"
//
#include <wx/wx.h>
#include <wx/process.h>
#include <wx/stream.h>
#include "confcallgraph.h"
#include "gprofparser.h"
#include "dotwriter.h"
#include "static.h"


/**
 * @class CallGraph
 * @brief Class define structure for plugin interface.
 */
class CallGraph : public IPlugin
{
public:
	/**
	 * @brief Defautl constructor.
	 * @param manager
	 */
	CallGraph(IManager *manager);
	/**
	 * @brief Defautl destructor.
	 */
	~CallGraph();
	//--------------------------------------------
	//Abstract methods
	//--------------------------------------------
	/**
	 * @brief Function create tool bar menu and sets icon of plugin Call graph.
	 * @param parent
	 * @return 
	 */
	virtual clToolBar *CreateToolBar(wxWindow *parent);
	/**
	 * @brief Function create plugin menu for Call graph used in menu Plugins of Codelite.
	 * @param pluginsMenu
	 */
	virtual void CreatePluginMenu(wxMenu *pluginsMenu);
	
	/**
	 * @brief Create plugin's custom popup menu for different places.
	 * @param menu Pointer to parent menu
	 * @param type Parent menu type
	 */
	virtual void HookPopupMenu(wxMenu *menu, MenuType type);
	/*virtual void UnHookPopupMenu(wxMenu *menu, MenuType type);*/
	/**
	 * @brief Function unplug the plugin from CodeLite IDE.
	 */
	virtual void UnPlug();
	
	/**
	 * @brief Return string with value path for external application gprof which is stored in configuration data.
	 * @return Path to 'gprof' tool
	 */
	wxString GetGprofPath();
	/**
	 * @brief Return string with value path for external application dot which is stored in configuration data.
	 * @return Path to 'dot' tool
	 */
	wxString GetDotPath();
	
protected:
	/**
	 * @brief Function create and open About dialog Call graph plugin.
	 * @param event Reference to event class
	 */
	void OnAbout(wxCommandEvent &event);
	/**
	 * @brief Function create new tab page and insert picture with call graph and table with function of the project.
	 * @param event Reference to event class
	 */
	void OnShowCallGraph(wxCommandEvent &event);
	/**
	 * @brief Handle function to open dialog with settings for Call graph plugin. 
	 * @param event Reference to event class
	 */
	void OnSettings(wxCommandEvent &event);
	
	/**
	 * @brief Create custom plugin's popup menu.
	 * @return Plugin's popup menu
	 */
	wxMenu* CreateProjectPopMenu();
	/**
	 * @brief Pointer cgWnd type wxScrolledWindow used in tab page for display call graph picture.
	 */
	wxScrolledWindow *cgWnd; // pointer cgWnd type wxScrolledWindow used in tab page for display call graph picture
	/**
	 * @brief Pointer m_pInputStream type wxInputStream returned from gprof application.
	 */
	wxInputStream *m_pInputStream; // pointer m_pInputStream type wxInputStream returned from gprof application
	/**
	 * @brief Object confData type ConfCallGraph with stored configuration data.
	 */
	ConfCallGraph confData; // object confData type ConfCallGraph with stored configuration data

};

extern CallGraph* thePlugin;

#endif //CallGraph

