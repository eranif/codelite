//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : manager.h              
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
 #ifndef MANAGER_H
#define MANAGER_H

#include "acceltabledlg.h"
#include "singleton.h"
#include "cpptoken.h"
#include "wx/string.h"
#include "readtags.h"
#include "entry.h"
#include "project.h"
#include "context_base.h"
#include "ctags_manager.h"
#include "workspace.h"
#include <list>
#include <map>
#include "compile_request.h"
#include "clean_request.h"
#include "wx/event.h"
#include "filehistory.h"
#include "browse_record.h"
#include "wx/timer.h"
#include "debuggermanager.h"
#include "debuggerobserver.h"

class wxFrame;
class LEditor;
class AsyncExeCmd;
class QuickWatchDlg;

class Manager : public wxEvtHandler, public IDebuggerObserver
{
	friend class Singleton<Manager>;
	wxString  m_startupDir;
	CleanRequest *m_cleanRequest;
	CompileRequest *m_compileRequest;
	AsyncExeCmd *m_asyncExeCmd;
	FileHistory m_recentFiles;
	FileHistory m_recentWorkspaces;
	bool m_dbgCanInteract;
	bool m_dbgWaitingFirstBp;
	QuickWatchDlg *m_quickWatchDlg;
	int m_frameLineno;
	bool m_useTipWin;
	long m_tipWinPos;
	wxString m_installDir;
	
public:
	/*!
	 * \brief
	 * check if a workspace is open
	 * 
	 * \returns
	 * true if a workspace is open
	 */
	bool IsWorkspaceOpen() const;

	/*!
	 * \brief
	 * Open a file using file name and line number
	 * 
	 * \param fileName full path of the file name
	 * \param projectName project name, can be wxEmptyString
	 * \param lineno the cursor will be placed at lineno
	 * \param position the position of the match starting from begining
	 */
	bool OpenFile(const wxString &file_name, 
				  const wxString &projectName, 
				  int lineno = wxNOT_FOUND, 
				  long position = wxNOT_FOUND);

	void OpenFile(const BrowseRecord &rec);

	/**
	 * Create new file on the disk and open it in the main editor
	 * \param fileName file full path (including directories)
	 * \param vdFullPath path of the virtual directory
	 */
	bool AddNewFileToProject(const wxString &fileName, const wxString &vdFullPath, bool openIt = true);

	/**
	 * Add an existing file to workspace
	 * \param fileName file full path (including directories)
	 * \param vdFullPath path of the virtual directory
	 */
	bool AddFileToProject(const wxString &fileName, const wxString &vdFullPath, bool openIt = false);
	/**
	 * \brief 
	 * \param files
	 * \param vdFullPath
	 * \param actualAdded
	 * \return 
	 */
	void AddFilesToProject(const wxArrayString &files, const wxString &vdFullPath, wxArrayString &actualAdded);

	/*!
	 * \brief
	 * Set the title for the page (tab)
	 */
	void SetPageTitle(wxWindow *page, const wxString &name);

	/*!
	 * \brief
	 * Return the page title 
	 */	
	const wxString GetPageTitle(wxWindow *win);
	
	/*!
	 * \brief
	 * Save all open documents
	 */	
	void SaveAll(bool includeUntitled = true);

	/*!
	 * \brief
	 * Free all singleton objects initialised in CodeLite
	 */	
	void UnInitialize();

	/*!
	 * \brief
	 * Return a pointer to the main frame
	 */	
	wxFrame *GetMainFrame();

	/*!
	 * \brief
	 * Create a workspace with a given name and path
	 * 
	 * \param name
	 * workspace name
	 * 
	 * \param path
	 * workspace path
	 * 
	 */
	void CreateWorkspace(const wxString &name, const wxString &path);
	 
	/**
	 * \brief create an empty project
	 * \param name project name
	 * \param path project file path
	 * \param type project type, Project::STATIC_LIBRARY, Project::DYNAMIC_LIBRARY or Project::EXECUTABLE
	 */
	void CreateProject(ProjectData &data);

	/**
	 * Open an existing workspace by path
	 */
	void OpenWorkspace(const wxString &path);

	/**
	 * Close the current workspace and save all
	 * changes
	 */
	void CloseWorkspace();

	/**
	 * Return a tree for the file view of a single project
	 */
	ProjectTreePtr GetProjectFileViewTree(const wxString &projectName);

	/**
	 * Return all project names under this workspace
	 */
	void GetProjectList(wxArrayString &list);

	/**
	 * find project by name
	 */
	ProjectPtr GetProject(const wxString &name) const;

	/**
	 * Add an existing project to the workspace. If no workspace is open,
	 * this function does nothing
	 * \param path project file path name to add
	 */
	void AddProject(const wxString &path);

	/**
	 * Remove the a project from the workspace
	 * \param name project name to remove
	 * \return true on success false otherwise
	 */
	bool RemoveProject(const wxString &name) ;

	/**
	 * \return active project name
	 */
	wxString GetActiveProjectName();

	/**
	 * Set project as active
	 * \param name project name to set as active
	 */
	void SetActiveProject(const wxString &name);

	/**
	 * Add new virtual directory to the workspace.
	 * \param virtualDirFullPath a dot separated string of the new virtual directory full path up to the parent project
	 *        for example: to add a new VD name VD3 under: Project1->VD1->VD2 path should contain: Project1.VD1.VD2.VD3
	 */
	void AddVirtualDirectory(const wxString &virtualDirFullPath);

	/**
	 * Remove virtual directory from the workspace.
	 * \param virtualDirFullPath a dot separated string of the virtual directory to be removed
	 */
	void RemoveVirtualDirectory(const wxString &virtualDirFullPath);

	/**
	 * Save workspace
	 */
	void SaveWorkspace();

	/**
	 * remove file from the workspace
	 * \param fileName the full path of the file to be removed
	 * \param vdFullPath the files' virtual directory path (including project)
	 */
	bool RemoveFile(const wxString &fileName, const wxString &vdFullPath);

	/**
	 * Return a project working directory
	 * \param project project name
	 */
	wxString GetProjectCwd(const wxString &project) const;

	/**
	 * Show output pane and set focus to focusWin
	 * \param focusWin tab name to set the focus
	 * \return return true if the output pane was hidden and this function forced it to appear. false if the window was already
	 * shown and nothing needed to be done 
	 */
	bool ShowOutputPane(wxString focusWin = wxEmptyString, bool commit = true );

	/**
	 * Show the debugger pane
	 */
	void ShowDebuggerPane(bool commit = true);

	/**
	 * Show the main toolbar
	 * \param show set to true to show it, false otherwise
	 */
	void ShowMainToolbar(bool show = true);
	
	/**
	 * Show the workspace pane and set focus to focusWin
	 * \param focusWin tab name to set the focus
	 */
	void ShowWorkspacePane(wxString focusWin = wxEmptyString, bool commit = true );

	/**
	 * Hide pane
	 */
	void HidePane(const wxString &paneName, bool commit = true);

	/**
	 * Return the context by file name
	 * \param fileName
	 * \param parent the lexer's parent
	 * \return lexer , if no lexer is matched with the fileName, the "Default" context is returned
	 */
	ContextBasePtr NewContextByFileName(const wxFileName &fileName, LEditor *parent) const;

	/**
	 * Reload configuration changes and apply them on open documents
	 */ 
	void ApplySettingsChanges();

	/**
	 * Return project settings by name
	 * \param projectName project name
	 * \return project settings smart prt
	 */
	ProjectSettingsPtr GetProjectSettings(const wxString &projectName) const;

	/**
	 * Set project settings
	 * \param projectName project name
	 * \param settings settings to update
	 */
	void SetProjectSettings(const wxString &projectName, ProjectSettingsPtr settings);

	/** 
	 * Return the workspace build matrix
	 */
	BuildMatrixPtr GetWorkspaceBuildMatrix() const;

	/**
	 * Set or update the workspace build matrix
	 */
	void SetWorkspaceBuildMatrix(BuildMatrixPtr matrix);

	/**
	 * Hide/Show all panes. This function saves the current prespective and 
	 * then hides all panes, when called again, all panes are restored
	 */
	void TogglePanes();

	/**
	 * create default new compiler
	 * \return true on success false when a compiler with this name already exist
	 */
	bool CreateDefaultNewCompiler(const wxString &name);

	/**
	 * delete compiler
	 * \param name compiler to delete
	 */
	bool DeleteCompiler(const wxString &name);

	/**
	 * Return a list of availanle project templates
	 * \param list output
	 */
	void GetProjectTemplateList(std::list<ProjectPtr> &list);

	/**
	 * Save project as template
	 * \param proj project to duplicate
	 * \param name the template name
	 * \param description project short description 
	 */
	void SaveProjectTemplate(ProjectPtr proj, const wxString &name, const wxString &description);

	/**
	 * Set lite editor's startup directory
	 */
	void SetStarupDirectory(const wxString &path){ m_startupDir = path; }

	/**
	 * Get lite editor's startup directory
	 */
	const wxString &GetStarupDirectory() const { return m_startupDir; }
	
	/**
	 * \brief return the OS specific installation directory
	 */
	const wxString &GetInstallDir() const { return m_installDir; }
	
	/**
	 * \brief set the installation directory 
	 */
	void SetInstallDir(const wxString &dir) { m_installDir = dir; }
	
	/**
	 * Popup project dependencies dialog
	 * \param projectName project name
	 */
	void PopupProjectDependsDlg(const wxString &projectName);

	/**
	 * Pass a command to the compiler thread to clean the 
	 * given project
	 */
	void CleanProject(const wxString &project, bool projectOnly = false);
	
	/**
	 * Pass a command to the compiler thread to build the 
	 * given project
	 */
	void BuildProject(const wxString &project, bool projectOnly = false);

	/**
	 * \brief when building using custom build, execute the makefile generation command provided by the user
	 * \param project project to execute it for
	 */
	void RunCustomPreMakeCommand(const wxString &project);
	
	/**
	 * compile single file from a given
	 * given project
	 */
	void CompileFile(const wxString &project, const wxString &fileName);

	/** 
	 * Stop current build process
	 */
	void StopBuild();

	/**
	 * return true if a compilation is in process (either clean or build)
	 */
	bool IsBuildInProgress() const;

	/**
	 * return true a child program is running
	 */
	bool IsProgramRunning() const;

	/**
	 * Kill child program which is running
	 */
	void KillProgram();

	/**
	 * Write line to child program
	 * \param line 
	 */
	void WriteProgram(const wxString &line);

	/** 
	 * Execute the project with no debugger
	 */
	void ExecuteNoDebug(const wxString &projectName);

	// Update the toolbar's configuration choice controls
	void DoUpdateConfigChoiceControl();

	/**
	 * Set the workspace active build configuration name
	 * \param name active configuration name
	 */
	void SetWorkspaceConfigurationName(const wxString &name);

	/**
	 * Return the active's project build configuration that matches 
	 * the workspace selected configuration
	 */
	BuildConfigPtr GetActiveProjectBuildConf();

	/** 
	 * use an external database
	 */
	void SetExternalDatabase(const wxFileName &dbname);

	/** 
	 * close the currenlty open extern database
	 * and free all its resources
	 */
	void CloseExternalDatabase();

	/** 
	 * return list of files that are part of the workspace
	 */
	void GetWorkspaceFiles(std::vector<wxFileName> &files, bool absPath = false);

	/**
	 * check if a file is part of the workspace
	 * \param fileName the file name in absolute path
	 */
	bool IsFileInWorkspace(const wxString &fileName);

	/**
	 * Rebuild the database by removing all entries from the database
	 * that belongs to a given project, and then re-index all files 
	 * \param projectName project to re-tag
	 */
	void RetagProject(const wxString &projectName);

	/**
	 * retag workspace
	 */
	void RetagWorkspace();
	
	/**
	 * \brief retag a given file
	 * \param filename
	 */
	void RetagFile(const wxString &filename);
	
	/**
	 * Add a debug message in the debug output window
	 * \param msg 
	 */
	void DebugMessage(wxString msg);

	/**
	 * Add a message in the output tab window
	 * \param msg 
	 */
	void OutputMessage(wxString msg);

	/**
	 * return the project name that 'fullPathFileName' belogs to. if 2 matches were found, return
	 * the first one, or empty string if no match was found
	 */
	wxString GetProjectNameByFile(const wxString &fullPathFileName);

	/** 
	* Import a workspace from a makefile
	*/ 	
// 	void ImportFromMakefile(const wxString &path);

	/**
	 * Return the active editor or NULL if none
	 */
	LEditor *GetActiveEditor() const;

	/**
	 * Return the container editor for fileName or NULL
	 */
	LEditor *FindEditorByFileName(const wxString &fileName) const;

	/**
	 * add single file to the recently opened files
	 */ 
	void AddToRecentlyOpenedFiles(const wxString &fileName);
	
	/**
	 * return the FileHistory object that holds the recently opened
	 * files data
	 */
	FileHistory &GetRecentlyOpenedFilesClass() {return m_recentFiles;}

	/**
	 * add workspace file to the recently opened workspaces
	 */ 
	void AddToRecentlyOpenedWorkspaces(const wxString &fileName);
	
	/**
	 * return the FileHistory object that holds the recently opened
	 * workspace data
	 */
	FileHistory &GetRecentlyOpenedWorkspacesClass() {return m_recentWorkspaces;}

	/**
	 * Return list of recently opened files from the configuration file
	 * \param files [output]
	 */
	void GetRecentlyOpenedFiles(wxArrayString &files);

	/**
	 * Return list of recently opened workspaces from the configuration file
	 * \param files [output] 
	 */
	void GetRecentlyOpenedWorkspaces(wxArrayString &files);

	/**
	 * insert message to the status bar at a given column
	 * \param msg 
	 * \param col 
	 */
	void SetStatusMessage(const wxString &msg, int col);

	/**
	 * close all opened documents
	 */
	void CloseAll();
	
	/**
	 * \brief close all open editors except for the active one
	 */
	void CloseAllButThis(wxWindow *curreditor);
	
	bool MoveFileToVD(const wxString &fileName, const wxString &srcVD, const wxString &targetVD);

	/**
	 * return list of files in absolute path of a given project
	 * \param project project name
	 */
	void GetProjectFiles(const wxString &project, wxArrayString &files);

	/**
	 * return list of files in absolute path of the whole workspace
	 */
	void GetWorkspaceFiles(wxArrayString &files);
	
	/**
	 * Import a MS Solution file and open it in the editor
	 * \param path path to the .sln file
	 */
	void ImportMSVSSolution(const wxString &path);

	/**
	 * Expand variables to their real value, if expanding fails
	 * the return value is same as input. The variable is expanded
	 * in the project context
	 */
	wxString ExpandVariables(const wxString &expression, ProjectPtr proj);

	/**
	 * return true if the last buid ended successfully
	 */
	bool IsBuildEndedSuccessfully() const;

	void OnOutputWindow(wxCommandEvent &e);
	void OnDebuggerWindow(wxCommandEvent &e);
	
	/**
	 * \brief update the path & name of the build tool 
	 * on windows, try to locate make, followed by mingw32-make 
	 */
	void UpdateBuildTools();
	
	/**
	 * \brief open file specified by the 'fileName' parameter and append 'text'
	 * to its content 
	 * \param fileName file to open. Must be in full path
	 * \param text string text to append
	 * \return true on success, false otherwise
	 */
	bool OpenFileAndAppend(const wxString &fileName, const wxString &text);
	
	/**
	 * \brief update the menu bar accelerators
	 */
	void UpdateMenuAccelerators();
	
	/**
	 * \brief load accelerator table from the configuration section
	 * \param file
	 * \param map
	 */
	void LoadAcceleratorTable(const wxString &file, MenuItemDataMap &map);
	
	/**
	 * \brief retrun map of the accelerator table. the StringMap maps between the actions and their accelerators
	 */
	void GetAcceleratorMap(MenuItemDataMap& accelMap);
	
	/**
	 * \brief retrun map of the default accelerator table
	 */
	void GetDefaultAcceleratorMap(MenuItemDataMap& accelMap);
	
	/**
	 * \brief parse list of files and construct a token database that will be used for refactoring
	 * \param word word to search
	 * \param files list of files to parse 
	 */
	void BuildRefactorDatabase(const wxString& word, CppTokensMap &l);
	
	/**
	 * \brief replace list of toens representd by li with 'word'
	 * \param li
	 * \return 
	 */
	void ReplaceInFiles(const wxString &word, std::list<CppToken> &li);
	
	void UpdateMenu(wxMenu *menu, MenuItemDataMap &accelMap, std::vector< wxAcceleratorEntry > &accelVec);
	
	/**
	 * \brief return the project excution command as it appears in the project settings
	 * \param projectName
	 * \param wd the working directory that the command should be running from 
	 * \param considerPauseWhenExecuting when set to true (default) CodeLite will take into consideration the value set in the project
	 * settings 'Pause when execution ends' 
	 * \return project execution command or wxEmptyString if the project does not exist
	 */
	wxString GetProjectExecutionCommand(const wxString &projectName, wxString &wd, bool considerPauseWhenExecuting = true);
	
	/**
	 * \brief find pattern in editor. If match succeeded, try to find 'name' inside the pattern
	 * and highlight it 
	 */
	void FindAndSelect(LEditor* editor, wxString& pattern, const wxString& name);
	
	//--------------------------------------------------------------------
	//IDebuggerObserver implementation. These set of functions are called
	//from the debugger whenever event occurs there
	//--------------------------------------------------------------------
	void UpdateStopped();
	void UpdateAddLine(const wxString &line);
	void UpdateBpAdded();
	void UpdateFileLine(const wxString &file, int lineno);
	void UpdateGotControl(DebuggerReasons reason);
	void UpdateLostControl();
	void UpdateLocals(TreeNode<wxString, NodeData> *tree);
	void UpdateExpression(const wxString &expression, const wxString &evaluated);
	void UpdateQuickWatch(const wxString &expression, TreeNode<wxString, NodeData> *tree);
	void UpdateStackList(const StackEntryArray &stackArray);

	//----------------------------------------------------------
	// Debugging API
	//----------------------------------------------------------
	void DbgStart(long pid = wxNOT_FOUND);
	void DbgStop();
	void DbgDeleteBreakpoint(const BreakpointInfo &bp);
	void DbgDeleteAllBreakpoints();
	void DbgDoSimpleCommand(int cmd);
	void DbgMarkDebuggerLine(const wxString &fileName, int lineno);
	void DbgUnMarkDebuggerLine();
	void DbgQuickWatch(const wxString &expression, bool useTipWin = false, long pos = wxNOT_FOUND);
	void DbgCancelQuickWatchTip();
	void DbgSetFrame(int frame, int lineno);
	void DbgSetThread(long threadId);
	bool DbgCanInteract(){return m_dbgCanInteract;}
	void UpdateDebuggerPane();
	
protected:
	Manager(void);
	virtual ~Manager(void);
	void OnProcessEnd(wxProcessEvent &event);
	void DoShowPane(const wxString &pane);
	
private:
	void RemoveProjectNameFromOpenFiles(const std::vector<wxFileName> &project_files);

	/**
	 * Update the symbol & file tress
	 */
	void RebuildFileView();
	
	/**
	 * Remove a file from the gui tree
	 */
	void RemoveFileFromSymbolTree(const wxFileName &fileName, const wxString &project);
	
	void DumpMenu( wxMenu *menu, const wxString &label, wxString &content );
};

typedef Singleton<Manager> ManagerST;

#endif // MANAGER_H

