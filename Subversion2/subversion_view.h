//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2009 by Eran Ifrah
// file name            : subversion_view.h
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

#ifndef __subversion_page__
#define __subversion_page__

#include "bitmap_loader.h"
#include "clFileSystemEvent.h"
#include "clWorkspaceEvent.hpp"
#include "cl_command_event.h"
#include "subversion2_ui.h"
#include "svn_console.h"
#include "svncommand.h"
#include "svninfo.h"
#include "svntreedata.h"

class Subversion2;
class wxMenu;

class SvnPageSelectionInfo
{

public:
    wxArrayString m_paths;
    SvnTreeData::SvnNodeType m_selectionType;

public:
    SvnPageSelectionInfo()
        : m_selectionType(SvnTreeData::SvnNodeTypeInvalid)
    {
    }
    virtual ~SvnPageSelectionInfo() {}

    void Clear()
    {
        m_paths.Clear();
        m_selectionType = SvnTreeData::SvnNodeTypeInvalid;
    }
};

/** Implementing SubversionPageBase */
class SubversionView : public SubversionPageBase
{
    Subversion2* m_plugin;
    wxTreeItemId m_modifiedItems;
    SvnCommand m_simpleCommand;
    SvnCommand m_diffCommand;
    wxString m_curpath;
    SvnConsole* m_subversionConsole;
    int m_fileExplorerLastBaseImgIdx;
    wxFileName m_workspaceFile;

public:
    enum { SvnInfo_Tag, SvnInfo_Branch, SvnInfo_Info };

    /**
     * @brief note that this function accepts arguments by value
     * on purpose (since the caller object might get deleted before
     * it finish processing)
     */
    void FinishDiff(wxString output, wxFileName fileBeingDiffed);

protected:
    virtual void OnUnversionedItemActivated(wxDataViewEvent& event);
    virtual void OnUnversionedItemsContextMenu(wxDataViewEvent& event);
    virtual void OnViewUpdateUI(wxUpdateUIEvent& event);
    virtual void OnContextMenu(wxDataViewEvent& event);
    virtual void OnSciStcChange(wxStyledTextEvent& event);
    virtual void OnCharAdded(wxStyledTextEvent& event);
    virtual void OnKeyDown(wxKeyEvent& event);
    virtual void OnUpdateUI(wxStyledTextEvent& event);
    void CreatGUIControls();
    void ClearAll();
    void DoAddChangedFiles(const wxString& status, const wxArrayString& files);
    void DoAddUnVersionedFiles(const wxArrayString& files);
    int DoGetIconIndex(const wxString& filename);
    void DoGetSelectedFiles(wxArrayString& paths, bool absPath = false);
    void DoGetAllFiles(wxArrayString& paths);
    void DoLinkEditor();
    void DoChangeRootPathUI(const wxString& path);
    void DoRootDirChanged(const wxString& path);
    wxString DoGetCurRepoPath() const;
    wxArrayString GetSelectedUnversionedFiles() const;
    int GetImageIndex(const wxFileName& filepath) const;
    void UpdateStatusBar(const wxString& path);

protected:
    // Menu management
    void CreateFileMenu(wxMenu* menu);
    void CreateRootMenu(wxMenu* menu);
    void DoCloseView();

protected:
    // Handlers for SubversionPageBase events.
    void OnChangeRootDir(wxCommandEvent& event);
    void OnCloseView(wxCommandEvent& event);
    void OnTreeMenu(wxTreeEvent& event);
    void OnItemActivated(wxDataViewEvent& event);

    // IDE Events
    void OnWorkspaceLoaded(clWorkspaceEvent& event);
    void OnWorkspaceClosed(clWorkspaceEvent& event);
    void OnClearOuptut(wxCommandEvent& event);
    void OnRefreshView(wxCommandEvent& event);
    void OnFileSaved(clCommandEvent& event);
    void OnFileRenamed(clFileSystemEvent& event);
    void OnFileAdded(clCommandEvent& event);
    void OnSettings(wxCommandEvent& event);
    void OnActiveEditorChanged(wxCommandEvent& event);
    void OnOpenFile(wxCommandEvent& event);
    void OnAppActivated(wxCommandEvent& event);

    // Svn events
    void OnCommit(wxCommandEvent& event);
    void OnCommitGotoAnything(wxCommandEvent& event);
    void OnUpdateGotoAnything(wxCommandEvent& event);
    void OnUpdate(wxCommandEvent& event);
    void OnRevert(wxCommandEvent& event);
    // void OnAdd(wxCommandEvent& event);
    void OnDelete(wxCommandEvent& event);
    void OnBranch(wxCommandEvent& event);
    void OnTag(wxCommandEvent& event);
    void OnResolve(wxCommandEvent& event);
    void OnDiff(wxCommandEvent& event);
    void OnPatch(wxCommandEvent& event);
    void OnPatchDryRun(wxCommandEvent& event);
    void OnStop(wxCommandEvent& event);
    void OnCleanup(wxCommandEvent& event);
    void OnShowSvnInfo(wxCommandEvent& event);
    void OnCheckout(wxCommandEvent& event);
    void OnIgnoreFile(wxCommandEvent& event);
    void OnIgnoreFilePattern(wxCommandEvent& event);
    void OnBlame(wxCommandEvent& event);
    void OnLinkEditor(wxCommandEvent& event);
    void OnSwitch(wxCommandEvent& event);
    void OnProperties(wxCommandEvent& event);
    void OnLog(wxCommandEvent& event);
    void OnLock(wxCommandEvent& event);
    void OnUnLock(wxCommandEvent& event);
    void OnRename(wxCommandEvent& event);
    void OnAdd(wxCommandEvent& event);
    void OnOpenUnverionsedFiles(wxCommandEvent& event);
    DECLARE_EVENT_TABLE()

    void OnStopUI(wxUpdateUIEvent& event);
    void OnClearOuptutUI(wxUpdateUIEvent& event);

public:
    /** Constructor */
    SubversionView(wxWindow* parent, Subversion2* plugin);
    virtual ~SubversionView();

    SvnConsole* GetSubversionConsole() { return m_subversionConsole; }
    void DisconnectEvents();
    void UpdateTree(const wxArrayString& modifiedFiles, const wxArrayString& conflictedFiles,
                    const wxArrayString& unversionedFiles, const wxArrayString& newFiles,
                    const wxArrayString& deletedFiles, const wxArrayString& lockedFiles,
                    const wxArrayString& ignoreFiles, bool fileExplorerOnly, const wxString& rootDir);
    void BuildTree();
    void BuildTree(const wxString& root);
    void BuildExplorerTree(const wxString& root);

    wxString GetRootDir() const { return DoGetCurRepoPath(); }
    bool IsValid() const
    {
        wxString curpath = DoGetCurRepoPath();
        return !curpath.IsEmpty() && curpath != _("<No repository path is selected>");
    }
};

#endif // __subversion_page__
