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

#include "clToolBar.h"
#include "cl_aui_tb_are.h"
#include "cl_defs.h"
#include "codelite_events.h"
#include "imanager.h"
#include "plugin_version.h"
#include "plugindata.h"
#include "wx/event.h"
#include "wx/notebook.h"
#include "wx/toolbar.h"
#include <wx/app.h>
#include <wx/menu.h>
#include <wx/pen.h>

#ifdef _WIN32
#define STDCALL __stdcall
#define EXPORT __declspec(dllexport)
#else
#define STDCALL
#define EXPORT
#endif

#if defined(__WXMSW__) || defined(__WXGTK__)
#define CL_PLUGIN_API extern "C" EXPORT
#else
// OSX
#define CL_PLUGIN_API extern "C" EXPORT
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

//------------------------------------------------------------------
// each plugin must implement this interface
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
    IManager* m_mgr = nullptr;
    wxMenu* m_pluginsMenu = nullptr;

protected:
    /**
     * @brief delete plugin's menu from the menu bar
     */
    void DeletePluginMenu(wxWindowID id)
    {
        if(!GetPluginsMenu()) {
            return;
        }
        if(GetPluginsMenu()->FindItem(id)) {
            GetPluginsMenu()->Delete(id);
        }
    }

public:
    IPlugin(IManager* manager)
        : m_mgr(manager)
    {
    }
    virtual ~IPlugin() {}

    //-----------------------------------------------
    // The interface
    //-----------------------------------------------
    void SetPluginsMenu(wxMenu* menu) { m_pluginsMenu = menu; }
    const wxMenu* GetPluginsMenu() const { return m_pluginsMenu; }
    wxMenu* GetPluginsMenu() { return m_pluginsMenu; }

    /**
     * @brief return the plugin's short name
     * @return
     */
    virtual const wxString& GetShortName() const { return m_shortName; }
    /**
     * @brief return the plugin's long and more descriptive name
     * @return
     */
    virtual const wxString& GetLongName() const { return m_longName; }

    /**
     * When CodeLite loads all the plugins, this function is called.
     * \param toolbar
     */
    virtual void CreateToolBar(clToolBar* toolbar) = 0;

    /**
     * Every plugin can place a sub menu in the 'Plugins' Menu at the menu bar
     * \param pluginsMenu
     */
    virtual void CreatePluginMenu(wxMenu* pluginsMenu) = 0;

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
    virtual void HookPopupMenu(wxMenu* menu, MenuType type)
    {
        wxUnusedVar(type);
        wxUnusedVar(menu);
    };

    /**
     * @brief load image file from /path/to/install/plugins/resources/
     * @param name file name (name+extension)
     * @return Bitmap of wxNullBitmap if no match was found
     */
    virtual wxBitmap LoadBitmapFile(const wxString& name, wxBitmapType type = wxBITMAP_TYPE_PNG)
    {
        wxBitmap bmp;
#ifdef __WXGTK__
        // /usr/share/codelite
        wxString pluginsDir = clStandardPaths::Get().GetDataDir();
#else
#ifdef USE_POSIX_LAYOUT
        wxString pluginsDir(clStandardPaths::Get().GetDataDir());
#else
        wxString pluginsDir(m_mgr->GetInstallDirectory() + wxT("/plugins"));
#endif
#endif
        wxString basePath(pluginsDir + wxT("/resources/"));

        bmp.LoadFile(basePath + name, type);
        if(bmp.IsOk()) {
            return bmp;
        }
        return wxNullBitmap;
    }

    /**
     * @brief allow the plugins to hook a tab in the project settings
     * @param notebook the parent
     * @param configName the associated configuration name
     */
    virtual void HookProjectSettingsTab(wxBookCtrlBase* notebook, const wxString& projectName,
                                        const wxString& configName)
    {
        wxUnusedVar(notebook);
        wxUnusedVar(projectName);
        wxUnusedVar(configName);
    }

    /**
     * @brief Unhook any tab from the project settings dialog
     * @param notebook the parent
     * @param configName the associated configuration name
     */
    virtual void UnHookProjectSettingsTab(wxBookCtrlBase* notebook, const wxString& projectName,
                                          const wxString& configName)
    {
        wxUnusedVar(notebook);
        wxUnusedVar(projectName);
        wxUnusedVar(configName);
    }
};

#define CHECK_CL_SHUTDOWN()               \
    {                                     \
        if(m_mgr->IsShutdownInProgress()) \
            return;                       \
    }

// Every dll must contain at least this function
typedef IPlugin* (*GET_PLUGIN_CREATE_FUNC)(IManager*);
typedef PluginInfo* (*GET_PLUGIN_INFO_FUNC)();
typedef int (*GET_PLUGIN_INTERFACE_VERSION_FUNC)();
#endif // PLUGIN_H
