//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : imanager.h              
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
 #ifndef IMANAGER_H
#define IMANAGER_H

#include "ieditor.h"
#include "iconfigtool.h"
#include "wx/treectrl.h"
#include "custom_notebook.h"

class TagsManager;
class Workspace;
class EnvironmentConfig;
class JobQueue;

//--------------------------
//Auxulary class
//--------------------------
class TreeItemInfo {
public:
	wxTreeItemId m_item;
	wxFileName m_fileName;	//< FileName where available (FileView & File Explorer trees) 
	wxString m_text;		//< Tree item text (all)
	int	m_itemType;			//< For FileView items (FileView only)
};

//---------------------------
// List of availabla trees 
//---------------------------
enum TreeType {
	TreeFileView = 0,
	TreeFileExplorer,
};

//------------------------------------------------------------------
// Defines the interface of the manager
//------------------------------------------------------------------
/**
 * @class IManager
 * @author Eran
 * @date 05/07/08
 * @file imanager.h
 * @brief every plugin holds an instance of this class. 
 * You should use this class to interact with CodeLite
 */
class IManager {
public:
	IManager(){}
	virtual ~IManager(){}
	
	//return the current editor
	/**
	 * @brief return the active editor 
	 * @return pointer to the current editor, or NULL incase the active editor is not of type LEditor or no active editor open
	 */
	virtual IEditor *GetActiveEditor() = 0; 
	/**
	 * @brief open file and make it the active editor
	 * @param fileName the file to open - use absolute path
	 * @param projectName project to associate this file - can be wxEmptyString
	 * @param lineno if lineno is not wxNOT_FOUD, the caret will placed on this line number
	 */
	virtual void OpenFile(const wxString &fileName, const wxString &projectName, int lineno = wxNOT_FOUND) = 0;
	/**
	 * @brief return a pointer to the configuration tool
	 * @sa IConfigTool
	 */
	virtual IConfigTool *GetConfigTool() = 0;
	
	/**
	 * @brief return TreeItemInfo for the selected tree item
	 * @param type the tree we are interested in 
	 * @sa TreeItemInfo
	 * @sa TreeType 
	 */
	virtual TreeItemInfo GetSelectedTreeItemInfo(TreeType type) = 0;
	/**
	 * @brief returns a pointer to wxTreeCtrl by type
	 * @param type the type of tree
	 * @sa TreeType
	 */
	virtual wxTreeCtrl *GetTree(TreeType type) = 0;
	/**
	 * @brief return a pointer to the output pane notebook (the one with the 'output' title)
	 * @return pointer to Notebook 
	 * @sa Notebook 
	 */
	virtual Notebook *GetOutputPaneNotebook() = 0;
	/**
	 * @brief return the startup directory of CodeLite which is also the base directory for searching installation files
	 * @return a full path to the startup directory
	 */
	virtual wxString GetStartupDirectory() const = 0;
	/**
	 * @brief add project to the workspace
	 * @param path full path to the project to add
	 */
	virtual void AddProject(const wxString & path) = 0;
	/**
	 * @brief return true of a workspace is already open
	 */
	virtual bool IsWorkspaceOpen() const = 0;
	/**
	 * @brief return an instance to the tags manager - which allows access to CodeLite CodeCompletion API
	 * @return a pointer to the tags manager
	 * @sa TagsManager
	 */
	virtual TagsManager *GetTagsManager() = 0;
	/**
	 * @brief return a pointer to the workspace manager
	 * @sa Workspace 
	 */
	virtual Workspace *GetWorkspace() = 0;
	/**
	 * @brief add files to a virtual folder in the project
	 * @param item a tree item which represents the tree item of the virtual folder
	 * @param paths an array of files to add
	 * @return true on sucesss, false otherwise
	 */
	virtual bool AddFilesToVirtualFodler(wxTreeItemId &item, wxArrayString &paths) = 0;
	/**
	 * @brief return the size of the icons used by CodeLite
	 * @return 16 or 24
	 */
	virtual int GetToolbarIconSize() = 0;
	/**
	 * @brief return a pointer to the main notebook (the editor' book)
	 * @return pointer to the main notebook
	 */
	virtual Notebook *GetMainNotebook() = 0;
	/**
	 * @brief return a pointer to the docking manager (wxAUI)
	 */
	virtual wxAuiManager* GetDockingManager() = 0;
	/**
	 * @brief return a pointer to the environment manager
	 * @sa EnvironmentConfig 
	 */
	virtual EnvironmentConfig *GetEnv() = 0;
	/**
	 * @brief return a pointer to the job queue manager
	 * \return job queue manager
	 */
	virtual JobQueue *GetJobQueue() = 0;
	
	/**
	 * \brief return the project execution command as set in the project's settings
	 * \param projectName the project
	 * \param wd starting dirctory
	 * \return the execution command or wxEmptyString if the project does not exist
	 */
	virtual wxString GetProjectExecutionCommand(const wxString &projectName, wxString &wd) = 0;
};

#endif //IMANAGER_H

