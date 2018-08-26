//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : callgraph.h
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

// forward references
class wxFileOutputStream;
class wxTextOutputStream;

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
    CallGraph(IManager* manager);
    /**
     * @brief Defautl destructor.
     */
    ~CallGraph();
    //--------------------------------------------
    // Abstract methods
    //--------------------------------------------
    /**
     * @brief Function create tool bar menu and sets icon of plugin Call graph.
     * @param parent
     * @return
     */
    virtual void CreateToolBar(clToolBar* toolbar);
    /**
     * @brief Function create plugin menu for Call graph used in menu Plugins of Codelite.
     * @param pluginsMenu
     */
    virtual void CreatePluginMenu(wxMenu* pluginsMenu);

    /**
     * @brief Function unplug the plugin from CodeLite IDE.
     */
    virtual void UnPlug();

    virtual void HookPopupMenu(wxMenu* menu, MenuType type);

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

    wxFileOutputStream* m_LogFile;

    void LogFn(wxString s);

protected:
    void MessageBox(const wxString& msg, unsigned long icon_mask);
    wxString LocateApp(const wxString& app_name);

    /**
     * @brief Function create and open About dialog Call graph plugin.
     * @param event Reference to event class
     */
    void OnAbout(wxCommandEvent& event);
    /**
     * @brief Function create new tab page and insert picture with call graph and table with function of the project.
     * @param event Reference to event class
     */
    void OnShowCallGraph(wxCommandEvent& event);
    /**
     * @brief Handle function to open dialog with settings for Call graph plugin.
     * @param event Reference to event class
     */
    void OnSettings(wxCommandEvent& event);

    /**
     * @brief Create custom plugin's popup menu.
     * @return Plugin's popup menu
     */
    wxMenu* CreateProjectPopMenu();
    /**
     * @brief Pointer cgWnd type wxScrolledWindow used in tab page for display call graph picture.
     */
    wxScrolledWindow* cgWnd; // pointer cgWnd type wxScrolledWindow used in tab page for display call graph picture
                             /**
                              * @brief Pointer m_pInputStream type wxInputStream returned from gprof application.
                              */
    // wxInputStream *m_pInputStream; // pointer m_pInputStream type wxInputStream returned from gprof application
    /**
     * @brief Object confData type ConfCallGraph with stored configuration data.
     */
    ConfCallGraph confData; // object confData type ConfCallGraph with stored configuration data

    // wxString        m_ProfiledBinFullName;
};

extern CallGraph* thePlugin;

#endif // CallGraph
