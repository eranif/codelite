//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : mainbook.cpp
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
#include "mainbook.h"

#include "FilesModifiedDlg.h"
#include "NotebookNavigationDlg.h"
#include "WelcomePage.h"
#include "clFileOrFolderDropTarget.h"
#include "clImageViewer.h"
#include "clWorkspaceManager.h"
#include "cl_defs.h"
#include "close_all_dlg.h"
#include "ctags_manager.h"
#include "editor_config.h"
#include "editorframe.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "filechecklist.h"
#include "findreplacedlg.h"
#include "frame.h"
#include "globals.h"
#include "ieditor.h"
#include "macros.h"
#include "manager.h"
#include "new_quick_watch_dlg.h"
#include "pluginmanager.h"
#include "quickfindbar.h"

#include <algorithm>
#include <imanager.h>
#include <unordered_map>
#include <wx/aui/framemanager.h>
#include <wx/regex.h>
#include <wx/wupdlock.h>
#include <wx/xrc/xmlres.h>

MainBook::MainBook(wxWindow* parent)
    : wxPanel(parent)
    , m_navBar(NULL)
    , m_book(NULL)
    , m_useBuffereLimit(true)
    , m_isWorkspaceReloading(false)
    , m_reloadingDoRaise(true)
    , m_filesModifiedDlg(NULL)
    , m_welcomePage(NULL)
    , m_findBar(NULL)
{
    Hide();
    CreateGuiControls();
    ConnectEvents();
}

void MainBook::CreateGuiControls()
{
    wxBoxSizer* sz = new wxBoxSizer(wxVERTICAL);
    SetSizer(sz);
#if CL_USE_NATIVEBOOK
    long style = kNotebook_AllowDnD |                  // Allow tabs to move
                 kNotebook_MouseMiddleClickClosesTab | // Handle mouse middle button when clicked on a tab
                 kNotebook_MouseMiddleClickFireEvent | // instead of closing the tab, fire an event
                 kNotebook_ShowFileListButton |        // show drop down list of all open tabs
                 kNotebook_EnableNavigationEvent |     // Notify when user hit Ctrl-TAB or Ctrl-PGDN/UP
                 kNotebook_NewButton;
#else
    long style = kNotebook_AllowDnD |                  // Allow tabs to move
                 kNotebook_MouseMiddleClickClosesTab | // Handle mouse middle button when clicked on a tab
                 kNotebook_MouseMiddleClickFireEvent | // instead of closing the tab, fire an event
                 kNotebook_ShowFileListButton |        // show drop down list of all open tabs
                 kNotebook_EnableNavigationEvent |     // Notify when user hit Ctrl-TAB or Ctrl-PGDN/UP
                 kNotebook_UnderlineActiveTab |        // Mark active tab with dedicated coloured line
                 kNotebook_DynamicColours |            // The tabs colour adjust to the editor's theme
                 kNotebook_NewButton;
    style |= kNotebook_AllowForeignDnD;
#endif

    if(EditorConfigST::Get()->GetOptions()->IsTabHasXButton()) {
        style |= (kNotebook_CloseButtonOnActiveTabFireEvent | kNotebook_CloseButtonOnActiveTab);
    }
    if(EditorConfigST::Get()->GetOptions()->IsMouseScrollSwitchTabs()) {
        style |= kNotebook_MouseScrollSwitchTabs;
    }

    if(clConfig::Get().Read("HideTabBar", false)) {
        style |= kNotebook_HideTabBar;
    }

    // load the notebook style from the configuration settings
    m_book = new Notebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, style);
    sz->Add(m_book, 1, wxEXPAND);
    sz->Layout();
}

void MainBook::ConnectEvents()
{
    m_book->Bind(wxEVT_BOOK_PAGE_CLOSING, &MainBook::OnPageClosing, this);
    m_book->Bind(wxEVT_BOOK_PAGE_CLOSED, &MainBook::OnPageClosed, this);
    m_book->Bind(wxEVT_BOOK_PAGE_CHANGED, &MainBook::OnPageChanged, this);
    m_book->Bind(wxEVT_BOOK_PAGE_CHANGING, &MainBook::OnPageChanging, this);
    m_book->Bind(wxEVT_BOOK_PAGE_CLOSE_BUTTON, &MainBook::OnClosePage, this);
    m_book->Bind(wxEVT_BOOK_NEW_PAGE, &MainBook::OnMouseDClick, this);
    m_book->Bind(wxEVT_BOOK_TAB_DCLICKED, &MainBook::OnTabDClicked, this);
    m_book->Bind(wxEVT_BOOK_TAB_CONTEXT_MENU, &MainBook::OnTabLabelContextMenu, this);

    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_LOADED, &MainBook::OnWorkspaceLoaded, this);
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_CLOSED, &MainBook::OnWorkspaceClosed, this);
    EventNotifier::Get()->Connect(wxEVT_PROJ_FILE_ADDED, clCommandEventHandler(MainBook::OnProjectFileAdded), NULL,
                                  this);
    EventNotifier::Get()->Connect(wxEVT_PROJ_FILE_REMOVED, clCommandEventHandler(MainBook::OnProjectFileRemoved), NULL,
                                  this);
    EventNotifier::Get()->Bind(wxEVT_DEBUG_ENDED, &MainBook::OnDebugEnded, this);
    EventNotifier::Get()->Connect(wxEVT_INIT_DONE, wxCommandEventHandler(MainBook::OnInitDone), NULL, this);

    EventNotifier::Get()->Bind(wxEVT_DETACHED_EDITOR_CLOSED, &MainBook::OnDetachedEditorClosed, this);
    EventNotifier::Get()->Bind(wxEVT_SYS_COLOURS_CHANGED, &MainBook::OnThemeChanged, this);
    EventNotifier::Get()->Bind(wxEVT_EDITOR_CONFIG_CHANGED, &MainBook::OnEditorSettingsChanged, this);
    EventNotifier::Get()->Bind(wxEVT_CMD_COLOURS_FONTS_UPDATED, &MainBook::OnColoursAndFontsChanged, this);
    EventNotifier::Get()->Bind(wxEVT_EDITOR_SETTINGS_CHANGED, &MainBook::OnSettingsChanged, this);

    EventNotifier::Get()->Bind(wxEVT_EDITOR_MODIFIED, &MainBook::OnEditorModified, this);
    EventNotifier::Get()->Bind(wxEVT_FILE_SAVED, &MainBook::OnEditorSaved, this);
    // we handle a "load file" as if the file was saved
    EventNotifier::Get()->Bind(wxEVT_FILE_LOADED, &MainBook::OnEditorSaved, this);

    Bind(wxEVT_IDLE, &MainBook::OnIdle, this);
}

MainBook::~MainBook()
{
    wxDELETE(m_filesModifiedDlg);
    m_book->Unbind(wxEVT_BOOK_PAGE_CLOSING, &MainBook::OnPageClosing, this);
    m_book->Unbind(wxEVT_BOOK_PAGE_CLOSED, &MainBook::OnPageClosed, this);
    m_book->Unbind(wxEVT_BOOK_PAGE_CHANGED, &MainBook::OnPageChanged, this);
    m_book->Unbind(wxEVT_BOOK_PAGE_CHANGING, &MainBook::OnPageChanging, this);
    m_book->Unbind(wxEVT_BOOK_PAGE_CLOSE_BUTTON, &MainBook::OnClosePage, this);
    m_book->Unbind(wxEVT_BOOK_NEW_PAGE, &MainBook::OnMouseDClick, this);
    m_book->Unbind(wxEVT_BOOK_TAB_DCLICKED, &MainBook::OnTabDClicked, this);
    m_book->Unbind(wxEVT_BOOK_TAB_CONTEXT_MENU, &MainBook::OnTabLabelContextMenu, this);

    EventNotifier::Get()->Unbind(wxEVT_SYS_COLOURS_CHANGED, &MainBook::OnThemeChanged, this);

    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_LOADED, &MainBook::OnWorkspaceLoaded, this);
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_CLOSED, &MainBook::OnWorkspaceClosed, this);
    EventNotifier::Get()->Disconnect(wxEVT_PROJ_FILE_ADDED, clCommandEventHandler(MainBook::OnProjectFileAdded), NULL,
                                     this);
    EventNotifier::Get()->Disconnect(wxEVT_PROJ_FILE_REMOVED, clCommandEventHandler(MainBook::OnProjectFileRemoved),
                                     NULL, this);
    EventNotifier::Get()->Unbind(wxEVT_DEBUG_ENDED, &MainBook::OnDebugEnded, this);
    EventNotifier::Get()->Disconnect(wxEVT_INIT_DONE, wxCommandEventHandler(MainBook::OnInitDone), NULL, this);

    EventNotifier::Get()->Unbind(wxEVT_DETACHED_EDITOR_CLOSED, &MainBook::OnDetachedEditorClosed, this);
    EventNotifier::Get()->Unbind(wxEVT_EDITOR_CONFIG_CHANGED, &MainBook::OnEditorSettingsChanged, this);
    EventNotifier::Get()->Unbind(wxEVT_CMD_COLOURS_FONTS_UPDATED, &MainBook::OnColoursAndFontsChanged, this);
    EventNotifier::Get()->Unbind(wxEVT_EDITOR_SETTINGS_CHANGED, &MainBook::OnSettingsChanged, this);
    EventNotifier::Get()->Unbind(wxEVT_EDITOR_MODIFIED, &MainBook::OnEditorModified, this);
    EventNotifier::Get()->Unbind(wxEVT_FILE_SAVED, &MainBook::OnEditorSaved, this);
    EventNotifier::Get()->Unbind(wxEVT_FILE_LOADED, &MainBook::OnEditorSaved, this);

    Unbind(wxEVT_IDLE, &MainBook::OnIdle, this);
    if(m_findBar) {
        EventNotifier::Get()->Unbind(wxEVT_ALL_EDITORS_CLOSED, &MainBook::OnAllEditorClosed, this);
        EventNotifier::Get()->Unbind(wxEVT_ACTIVE_EDITOR_CHANGED, &MainBook::OnEditorChanged, this);
    }
}

void MainBook::OnMouseDClick(wxBookCtrlEvent& e)
{
    wxUnusedVar(e);
    NewEditor();
}

void MainBook::OnPageClosing(wxBookCtrlEvent& e)
{
    e.Skip();
    clEditor* editor = dynamic_cast<clEditor*>(m_book->GetPage(e.GetSelection()));
    if(editor) {
        if(AskUserToSave(editor)) {
            SendCmdEvent(wxEVT_EDITOR_CLOSING, (IEditor*)editor);
        } else {
            e.Veto();
        }

    } else {

        // Unknown type, ask the plugins - maybe they know about this type
        wxNotifyEvent closeEvent(wxEVT_NOTIFY_PAGE_CLOSING);
        closeEvent.SetClientData(m_book->GetPage(e.GetSelection()));
        EventNotifier::Get()->ProcessEvent(closeEvent);
        if(!closeEvent.IsAllowed()) {
            e.Veto();
        }
    }
}

void MainBook::OnPageClosed(wxBookCtrlEvent& e)
{
    e.Skip();
    SelectPage(m_book->GetCurrentPage());

    // any editors left open?
    clEditor* editor = NULL;
    for(size_t i = 0; i < m_book->GetPageCount() && editor == NULL; i++) {
        editor = dynamic_cast<clEditor*>(m_book->GetPage(i));
    }

    if(m_book->GetPageCount() == 0) {
        SendCmdEvent(wxEVT_ALL_EDITORS_CLOSED);
        ShowQuickBar(false);

        // if no workspace is opened, show the "welcome page"
        if(!clWorkspaceManager::Get().IsWorkspaceOpened()) {
            ShowWelcomePage(true);
        }
    }
    DoUpdateNotebookTheme();
}

void MainBook::OnProjectFileAdded(clCommandEvent& e)
{
    e.Skip();
    const wxArrayString& files = e.GetStrings();
    for(size_t i = 0; i < files.GetCount(); i++) {
        clEditor* editor = FindEditor(files.Item(i));
        if(editor) {
            wxString fileName = CLRealPath(editor->GetFileName().GetFullPath());
            if(files.Index(fileName) != wxNOT_FOUND) {
                editor->SetProject(ManagerST::Get()->GetProjectNameByFile(fileName));
            }
        }
    }
}

void MainBook::OnProjectFileRemoved(clCommandEvent& e)
{
    e.Skip();
    const wxArrayString& files = e.GetStrings();
    for(size_t i = 0; i < files.GetCount(); ++i) {
        clEditor* editor = FindEditor(files.Item(i));
        if(editor && files.Index(CLRealPath(editor->GetFileName().GetFullPath())) != wxNOT_FOUND) {
            editor->SetProject(wxEmptyString);
        }
    }
}

void MainBook::OnWorkspaceLoaded(clWorkspaceEvent& e)
{
    e.Skip();
    CloseAll(false); // get ready for session to be restored by clearing out existing pages
    ShowWelcomePage(false);
}

void MainBook::OnWorkspaceClosed(clWorkspaceEvent& e)
{
    e.Skip();
    CloseAll(false); // make sure no unsaved files
    clStatusBar* sb = clGetManager()->GetStatusBar();
    if(sb) {
        sb->SetSourceControlBitmap(wxNullBitmap, "", "");
    }
    ShowWelcomePage(true);
}

bool MainBook::AskUserToSave(clEditor* editor)
{
    if(!editor || !editor->GetModify())
        return true;

    // unsaved changes
    wxString msg;
    msg << _("Save changes to '") << editor->GetFileName().GetFullName() << wxT("' ?");
    long style = wxYES_NO | wxICON_WARNING;
    if(!ManagerST::Get()->IsShutdownInProgress()) {
        style |= wxCANCEL;
    }

    int answer = wxMessageBox(msg, _("Confirm"), style, clMainFrame::Get());
    switch(answer) {
    case wxYES:
        return editor->SaveFile();
    case wxNO:
        editor->SetSavePoint();
        return true;
    case wxCANCEL:
        return false;
    }

    return true; // to avoid compiler warnings
}

void MainBook::ClearFileHistory()
{
    size_t count = m_recentFiles.GetCount();
    for(size_t i = 0; i < count; i++) {
        m_recentFiles.RemoveFileFromHistory(0);
    }
    clConfig::Get().ClearRecentFiles();
}

void MainBook::GetRecentlyOpenedFiles(wxArrayString& files) { files = clConfig::Get().GetRecentFiles(); }

void MainBook::ShowNavBar(bool s)
{
    if(m_navBar) {
        m_navBar->DoShow(s);
    }
}

void MainBook::SaveSession(SessionEntry& session, wxArrayInt* excludeArr) { CreateSession(session, excludeArr); }

void MainBook::RestoreSession(SessionEntry& session)
{
    if(session.GetTabInfoArr().empty())
        return; // nothing to restore

    CloseAll(false);
    size_t sel = session.GetSelectedTab();
    const std::vector<TabInfo>& vTabInfoArr = session.GetTabInfoArr();
    for(size_t i = 0; i < vTabInfoArr.size(); i++) {
        const TabInfo& ti = vTabInfoArr[i];
        m_reloadingDoRaise = (i == vTabInfoArr.size() - 1); // Raise() when opening only the last editor
        clEditor* editor = OpenFile(ti.GetFileName(), wxEmptyString, wxNOT_FOUND, wxNOT_FOUND, OF_None);
        if(!editor) {
            if(i < sel) {
                // have to adjust selected tab number because couldn't open tab
                sel--;
            }
            continue;
        }

        editor->SetFirstVisibleLine(ti.GetFirstVisibleLine());
        editor->SetEnsureCaretIsVisible(editor->PositionFromLine(ti.GetCurrentLine()));
        editor->LoadMarkersFromArray(ti.GetBookmarks());
        editor->LoadCollapsedFoldsFromArray(ti.GetCollapsedFolds());
    }
    m_book->SetSelection(sel);
}

clEditor* MainBook::GetActiveEditor(bool includeDetachedEditors)
{
    if(includeDetachedEditors) {
        EditorFrame::List_t::iterator iter = m_detachedEditors.begin();
        for(; iter != m_detachedEditors.end(); ++iter) {
            if((*iter)->GetEditor()->IsFocused()) {
                return (*iter)->GetEditor();
            }
        }
    }

    if(!GetCurrentPage()) {
        return NULL;
    }
    return dynamic_cast<clEditor*>(GetCurrentPage());
}

void MainBook::GetAllTabs(clTab::Vec_t& tabs)
{
    tabs.clear();
#if USE_AUI_NOTEBOOK
    m_book->GetAllTabs(tabs);
#else
    clTabInfo::Vec_t tabsInfo;
    m_book->GetAllTabs(tabsInfo);

    // Convert into "clTab" array
    std::for_each(tabsInfo.begin(), tabsInfo.end(), [&](clTabInfo::Ptr_t tabInfo) {
        clTab t;
        t.bitmap = tabInfo->GetBitmap();
        t.text = tabInfo->GetLabel();
        t.window = tabInfo->GetWindow();

        clEditor* editor = dynamic_cast<clEditor*>(t.window);
        if(editor) {
            t.isFile = true;
            t.isModified = editor->GetModify();
            t.filename = editor->GetFileName();
        }
        tabs.push_back(t);
    });
#endif
}

void MainBook::GetAllEditors(clEditor::Vec_t& editors, size_t flags)
{
    editors.clear();
    if(!(flags & kGetAll_DetachedOnly)) {
        // Collect booked editors
        if(!(flags & kGetAll_RetainOrder)) {
            // Most of the time we don't care about the order the tabs are stored in
            for(size_t i = 0; i < m_book->GetPageCount(); i++) {
                clEditor* editor = dynamic_cast<clEditor*>(m_book->GetPage(i));
                if(editor) {
                    editors.push_back(editor);
                }
            }
        } else {
            for(size_t i = 0; i < m_book->GetPageCount(); i++) {
                clEditor* editor = dynamic_cast<clEditor*>(m_book->GetPage(i));
                if(editor) {
                    editors.push_back(editor);
                }
            }
        }
    }
    if((flags & kGetAll_IncludeDetached) || (flags & kGetAll_DetachedOnly)) {
        // Add the detached editors
        EditorFrame::List_t::iterator iter = m_detachedEditors.begin();
        for(; iter != m_detachedEditors.end(); ++iter) {
            editors.push_back((*iter)->GetEditor());
        }
    }
}

int MainBook::FindEditorIndexByFullPath(const wxString& fullpath)
{
#ifdef __WXGTK__
    // On gtk either fileName or the editor filepath (or both) may be (or their paths contain) symlinks
    wxString fileNameDest = CLRealPath(fullpath);
#endif

    for(size_t i = 0; i < m_book->GetPageCount(); i++) {
        clEditor* editor = dynamic_cast<clEditor*>(m_book->GetPage(i));
        if(editor) {
            // are we a remote path?
            if(editor->IsRemoteFile()) {
                if(editor->GetRemoteData()->GetRemotePath() == fullpath ||
                   editor->GetRemoteData()->GetLocalPath() == fullpath) {
                    return i;
                }
            } else {
                // local path
                wxString unixStyleFile(CLRealPath(editor->GetFileName().GetFullPath()));
                wxString nativeFile(unixStyleFile);
#ifdef __WXMSW__
                unixStyleFile.Replace(wxT("\\"), wxT("/"));
#endif

#ifdef __WXGTK__
                // On Unix files are case sensitive
                if(nativeFile.Cmp(fullpath) == 0 || unixStyleFile.Cmp(fullpath) == 0 ||
                   unixStyleFile.Cmp(fileNameDest) == 0)
#else
                // Compare in no case sensitive manner
                if(nativeFile.CmpNoCase(fullpath) == 0 || unixStyleFile.CmpNoCase(fullpath) == 0)
#endif
                {
                    return i;
                }

#if defined(__WXGTK__)
                // Try again, dereferencing the editor fpath
                wxString editorDest = CLRealPath(unixStyleFile);
                if(editorDest.Cmp(fullpath) == 0 || editorDest.Cmp(fileNameDest) == 0) {
                    return i;
                }
#endif
            }
        }
    }
    return wxNOT_FOUND;
}

clEditor* MainBook::FindEditor(const wxString& fileName)
{
    int index = FindEditorIndexByFullPath(fileName);
    if(index != wxNOT_FOUND) {
        return dynamic_cast<clEditor*>(m_book->GetPage(index));
    }

    // try the detached editors
    EditorFrame::List_t::iterator iter = m_detachedEditors.begin();
    for(; iter != m_detachedEditors.end(); ++iter) {
        if((*iter)->GetEditor()->GetFileName().GetFullPath() == fileName) {
            return (*iter)->GetEditor();
        }
    }
    return nullptr;
}

wxWindow* MainBook::FindPage(const wxString& text)
{
    for(size_t i = 0; i < m_book->GetPageCount(); i++) {
        clEditor* editor = dynamic_cast<clEditor*>(m_book->GetPage(i));
        if(editor && CLRealPath(editor->GetFileName().GetFullPath()).CmpNoCase(text) == 0) {
            return editor;
        }

        if(m_book->GetPageText(i) == text)
            return m_book->GetPage(i);
    }
    return NULL;
}

clEditor* MainBook::NewEditor()
{
    static int fileCounter = 0;

    wxString fileNameStr(_("Untitled"));
    fileNameStr << ++fileCounter;
    wxFileName fileName(fileNameStr);

#if !CL_USE_NATIVEBOOK
    // A Nice trick: hide the notebook, open the editor
    // and then show it
    bool hidden(false);
    if(m_book->GetPageCount() == 0)
        hidden = GetSizer()->Hide(m_book);
#endif

    clEditor* editor = new clEditor(m_book);
    editor->SetFileName(fileName);
    AddBookPage(editor, fileName.GetFullName(), fileName.GetFullPath(), wxNOT_FOUND, true, wxNOT_FOUND);

#if !CL_USE_NATIVEBOOK
    if(m_book->GetSizer()) {
        m_book->GetSizer()->Layout();
    }

    // SHow the notebook
    if(hidden)
        GetSizer()->Show(m_book);
#endif

    editor->SetActive();
    return editor;
}

static bool IsFileExists(const wxFileName& filename)
{
#ifdef __WXMSW__
    /*    wxString drive  = filename.GetVolume();
        if(drive.Length()>1)
            return false;*/

    return filename.FileExists();
#else
    return filename.FileExists();
#endif
}

clEditor* MainBook::OpenFile(const wxString& file_name, const wxString& projectName, int lineno, long position,
                             OF_extra extra /*=OF_AddJump*/, bool preserveSelection /*=true*/,
                             int bmp /*= wxNullBitmap*/, const wxString& tooltip /* wxEmptyString */)
{
    wxFileName fileName(CLRealPath(file_name));
    fileName.MakeAbsolute();

#ifdef __WXMSW__
    // Handle cygwin paths
    wxString curpath = fileName.GetFullPath();
    static wxRegEx reCygdrive("/cygdrive/([A-Za-z])");
    if(reCygdrive.Matches(curpath)) {
        // Replace the /cygdrive/c with volume C:
        wxString volume = reCygdrive.GetMatch(curpath, 1);
        volume << ":";
        reCygdrive.Replace(&curpath, volume);
        fileName = curpath;
    }
#endif

    if(!IsFileExists(fileName)) {
        clDEBUG() << "Failed to open:" << fileName << ". No such file or directory";
        return NULL;
    }

    if(FileExtManager::GetType(fileName.GetFullPath()) == FileExtManager::TypeBmp) {
        // a bitmap file, open it using an image viewer
        DoOpenImageViewer(fileName);
        return NULL;
    }

    wxString projName = projectName;
    if(projName.IsEmpty()) {
        wxString filePath(fileName.GetFullPath());
        // try to match a project name to the file. otherwise, CC may not work
        projName = ManagerST::Get()->GetProjectNameByFile(filePath);
        // If (in Linux) the project contains a *symlink* this filepath, not the realpath,
        // filePath will have been changed by GetProjectNameByFile() to that symlink.
        // So update fileName in case
        fileName = wxFileName(filePath);
    }

    clEditor* editor = GetActiveEditor(true);
    BrowseRecord jumpfrom = editor ? editor->CreateBrowseRecord() : BrowseRecord();

    editor = FindEditor(fileName.GetFullPath());
    if(editor) {
        editor->SetProject(projName);
    } else if(fileName.IsOk() == false) {
        return NULL;

    } else if(!fileName.FileExists()) {
        return NULL;

    } else {
#if !CL_USE_NATIVEBOOK
        // A Nice trick: hide the notebook, open the editor
        // and then show it
        bool hidden(false);
        if(m_book->GetPageCount() == 0)
            hidden = GetSizer()->Hide(m_book);
#endif
        editor = new clEditor(m_book);
        editor->SetEditorBitmap(bmp);
        editor->Create(projName, fileName);

        int sel = m_book->GetSelection();

        // The tab label is the filename + last dir
        wxString label = CreateLabel(fileName, false);
        if((extra & OF_PlaceNextToCurrent) && (sel != wxNOT_FOUND)) {
            AddBookPage(editor, label, tooltip.IsEmpty() ? fileName.GetFullPath() : tooltip, bmp, false, sel + 1);
        } else {
            AddBookPage(editor, label, tooltip.IsEmpty() ? fileName.GetFullPath() : tooltip, bmp, false, wxNOT_FOUND);
        }
        editor->SetSyntaxHighlight();
        ManagerST::Get()->GetBreakpointsMgr()->RefreshBreakpointsForEditor(editor);

        // mark the editor as read only if needed
        MarkEditorReadOnly(editor);

#if !CL_USE_NATIVEBOOK
        // Show the notebook
        if(hidden)
            GetSizer()->Show(m_book);
#endif

        if(position == wxNOT_FOUND && lineno == wxNOT_FOUND && editor->GetContext()->GetName() == wxT("C++")) {
            // try to find something interesting in the file to put the caret at
            // for now, just skip past initial blank lines and comments
            for(lineno = 0; lineno < editor->GetLineCount(); lineno++) {
                switch(editor->GetStyleAt(editor->PositionFromLine(lineno))) {
                case wxSTC_C_DEFAULT:
                case wxSTC_C_COMMENT:
                case wxSTC_C_COMMENTDOC:
                case wxSTC_C_COMMENTLINE:
                case wxSTC_C_COMMENTLINEDOC:
                    continue;
                }
                // if we got here, it's a line to stop on
                break;
            }
            if(lineno == editor->GetLineCount()) {
                lineno = 1; // makes sure a navigation record gets saved
            }
        }
    }

    if(position != wxNOT_FOUND) {
        editor->SetEnsureCaretIsVisible(position, preserveSelection);
        editor->SetLineVisible(editor->LineFromPosition(position));

    } else if(lineno != wxNOT_FOUND) {
        editor->CallAfter(&clEditor::CenterLine, lineno, wxNOT_FOUND);
    }

    if(m_reloadingDoRaise) {
        if(GetActiveEditor() == editor) {
            editor->SetActive();
        } else {
            SelectPage(editor);
        }
    }

    // Add this file to the history. Don't check for uniqueness:
    // if it's already on the list, wxFileHistory will move it to the top
    // Also, sync between the history object and the configuration file
    m_recentFiles.AddFileToHistory(fileName.GetFullPath());
    clConfig::Get().AddRecentFile(fileName.GetFullPath());

    if(extra & OF_AddJump) {
        BrowseRecord jumpto = editor->CreateBrowseRecord();
        NavMgr::Get()->StoreCurrentLocation(jumpfrom, jumpto);
    }
    return editor;
}

bool MainBook::AddBookPage(wxWindow* win, const wxString& text, const wxString& tooltip, int bmp, bool selected,
                           int insert_at_index)
{
    ShowWelcomePage(false);
    if(m_book->GetPageIndex(win) != wxNOT_FOUND)
        return false;
    if(insert_at_index == wxNOT_FOUND) {
        m_book->AddPage(win, text, selected, bmp);
    } else {
        if(!m_book->InsertPage(insert_at_index, win, text, selected, bmp)) {
            // failed to insert, append it
            m_book->AddPage(win, text, selected, bmp);
        }
    }

    if(!tooltip.IsEmpty()) {
        m_book->SetPageToolTip(m_book->GetPageIndex(win), tooltip);
    }
    return true;
}

bool MainBook::SelectPage(wxWindow* win)
{
    int index = m_book->GetPageIndex(win);
    if(index != wxNOT_FOUND && m_book->GetSelection() != index) {
#if !CL_USE_NATIVEBOOK
        m_book->ChangeSelection(index);
#else
        m_book->SetSelection(index);
#endif
    }
    return DoSelectPage(win);
}

bool MainBook::UserSelectFiles(std::vector<std::pair<wxFileName, bool>>& files, const wxString& title,
                               const wxString& caption, bool cancellable)
{
    if(files.empty())
        return true;

    FileCheckList dlg(clMainFrame::Get(), wxID_ANY, title);
    dlg.SetCaption(caption);
    dlg.SetFiles(files);
    dlg.SetCancellable(cancellable);
    bool res = dlg.ShowModal() == wxID_OK;
    files = dlg.GetFiles();
    return res;
}

bool MainBook::SaveAll(bool askUser, bool includeUntitled)
{
    // turn the 'saving all' flag on so we could 'Veto' all focus events
    clEditor::Vec_t editors;
    GetAllEditors(editors, MainBook::kGetAll_IncludeDetached);

    std::vector<std::pair<wxFileName, bool>> files;
    size_t n = 0;
    for(size_t i = 0; i < editors.size(); i++) {
        if(!editors[i]->GetModify())
            continue;

        if(!includeUntitled && !editors[i]->GetFileName().FileExists())
            continue; // don't save new documents that have not been saved to disk yet

        files.push_back(std::make_pair(editors[i]->GetFileName(), true));
        editors[n++] = editors[i];
    }
    editors.resize(n);

    bool res = !askUser || UserSelectFiles(files, _("Save Modified Files"),
                                           _("Some files are modified.\nChoose the files you would like to save."));
    if(res) {
        for(size_t i = 0; i < files.size(); i++) {
            if(files[i].second) {
                editors[i]->SaveFile();
            }
        }
    }
    // And notify the plugins to save their tabs (this function only cover editors)
    clCommandEvent saveAllEvent(wxEVT_SAVE_ALL_EDITORS);
    EventNotifier::Get()->AddPendingEvent(saveAllEvent);
    return res;
}

void MainBook::ReloadExternallyModified(bool prompt)
{
    if(m_isWorkspaceReloading)
        return;
    static int depth = wxNOT_FOUND;
    ++depth;

    // Protect against recursion
    if(depth == 2) {
        depth = wxNOT_FOUND;
        return;
    }

    clEditor::Vec_t editors;
    GetAllEditors(editors, MainBook::kGetAll_IncludeDetached);

    time_t workspaceModifiedTimeBefore = clCxxWorkspaceST::Get()->GetFileLastModifiedTime();

    // filter list of editors for any whose files have been modified
    std::vector<std::pair<wxFileName, bool>> files;
    size_t n = 0;
    for(size_t i = 0; i < editors.size(); i++) {
        time_t diskTime = editors[i]->GetFileLastModifiedTime();
        time_t editTime = editors[i]->GetEditorLastModifiedTime();
        if(diskTime != editTime) {
            // update editor last mod time so that we don't keep bugging the user over the same file,
            // unless it gets changed again
            editors[i]->SetEditorLastModifiedTime(diskTime);

            // A last check: see if the content of the file has actually changed. This avoids unnecessary reload
            // offers after e.g. git stash
            if(!CompareFileWithString(editors[i]->GetFileName().GetFullPath(), editors[i]->GetText())) {
                files.push_back(std::make_pair(editors[i]->GetFileName(), !editors[i]->GetModify()));
                editors[n++] = editors[i];
            }
        }
    }
    editors.resize(n);
    if(n == 0)
        return;

    if(prompt) {

        int res = clConfig::Get().GetAnnoyingDlgAnswer("FilesModifiedDlg", wxNOT_FOUND);
        if(res == wxID_CANCEL) {
            return; // User had previous ticked the 'Remember my answer' checkbox after he'd just chosen Ignore
        }

        if(res == wxNOT_FOUND) {
            // User hasn't previously ticked the 'Remember my answer' checkbox
            // Show the dialog
            res = GetFilesModifiedDlg()->ShowModal();

            if(GetFilesModifiedDlg()->GetRememberMyAnswer()) {
                clConfig::Get().SetAnnoyingDlgAnswer("FilesModifiedDlg", res);
            }

            if(res == FilesModifiedDlg::kID_BUTTON_IGNORE) {
                return;
            }
        }

        if(res == FilesModifiedDlg::kID_BUTTON_CHOOSE) {
            UserSelectFiles(
                files, _("Reload Modified Files"),
                _("Files have been modified outside the editor.\nChoose which files you would like to reload."), false);
        }
    }

    time_t workspaceModifiedTimeAfter = clCxxWorkspaceST::Get()->GetFileLastModifiedTime();
    if(workspaceModifiedTimeBefore != workspaceModifiedTimeAfter) {
        // a workspace reload occurred between the "Reload Modified Files" and
        // the "Reload WOrkspace" dialog, cancel this it's not needed anymore
        return;
    }

    // See issue: https://github.com/eranif/codelite/issues/663
    clEditor::Vec_t editorsAgain;
    GetAllEditors(editorsAgain, MainBook::kGetAll_IncludeDetached);

    // Make sure that the tabs that we have opened
    // are still available in the main book
    clEditor::Vec_t realEditorsList;
    std::sort(editors.begin(), editors.end());
    std::sort(editorsAgain.begin(), editorsAgain.end());
    std::set_intersection(editorsAgain.begin(), editorsAgain.end(), editors.begin(), editors.end(),
                          std::back_inserter(realEditorsList));

    // Update the "files" list
    if(editors.size() != realEditorsList.size()) {
        // something went wrong here...
        CallAfter(&MainBook::ReloadExternallyModified, prompt);
        return;
    }

    // reset the recursive protector
    depth = wxNOT_FOUND;

    std::vector<wxFileName> filesToRetag;
    for(size_t i = 0; i < files.size(); i++) {
        if(files[i].second) {
            editors[i]->ReloadFromDisk(true);
            filesToRetag.push_back(files[i].first);
        }
    }

    if(filesToRetag.size() > 1) {
        TagsManagerST::Get()->ParseWorkspaceIncremental();
    }
}

bool MainBook::ClosePage(wxWindow* page)
{
    int pos = m_book->GetPageIndex(page);
    if(pos != wxNOT_FOUND && m_book->GetPage(pos) == m_welcomePage) {
        m_book->RemovePage(pos, true);
        return true;
    } else {
        return pos != wxNOT_FOUND && m_book->DeletePage(pos);
    }
}

bool MainBook::CloseAllButThis(wxWindow* page)
{
    wxBusyCursor bc;
    clEditor::Vec_t editors;
    GetAllEditors(editors, kGetAll_IncludeDetached);

    std::vector<std::pair<wxFileName, bool>> files;
    std::unordered_map<wxString, clEditor*> M;
    for(clEditor* editor : editors) {
        // collect all modified files, except for "page"
        if(editor->IsEditorModified() && editor->GetCtrl() != page) {
            const wxFileName& fn = editor->GetFileName();
            files.push_back({ fn, true });
            M[fn.GetFullPath()] = editor;
        }
    }

    if(!files.empty() && !UserSelectFiles(files, _("Save Modified Files"),
                                          _("Some files are modified.\nChoose the files you would like to save."))) {
        return false;
    }

    for(const auto& p : files) {
        wxString fullpath = p.first.GetFullPath();
        if(p.second) {
            M[fullpath]->SaveFile();
        } else {
            M[fullpath]->SetSavePoint();
        }
    }

    for(clEditor* editor : editors) {
        if(editor->GetCtrl() == page) {
            continue;
        }
        ClosePage(editor, true);
    }
    return true;
}

bool MainBook::CloseAll(bool cancellable)
{
    wxBusyCursor bc;
    clEditor::Vec_t editors;
    GetAllEditors(editors, kGetAll_IncludeDetached);

    // filter list of editors for any that need to be saved
    std::vector<std::pair<wxFileName, bool>> files;
    size_t n = 0;
    for(size_t i = 0; i < editors.size(); ++i) {
        if(editors[i]->GetModify()) {
            files.push_back({ editors[i]->GetFileName(), true });
            editors[n++] = editors[i];
        }
    }
    editors.resize(n);

    if(!UserSelectFiles(files, _("Save Modified Files"),
                        _("Some files are modified.\nChoose the files you would like to save."), cancellable)) {
        return false;
    }

    // Files selected to be save, we save. The rest we mark as 'unmodified'
    for(size_t i = 0; i < files.size(); i++) {
        if(files[i].second) {
            editors[i]->SaveFile();
        } else {
            editors[i]->SetSavePoint();
        }
    }

    // Delete the files without notifications (it will be faster)
    clWindowUpdateLocker locker(this);
    SendCmdEvent(wxEVT_ALL_EDITORS_CLOSING);

    m_reloadingDoRaise = false;
    m_book->DeleteAllPages();
    m_reloadingDoRaise = true;

    // Delete all detached editors
    EditorFrame::List_t::iterator iter = m_detachedEditors.begin();
    for(; iter != m_detachedEditors.end(); ++iter) {
        (*iter)->Destroy(); // Destroying the frame will release the editor
    }

    // Since we got no more editors opened,
    // send a wxEVT_ALL_EDITORS_CLOSED event
    SendCmdEvent(wxEVT_ALL_EDITORS_CLOSED);

    // Update the quick-find-bar
    clGetManager()->SetStatusMessage("");

    // Update the frame's title
    clMainFrame::Get()->SetFrameTitle(NULL);

    DoHandleFrameMenu(NULL);

    // OutputTabWindow::OnEditUI will crash on >=wxGTK-2.9.3 if we don't set the focus somewhere that still exists
    // This workaround doesn't seem to work if applied earlier in the function :/
    m_book->SetFocus();

    return true;
}

wxString MainBook::GetPageTitle(wxWindow* page) const
{
    int selection = m_book->GetPageIndex(page);
    if(selection != wxNOT_FOUND)
        return m_book->GetPageText(selection);
    return wxEmptyString;
}

void MainBook::SetPageTitle(wxWindow* page, const wxString& name)
{
    int selection = m_book->GetPageIndex(page);
    if(selection != wxNOT_FOUND) {
        m_book->SetPageText(selection, name);
    }
}

void MainBook::ApplySettingsChanges()
{
    DoUpdateEditorsThemes();
    clEditor::Vec_t allEditors;
    GetAllEditors(allEditors, MainBook::kGetAll_IncludeDetached);
    for(auto editor : allEditors) {
        editor->UpdateOptions();
    }

    clMainFrame::Get()->UpdateAUI();
    clMainFrame::Get()->ShowOrHideCaptions();
}

void MainBook::ApplyTabLabelChanges()
{
    // We only want to do this if there *is* a change, and it's hard to be sure
    // by looking.  So store any previous state in:
    static int previousShowParentPath = -1;

    if(previousShowParentPath < 0 ||
       EditorConfigST::Get()->GetOptions()->IsTabShowPath() != (bool)previousShowParentPath) {
        previousShowParentPath = EditorConfigST::Get()->GetOptions()->IsTabShowPath();

        std::vector<clEditor*> editors;
        GetAllEditors(editors, MainBook::kGetAll_IncludeDetached);
        for(size_t i = 0; i < editors.size(); i++) {
            SetPageTitle(editors[i], editors[i]->GetFileName(), editors[i]->IsEditorModified());
        }
    }
}

void MainBook::UnHighlightAll()
{
    std::vector<clEditor*> editors;
    GetAllEditors(editors, MainBook::kGetAll_IncludeDetached);
    for(size_t i = 0; i < editors.size(); i++) {
        editors[i]->UnHighlightAll();
    }
}

void MainBook::DelAllBreakpointMarkers()
{
    std::vector<clEditor*> editors;
    GetAllEditors(editors, MainBook::kGetAll_IncludeDetached);
    for(size_t i = 0; i < editors.size(); i++) {
        editors[i]->DelAllBreakpointMarkers();
    }
}

void MainBook::SetViewEOL(bool visible)
{
    std::vector<clEditor*> editors;
    GetAllEditors(editors, MainBook::kGetAll_IncludeDetached);
    for(size_t i = 0; i < editors.size(); i++) {
        editors[i]->SetViewEOL(visible);
    }
}

void MainBook::HighlightWord(bool hl)
{
    std::vector<clEditor*> editors;
    GetAllEditors(editors, MainBook::kGetAll_IncludeDetached);
    for(size_t i = 0; i < editors.size(); i++) {
        editors[i]->HighlightWord(hl);
    }
}

void MainBook::ShowWhitespace(int ws)
{
    std::vector<clEditor*> editors;
    GetAllEditors(editors, MainBook::kGetAll_IncludeDetached);
    for(size_t i = 0; i < editors.size(); i++) {
        editors[i]->SetViewWhiteSpace(ws);
    }
}

void MainBook::UpdateColours()
{
    std::vector<clEditor*> editors;
    GetAllEditors(editors, MainBook::kGetAll_IncludeDetached);
    for(size_t i = 0; i < editors.size(); i++) {
        editors[i]->UpdateColours();
    }
}

void MainBook::UpdateBreakpoints()
{
    std::vector<clEditor*> editors;
    GetAllEditors(editors, MainBook::kGetAll_IncludeDetached);
    for(size_t i = 0; i < editors.size(); i++) {
        editors[i]->UpdateBreakpoints();
    }
    ManagerST::Get()->GetBreakpointsMgr()->RefreshBreakpointMarkers();
}

void MainBook::MarkEditorReadOnly(clEditor* editor)
{
    if(!editor) {
        return;
    }

    bool readOnly = (!editor->IsEditable()) || ::IsFileReadOnly(editor->GetFileName());
    if(readOnly && editor->GetModify()) {
        // an attempt to mark a modified file as read-only
        // ask the user to save his changes before
        ::wxMessageBox(_("Please save your changes before marking the file as read only"), "CodeLite",
                       wxOK | wxCENTER | wxICON_WARNING, this);
        return;
    }

    int lockBmp = m_book->GetBitmaps()->Add("lock");
    for(size_t i = 0; i < m_book->GetPageCount(); i++) {
        int orig_bmp = editor->GetEditorBitmap();
        if(editor == m_book->GetPage(i)) {
            m_book->SetPageBitmap(i, readOnly ? lockBmp : orig_bmp);
            break;
        }
    }
}

long MainBook::GetBookStyle() { return 0; }

bool MainBook::DoSelectPage(wxWindow* win)
{
    clEditor* editor = dynamic_cast<clEditor*>(win);
    if(editor) {
        editor->SetActive();
    }

    // Remove context menu if needed
    DoHandleFrameMenu(editor);

    if(!editor) {
        clMainFrame::Get()->SetFrameTitle(NULL);
        clMainFrame::Get()->GetStatusBar()->SetLinePosColumn(wxEmptyString);
        SendCmdEvent(wxEVT_CMD_PAGE_CHANGED, win);

    } else {
        wxCommandEvent event(wxEVT_ACTIVE_EDITOR_CHANGED);
        event.SetString(CLRealPath(editor->GetFileName().GetFullPath()));
        EventNotifier::Get()->AddPendingEvent(event);
    }
    return true;
}

void MainBook::OnPageChanged(wxBookCtrlEvent& e)
{
    e.Skip();
    int newSel = e.GetSelection();
    if(newSel != wxNOT_FOUND && m_reloadingDoRaise) {
        wxWindow* win = m_book->GetPage((size_t)newSel);
        if(win) {
            SelectPage(win);
        }
    }

    // Cancel any tooltip
    clEditor::Vec_t editors;
    GetAllEditors(editors, MainBook::kGetAll_IncludeDetached);
    for(size_t i = 0; i < editors.size(); ++i) {
        // Cancel any calltip when switching from the editor
        editors.at(i)->DoCancelCalltip();
    }
    DoUpdateNotebookTheme();
}

void MainBook::DoUpdateNotebookTheme()
{
#if !CL_USE_NATIVEBOOK
    size_t initialStyle = m_book->GetStyle();
    size_t style = m_book->GetStyle();
    // Close button
    if(!EditorConfigST::Get()->GetOptions()->IsTabHasXButton()) {
        style &= ~(kNotebook_CloseButtonOnActiveTab | kNotebook_CloseButtonOnActiveTabFireEvent);
    } else {
        style |= (kNotebook_CloseButtonOnActiveTab | kNotebook_CloseButtonOnActiveTabFireEvent);
    }
    if(initialStyle != style) {
        m_book->SetStyle(style);
    }
#endif
}

wxWindow* MainBook::GetCurrentPage() { return m_book->GetCurrentPage(); }
int MainBook::GetCurrentPageIndex() { return m_book->GetSelection(); }

void MainBook::OnClosePage(wxBookCtrlEvent& e)
{
    clWindowUpdateLocker locker(this);
    int where = e.GetSelection();
    if(where == wxNOT_FOUND) {
        return;
    }
    wxWindow* page = m_book->GetPage((size_t)where);
    if(page) {
        ClosePage(page);
    }
}

void MainBook::OnDebugEnded(clDebugEvent& e) { e.Skip(); }

void MainBook::DoHandleFrameMenu(clEditor* editor) { wxUnusedVar(editor); }

void MainBook::OnPageChanging(wxBookCtrlEvent& e)
{
    clEditor* editor = GetActiveEditor();
    if(editor) {
        editor->CallTipCancel();
    }
    e.Skip();
}

void MainBook::SetViewWordWrap(bool b)
{
    std::vector<clEditor*> editors;
    GetAllEditors(editors, MainBook::kGetAll_Default);
    for(size_t i = 0; i < editors.size(); i++) {
        editors[i]->SetWrapMode(b ? wxSTC_WRAP_WORD : wxSTC_WRAP_NONE);
    }
}

void MainBook::OnInitDone(wxCommandEvent& e)
{
    e.Skip();
    m_initDone = true;
    // Show the welcome page, but only if there are no open files
    if(GetPageCount() == 0) {
        ShowWelcomePage(true);
    }
}

wxWindow* MainBook::GetPage(size_t page) { return m_book->GetPage(page); }

bool MainBook::ClosePage(const wxString& text)
{
    int numPageClosed(0);
    bool closed = ClosePage(FindPage(text));
    while(closed) {
        ++numPageClosed;
        closed = ClosePage(FindPage(text));
    }
    return numPageClosed > 0;
}

size_t MainBook::GetPageCount() const { return m_book->GetPageCount(); }

void MainBook::DetachActiveEditor()
{
    if(GetActiveEditor()) {
        clEditor* editor = GetActiveEditor();
        m_book->RemovePage(m_book->GetSelection(), true);
        EditorFrame* frame = new EditorFrame(clMainFrame::Get(), editor, m_book->GetStyle());
        // Move it to the same position as the main frame
        frame->Move(EventNotifier::Get()->TopFrame()->GetPosition());
        // And show it
        frame->Show();
        // frame->Raise();
        m_detachedEditors.push_back(frame);
    }
}

void MainBook::OnDetachedEditorClosed(clCommandEvent& e)
{
    e.Skip();
    IEditor* editor = reinterpret_cast<IEditor*>(e.GetClientData());
    DoEraseDetachedEditor(editor);

    wxString alternateText = (editor && editor->IsEditorModified()) ? editor->GetCtrl()->GetText() : "";
    // Open the file again in the main book
    CallAfter(&MainBook::DoOpenFile, e.GetFileName(), alternateText);
}

void MainBook::DoEraseDetachedEditor(IEditor* editor)
{
    EditorFrame::List_t::iterator iter = m_detachedEditors.begin();
    for(; iter != m_detachedEditors.end(); ++iter) {
        if((*iter)->GetEditor() == editor) {
            m_detachedEditors.erase(iter);
            break;
        }
    }
}

void MainBook::OnWorkspaceReloadEnded(clWorkspaceEvent& e)
{
    e.Skip();
    m_isWorkspaceReloading = false;
}

void MainBook::OnWorkspaceReloadStarted(clWorkspaceEvent& e)
{
    e.Skip();
    m_isWorkspaceReloading = true;
}

void MainBook::ClosePageVoid(wxWindow* win) { ClosePage(win); }

void MainBook::CloseAllButThisVoid(wxWindow* win) { CloseAllButThis(win); }

void MainBook::CloseAllVoid(bool cancellable)
{
    CloseAll(cancellable);
    if(!clWorkspaceManager::Get().IsWorkspaceOpened()) {
        ShowWelcomePage(true);
    }
}

FilesModifiedDlg* MainBook::GetFilesModifiedDlg()
{
    if(!m_filesModifiedDlg)
        m_filesModifiedDlg = new FilesModifiedDlg(clMainFrame::Get());
    return m_filesModifiedDlg;
}

void MainBook::CreateSession(SessionEntry& session, wxArrayInt* excludeArr)
{
    std::vector<clEditor*> editors;
    GetAllEditors(editors, kGetAll_RetainOrder);

    // Remove editors which belong to the SFTP
    std::vector<clEditor*> editorsTmp;
    std::for_each(editors.begin(), editors.end(), [&](clEditor* editor) {
        IEditor* ieditor = dynamic_cast<IEditor*>(editor);
        if(ieditor->GetClientData("sftp") == NULL) {
            editorsTmp.push_back(editor);
        }
    });

    editors.swap(editorsTmp);

    session.SetSelectedTab(0);
    std::vector<TabInfo> vTabInfoArr;
    for(size_t i = 0; i < editors.size(); i++) {

        if(excludeArr && (excludeArr->GetCount() > i) && (!excludeArr->Item(i))) {
            // If we're saving only selected editors, and this isn't one of them...
            continue;
        }

        if(editors[i] == GetActiveEditor()) {
            session.SetSelectedTab(vTabInfoArr.size());
        }
        TabInfo oTabInfo;
        oTabInfo.SetFileName(editors[i]->GetFileName().GetFullPath());
        oTabInfo.SetFirstVisibleLine(editors[i]->GetFirstVisibleLine());
        oTabInfo.SetCurrentLine(editors[i]->GetCurrentLine());

        wxArrayString astrBookmarks;
        editors[i]->StoreMarkersToArray(astrBookmarks);
        oTabInfo.SetBookmarks(astrBookmarks);

        std::vector<int> folds;
        editors[i]->StoreCollapsedFoldsToArray(folds);
        oTabInfo.SetCollapsedFolds(folds);

        vTabInfoArr.push_back(oTabInfo);
    }
    session.SetTabInfoArr(vTabInfoArr);

    // Set the "Find In Files" file mask for this workspace
    FindReplaceData frd;
    frd.SetName("FindInFilesData");
    clConfig::Get().ReadItem(&frd);
    session.SetFindInFilesMask(frd.GetSelectedMask());
}

void MainBook::ShowTabBar(bool b) { wxUnusedVar(b); }

void MainBook::CloseTabsToTheRight(wxWindow* win)
{
    wxBusyCursor bc;
    // clWindowUpdateLocker locker(this);

    // Get list of tabs to close
    wxString text;
    std::vector<wxWindow*> windows;
    bool currentWinFound(false);
    for(size_t i = 0; i < m_book->GetPageCount(); ++i) {
        if(currentWinFound) {
            windows.push_back(m_book->GetPage(i));
        } else {
            if(m_book->GetPage(i) == win) {
                currentWinFound = true;
            }
        }
    }

    // start from right to left
    if(windows.empty())
        return;

    std::vector<wxWindow*> tabsToClose;
    for(int i = (int)(windows.size() - 1); i >= 0; --i) {
        if(windows.at(i) == win) {
            break;
        }
        tabsToClose.push_back(windows.at(i));
    }

    if(tabsToClose.empty())
        return;

    for(size_t i = 0; i < tabsToClose.size(); ++i) {
        ClosePage(tabsToClose.at(i));
    }
    if(m_book->GetSizer()) {
        m_book->GetSizer()->Layout();
    }
}

void MainBook::ShowNavigationDialog()
{
    if(!EditorConfigST::Get()->GetOptions()->IsCtrlTabEnabled()) {
        return;
    }

    if(m_book->GetPageCount() == 0) {
        return;
    }

    NotebookNavigationDlg *dlg = new NotebookNavigationDlg(EventNotifier::Get()->TopFrame(), m_book);
    if(dlg->ShowModal() == wxID_OK && dlg->GetSelection() != wxNOT_FOUND) {
        m_book->SetSelection(dlg->GetSelection());
    }
    wxDELETE(dlg);
}

void MainBook::MovePage(bool movePageRight)
{
    // Determine the new index
    int newIndex = wxNOT_FOUND;

    // Sanity
    if(m_book->GetPageCount() == 0) {
        return;
    }

    int pageCount = static_cast<int>(m_book->GetPageCount());
    if(movePageRight && ((m_book->GetSelection() + 1) < pageCount)) {
        newIndex = m_book->GetSelection() + 1;
    } else if(!movePageRight && (m_book->GetSelection() > 0)) {
        newIndex = m_book->GetSelection() - 1;
    }
    if(newIndex != wxNOT_FOUND) {
        m_book->MoveActivePage(newIndex);
    }
}

void MainBook::OnThemeChanged(clCommandEvent& e)
{
    e.Skip();
    SetBackgroundColour(clSystemSettings::GetDefaultPanelColour());
#if !CL_USE_NATIVEBOOK
    m_book->SetBackgroundColour(clSystemSettings::GetDefaultPanelColour());
    DoUpdateNotebookTheme();
#endif
    // force a redrawing of the main notebook
    m_book->SetStyle(m_book->GetStyle());
}

void MainBook::OnEditorSettingsChanged(wxCommandEvent& e)
{
    e.Skip();
    DoUpdateNotebookTheme();
}

void MainBook::OnTabDClicked(wxBookCtrlEvent& e)
{
    e.Skip();
    ManagerST::Get()->TogglePanes();
}

void MainBook::DoOpenImageViewer(const wxFileName& filename)
{
    clImageViewer* imageViewer = new clImageViewer(m_book, filename);
    size_t pos = m_book->GetPageCount();
    m_book->AddPage(imageViewer, filename.GetFullName(), true);
    m_book->SetPageToolTip(pos, filename.GetFullPath());
}

void MainBook::OnTabLabelContextMenu(wxBookCtrlEvent& e)
{
    e.Skip();
    wxWindow* book = static_cast<wxWindow*>(e.GetEventObject());
    if(book == m_book) {
        e.Skip(false);
        if(e.GetSelection() == m_book->GetSelection()) {
            // The tab requested for context menu is the active one
            DoShowTabLabelContextMenu();
        } else {
            // Make this tab the active one and requeue the context menu event
            m_book->SetSelection(e.GetSelection());
            DoShowTabLabelContextMenu();
        }
    }
}

bool MainBook::ClosePage(IEditor* editor, bool notify)
{
    wxWindow* page = dynamic_cast<wxWindow*>(editor->GetCtrl());
    if(!page)
        return false;
    int pos = m_book->GetPageIndex(page);
    return (pos != wxNOT_FOUND) && (m_book->DeletePage(pos, notify));
}

void MainBook::GetDetachedTabs(clTab::Vec_t& tabs)
{
    // Make sure that modified detached editors are also enabling the "Save" and "Save All" button
    const EditorFrame::List_t& detachedEditors = GetDetachedEditors();
    std::for_each(detachedEditors.begin(), detachedEditors.end(), [&](EditorFrame* fr) {
        clTab tabInfo;
        tabInfo.bitmap = wxNOT_FOUND;
        tabInfo.filename = fr->GetEditor()->GetFileName();
        tabInfo.isFile = true;
        tabInfo.isModified = fr->GetEditor()->GetModify();
        tabInfo.text = fr->GetEditor()->GetFileName().GetFullPath();
        tabInfo.window = fr->GetEditor();
        tabs.push_back(tabInfo);
    });
}

void MainBook::DoOpenFile(const wxString& filename, const wxString& content)
{
    clEditor* editor = OpenFile(filename);
    if(editor && !content.IsEmpty()) {
        editor->SetText(content);
    }
}

void MainBook::OnColoursAndFontsChanged(clCommandEvent& e)
{
    e.Skip();
    DoUpdateEditorsThemes();
}

void MainBook::DoUpdateEditorsThemes()
{
    std::vector<clEditor*> editors;
    GetAllEditors(editors, MainBook::kGetAll_IncludeDetached);
    for(size_t i = 0; i < editors.size(); i++) {
        editors[i]->SetSyntaxHighlight(editors[i]->GetContext()->GetName());
    }
}

void MainBook::OnSettingsChanged(wxCommandEvent& e)
{
    e.Skip();
    ApplyTabLabelChanges();

    // do we need to hide/show the tab bar?
    bool hideTabBar = clConfig::Get().Read("HideTabBar", false);
    size_t style = m_book->GetStyle();
    if(hideTabBar) {
        style |= kNotebook_HideTabBar;
    } else {
        style &= ~kNotebook_HideTabBar;
    }
    m_book->SetStyle(style);
}

clEditor* MainBook::OpenFile(const BrowseRecord& rec)
{
    clEditor* editor = OpenFile(rec.filename, rec.project, wxNOT_FOUND, wxNOT_FOUND, OF_None, true);
    if(editor) {
        // Determine the best position for the caret
        editor->CenterLine(rec.lineno, rec.column);
    }
    return editor;
}

void MainBook::DoShowTabLabelContextMenu()
{
    wxMenu* contextMenu = wxXmlResource::Get()->LoadMenu(wxT("editor_tab_right_click"));

    // Notify the plugins about the tab label context menu
    clContextMenuEvent event(wxEVT_CONTEXT_MENU_TAB_LABEL);
    event.SetMenu(contextMenu);
    EventNotifier::Get()->ProcessEvent(event);

    contextMenu = event.GetMenu();
    m_book->PopupMenu(contextMenu);
    wxDELETE(contextMenu);
}

void MainBook::InitWelcomePage()
{
    m_welcomePage = GetOrCreateWelcomePage();
    ShowWelcomePage(true);
}

void MainBook::DoShowWindow(wxWindow* win, bool show)
{
    wxUnusedVar(win);
    wxUnusedVar(show);
}

void MainBook::ShowWelcomePage(bool show)
{
    if(show) {
        GetSizer()->Show(m_book, false);
        GetSizer()->Show(m_welcomePage, true);
        m_welcomePage->GetDvTreeCtrlWorkspaces()->CallAfter(&clTreeCtrl::SetFocus);

    } else {
        GetSizer()->Show(m_book, true);
        GetSizer()->Show(m_welcomePage, false);
    }
    GetSizer()->Layout();
}

void MainBook::ShowQuickBarForPlugins()
{
    // Implement this
    if(m_findBar) {
        m_findBar->ShowForPlugins();
    }
}

void MainBook::ShowQuickBar(bool show_it)
{
    if(m_findBar) {
        if(show_it) {
            if(m_findBar->IsShown())
                return;

            // requested to show, but it is already shown
            m_findBar->Show(true);
            GetParent()->GetSizer()->Layout();

        } else {
            // hide id
            if(!m_findBar->IsShown())
                return;

            m_findBar->Show(false);
            GetParent()->GetSizer()->Layout();
        }
    }
}

void MainBook::ShowQuickBar(const wxString& findWhat, bool showReplace)
{
    if(m_findBar) {
        m_findBar->Show(findWhat, showReplace);
        GetParent()->GetSizer()->Layout();
    }
}

void MainBook::OnEditorChanged(wxCommandEvent& event)
{
    event.Skip();
    IEditor* editor = GetActiveEditor();
    if(editor) {
        m_findBar->SetEditor(editor->GetCtrl());
        return;
    }
    m_findBar->SetEditor(NULL);
}

void MainBook::OnAllEditorClosed(wxCommandEvent& event)
{
    event.Skip();
    m_findBar->SetEditor(NULL);
}

void MainBook::SetFindBar(QuickFindBar* findBar)
{
    if(m_findBar)
        return;
    m_findBar = findBar;
    EventNotifier::Get()->Bind(wxEVT_ALL_EDITORS_CLOSED, &MainBook::OnAllEditorClosed, this);
    EventNotifier::Get()->Bind(wxEVT_ACTIVE_EDITOR_CHANGED, &MainBook::OnEditorChanged, this);
}

void MainBook::ShowQuickBarToolBar(bool s)
{
    wxUnusedVar(s);
    if(m_findBar) {
        m_findBar->ShowToolBarOnly();
    }
}

void MainBook::SetPageTitle(wxWindow* page, const wxFileName& filename, bool modified)
{
    SetPageTitle(page, CreateLabel(filename, modified));
#if !CL_USE_NATIVEBOOK
    int index = m_book->GetPageIndex(page);
    if(index != wxNOT_FOUND) {
        m_book->SetPageModified(index, modified);
    }
#endif
}

wxString MainBook::CreateLabel(const wxFileName& fn, bool modified) const
{
    wxString label = fn.GetFullName();
    if(fn.GetDirCount() && EditorConfigST::Get()->GetOptions()->IsTabShowPath()) {
        label.Prepend(fn.GetDirs().Last() + wxFileName::GetPathSeparator());
    }

#if CL_USE_NATIVEBOOK
    if(modified) {
        label.Prepend(wxT("\u25CF"));
    }
#else
    wxUnusedVar(modified);
#endif
    return label;
}

int MainBook::GetBitmapIndexOrAdd(const wxString& name) { return m_book->GetBitmaps()->Add(name); }

WelcomePage* MainBook::GetOrCreateWelcomePage()
{
    if(m_welcomePage == nullptr) {
        m_welcomePage = new WelcomePage(this);
        m_welcomePage->Hide();
        GetSizer()->Add(m_welcomePage, 1, wxEXPAND);
    }
    return m_welcomePage;
}

clEditor* MainBook::OpenFileAsync(const wxString& file_name, std::function<void(IEditor*)>&& callback)
{
    wxString real_path = CLRealPath(file_name);
    auto editor = FindEditor(real_path);
    if(editor) {
        push_callback(std::move(callback), real_path);
        bool is_active = GetActiveEditor() == editor;
        if(!is_active) {
            // make this file the active
            int index = m_book->GetPageIndex(editor);
            m_book->SetSelection(index);
        }
    } else {
        editor = OpenFile(real_path);
        if(editor) {
            push_callback(std::move(callback), real_path);
        }
    }
    return editor;
}

void MainBook::push_callback(std::function<void(IEditor*)>&& callback, const wxString& fullpath)
{
    // register a callback for this insert
    if(m_callbacksTable.count(fullpath) == 0) {
        m_callbacksTable.insert({ fullpath, {} });
    }
    m_callbacksTable[fullpath].emplace_back(std::move(callback));
}

bool MainBook::has_callbacks(const wxString& fullpath) const
{
    return !m_callbacksTable.empty() && (m_callbacksTable.count(fullpath) > 0);
}

void MainBook::execute_callbacks_for_file(const wxString& fullpath)
{
    if(!has_callbacks(fullpath)) {
        return;
    }

    auto& V = m_callbacksTable[fullpath];
    if(V.empty()) {
        return; // cant really happen
    }

    IEditor* editor = FindEditor(fullpath);
    CHECK_PTR_RET(editor);

    for(auto& callback : V) {
        callback(editor);
    }

    // remove the callbacks
    m_callbacksTable.erase(fullpath);
}

void MainBook::OnIdle(wxIdleEvent& event)
{
    event.Skip();

    // avoid processing if not really needed
    if(!m_initDone || (m_book->GetPageCount() == 0)) {
        return;
    }

    auto editor = GetActiveEditor(false);
    CHECK_PTR_RET(editor);

    execute_callbacks_for_file(CLRealPath(editor->GetFileName().GetFullPath()));
}

void MainBook::OnEditorModified(clCommandEvent& event)
{
    event.Skip();
#if !CL_USE_NATIVEBOOK
    int index = FindEditorIndexByFullPath(event.GetFileName());
    CHECK_COND_RET(index != wxNOT_FOUND);

    auto editor = FindEditor(event.GetFileName());
    CHECK_PTR_RET(editor);
    m_book->SetPageModified(index, editor->GetModify());
#endif
}

void MainBook::OnEditorSaved(clCommandEvent& event)
{
    event.Skip();
#if !CL_USE_NATIVEBOOK
    int index = FindEditorIndexByFullPath(event.GetFileName());
    CHECK_COND_RET(index != wxNOT_FOUND);

    auto editor = FindEditor(event.GetFileName());
    CHECK_PTR_RET(editor);
    m_book->SetPageModified(index, editor->GetModify());
#endif
}
