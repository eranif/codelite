//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2009 by Eran Ifrah
// file name            : subversion2.h
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

#ifndef __Subversion2__
#define __Subversion2__

#include "plugin.h"
#include "svninfo.h"
#include "commitmessagescache.h"
#include "svncommand.h"
#include "svnsettingsdata.h"
#include "project.h"
#include "cl_command_event.h"
#include "clTabTogglerHelper.h"
#include "clFileSystemEvent.h"
#include "SvnShowFileChangesHandler.h"

class SubversionView;
class SvnConsole;
class wxMenu;
class wxMenuItem;

class Subversion2 : public IPlugin
{
private:
    SubversionView* m_subversionView;
    SvnConsole* m_subversionConsole;
    wxMenuItem* m_explorerSepItem;
    wxMenuItem* m_projectSepItem;
    SvnCommand m_simpleCommand;
    SvnCommand m_diffCommand;
    SvnCommand m_blameCommand;
    double m_svnClientVersion;
    CommitMessagesCache m_commitMessagesCache;
    bool m_skipRemoveFilesDlg;
    int m_clientVersion;
    wxString m_selectedFolder; // In the explorer view
    wxFileName m_selectedFile; // In the explorer view
    wxBitmap m_svnBitmap;
    clTabTogglerHelper::Ptr_t m_tabToggler;
    
protected:
    void OnSettings(wxCommandEvent& event);

public:
    void EditSettings();
    enum eCommandLineOption {
        kOpt_ForceInteractive,
    };
    /**
     * @brief ensure that the Subversion Page is view
     */
    void EnsureVisible();

protected:
    ///////////////////////////////////////////////////////////
    // File Explorer event handlers
    ///////////////////////////////////////////////////////////
    void OnCommit(wxCommandEvent& event);
    void OnUpdate(wxCommandEvent& event);
    void OnFolderAdd(wxCommandEvent& event);
    void OnDeleteFolder(wxCommandEvent& event);
    void OnFileExplorerRevertItem(wxCommandEvent& event);
    void OnRevertToRevision(wxCommandEvent& event);
    void OnFileExplorerDiff(wxCommandEvent& event);
    void OnLog(wxCommandEvent& event);
    void OnBlame(wxCommandEvent& event);
    void OnIgnoreFile(wxCommandEvent& event);
    void OnIgnoreFilePattern(wxCommandEvent& event);
    void OnShowFileChanges(wxCommandEvent& event);
    void OnSelectAsView(wxCommandEvent& event);
    void OnSwitchURL(wxCommandEvent& event);
    void OnLockFile(wxCommandEvent& event);
    void OnUnLockFile(wxCommandEvent& event);
    void OnFileExplorerRenameItem(wxCommandEvent& event);
    void OnSync(wxCommandEvent& event);

    ///////////////////////////////////////////////////////////
    // IDE events
    ///////////////////////////////////////////////////////////
    void OnGetCompileLine(clBuildEvent& event);
    void OnWorkspaceConfigChanged(wxCommandEvent& event);
    void OnProjectFileRemoved(clCommandEvent& event);
    void OnFileDeleted(clFileSystemEvent& event);
    void OnFolderDeleted(clFileSystemEvent& event);
    void OnFolderContextMenu(clContextMenuEvent& event);
    void OnFileContextMenu(clContextMenuEvent& event);
    void OnGotoAnythingShowing(clGotoEvent& e);
    
    wxMenu* CreateFileExplorerPopMenu(bool isFile);
    bool IsSubversionViewDetached();
    wxMenu* CreateProjectPopMenu();

public:
    void DoGetSvnInfoSync(SvnInfo& svnInfo, const wxString& workingDirectory);
    void DoGetSvnClientVersion();
    void DoSwitchURL(const wxString& workingDirectory, const wxString& sourceUrl, wxCommandEvent& event);
    void DoLockFile(const wxString& workingDirectory, const wxArrayString& fullpaths, wxCommandEvent& event, bool lock);
    void
    DoRename(const wxString& workingDirectory, const wxString& oldname, const wxString& newname, wxCommandEvent& event);
    void DoCommit(const wxArrayString& files, const wxString& workingDirectory, wxCommandEvent& event);
    void DoFilesDeleted(const wxArrayString& files, bool isFolder = false);
    
public:
    Subversion2(IManager* manager);
    ~Subversion2();

    //--------------------------------------------
    // Abstract methods
    //--------------------------------------------
    virtual void CreateToolBar(clToolBar* toolbar);
    virtual void CreatePluginMenu(wxMenu* pluginsMenu);
    virtual void HookPopupMenu(wxMenu* menu, MenuType type);
    virtual void UnPlug();

    SvnConsole* GetConsole();

    IManager* GetManager() { return m_mgr; }

    SubversionView* GetSvnView() { return m_subversionView; }

    SvnSettingsData GetSettings();
    void SetSettings(SvnSettingsData& ssd);
    wxString GetSvnExeName();
    wxString GetSvnExeNameNoConfigDir();
    wxString GetUserConfigDir();
    void RecreateLocalSvnConfigFile();
    void Patch(bool dryRun, const wxString& workingDirectory, wxEvtHandler* owner, int id);
    void IgnoreFiles(const wxArrayString& files, bool pattern);
    void Blame(wxCommandEvent& event, const wxArrayString& files);

    void SetSvnClientVersion(double svnClientVersion) { this->m_svnClientVersion = svnClientVersion; }

    double GetSvnClientVersion() const { return m_svnClientVersion; }
    CommitMessagesCache& GetCommitMessagesCache() { return m_commitMessagesCache; }

    bool LoginIfNeeded(wxCommandEvent& event, const wxString& workingDirectory, wxString& loginString);
    bool GetNonInteractiveMode(wxCommandEvent& event);
    bool IsPathUnderSvn(const wxString& path);
    void ChangeLog(const wxString& path, const wxString& fullpath, wxCommandEvent& event);
    void FinishSyncProcess(ProjectPtr& proj,
                           const wxString& workDir,
                           bool excludeBin,
                           const wxString& excludeExtensions,
                           const wxString& output);

    void AddCommandLineOption(wxString& command, Subversion2::eCommandLineOption opt);
    
    void ShowRecentChanges(const wxString& file);
    void ShowRecentChangesDialog(const SvnShowDiffChunk::List_t& changes);
    
protected:
    void DoInitialize();
    void DoSetSSH();
    void DoGetSvnVersion();
    wxArrayString DoGetSvnStatusQuiet(const wxString& wd);
    bool NormalizeDir(wxString& wd);
    std::vector<wxString> GetLocalAddsDels(const wxString& wd);
    std::vector<wxString> GetFilesMarkedBinary(const wxString& wd);
    std::vector<wxString> RemoveExcludeExts(const std::vector<wxString>& aryInFiles, const wxString& excludeExtensions);

    wxString DoGetFileExplorerItemFullPath();
    wxString DoGetFileExplorerItemPath();

    wxArrayString DoGetFileExplorerFiles();
    wxString DoGetFileExplorerFilesAsString();
    wxArrayString DoGetFileExplorerFilesToCommitRelativeTo(const wxString& wd);
};

#endif // Subversion2
