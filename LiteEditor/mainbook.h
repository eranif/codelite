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
#include "clAuiBook.hpp"
#include "clEditorBar.h"
#include "cl_command_event.h"
#include "cl_editor.h"
#include "filehistory.h"
#include "quickfindbar.h"
#include "sessionmanager.h"

#include <functional>
#include <vector>
#include <wx/aui/auibook.h>
#include <wx/panel.h>

class FilesModifiedDlg;

class IEditor;
class MessagePane;
class clEditorBar;
class WelcomePage;

class MainBook : public wxPanel
{
    typedef std::vector<std::function<void(IEditor*)>> CallbackVec_t;

private:
    FileHistory m_recentFiles;
    clEditorBar* m_navBar;
    MainNotebook* m_book = nullptr;
    bool m_useBuffereLimit;
    bool m_isWorkspaceReloading;
    bool m_reloadingDoRaise; // Prevents multiple Raises() during RestoreSession()
    FilesModifiedDlg* m_filesModifiedDlg;
    std::unordered_map<wxString, TagEntryPtr> m_currentNavBarTags;
    WelcomePage* m_welcomePage = nullptr;
    QuickFindBar* m_findBar;
    std::unordered_map<wxString, CallbackVec_t> m_callbacksTable;
    bool m_initDone = false;

public:
    enum {
        kGetAll_Default = 0,                       // Default
        kGetAll_IncludeDetached = kGetAll_Default, // for backward compatability, keep this symbol
        kGetAll_RetainOrder = (1 << 0),            // Order must be kept
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
    void OnEditorModified(clCommandEvent& event);
    void OnEditorSaved(clCommandEvent& event);
    void OnProjectFileAdded(clCommandEvent& e);
    void OnProjectFileRemoved(clCommandEvent& e);
    void OnWorkspaceLoaded(clWorkspaceEvent& e);
    void OnWorkspaceClosed(clWorkspaceEvent& e);
    void OnDebugEnded(clDebugEvent& e);
    void OnInitDone(wxCommandEvent& e);
    void OnThemeChanged(clCommandEvent& e);
    void OnColoursAndFontsChanged(clCommandEvent& e);
    bool DoSelectPage(wxWindow* win);
    void DoHandleFrameMenu(clEditor* editor);
    void OnWorkspaceReloadStarted(clWorkspaceEvent& e);
    void OnWorkspaceReloadEnded(clWorkspaceEvent& e);
    void OnEditorSettingsChanged(wxCommandEvent& e);
    void OnSettingsChanged(wxCommandEvent& e);
    void OnIdle(wxIdleEvent& event);
    void OnSessionLoaded(clCommandEvent& event);
    /**
     * @brief return proper tab label for a given filename
     */
    wxString CreateLabel(const wxFileName& fn, bool modified) const;
    /**
     * @brief open file and set an alternate content
     */
    void DoOpenFile(const wxString& filename, const wxString& content = "");

    /**
     * @brief display the welcome page
     */
    void ShowWelcomePage(bool show);
    void DoShowWindow(wxWindow* win, bool show);

    void OnEditorChanged(wxCommandEvent& event);
    void OnAllEditorClosed(wxCommandEvent& event);

    void push_callback(std::function<void(IEditor*)>&& callback, const wxString& fullpath);
    void execute_callbacks_for_file(const wxString& fullpath);
    bool has_callbacks(const wxString& fullpath) const;

    int FindEditorIndexByFullPath(const wxString& fullpath);
    void DoRestoreSession(const SessionEntry& entry);

public:
    MainBook(wxWindow* parent);
    virtual ~MainBook();

    WelcomePage* GetWelcomePage(bool createIfMissing = true);

    void SetFindBar(QuickFindBar* findBar);
    QuickFindBar* GetFindBar() const { return m_findBar; }

    /**
     * @brief create the welcome page
     */
    void InitWelcomePage();

    static bool AskUserToSave(clEditor* editor);
    /**
     * @brief show the navigation dialog
     */
    void ShowNavigationDialog();

    /**
     * @brief move the active page right or left
     */
    void MovePage(bool movePageRight);

    void ClearFileHistory();
    void GetRecentlyOpenedFiles(wxArrayString& files);
    FileHistory& GetRecentlyOpenedFilesClass() { return m_recentFiles; }
    void ShowQuickBarForPlugins();
    void ShowQuickBar(bool s);
    void ShowQuickBarToolBar(bool s);
    void ShowQuickBar(const wxString& findWhat, bool showReplace);
    void ShowTabBar(bool b);
    void ShowNavBar(bool s = true);
    /**
     * @brief is navigation bar enabled by user?
     */
    bool IsNavBarEnabled() const { return m_navBar && m_navBar->ShouldShow(); }
    /**
     * @brief is navigation bar shown (either by user or automatically)?
     */
    bool IsNavBarShown() const { return m_navBar && m_navBar->IsShown(); }
    clEditorBar* GetEditorBar() { return m_navBar; }
    void SetEditorBar(clEditorBar* bar) { m_navBar = bar; }

    void SaveSession(SessionEntry& session, wxArrayInt* excludeArr = NULL);
    void RestoreSession(const SessionEntry& session);
    /**
     * @brief create session from current IDE state
     */
    void CreateSession(SessionEntry& session, wxArrayInt* excludeArr = NULL);

    clEditor* GetActiveEditor();
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

    clEditor* FindEditor(const wxString& fileName);
    bool CloseEditor(const wxString& fileName) { return ClosePage(FindEditor(fileName)); }

    wxWindow* GetCurrentPage();
    int GetCurrentPageIndex();
    wxWindow* GetPage(size_t page);
    size_t GetPageCount() const;
    wxWindow* FindPage(const wxString& text);
    /**
     * @brief return the bitmap index or add it if it is missing and then return its index
     */
    int GetBitmapIndexOrAdd(const wxString& name);
    clEditor* NewEditor();

    /**
     * @brief open or select ((if the file is already loaded in CodeLite) editor with a given `file_name` to the
     * notebook control and make it active Once the page is **visible**, execute the callback provided by the user
     * @param callback user callback to be executed once the editor is visible on screen
     * On some platforms (e.g. `GTK`) various operations (e.g. `CenterLine()`) will not work as intended unless the
     * editor is actually visible on screen. This way you
     * can delay the call the `CenterLine()` after the editor is visible
     */
    clEditor* OpenFileAsync(const wxString& file_name, std::function<void(IEditor*)>&& callback);

    /**
     * @brief open a remote file into the editor mainbook
     */
    clEditor* OpenRemoteFile(const wxString& local_path, const wxString& remote_path, const wxString& ssh_account,
                             const wxString& tooltip = wxEmptyString);

    clEditor* OpenFile(const wxString& file_name, const wxString& projectName = wxEmptyString, int lineno = wxNOT_FOUND,
                       long position = wxNOT_FOUND, OF_extra extra = OF_AddJump, bool preserveSelection = true,
                       int bmp = wxNOT_FOUND, const wxString& tooltip = wxEmptyString);
    /**
     * @brief open file based on a browsing record
     */
    clEditor* OpenFile(const BrowseRecord& rec);

    /**
     * @brief a simpler version: open a file with a given tooltip and bitmap
     */
    clEditor* OpenFile(const wxString& file_name, int bmp, const wxString& tooltip = wxEmptyString)
    {
        return OpenFile(file_name, "", wxNOT_FOUND, wxNOT_FOUND, OF_AddJump, false, bmp, tooltip);
    }

    /**
     * @brief add page to the main book
     */
    bool AddBookPage(wxWindow* win, const wxString& text, const wxString& tooltip, int bmp, bool selected,
                     int insert_at_index);
    bool SelectPage(wxWindow* win);

    bool UserSelectFiles(std::vector<std::pair<wxFileName, bool>>& files, const wxString& title,
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
    void ApplyTabLabelChanges();
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

    MainNotebook* GetNotebook() { return m_book; }
};

#endif // MAINBOOK_H
