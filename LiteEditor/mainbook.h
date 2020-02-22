//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : mainbook.h
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
#ifndef MAINBOOK_H
#define MAINBOOK_H

#include "Notebook.h"
#include "clEditorBar.h"
#include "clMultiBook.h"
#include "cl_command_event.h"
#include "editorframe.h"
#include "filehistory.h"
#include "quickfindbar.h"
#include "sessionmanager.h"
#include "wxStringHash.h"
#include <set>
#include <wx/panel.h>
#include "navigationmanager.h"

class FilesModifiedDlg;


class MessagePane;
class clEditorBar;
class MainBook : public wxPanel
{
private:
    FileHistory m_recentFiles;
    clEditorBar* m_navBar;
    Notebook* m_book;
    bool m_useBuffereLimit;
    EditorFrame::List_t m_detachedEditors;
    bool m_isWorkspaceReloading;
    bool m_reloadingDoRaise; // Prevents multiple Raises() during RestoreSession()
    FilesModifiedDlg* m_filesModifiedDlg;
    std::unordered_map<wxString, TagEntryPtr> m_currentNavBarTags;
    wxWindow* m_welcomePage;
    QuickFindBar* m_findBar;

public:
    enum {
        kGetAll_Default = 0x00000000,         // booked editors only
        kGetAll_RetainOrder = 0x00000001,     // Order must keep
        kGetAll_IncludeDetached = 0x00000002, // return both booked editors and detached
        kGetAll_DetachedOnly = 0x00000004,    // return detached editors only
    };

private:
    FilesModifiedDlg* GetFilesModifiedDlg();
    void DoShowTabLabelContextMenu();
    void CreateGuiControls();
    void ConnectEvents();
    void DoUpdateNotebookTheme();
    void DoOpenImageViewer(const wxFileName& filename);
    void DoUpdateEditorsThemes();

    void OnMouseDClick(wxBookCtrlEvent& e);
    void OnTabDClicked(wxBookCtrlEvent& e);
    void OnTabLabelContextMenu(wxBookCtrlEvent& e);
    void OnPageClosing(wxBookCtrlEvent& e);
    void OnPageClosed(wxBookCtrlEvent& e);
    void OnPageChanged(wxBookCtrlEvent& e);
    void OnClosePage(wxBookCtrlEvent& e);
    void OnPageChanging(wxBookCtrlEvent& e);
    void OnProjectFileAdded(clCommandEvent& e);
    void OnProjectFileRemoved(clCommandEvent& e);
    void OnWorkspaceLoaded(wxCommandEvent& e);
    void OnWorkspaceClosed(wxCommandEvent& e);
    void OnDebugEnded(clDebugEvent& e);
    void OnInitDone(wxCommandEvent& e);
    void OnDetachedEditorClosed(clCommandEvent& e);
    void OnThemeChanged(wxCommandEvent& e);
    void OnColoursAndFontsChanged(clCommandEvent& e);
    bool DoSelectPage(wxWindow* win);
    void DoHandleFrameMenu(clEditor* editor);
    void DoEraseDetachedEditor(IEditor* editor);
    void OnWorkspaceReloadStarted(clCommandEvent& e);
    void OnWorkspaceReloadEnded(clCommandEvent& e);
    void OnEditorSettingsChanged(wxCommandEvent& e);
    void OnCacheUpdated(clCommandEvent& e);
    void OnUpdateNavigationBar(clCodeCompletionEvent& e);
    void OnNavigationBarMenuShowing(clContextMenuEvent& e);
    void OnNavigationBarMenuSelectionMade(clCommandEvent& e);
    void OnSettingsChanged(wxCommandEvent& e);
    /**
     * @brief return proper tab label for a given filename
     */
    wxString CreateLabel(const wxFileName& fn, bool modified) const;
    /**
     * @brief open file and set an alternate content
     */
    void DoOpenFile(const wxString& filename, const wxString& content = "");

    /**
     * @brief update the navigation bar (C++)
     * @param editor
     */
    void UpdateNavBar(clEditor* editor);

    /**
     * @brief display the welcome page
     */
    void ShowWelcomePage(bool show);
    void DoShowWindow(wxWindow* win, bool show);

    void OnEditorChanged(wxCommandEvent& event);
    void OnAllEditorClosed(wxCommandEvent& event);

public:
    MainBook(wxWindow* parent);
    virtual ~MainBook();

    void SetFindBar(QuickFindBar* findBar);
    QuickFindBar* GetFindBar() const { return m_findBar; }
    
    /**
     * @brief register a welcome page. This page is displayed whenever there are no tabs open
     * in CodeLite. If there is already a welcome page registered, this call destroys the previous one
     */
    void RegisterWelcomePage(wxWindow* welcomePage);

    static bool AskUserToSave(clEditor* editor);
    /**
     * @brief show the navigation dialog
     */
    void ShowNavigationDialog();

    /**
     * @brief move the active page right or left
     */
    void MovePage(bool movePageRight);

    const EditorFrame::List_t& GetDetachedEditors() const { return m_detachedEditors; }
    void DetachActiveEditor();
    void ClearFileHistory();
    void GetRecentlyOpenedFiles(wxArrayString& files);
    FileHistory& GetRecentlyOpenedFilesClass() { return m_recentFiles; }
    void ShowQuickBarForPlugins();
    void ShowQuickBar(bool s);
    void ShowQuickBarToolBar(bool s);
    void ShowQuickBar(const wxString& findWhat, bool showReplace);
    void ShowTabBar(bool b);
    void ShowNavBar(bool s = true);
    bool IsNavBarShown() { return m_navBar && m_navBar->IsShown(); }
    clEditorBar* GetEditorBar() { return m_navBar; }
    void SetEditorBar(clEditorBar* bar) { m_navBar = bar; }
    
    void SaveSession(SessionEntry& session, wxArrayInt* excludeArr = NULL);
    void RestoreSession(SessionEntry& session);
    /**
     * @brief create session from current IDE state
     */
    void CreateSession(SessionEntry& session, wxArrayInt* excludeArr = NULL);

    clEditor* GetActiveEditor(bool includeDetachedEditors = false);
    /**
     * @brief return vector of all editors in the notebook. This function only returns instances of type clEditor
     * @param editors [output]
     * @param flags kGetAll_*
     */
    void GetAllEditors(clEditor::Vec_t& editors, size_t flags);
    /**
     * @brief return vector of all tabs in the notebook
     * @param tabs [output]
     */
    void GetAllTabs(clTab::Vec_t& tabs);

    /**
     * @brief return a list of the detached tabs
     */
    void GetDetachedTabs(clTab::Vec_t& tabs);

    clEditor* FindEditor(const wxString& fileName);
    bool CloseEditor(const wxString& fileName) { return ClosePage(FindEditor(fileName)); }

    wxWindow* GetCurrentPage();
    int GetCurrentPageIndex();
    wxWindow* GetPage(size_t page);
    size_t GetPageCount() const;
    wxWindow* FindPage(const wxString& text);

    clEditor* NewEditor();

    clEditor* OpenFile(const wxString& file_name, const wxString& projectName = wxEmptyString, int lineno = wxNOT_FOUND,
                       long position = wxNOT_FOUND, OF_extra extra = OF_AddJump, bool preserveSelection = true,
                       const wxBitmap& bmp = wxNullBitmap, const wxString& tooltip = wxEmptyString);
    /**
     * @brief open file based on a browsing record
     */
    clEditor* OpenFile(const BrowseRecord& rec);

    /**
     * @brief a simpler version: open a file with a given tooltip and bitmap
     */
    clEditor* OpenFile(const wxString& file_name, const wxBitmap& bmp, const wxString& tooltip = wxEmptyString)
    {
        return OpenFile(file_name, "", wxNOT_FOUND, wxNOT_FOUND, OF_AddJump, false, bmp, tooltip);
    }

    bool AddPage(wxWindow* win, const wxString& text, const wxString& tooltip = wxEmptyString,
                 const wxBitmap& bmp = wxNullBitmap, bool selected = false, int insert_at_index = wxNOT_FOUND);
    bool SelectPage(wxWindow* win);

    bool UserSelectFiles(std::vector<std::pair<wxFileName, bool> >& files, const wxString& title,
                         const wxString& caption, bool cancellable = true);

    bool SaveAll(bool askUser, bool includeUntitled);

    void ReloadExternallyModified(bool prompt);

    bool ClosePage(const wxString& text);

    bool ClosePage(wxWindow* win);
    bool ClosePage(IEditor* editor, bool prompt);
    bool CloseAllButThis(wxWindow* win);
    bool CloseAll(bool cancellable);

    // These 3 functions are meant to be used with CallAfter
    void ClosePageVoid(wxWindow* win);
    void CloseAllButThisVoid(wxWindow* win);
    void CloseTabsToTheRight(wxWindow* win);
    void CloseAllVoid(bool cancellable);

    wxString GetPageTitle(wxWindow* win) const;
    void SetPageTitle(wxWindow* page, const wxString& name);
    void SetPageTitle(wxWindow* page, const wxFileName& filename, bool modified);
    long GetBookStyle();

    void ApplySettingsChanges();
    void UnHighlightAll();
    void DelAllBreakpointMarkers();
    void SetViewEOL(bool visible);
    void SetViewWordWrap(bool b);
    void HighlightWord(bool hl);
    void ShowWhitespace(int ws);
    void UpdateColours();
    void UpdateBreakpoints();
    void MarkEditorReadOnly(clEditor* editor);

    void SetUseBuffereLimit(bool useBuffereLimit) { this->m_useBuffereLimit = useBuffereLimit; }
    bool GetUseBuffereLimit() const { return m_useBuffereLimit; }
};

#endif // MAINBOOK_H
