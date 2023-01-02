//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : clTreeCtrlPanel.h
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

#ifndef CLTREECTRLPANEL_H
#define CLTREECTRLPANEL_H

#include "bitmap_loader.h"
#include "clEnhancedToolBar.hpp"
#include "clFileSystemEvent.h"
#include "clFileViwerTreeCtrl.h"
#include "clToolBar.h"
#include "cl_command_event.h"
#include "cl_config.h"
#include "wxcrafter_plugin.h"

#include <imanager.h>

class clTreeCtrlPanelDefaultPage;
class WXDLLIMPEXP_SDK clTreeCtrlPanel : public clTreeCtrlPanelBase
{
public:
    enum {
        kShowHiddenFiles = (1 << 0),
        kShowHiddenFolders = (1 << 1),
        kLinkToEditor = (1 << 2),
        kShowRootFullPath = (1 << 3),
    };

protected:
    clConfig* m_config = nullptr;
    wxString m_viewName;
    clTreeCtrlPanelDefaultPage* m_defaultView = nullptr;
    wxString m_newfileTemplate;
    size_t m_newfileTemplateHighlightLen = 0;
    int m_options = (kShowHiddenFiles | kShowHiddenFolders | kLinkToEditor);
    clToolBar* m_toolbar = nullptr;
    wxString m_excludeFilePatterns;

protected:
    void ToggleView();
    void RefreshNonTopLevelFolder(const wxTreeItemId& item);
    virtual void OnLinkEditor(wxCommandEvent& event);
    virtual void OnLinkEditorUI(wxUpdateUIEvent& event);
    void OnFilesCreated(clFileSystemEvent& event);

public:
    clTreeCtrlPanel(wxWindow* parent);
    virtual ~clTreeCtrlPanel();

    /**
     * @brief getter for the toolbar
     */
    clToolBar* GetToolBar() { return m_toolbar; }

    /**
     * @brief set the tree options
     */
    void SetOptions(int options) { m_options = options; }

    /**
     * @brief set the new file template (default is "Untitled.txt")
     */
    void SetNewFileTemplate(const wxString& newfile, size_t charsToHighlight);

    void SetViewName(const wxString& viewName) { this->m_viewName = viewName; }
    const wxString& GetViewName() const { return m_viewName; }

    clConfig* GetConfig() { return m_config; }

    /**
     * @brief clear the view (i.e. close all top level folders)
     */
    void Clear();

    /**
     * @brief set exclude file pattern. Excluded files will not be shown in the tree
     * @param excludeFilePatterns
     */
    void SetExcludeFilePatterns(const wxString& excludeFilePatterns)
    {
        this->m_excludeFilePatterns = excludeFilePatterns;
    }

    const wxString& GetExcludeFilePatterns() const { return m_excludeFilePatterns; }

    /**
     * @brief return the configuration tool used for storing information about
     * this tree. Override it to provide a custom configuration tool
     */
    void SetConfig(clConfig* config) { this->m_config = config; }

    /**
     * @brief add top level folder
     */
    void AddFolder(const wxString& path);

    /**
     * @brief return an info about the selected items in the tree
     * @return
     */
    TreeItemInfo GetSelectedItemInfo();

    /**
     * @brief return 2 arrays of the selected items
     * @param folders [output]
     * @param files [output]
     */
    void GetSelections(wxArrayString& folders, wxArrayString& files);

    /**
     * @brief select a given filename in the tree. Expand the tree if needed
     */
    bool ExpandToFile(const wxFileName& filename);

    /**
     * @brief return true if a folder is opened in this view
     */
    bool IsFolderOpened() const;

    /**
     * @brief refresh the entire tree, unconditionally
     */
    void RefreshTree();

protected:
    void UpdateItemDeleted(const wxTreeItemId& item);
    void GetTopLevelFolders(wxArrayString& paths, wxArrayTreeItemIds& items) const;

    /**
     * @brief ensure that item is selected (single selection)
     */
    void SelectItem(const wxTreeItemId& item);
    /**
     * @brief return list of selected files and folders. In addition return the
     * tree ctrl items. You can always assume that the folders and the folderItems are of the same
     * size. Same for the file arrays
     */
    void GetSelections(wxArrayString& folders, wxArrayTreeItemIds& folderItems, wxArrayString& files,
                       wxArrayTreeItemIds& fileItems);

    // void version of the expandToFile, so it can be called with CallAfter
    void ExpandToFileVoid(const wxFileName& fn);

    // Make the event handler functions virtual
    // so any subclass could override them
    virtual void OnActiveEditorChanged(wxCommandEvent& event);
    virtual void OnFindInFilesShowing(clFindInFilesEvent& event);
    virtual void OnInitDone(wxCommandEvent& event);
    virtual void OnContextMenu(wxTreeEvent& event);
    virtual void OnItemActivated(wxTreeEvent& event);
    virtual void OnItemExpanding(wxTreeEvent& event);
    virtual void OnCloseFolder(wxCommandEvent& event);
    virtual void OnNewFolder(wxCommandEvent& event);
    virtual void OnNewFile(wxCommandEvent& event);
    virtual void OnOpenFile(wxCommandEvent& event);
    virtual void OnOpenWithDefaultApplication(wxCommandEvent& event);
    virtual void OnRenameFile(wxCommandEvent& event);
    virtual void OnDeleteSelections(wxCommandEvent& event);
    virtual void OnRenameFolder(wxCommandEvent& event);
    virtual void OnFindInFilesFolder(wxCommandEvent& event);
    virtual void OnOpenContainingFolder(wxCommandEvent& event);
    virtual void OnOpenShellFolder(wxCommandEvent& event);
    virtual void OnFolderDropped(clCommandEvent& event);
    virtual void OnRefresh(wxCommandEvent& event);
    void OnOpenFolder(wxCommandEvent& event);
    // Helpers
    void DoExpandItem(const wxTreeItemId& parent, bool expand);
    void DoRenameItem(const wxTreeItemId& item, const wxString& oldname, const wxString& newname);

    bool IsTopLevelFolder(const wxTreeItemId& item);

    clTreeCtrlData* GetItemData(const wxTreeItemId& item) const;
    wxTreeItemId DoAddFolder(const wxTreeItemId& parent, const wxString& path);
    wxTreeItemId DoAddFile(const wxTreeItemId& parent, const wxString& path);
    void DoCloseFolder(const wxTreeItemId& item);
};
#endif // CLTREECTRLPANEL_H
