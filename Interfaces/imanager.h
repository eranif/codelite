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
#include "project.h"
#include "Notebook.h"
#include "optionsconfig.h"
#include "queuecommand.h"
#include <wx/aui/framemanager.h>
#include "bitmap_loader.h"
#include <vector>
#include "debugger.h"
#include "clStatusBar.h"
#include "clTab.h"

class clEditorBar;
class clWorkspaceView;
class TagsManager;
class clCxxWorkspace;
class EnvironmentConfig;
class JobQueue;
class wxApp;
class IPlugin;
class BuildManager;
class BuildSettingsConfig;
class NavMgr;
class IMacroManager;
class wxAuiManager;

//--------------------------
// Auxulary class
//--------------------------

class TreeItemInfo
{
public:
    wxTreeItemId m_item;
    wxFileName m_fileName; //< FileName where available (FileView & File Explorer trees)
    wxString m_text;       //< Tree item text (all)
    int m_itemType;        //< For FileView items (FileView only)
    wxArrayString m_paths; //< For file explorer which supports multiple selection (File Explorer tree only)
};

//---------------------------
// List of available trees
//---------------------------
enum TreeType { TreeFileView = 0, TreeFileExplorer };

enum eOutputPaneTab { kOutputTab_Build, kOutputTab_Output };

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
class IManager
{
    wxArrayString m_workspaceTabs;
    wxArrayString m_outputTabs;

public:
    IManager() {}
    virtual ~IManager() {}

    /**
     * @brief return a list of all possible output tabs registered by the user
     */
    const wxArrayString& GetOutputTabs() const { return m_outputTabs; }

    /**
     * @brief return a list of all possible workspace tabs
     */
    const wxArrayString& GetWorkspaceTabs() const { return m_workspaceTabs; }

    /**
     * @brief register a workspace tab
     */
    void AddWorkspaceTab(const wxString& tabLabel)
    {
        if(m_workspaceTabs.Index(tabLabel) == wxNOT_FOUND) {
            m_workspaceTabs.Add(tabLabel);
        }
    }

    /**
     * @brief register output pane tab
     */
    void AddOutputTab(const wxString& tabLabel)
    {
        if(m_outputTabs.Index(tabLabel) == wxNOT_FOUND) {
            m_outputTabs.Add(tabLabel);
        }
    }

    /**
     * @brief show the output pane and if provided, select 'selectedWindow'
     * @param selectWindow tab within the 'Output Pane' to select, if empty don't change
     * the selection
     */
    virtual void ShowOutputPane(const wxString& selectWindow = "") = 0;

    /**
     * @brief show the toolbar. This only works when using the native toolbar
     */
    virtual void ShowToolBar(bool show = true) = 0;

    /**
     * @brief is the native toolbar visible?
     */
    virtual bool IsToolBarShown() const = 0;
    /**
     * @brief toggle the output pane
     * @param selectWindow tab within the 'Output Pane' to select, if empty don't change
     * the selection
     */
    virtual void ToggleOutputPane(const wxString& selectedWindow = "") = 0;

    // return the current editor
    /**
     * @brief return the active editor
     * @return pointer to the current editor, or NULL incase the active editor is not of type LEditor or no active
     * editor open
     */
    virtual IEditor* GetActiveEditor() = 0;

    /**
     * @brief return the main frame's status bar
     */
    virtual clStatusBar* GetStatusBar() = 0;

    /**
     * @brief return the navigation bar (the one that contains the "File Name" + class::func location at the bottom of
     * the editor)
     * @return
     */
    virtual clEditorBar* GetNavigationBar() = 0;
    /**
     * @brief open file and make it the active editor
     * @param fileName the file to open - use absolute path
     * @param projectName project to associate this file - can be wxEmptyString
     * @param lineno if lineno is not wxNOT_FOUD, the caret will placed on this line number
     * @return true if file opened
     */
    virtual IEditor* OpenFile(const wxString& fileName, const wxString& projectName = wxEmptyString,
                              int lineno = wxNOT_FOUND) = 0;

    /**
     * @brief open a file with a given tooltip and bitmap
     */
    virtual IEditor* OpenFile(const wxString& fileName, const wxBitmap& bmp,
                              const wxString& tooltip = wxEmptyString) = 0;

    /**
     * @brief Open file using browsing record
     * @param rec browsing record
     * @return true on success false otherwise
     */
    virtual IEditor* OpenFile(const BrowseRecord& rec) = 0;

    /**
     * @brief return a pointer to the configuration tool
     * @sa IConfigTool
     */
    virtual IConfigTool* GetConfigTool() = 0;

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
    virtual wxTreeCtrl* GetTree(TreeType type) = 0;

    /**
     * @brief return a pointer to the workspace pane notebook (the one with the 'workspace' title)
     * @return pointer to Notebook
     * @sa Notebook
     */
    virtual Notebook* GetWorkspacePaneNotebook() = 0;

    /**
     * @brief return a pointer to the output pane notebook (the one with the 'output' title)
     * @return pointer to Notebook
     * @sa Notebook
     */
    virtual Notebook* GetOutputPaneNotebook() = 0;

    /**
     * @brief append text line to the tab in the "Output View"
     */
    virtual void AppendOutputTabText(eOutputPaneTab tab, const wxString& text) = 0;

    /**
     * @brief clear the content of the selected output tab
     */
    virtual void ClearOutputTab(eOutputPaneTab tab) = 0;

    virtual wxPanel* GetEditorPaneNotebook() = 0;
    virtual void AddEditorPage(wxWindow* page, const wxString& name, const wxString& tooltip = wxEmptyString) = 0;
    virtual wxWindow* GetActivePage() = 0;
    virtual wxWindow* GetPage(size_t page) = 0;

    /**
     * @brief return the startup directory of CodeLite which is also the base directory for searching installation files
     * @return a full path to the startup directory
     */
    virtual wxString GetStartupDirectory() const = 0;

    /**
     * @brief return the installation directory of codelite
     * @return a full path to codelite installation
     */
    virtual wxString GetInstallDirectory() const = 0;

    /**
     * @brief add project to the workspace
     * @param path full path to the project to add
     */
    virtual void AddProject(const wxString& path) = 0;

    /**
     * @brief return true if a workspace is already open
     */
    virtual bool IsWorkspaceOpen() const = 0;

    /**
     * @brief return an instance to the tags manager - which allows access to CodeLite CodeCompletion API
     * @return a pointer to the tags manager
     * @sa TagsManager
     */
    virtual TagsManager* GetTagsManager() = 0;
    /**
     * @brief return a pointer to the ** C++ ** workspace manager
     */
    virtual clCxxWorkspace* GetWorkspace() = 0;

    /**
     * @brief return the workspace view tab
     * @return
     */
    virtual clWorkspaceView* GetWorkspaceView() = 0;

    /**
     * @brief add files to a virtual folder in the project
     * @param item a tree item which represents the tree item of the virtual folder
     * @param paths an array of files to add
     * @return true on sucesss, false otherwise
     */
    virtual bool AddFilesToVirtualFolder(wxTreeItemId& item, wxArrayString& paths) = 0;

    /**
     * @brief add files to a virtual folder in the project
     * @param vdFullPath virtual directory full path in the form of <project>:vd1:vd2:...:vdN
     * @param paths an array of files to add
     * @return true on sucesss, false otherwise
     */
    virtual bool AddFilesToVirtualFolder(const wxString& vdFullPath, wxArrayString& paths) = 0;

    /**
     * @brief Add a pair of cpp/h files to the :src/include folders, if these exist
     * @param vdFullPath virtual directory full path in the form of <project>:vd1:vd2:...:vdN
     * @param paths an array of files to add
     * @return true on sucesss, false otherwise
     */
    virtual bool AddFilesToVirtualFolderIntelligently(const wxString& vdFullPath, wxArrayString& paths) = 0;

    /**
     * @brief Redefine the files / folders that make up a project
     * @param proj Ref to the project to modify
     * @param path The root directory of the files to add
     * @param files The list of file paths w/o separate folder entries
     */
    virtual void RedefineProjFiles(ProjectPtr proj, const wxString& path, std::vector<wxString>& files) = 0;

    /**
     * @brief create virtual folder to parentPath
     * @param parentPath parent virtual directory full path in the form of <project>:vd1:vd2:...:vdN which must exist
     * @param vdName child VD name
     * @return true on success, false otherwise
     */
    virtual bool CreateVirtualDirectory(const wxString& parentPath, const wxString& vdName) = 0;

    /**
     * @brief return the size of the icons used by CodeLite
     * @return 16 or 24
     */
    virtual int GetToolbarIconSize() = 0;

    /**
     * @brief return true if toobars are allowed for plugins. This is useful for the Mac port of
     * codelite. On Mac, only single toolbar is allowed in the application (otherwise, the application
     * does not feet into the environment)
     * @return true if plugin can create a toolbar, false otherwise
     */
    virtual bool AllowToolbar() = 0;

    /**
     * @brief return a pointer to the docking manager (wxAUI)
     */
    virtual wxAuiManager* GetDockingManager() = 0;
    /**
     * @brief return a pointer to the environment manager
     * @sa EnvironmentConfig
     */
    virtual EnvironmentConfig* GetEnv() = 0;
    /**
     * @brief return a pointer to the job queue manager
     * @return job queue manager
     */
    virtual JobQueue* GetJobQueue() = 0;

    /**
     * @brief return the project execution command as set in the project's settings
     * @param projectName the project
     * @param wd starting dirctory
     * @return the execution command or wxEmptyString if the project does not exist
     */
    virtual wxString GetProjectExecutionCommand(const wxString& projectName, wxString& wd) = 0;

    /**
     * @brief return the application
     */
    virtual wxApp* GetTheApp() = 0;

    /**
     * @brief reload the current workspace, this function does not do anything if a workspace is not opened
     */
    virtual void ReloadWorkspace() = 0;

    /**
     * @brief search for loaded plugin by its name, if the plugin is loaded returns its pointer
     * @param pluginName plugin to search
     * @return pointer to the plugin or NULL if it is not loaded
     */
    virtual IPlugin* GetPlugin(const wxString& pluginName) = 0;

    /**
     * @brief output window for receiving async cmd events
     */
    virtual wxEvtHandler* GetOutputWindow() = 0;

    /**
     * @brief save all modified files
     */
    virtual bool SaveAll() = 0;

    /**
     * @brief return the editor's settings object
     */
    virtual OptionsConfigPtr GetEditorSettings() = 0;

    /**
     * @brief search for pattern in active editor and select name if found
     */
    virtual void FindAndSelect(const wxString& pattern, const wxString& name, int pos = 0) = 0;

    /**
     * @brief return tag at caret in active editor (if ambiguous, user will get a selection dialog)
     * @param scoped whether to do scope-aware lookup
     * @param impl whether to return declaration (false) or definition (true)
     * @return the found tag or NULL
     */
    virtual TagEntryPtr GetTagAtCaret(bool scoped, bool impl) = 0;

    /**
     * @brief show a (short) message in the status bar
     * @param msg the string to display
     * @param col the statusbar pane to use
     * @param seconds_to_live how many seconds to display it for; 0 == forever; -1 == use the default
     */
    virtual void SetStatusMessage(const wxString& msg, int seconds_to_live = wxID_ANY) = 0;

    /**
     * @brief start processing commands from the queue
     */
    virtual void ProcessCommandQueue() = 0;

    /**
     * @brief place a command on the internal queue of codelite to be processed. Each command is executed on
     * a separated process. The queue will not start processing, until a call to ProcessCommandQueue() is issued
     * @param cmd command to process
     */
    virtual void PushQueueCommand(const QueueCommand& cmd) = 0;

    /**
     * @brief stop the current process execution and clear all commands from the queue
     */
    virtual void StopAndClearQueue() = 0;

    /**
     * return true if a compilation is in process (either clean or build)
     */
    virtual bool IsBuildInProgress() const = 0;

    /**
     * @brief return true if a shutdown is currently in progress
     * @return
     */
    virtual bool IsShutdownInProgress() const = 0;

    /**
     * return true if the last buid ended successfully
     */
    virtual bool IsBuildEndedSuccessfully() const = 0;

    /**
     * @brief return the project name of a file
     * @param fullPathFileName file to search
     * @return project name or wxEmptyString if the search failed
     */
    virtual wxString GetProjectNameByFile(const wxString& fullPathFileName) = 0;

    /**
     * @brief accessor to singleton object in the application
     */
    virtual BuildManager* GetBuildManager() = 0;

    /**
     * @brief accessor to singleton object in the application
     */
    virtual BuildSettingsConfig* GetBuildSettingsConfigManager() = 0;

    /**
     * @brief return the singleton object of the navigation manager
     */
    virtual NavMgr* GetNavigationMgr() = 0;

    /**
     * @brief close the named page in the mainbook
     * @param the tab title.
     * @note if there are multiple items with this title, all of them will be closed
     */
    virtual bool ClosePage(const wxString& title) = 0;

    /**
     * @brief close an editor with a given file name (full path)
     */
    virtual bool ClosePage(const wxFileName& filename) = 0;

    /**
     * @brief close 'editor' from the notebook
     * @param editor editor to close
     * @param prompt if set to 'true' prompt if editor is modified, otherwise, close withotu prompting
     */
    virtual bool CloseEditor(IEditor* editor, bool prompt = true) = 0;

    /**
     * @brief return named window in mainbook
     */
    virtual wxWindow* FindPage(const wxString& text) = 0;

    /**
     * @brief add a page to the mainbook
     */
    virtual bool AddPage(wxWindow* win, const wxString& text, const wxString& tooltip = wxEmptyString,
                         const wxBitmap& bmp = wxNullBitmap, bool selected = false) = 0;

    /**
     * @brief select a window in mainbook
     */
    virtual bool SelectPage(wxWindow* win) = 0;

    /**
     * @brief set the page's title
     */
    virtual void SetPageTitle(wxWindow* win, const wxString& title) = 0;

    /**
     * @brief set the page's title
     */
    virtual wxString GetPageTitle(wxWindow* win) const = 0;

    /**
     * @brief open new editor "untitiled"
     * @return pointer to the editor
     */
    virtual IEditor* NewEditor() = 0;

    /**
     * @brief return the default icons loader class
     */
    virtual BitmapLoader* GetStdIcons() = 0;

    /**
     * @brief return the compilation flags for a file in a given project
     */
    virtual wxArrayString GetProjectCompileFlags(const wxString& projectName, bool isCppFile) = 0;

    /**
     * @brief return the selected project item. Note that this is different than
     * returning the *active* project. A selected project, is the project that it is
     * selected in the tree "blue highlight" or the parent of the selected item
     */
    virtual ProjectPtr GetSelectedProject() const = 0;

    /**
     * @brief search the mainbook for an editor representing a given filename
     * return IEditor* or NULL if no match was found
     */
    virtual IEditor* FindEditor(const wxString& filename) const = 0;

    /**
     * @brief enable/disable clang code completion
     */
    virtual void EnableClangCodeCompletion(bool b) = 0;

    /**
     * @brief return the number of pages in the main editor
     */
    virtual size_t GetPageCount() const = 0;

    /**
     * @brief return list of all open editors in the main notebook.
     * This function returns only instances of IEditor (i.e. a file text editor)
     */
    virtual size_t GetAllEditors(IEditor::List_t& editors, bool inOrder = false) = 0;

    /**
     * @brief return list of open tabs in the main notebook. If you need only editors, use GetAllEditors
     * @param tabs [output]
     * @param inOrder retain the editors order
     */
    virtual size_t GetAllTabs(clTab::Vec_t& tabs) = 0;

    // ---------------------------------------------
    // Breakpoint management
    // ---------------------------------------------
    /**
     * @brief return a vector of all the current breakpoints set by the user
     */
    virtual size_t GetAllBreakpoints(BreakpointInfo::Vec_t& breakpoints) = 0;

    /**
     * @brief delete all breakpoints assigned by the user
     */
    virtual void DeleteAllBreakpoints() = 0;

    /**
     * @brief set breakpoints (override any existing breakpoints)
     * this function also refreshes the editors markers
     */
    virtual void SetBreakpoints(const BreakpointInfo::Vec_t& breakpoints) = 0;

    /**
     * @brief process a standard edit event ( wxID_COPY, wxID_PASTE etc)
     * @param e the event to process
     * @param editor the editor
     */
    virtual void ProcessEditEvent(wxCommandEvent& e, IEditor* editor) = 0;

    /**
     * @brief add 'fileName' to the list of recently used workspaces
     * @param fileName
     */
    virtual void AddWorkspaceToRecentlyUsedList(const wxFileName& fileName) = 0;

    /**
     * @brief store a new session for the workspace file associated with the file 'workspaceFile'
     */
    virtual void StoreWorkspaceSession(const wxFileName& workspaceFile) = 0;

    /**
     * @brief load the session associated with 'workspaceFile'
     */
    virtual void LoadWorkspaceSession(const wxFileName& workspaceFile) = 0;
    // ----------------------------------------------
    // Perspective management
    // ----------------------------------------------

    /**
     * @brief load perspective by name. If no such perspective exists
     * this function does nothing
     */
    virtual void LoadPerspective(const wxString& perspectiveName) = 0;

    /**
     * @brief Save the current perspective by name
     * this function also makes sure that the 'Perspective' menu is updated
     */
    virtual void SavePerspective(const wxString& perspectiveName) = 0;

    // Search
    /**
     * @brief open the find in files dialog and select 'path' to search in
     */
    virtual void OpenFindInFileForPath(const wxString& path) = 0;

    /**
     * @brief open the find in files dialog with multiple search paths
     */
    virtual void OpenFindInFileForPaths(const wxArrayString& paths) = 0;
};

#endif // IMANAGER_H
