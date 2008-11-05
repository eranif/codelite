//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : pluginmanager.cpp
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
#include "environmentconfig.h"
#include "jobqueue.h"
#include "pluginmanager.h"
#include "pluginsdata.h"
#include "pluginconfig.h"
#include "optionsconfig.h"
#include "language.h"
#include "manager.h"
#include "wx/filename.h"
#include <wx/log.h>
#include <wx/dir.h>
#include "frame.h"
#include "editor_config.h"
#include "fileexplorertree.h"
#include "workspace_pane.h"
#include "fileview.h"
#include "wx/xrc/xmlres.h"
#include "ctags_manager.h"
#include "fileexplorer.h"
#include "plugin_version.h"

PluginManager *PluginManager::Get()
{
	static PluginManager theManager;
	return &theManager;
}

void PluginManager::UnLoad()
{
	std::map<wxString, IPlugin*>::iterator plugIter = m_plugins.begin();
	for (; plugIter != m_plugins.end(); plugIter++) {
		IPlugin *plugin = plugIter->second;
		plugin->UnPlug();
		delete plugin;
	}

	std::list<clDynamicLibrary*>::iterator iter = m_dl.begin();
	for ( ; iter != m_dl.end(); iter++ ) {
		( *iter )->Detach();
		delete ( *iter );
	}

	m_dl.clear();
	m_plugins.clear();
	PluginConfig::Instance()->Release();
}

PluginManager::~PluginManager()
{
}

void PluginManager::Load()
{
	wxString ext;
#if defined (__WXGTK__) || defined (__WXMAC__)
	ext = wxT("so");
#else
	ext = wxT("dll");
#endif
	wxString fileSpec( wxT( "*." ) + ext );
	PluginConfig::Instance()->Load(wxT("config/plugins.xml"));

	PluginsData pluginsData;
	PluginConfig::Instance()->ReadObject(wxT("plugins_data"), &pluginsData);

	//get the map of all available plugins
	m_pluginsInfo = pluginsData.GetInfo();
	std::map<wxString, PluginInfo> actualPlugins;

	//set the managers
	//this code assures us that the shared objects will see the same instances as the application
	//does
	LanguageST::Get()->SetTagsManager( GetTagsManager() );
	TagsManagerST::Get()->SetLanguage( LanguageST::Get() );

	if ( wxDir::Exists( ManagerST::Get()->GetInstallDir() + wxT( "/plugins" ) ) ) {
		//get list of dlls
		wxArrayString files;
		wxDir::GetAllFiles( ManagerST::Get()->GetInstallDir() + wxT( "/plugins" ), &files, fileSpec, wxDIR_FILES );

		for ( size_t i=0; i<files.GetCount(); i++ ) {
			clDynamicLibrary *dl = new clDynamicLibrary();
			wxString fileName( files.Item( i ) );
			if ( !dl->Load( fileName ) ) {
				wxLogMessage( wxT( "Failed to load plugin's dll: " ) + fileName );
                if (!dl->GetError().IsEmpty())
                    wxLogMessage(dl->GetError());
				delete dl;
				continue;
			}

			bool success( false );
			GET_PLUGIN_INFO_FUNC pfnGetPluginInfo = ( GET_PLUGIN_INFO_FUNC )dl->GetSymbol( wxT( "GetPluginInfo" ), &success );
			if ( !success ) {
                wxLogMessage(wxT("Failed to find GetPluginInfo in dll: ") + fileName);
                if (!dl->GetError().IsEmpty())
                    wxLogMessage(dl->GetError());
				delete dl;
				continue;
			}
			
			// load the plugin version method
			// if the methods does not exist, handle it as if it has value of 100 (lowest version API)
			int interface_version(100);
			GET_PLUGIN_INTERFACE_VERSION_FUNC pfnInterfaceVersion = (GET_PLUGIN_INTERFACE_VERSION_FUNC) dl->GetSymbol(wxT("GetPluginInterfaceVersion"), &success);
			if( success ) {
				interface_version = pfnInterfaceVersion();
			} else {
                wxLogMessage(wxT("Failed to find GetPluginInterfaceVersion() in dll: ") + fileName);
                if (!dl->GetError().IsEmpty())
                    wxLogMessage(dl->GetError());
            }
			
			if( interface_version != PLUGIN_INTERFACE_VERSION ) {
				wxLogMessage(wxString::Format(wxT("Version interface mismatch error for plugin '%s'. Plugin's interface version is '%d', CodeLite interface version is '%d'"), 
													fileName.c_str(), 
													interface_version, 
													PLUGIN_INTERFACE_VERSION));
				delete dl;
				continue;
			}
			
			//check if this dll can be loaded
			PluginInfo pluginInfo = pfnGetPluginInfo();
			std::map< wxString, PluginInfo>::const_iterator iter = m_pluginsInfo.find(pluginInfo.GetName());
			if (iter == m_pluginsInfo.end()) {
				//new plugin?, add it
				pluginInfo.SetEnabled(true);
				actualPlugins[pluginInfo.GetName()] = pluginInfo;
			} else {
				//we have a match
				PluginInfo pi = iter->second;
				pluginInfo.SetEnabled(pi.GetEnabled());

				actualPlugins[pluginInfo.GetName()] = pluginInfo;
				if (pluginInfo.GetEnabled() == false) {
					delete dl;
					continue;
				}
			}

			//try and load the plugin
			GET_PLUGIN_CREATE_FUNC pfn = ( GET_PLUGIN_CREATE_FUNC )dl->GetSymbol( wxT( "CreatePlugin" ), &success );
			if ( !success ) {
                wxLogMessage(wxT("Failed to find CreatePlugin() in dll: ") + fileName);
                if (!dl->GetError().IsEmpty())
                    wxLogMessage(dl->GetError());

				//mark this plugin as not available
				pluginInfo.SetEnabled(false);
				actualPlugins[pluginInfo.GetName()] = pluginInfo;

				delete dl;
				continue;
			}

			IPlugin *plugin = pfn( ( IManager* )this );
			wxLogMessage( wxT( "Loaded plugin: " ) + plugin->GetLongName() );
			m_plugins[plugin->GetShortName()] = plugin;

			//load the toolbar
			wxToolBar *tb = plugin->CreateToolBar( ManagerST::Get()->GetMainFrame() );
			if ( tb ) {
				Frame::Get()->GetDockingManager().AddPane( tb, wxAuiPaneInfo().Name( plugin->GetShortName() ).LeftDockable( true ).RightDockable( true ).Caption( plugin->GetShortName() ).ToolbarPane().Top() );

				//Add menu entry at the 'View->Toolbars' menu for this toolbar
				int ii = Frame::Get()->GetMenuBar()->FindMenu( wxT( "View" ) );
				if ( ii != wxNOT_FOUND ) {
					wxMenu *viewMenu = Frame::Get()->GetMenuBar()->GetMenu( ii );
					wxMenu *submenu = NULL;
					wxMenuItem *item = viewMenu->FindItem( XRCID("toolbars_menu") );
					if (item) {
						submenu = item->GetSubMenu();
						//add the new toolbar entry at the end of this menu

						int id = wxNewId();
						wxString text(plugin->GetShortName());
						text << wxT(" ToolBar");
						wxMenuItem *newItem = new wxMenuItem(submenu, id, text, wxEmptyString, wxITEM_CHECK);
						submenu->Append(newItem);
						Frame::Get()->RegisterToolbar(id, plugin->GetShortName());
					}
				}
			}

			//let the plugin plug its menu in the 'Plugins' menu at the menu bar
			//the create menu will be placed as a sub menu of the 'Plugin' menu
			int idx = Frame::Get()->GetMenuBar()->FindMenu( wxT( "Plugins" ) );
			if ( idx != wxNOT_FOUND ) {
				wxMenu *pluginsMenu = Frame::Get()->GetMenuBar()->GetMenu( idx );
				plugin->CreatePluginMenu( pluginsMenu );
			}

			//keep the dynamic load library
			m_dl.push_back( dl );
		}
		Frame::Get()->GetDockingManager().Update();

		//save the plugins data
		PluginsData pluginsData;
		pluginsData.SetInfo(actualPlugins);
		PluginConfig::Instance()->WriteObject(wxT("plugins_data"), &pluginsData);
	}
}

IEditor *PluginManager::GetActiveEditor()
{
	return( IEditor* )ManagerST::Get()->GetActiveEditor();
}

IConfigTool* PluginManager::GetConfigTool()
{
	return EditorConfigST::Get();
}

void PluginManager::HookPopupMenu( wxMenu *menu, MenuType type )
{
	std::map<wxString, IPlugin*>::iterator iter = m_plugins.begin();
	for ( ; iter != m_plugins.end(); iter++ ) {
		iter->second->HookPopupMenu( menu, type );
	}
}

void PluginManager::UnHookPopupMenu( wxMenu *menu, MenuType type )
{
	std::map<wxString, IPlugin*>::iterator iter = m_plugins.begin();
	for ( ; iter != m_plugins.end(); iter++ ) {
		iter->second->UnHookPopupMenu( menu, type );
	}
}

TreeItemInfo PluginManager::GetSelectedTreeItemInfo( TreeType type )
{
	TreeItemInfo info;
	switch ( type ) {
	case TreeFileExplorer:
		return Frame::Get()->GetFileExplorer()->GetFileTree()->GetSelectedItemInfo();
	case TreeFileView:
		return Frame::Get()->GetWorkspacePane()->GetFileViewTree()->GetSelectedItemInfo();
	default:
		return info;
	}
}

wxTreeCtrl *PluginManager::GetTree(TreeType type)
{
	switch ( type ) {
	case TreeFileExplorer:
		return Frame::Get()->GetFileExplorer()->GetFileTree();
	case TreeFileView:
		return Frame::Get()->GetWorkspacePane()->GetFileViewTree();
	default:
		return NULL;
	}
}

Notebook *PluginManager::GetOutputPaneNotebook()
{
	return Frame::Get()->GetOutputPane()->GetNotebook();
}

Notebook *PluginManager::GetWorkspacePaneNotebook()
{
    return Frame::Get()->GetWorkspacePane()->GetNotebook();
}

bool PluginManager::OpenFile(const wxString &fileName, const wxString &projectName, int lineno)
{
	return ManagerST::Get()->OpenFile(fileName, projectName, lineno);
}

wxString PluginManager::GetStartupDirectory() const
{
	return ManagerST::Get()->GetStarupDirectory();
}

void PluginManager::AddProject(const wxString &path)
{
	ManagerST::Get()->AddProject(path);
}

bool PluginManager::IsWorkspaceOpen() const
{
	return ManagerST::Get()->IsWorkspaceOpen();
}

TagsManager *PluginManager::GetTagsManager()
{
	return TagsManagerST::Get();
}

Workspace *PluginManager::GetWorkspace()
{
	return WorkspaceST::Get();
}

bool PluginManager::AddFilesToVirtualFodler(wxTreeItemId &item, wxArrayString &paths)
{
	return Frame::Get()->GetWorkspacePane()->GetFileViewTree()->AddFilesToVirtualFodler(item, paths);
}

bool PluginManager::AddFilesToVirtualFodler(const wxString &vdFullPath, wxArrayString &paths)
{
	return Frame::Get()->GetWorkspacePane()->GetFileViewTree()->AddFilesToVirtualFodler(vdFullPath, paths);
}

int PluginManager::GetToolbarIconSize()
{
	//for now return 24 by default
	OptionsConfigPtr options = EditorConfigST::Get()->GetOptions();
	if (options) {
		return options->GetIconsSize();
	}
	return 24;
}

Notebook* PluginManager::GetMainNotebook()
{
	return Frame::Get()->GetNotebook();
}

wxAuiManager* PluginManager::GetDockingManager()
{
	return &Frame::Get()->GetDockingManager();
}

EnvironmentConfig* PluginManager::GetEnv()
{
	return EnvironmentConfig::Instance();
}

JobQueue* PluginManager::GetJobQueue()
{
	return JobQueueSingleton::Instance();
}
wxString PluginManager::GetProjectExecutionCommand(const wxString& projectName, wxString& wd)
{
	return ManagerST::Get()->GetProjectExecutionCommand(projectName, wd, false);
}

wxApp* PluginManager::GetTheApp()
{
	return wxTheApp;
}

void PluginManager::ReloadWorkspace()
{
	ManagerST::Get()->ReloadWorkspace();
}

IPlugin* PluginManager::GetPlugin(const wxString& pluginName)
{
	std::map<wxString, IPlugin*>::iterator iter = m_plugins.find(pluginName);
	if(iter != m_plugins.end()){
		return iter->second;
	}
	return NULL;
}

void PluginManager::AppendOutputMsg(const wxString& msg)
{
	ManagerST::Get()->OutputMessage(msg);
}

void PluginManager::SaveAll()
{
	ManagerST::Get()->SaveAll(false);
}

wxString PluginManager::GetInstallDirectory() const
{
	return ManagerST::Get()->GetInstallDir();
}

IKeyboard* PluginManager::GetKeyboardManager()
{
	return &m_keyboardMgr;
}

bool PluginManager::CreateVirtualDirectory(const wxString& parentPath, const wxString& vdName)
{
	return Frame::Get()->GetWorkspacePane()->GetFileViewTree()->CreateVirtualDirectory(parentPath, vdName);
}

OptionsConfigPtr PluginManager::GetEditorSettings()
{
    return EditorConfigST::Get()->GetOptions();
}

void PluginManager::FindAndSelect(const wxString& pattern, const wxString& name)
{
    LEditor *editor = ManagerST::Get()->GetActiveEditor();
    if (editor) {
        ManagerST::Get()->FindAndSelect(editor, const_cast<wxString&>(pattern), name);
    }
}
