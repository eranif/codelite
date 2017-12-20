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
#include "FilesModifiedDlg.h"
#include "NotebookNavigationDlg.h"
#include "clAuiMainNotebookTabArt.h"
#include "clFileOrFolderDropTarget.h"
#include "clImageViewer.h"
#include "clang_code_completion.h"
#include "close_all_dlg.h"
#include "ctags_manager.h"
#include "editor_config.h"
#include "editorframe.h"
#include "event_notifier.h"
#include "filechecklist.h"
#include "frame.h"
#include "globals.h"
#include "ieditor.h"
#include "mainbook.h"
#include "manager.h"
#include "message_pane.h"
#include "new_quick_watch_dlg.h"
#include "pluginmanager.h"
#include "theme_handler.h"
#include <algorithm>
#include <wx/regex.h>
#include <wx/wupdlock.h>
#include <wx/xrc/xmlres.h>

MainBook::MainBook(wxWindow* parent)
    : wxPanel(parent)
    , m_navBar(NULL)
    , m_book(NULL)
    , m_quickFindBar(NULL)
    , m_useBuffereLimit(true)
    , m_isWorkspaceReloading(false)
    , m_reloadingDoRaise(true)
    , m_filesModifiedDlg(NULL)
{
    CreateGuiControls();
    ConnectEvents();
}

void MainBook::CreateGuiControls()
{
    wxBoxSizer* sz = new wxBoxSizer(wxVERTICAL);
    SetSizer(sz);

    m_messagePane = new MessagePane(this);
    sz->Add(m_messagePane, 0, wxALL | wxEXPAND, 5, NULL);

#if USE_AUI_NOTEBOOK
    long style = wxAUI_NB_TOP | wxAUI_NB_TAB_SPLIT | wxAUI_NB_TAB_MOVE | wxAUI_NB_CLOSE_ON_ACTIVE_TAB |
                 wxAUI_NB_WINDOWLIST_BUTTON | kNotebook_DynamicColours | kNotebook_MouseMiddleClickClosesTab;
#else
    long style = kNotebook_AllowDnD |                  // Allow tabs to move
                 kNotebook_MouseMiddleClickClosesTab | // Handle mouse middle button when clicked on a tab
                 kNotebook_MouseMiddleClickFireEvent | // instead of closing the tab, fire an event
                 kNotebook_ShowFileListButton |        // show drop down list of all open tabs
                 kNotebook_EnableNavigationEvent |     // Notify when user hit Ctrl-TAB or Ctrl-PGDN/UP
                 kNotebook_UnderlineActiveTab |        // Mark active tab with dedicated coloured line
                 kNotebook_DynamicColours;             // The tabs colour adjust to the editor's theme
    if(EditorConfigST::Get()->GetOptions()->IsTabHasXButton()) {
        style |= (kNotebook_CloseButtonOnActiveTabFireEvent | kNotebook_CloseButtonOnActiveTab);
    }
    if(EditorConfigST::Get()->GetOptions()->IsMouseScrollSwitchTabs()) { style |= kNotebook_MouseScrollSwitchTabs; }
#endif

    // load the notebook style from the configuration settings
    m_book = new Notebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, style);
    sz->Add(m_book, 1, wxEXPAND);

    m_navBar = new clEditorBar(this);
    sz->Add(m_navBar, 0, wxEXPAND);

    m_quickFindBar = new QuickFindBar(this);
    DoPositionFindBar(2);
    sz->Layout();
}

void MainBook::ConnectEvents()
{
    m_book->Bind(wxEVT_BOOK_PAGE_CLOSING, &MainBook::OnPageClosing, this);
    m_book->Bind(wxEVT_BOOK_PAGE_CLOSED, &MainBook::OnPageClosed, this);
    m_book->Bind(wxEVT_BOOK_PAGE_CHANGED, &MainBook::OnPageChanged, this);
    m_book->Bind(wxEVT_BOOK_PAGE_CHANGING, &MainBook::OnPageChanging, this);
    m_book->Bind(wxEVT_BOOK_PAGE_CLOSE_BUTTON, &MainBook::OnClosePage, this);
    m_book->Bind(wxEVT_BOOK_NAVIGATING, &MainBook::OnNavigating, this);
    m_book->Bind(wxEVT_BOOK_TABAREA_DCLICKED, &MainBook::OnMouseDClick, this);
    m_book->Bind(wxEVT_BOOK_TAB_DCLICKED, &MainBook::OnTabDClicked, this);
    m_book->Bind(wxEVT_BOOK_TAB_CONTEXT_MENU, &MainBook::OnTabLabelContextMenu, this);

    EventNotifier::Get()->Connect(wxEVT_WORKSPACE_LOADED, wxCommandEventHandler(MainBook::OnWorkspaceLoaded), NULL,
                                  this);
    EventNotifier::Get()->Connect(wxEVT_PROJ_FILE_ADDED, clCommandEventHandler(MainBook::OnProjectFileAdded), NULL,
                                  this);
    EventNotifier::Get()->Connect(wxEVT_PROJ_FILE_REMOVED, clCommandEventHandler(MainBook::OnProjectFileRemoved), NULL,
                                  this);
    EventNotifier::Get()->Connect(wxEVT_WORKSPACE_CLOSED, wxCommandEventHandler(MainBook::OnWorkspaceClosed), NULL,
                                  this);
    EventNotifier::Get()->Bind(wxEVT_DEBUG_ENDED, &MainBook::OnDebugEnded, this);
    EventNotifier::Get()->Connect(wxEVT_INIT_DONE, wxCommandEventHandler(MainBook::OnInitDone), NULL, this);

    EventNotifier::Get()->Bind(wxEVT_DETACHED_EDITOR_CLOSED, &MainBook::OnDetachedEditorClosed, this);
    EventNotifier::Get()->Bind(wxEVT_CL_THEME_CHANGED, &MainBook::OnThemeChanged, this);
    EventNotifier::Get()->Bind(wxEVT_EDITOR_CONFIG_CHANGED, &MainBook::OnEditorSettingsChanged, this);
    EventNotifier::Get()->Bind(wxEVT_CXX_SYMBOLS_CACHE_UPDATED, &MainBook::OnCacheUpdated, this);
    EventNotifier::Get()->Bind(wxEVT_CC_UPDATE_NAVBAR, &MainBook::OnUpdateNavigationBar, this);
    EventNotifier::Get()->Bind(wxEVT_CMD_COLOURS_FONTS_UPDATED, &MainBook::OnColoursAndFontsChanged, this);
    EventNotifier::Get()->Bind(wxEVT_NAVBAR_SCOPE_MENU_SHOWING, &MainBook::OnNavigationBarMenuShowing, this);
    EventNotifier::Get()->Bind(wxEVT_NAVBAR_SCOPE_MENU_SELECTION_MADE, &MainBook::OnNavigationBarMenuSelectionMade,
                               this);
}

MainBook::~MainBook()
{
    wxDELETE(m_filesModifiedDlg);
    m_book->Unbind(wxEVT_BOOK_PAGE_CLOSING, &MainBook::OnPageClosing, this);
    m_book->Unbind(wxEVT_BOOK_PAGE_CLOSED, &MainBook::OnPageClosed, this);
    m_book->Unbind(wxEVT_BOOK_PAGE_CHANGED, &MainBook::OnPageChanged, this);
    m_book->Unbind(wxEVT_BOOK_PAGE_CHANGING, &MainBook::OnPageChanging, this);
    m_book->Unbind(wxEVT_BOOK_PAGE_CLOSE_BUTTON, &MainBook::OnClosePage, this);
    m_book->Unbind(wxEVT_BOOK_NAVIGATING, &MainBook::OnNavigating, this);
    m_book->Unbind(wxEVT_BOOK_TABAREA_DCLICKED, &MainBook::OnMouseDClick, this);
    m_book->Unbind(wxEVT_BOOK_TAB_DCLICKED, &MainBook::OnTabDClicked, this);
    m_book->Unbind(wxEVT_BOOK_TAB_CONTEXT_MENU, &MainBook::OnTabLabelContextMenu, this);

    EventNotifier::Get()->Unbind(wxEVT_CL_THEME_CHANGED, &MainBook::OnThemeChanged, this);

    EventNotifier::Get()->Disconnect(wxEVT_WORKSPACE_LOADED, wxCommandEventHandler(MainBook::OnWorkspaceLoaded), NULL,
                                     this);
    EventNotifier::Get()->Disconnect(wxEVT_PROJ_FILE_ADDED, clCommandEventHandler(MainBook::OnProjectFileAdded), NULL,
                                     this);
    EventNotifier::Get()->Disconnect(wxEVT_PROJ_FILE_REMOVED, clCommandEventHandler(MainBook::OnProjectFileRemoved),
                                     NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_WORKSPACE_CLOSED, wxCommandEventHandler(MainBook::OnWorkspaceClosed), NULL,
                                     this);
    EventNotifier::Get()->Unbind(wxEVT_DEBUG_ENDED, &MainBook::OnDebugEnded, this);
    EventNotifier::Get()->Disconnect(wxEVT_INIT_DONE, wxCommandEventHandler(MainBook::OnInitDone), NULL, this);

    EventNotifier::Get()->Unbind(wxEVT_DETACHED_EDITOR_CLOSED, &MainBook::OnDetachedEditorClosed, this);
    EventNotifier::Get()->Unbind(wxEVT_EDITOR_CONFIG_CHANGED, &MainBook::OnEditorSettingsChanged, this);
    EventNotifier::Get()->Unbind(wxEVT_CXX_SYMBOLS_CACHE_UPDATED, &MainBook::OnCacheUpdated, this);
    EventNotifier::Get()->Unbind(wxEVT_CC_UPDATE_NAVBAR, &MainBook::OnUpdateNavigationBar, this);
    EventNotifier::Get()->Unbind(wxEVT_CMD_COLOURS_FONTS_UPDATED, &MainBook::OnColoursAndFontsChanged, this);
    EventNotifier::Get()->Unbind(wxEVT_NAVBAR_SCOPE_MENU_SHOWING, &MainBook::OnNavigationBarMenuShowing, this);
    EventNotifier::Get()->Unbind(wxEVT_NAVBAR_SCOPE_MENU_SELECTION_MADE, &MainBook::OnNavigationBarMenuSelectionMade,
                                 this);
}

void MainBook::OnMouseDClick(wxBookCtrlEvent& e)
{
    wxUnusedVar(e);
    NewEditor();
}

void MainBook::OnPageClosing(wxBookCtrlEvent& e)
{
    e.Skip();

    LEditor* editor = dynamic_cast<LEditor*>(m_book->GetPage(e.GetSelection()));
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
        if(!closeEvent.IsAllowed()) { e.Veto(); }
    }
}

void MainBook::OnPageClosed(wxBookCtrlEvent& e)
{
    e.Skip();
    SelectPage(m_book->GetCurrentPage());
    m_quickFindBar->SetEditor(GetActiveEditor());

    // any editors left open?
    LEditor* editor = NULL;
    for(size_t i = 0; i < m_book->GetPageCount() && editor == NULL; i++) {
        editor = dynamic_cast<LEditor*>(m_book->GetPage(i));
    }

    if(m_book->GetPageCount() == 0) {
        SendCmdEvent(wxEVT_ALL_EDITORS_CLOSED);
        ShowQuickBar(false);
    }
    DoUpdateNotebookTheme();
}

void MainBook::OnProjectFileAdded(clCommandEvent& e)
{
    e.Skip();
    const wxArrayString& files = e.GetStrings();
    for(size_t i = 0; i < files.GetCount(); i++) {
        LEditor* editor = FindEditor(files.Item(i));
        if(editor) {
            wxString fileName = editor->GetFileName().GetFullPath();
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
        LEditor* editor = FindEditor(files.Item(i));
        if(editor && files.Index(editor->GetFileName().GetFullPath()) != wxNOT_FOUND) {
            editor->SetProject(wxEmptyString);
        }
    }
}

void MainBook::OnWorkspaceLoaded(wxCommandEvent& e)
{
    e.Skip();
    CloseAll(false); // get ready for session to be restored by clearing out existing pages
}

void MainBook::OnWorkspaceClosed(wxCommandEvent& e)
{
    e.Skip();
    CloseAll(false); // make sure no unsaved files
    clStatusBar* sb = clGetManager()->GetStatusBar();
    if(sb) { sb->SetSourceControlBitmap(wxNullBitmap, "", ""); }
}

bool MainBook::AskUserToSave(LEditor* editor)
{
    if(!editor || !editor->GetModify()) return true;

    // unsaved changes
    wxString msg;
    msg << _("Save changes to '") << editor->GetFileName().GetFullName() << wxT("' ?");
    long style = wxYES_NO;
    if(!ManagerST::Get()->IsShutdownInProgress()) { style |= wxCANCEL; }

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

void MainBook::UpdateNavBar(LEditor* editor)
{
    TagEntryPtr tag = NULL;
    if(m_navBar->IsShown()) {
        if(editor && !editor->GetProject().IsEmpty()) {
            TagEntryPtrVector_t tags;
            if(TagsManagerST::Get()->GetFileCache()->Find(editor->GetFileName(), tags,
                                                          clCxxFileCacheSymbols::kFunctions)) {
                // the tags are already sorted by line
                // find the entry that is closest to the current line

                // lower_bound: find the first entry that !(entry < val)
                TagEntryPtr dummy(new TagEntry());
                dummy->SetLine(editor->GetCurrentLine() + 2);
                TagEntryPtrVector_t::iterator iter =
                    std::lower_bound(tags.begin(), tags.end(), dummy,
                                     [&](TagEntryPtr a, TagEntryPtr b) { return a->GetLine() < b->GetLine(); });
                if(iter != tags.end()) {
                    if(iter != tags.begin()) {
                        std::advance(iter, -1); // we want the previous match
                    }
                    // we got a match
                    tag = (*iter);
                } else if(!tags.empty()) {
                    // take the last element
                    tag = tags.back();
                }
            } else {
                TagsManagerST::Get()->GetFileCache()->RequestSymbols(editor->GetFileName());
            }
        }
        if(tag) {
            m_navBar->SetMessage(tag->GetScope(), tag->GetName());
        } else {
            m_navBar->SetMessage("", "");
        }
    }
}

void MainBook::ShowNavBar(bool s)
{
    m_navBar->DoShow(s);
    LEditor* editor = GetActiveEditor();
    CHECK_PTR_RET(editor);

    // Update the navigation bar
    clCodeCompletionEvent evtUpdateNavBar(wxEVT_CC_UPDATE_NAVBAR);
    evtUpdateNavBar.SetEditor(editor);
    evtUpdateNavBar.SetLineNumber(editor->GetCtrl()->GetCurrentLine());
    EventNotifier::Get()->AddPendingEvent(evtUpdateNavBar);
}

void MainBook::SaveSession(SessionEntry& session, wxArrayInt* excludeArr) { CreateSession(session, excludeArr); }

void MainBook::RestoreSession(SessionEntry& session)
{
    if(session.GetTabInfoArr().empty()) return; // nothing to restore

    CloseAll(false);
    size_t sel = session.GetSelectedTab();
    const std::vector<TabInfo>& vTabInfoArr = session.GetTabInfoArr();
    for(size_t i = 0; i < vTabInfoArr.size(); i++) {
        const TabInfo& ti = vTabInfoArr[i];
        m_reloadingDoRaise = (i == vTabInfoArr.size() - 1); // Raise() when opening only the last editor
        LEditor* editor = OpenFile(ti.GetFileName());
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

LEditor* MainBook::GetActiveEditor(bool includeDetachedEditors)
{
    if(includeDetachedEditors) {
        EditorFrame::List_t::iterator iter = m_detachedEditors.begin();
        for(; iter != m_detachedEditors.end(); ++iter) {
            if((*iter)->GetEditor()->IsFocused()) { return (*iter)->GetEditor(); }
        }
    }

    if(!GetCurrentPage()) { return NULL; }
    return dynamic_cast<LEditor*>(GetCurrentPage());
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

        LEditor* editor = dynamic_cast<LEditor*>(t.window);
        if(editor) {
            t.isFile = true;
            t.isModified = editor->IsModified();
            t.filename = editor->GetFileName();
        }
        tabs.push_back(t);
    });
#endif
}

void MainBook::GetAllEditors(LEditor::Vec_t& editors, size_t flags)
{
    editors.clear();
    if(!(flags & kGetAll_DetachedOnly)) {
        // Collect booked editors
        if(!(flags & kGetAll_RetainOrder)) {
            // Most of the time we don't care about the order the tabs are stored in
            for(size_t i = 0; i < m_book->GetPageCount(); i++) {
                LEditor* editor = dynamic_cast<LEditor*>(m_book->GetPage(i));
                if(editor) { editors.push_back(editor); }
            }
        } else {
            for(size_t i = 0; i < m_book->GetPageCount(); i++) {
                LEditor* editor = dynamic_cast<LEditor*>(m_book->GetPage(i));
                if(editor) { editors.push_back(editor); }
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

LEditor* MainBook::FindEditor(const wxString& fileName)
{
    wxString unixStyleFile(fileName);
#ifdef __WXMSW__
    unixStyleFile.Replace(wxT("\\"), wxT("/"));
#endif
    // On gtk either fileName or the editor filepath (or both) may be (or their paths contain) symlinks
    wxString fileNameDest = CLRealPath(fileName);

    for(size_t i = 0; i < m_book->GetPageCount(); i++) {
        LEditor* editor = dynamic_cast<LEditor*>(m_book->GetPage(i));
        if(editor) {
            wxString unixStyleFile(editor->GetFileName().GetFullPath());
            wxString nativeFile(unixStyleFile);
#ifdef __WXMSW__
            unixStyleFile.Replace(wxT("\\"), wxT("/"));
#endif

#ifndef __WXMSW__
            // On Unix files are case sensitive
            if(nativeFile.Cmp(fileName) == 0 || unixStyleFile.Cmp(fileName) == 0 ||
               unixStyleFile.Cmp(fileNameDest) == 0)
#else
            // Compare in no case sensitive manner
            if(nativeFile.CmpNoCase(fileName) == 0 || unixStyleFile.CmpNoCase(fileName) == 0 ||
               unixStyleFile.CmpNoCase(fileNameDest) == 0)
#endif
            {
                return editor;
            }

#if defined(__WXGTK__)
            // Try again, dereferencing the editor fpath
            wxString editorDest = CLRealPath(unixStyleFile);
            if(editorDest.Cmp(fileName) == 0 || editorDest.Cmp(fileNameDest) == 0) { return editor; }
#endif
        }
    }

    // try the detached editors
    EditorFrame::List_t::iterator iter = m_detachedEditors.begin();
    for(; iter != m_detachedEditors.end(); ++iter) {
        if((*iter)->GetEditor()->GetFileName().GetFullPath() == fileName) { return (*iter)->GetEditor(); }
    }
    return NULL;
}

wxWindow* MainBook::FindPage(const wxString& text)
{
    for(size_t i = 0; i < m_book->GetPageCount(); i++) {
        LEditor* editor = dynamic_cast<LEditor*>(m_book->GetPage(i));
        if(editor && editor->GetFileName().GetFullPath().CmpNoCase(text) == 0) { return editor; }

        if(m_book->GetPageText(i) == text) return m_book->GetPage(i);
    }
    return NULL;
}

LEditor* MainBook::NewEditor()
{
    static int fileCounter = 0;

    wxString fileNameStr(_("Untitled"));
    fileNameStr << ++fileCounter;
    wxFileName fileName(fileNameStr);

    // A Nice trick: hide the notebook, open the editor
    // and then show it
    bool hidden(false);
    if(m_book->GetPageCount() == 0) hidden = GetSizer()->Hide(m_book);

    LEditor* editor = new LEditor(m_book);
    editor->SetFileName(fileName);
    AddPage(editor, fileName.GetFullName(), fileName.GetFullPath(), wxNullBitmap, true);

#ifdef __WXMAC__
    m_book->GetSizer()->Layout();
#endif

    // SHow the notebook
    if(hidden) GetSizer()->Show(m_book);

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

LEditor* MainBook::OpenFile(const wxString& file_name, const wxString& projectName, int lineno, long position,
                            OF_extra extra /*=OF_AddJump*/, bool preserveSelection /*=true*/,
                            const wxBitmap& bmp /*= wxNullBitmap*/, const wxString& tooltip /* wxEmptyString */)
{
    wxFileName fileName(file_name);
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
        wxLogMessage(wxT("Failed to open: %s: No such file or directory"), fileName.GetFullPath().c_str());
        return NULL;
    }

    if(FileExtManager::GetType(fileName.GetFullName()) == FileExtManager::TypeBmp) {
        // a bitmap file, open it using an image viewer
        DoOpenImageViewer(fileName);
        return NULL;
    }

    wxString projName = projectName;
    if(projName.IsEmpty()) {
        // try to match a project name to the file. otherwise, CC may not work
        projName = ManagerST::Get()->GetProjectNameByFile(fileName.GetFullPath());
    }

    LEditor* editor = GetActiveEditor(true);
    BrowseRecord jumpfrom = editor ? editor->CreateBrowseRecord() : BrowseRecord();

    editor = FindEditor(fileName.GetFullPath());
    if(editor) {
        editor->SetProject(projName);
    } else if(fileName.IsOk() == false) {
        wxLogMessage(wxT("Invalid file name: ") + fileName.GetFullPath());
        return NULL;

    } else if(!fileName.FileExists()) {
        wxLogMessage(wxT("File: ") + fileName.GetFullPath() + wxT(" does not exist!"));
        return NULL;

    } else {

        // A Nice trick: hide the notebook, open the editor
        // and then show it
        bool hidden(false);
        if(m_book->GetPageCount() == 0) hidden = GetSizer()->Hide(m_book);

        editor = new LEditor(m_book);
        editor->SetEditorBitmap(bmp);
        editor->Create(projName, fileName);

        int sel = m_book->GetSelection();
        if((extra & OF_PlaceNextToCurrent) && (sel != wxNOT_FOUND)) {
            AddPage(editor, fileName.GetFullName(), tooltip.IsEmpty() ? fileName.GetFullPath() : tooltip, bmp, false,
                    sel + 1);
        } else {
            AddPage(editor, fileName.GetFullName(), tooltip.IsEmpty() ? fileName.GetFullPath() : tooltip, bmp);
        }
        editor->SetSyntaxHighlight();

        // mark the editor as read only if neede
        MarkEditorReadOnly(editor);

        // SHow the notebook
        if(hidden) GetSizer()->Show(m_book);

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
        editor->CenterLine(lineno);
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
        NavMgr::Get()->AddJump(jumpfrom, jumpto);
    }
#if !CL_USE_NATIVEBOOK
    if(m_book->GetPageCount() == 1) { m_book->GetSizer()->Layout(); }
#endif
    return editor;
}

bool MainBook::AddPage(wxWindow* win, const wxString& text, const wxString& tooltip, const wxBitmap& bmp, bool selected,
                       int insert_at_index /*=wxNOT_FOUND*/)
{
    if(m_book->GetPageIndex(win) != wxNOT_FOUND) return false;
    long MaxBuffers = clConfig::Get().Read(kConfigMaxOpenedTabs, 15);
    bool closeLastTab = ((long)(m_book->GetPageCount()) >= MaxBuffers) && GetUseBuffereLimit();
    if(insert_at_index == wxNOT_FOUND) {
        m_book->AddPage(win, text, selected, bmp);
    } else {
        if(!m_book->InsertPage(insert_at_index, win, text, selected, bmp)) {
            // failed to insert, append it
            m_book->AddPage(win, text, selected, bmp);
        }
    }

    if(closeLastTab) {
#if 0
        // We have reached the limit of the number of open buffers
        // Close the last used buffer
        const wxArrayPtrVoid& arr = m_book->GetHistory();
        if(arr.GetCount()) {
            // We got at least one page, close the last used
            wxWindow* tab = static_cast<wxWindow*>(arr.Item(arr.GetCount() - 1));
            ClosePage(tab);
        }
#endif
    }

#if !CL_USE_NATIVEBOOK
    if(m_book->GetPageCount() == 1) { m_book->GetSizer()->Layout(); }
#endif
    if(!tooltip.IsEmpty()) { m_book->SetPageToolTip(m_book->GetPageIndex(win), tooltip); }
    return true;
}

bool MainBook::SelectPage(wxWindow* win)
{
    int index = m_book->GetPageIndex(win);
    if(index != wxNOT_FOUND && m_book->GetSelection() != index) {
#if USE_AUI_NOTEBOOK
        m_book->ChangeSelection(index);
#else
        m_book->SetSelection(index);
#endif
    }
    return DoSelectPage(win);
}

bool MainBook::UserSelectFiles(std::vector<std::pair<wxFileName, bool> >& files, const wxString& title,
                               const wxString& caption, bool cancellable)
{
    if(files.empty()) return true;

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
    LEditor::Vec_t editors;
    GetAllEditors(editors, MainBook::kGetAll_IncludeDetached);

    std::vector<std::pair<wxFileName, bool> > files;
    size_t n = 0;
    for(size_t i = 0; i < editors.size(); i++) {
        if(!editors[i]->GetModify()) continue;

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
            if(files[i].second) { editors[i]->SaveFile(); }
        }
    }
    // And notify the plugins to save their tabs (this function only cover editors)
    clCommandEvent saveAllEvent(wxEVT_SAVE_ALL_EDITORS);
    EventNotifier::Get()->AddPendingEvent(saveAllEvent);
    return res;
}

void MainBook::ReloadExternallyModified(bool prompt)
{
    if(m_isWorkspaceReloading) return;
    static int depth = wxNOT_FOUND;
    ++depth;

    // Protect against recursion
    if(depth == 2) {
        depth = wxNOT_FOUND;
        return;
    }

    LEditor::Vec_t editors;
    GetAllEditors(editors, MainBook::kGetAll_IncludeDetached);

    time_t workspaceModifiedTimeBefore = clCxxWorkspaceST::Get()->GetFileLastModifiedTime();

    // filter list of editors for any whose files have been modified
    std::vector<std::pair<wxFileName, bool> > files;
    size_t n = 0;
    for(size_t i = 0; i < editors.size(); i++) {
        time_t diskTime = editors[i]->GetFileLastModifiedTime();
        time_t editTime = editors[i]->GetEditorLastModifiedTime();
        if(diskTime != editTime) {
            // update editor last mod time so that we don't keep bugging the user over the same file,
            // unless it gets changed again
            editors[i]->SetEditorLastModifiedTime(diskTime);

            // A last check: see if the content of the file has actually changed. This avoids unnecessary reload offers
            // after e.g. git stash
            if(!CompareFileWithString(editors[i]->GetFileName().GetFullPath(), editors[i]->GetText())) {
                files.push_back(std::make_pair(editors[i]->GetFileName(), !editors[i]->GetModify()));
                editors[n++] = editors[i];
            }
        }
    }
    editors.resize(n);
    if(n == 0) return;

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

            if(res == FilesModifiedDlg::kID_BUTTON_IGNORE) { return; }
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
    LEditor::Vec_t editorsAgain;
    GetAllEditors(editorsAgain, MainBook::kGetAll_IncludeDetached);

    // Make sure that the tabs that we have opened
    // are still available in the main book
    LEditor::Vec_t realEditorsList;
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
        TagsManagerST::Get()->RetagFiles(filesToRetag, TagsManager::Retag_Quick);
        SendCmdEvent(wxEVT_FILE_RETAGGED, (void*)&filesToRetag);

    } else if(filesToRetag.size() == 1) {
        ManagerST::Get()->RetagFile(filesToRetag.at(0).GetFullPath());
        SendCmdEvent(wxEVT_FILE_RETAGGED, (void*)&filesToRetag);
    }
}

bool MainBook::ClosePage(wxWindow* page)
{
    int pos = m_book->GetPageIndex(page);
    return pos != wxNOT_FOUND && m_book->DeletePage(pos);
}

bool MainBook::CloseAllButThis(wxWindow* page)
{
    wxString text;

    clWindowUpdateLocker locker(this);

    int pos = m_book->GetPageIndex(page);
    if(pos != wxNOT_FOUND) {
        text = m_book->GetPageText(pos);
        m_book->RemovePage(pos);
    }

    bool res = CloseAll(true);
    if(pos != wxNOT_FOUND) { m_book->AddPage(page, text, true); }
    return res;
}

bool MainBook::CloseAll(bool cancellable)
{
    LEditor::Vec_t editors;
    GetAllEditors(editors, kGetAll_IncludeDetached);

    // filter list of editors for any that need to be saved
    std::vector<std::pair<wxFileName, bool> > files;
    size_t n = 0;
    for(size_t i = 0; i < editors.size(); i++) {
        if(editors[i]->GetModify()) {
            files.push_back(std::make_pair(editors[i]->GetFileName(), true));
            editors[n++] = editors[i];
        }
    }
    editors.resize(n);

    if(!UserSelectFiles(files, _("Save Modified Files"),
                        _("Some files are modified.\nChoose the files you would like to save."), cancellable))
        return false;

    for(size_t i = 0; i < files.size(); i++) {
        if(files[i].second) {
            editors[i]->SaveFile();
        } else {
            editors[i]->SetSavePoint();
        }
    }

    // Delete the files without notifications (it will be faster)
    clWindowUpdateLocker locker(this);
#if HAS_LIBCLANG
    ClangCodeCompletion::Instance()->CancelCodeComplete();
#endif

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
    m_quickFindBar->SetEditor(NULL);
    ShowQuickBar(false);

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
    if(selection != wxNOT_FOUND) return m_book->GetPageText(selection);
    return wxEmptyString;
}

void MainBook::SetPageTitle(wxWindow* page, const wxString& name)
{
    int selection = m_book->GetPageIndex(page);
    if(selection != wxNOT_FOUND) {
        // LEditor *editor = dynamic_cast<LEditor*>(page);
        m_book->SetPageText(selection, name);
    }
}

void MainBook::ApplySettingsChanges()
{
    DoUpdateEditorsThemes();

    clMainFrame::Get()->UpdateAUI();
    clMainFrame::Get()->ShowOrHideCaptions();

    // Last: reposition the findBar
    DoPositionFindBar(2);
}

void MainBook::UnHighlightAll()
{
    std::vector<LEditor*> editors;
    GetAllEditors(editors, MainBook::kGetAll_IncludeDetached);
    for(size_t i = 0; i < editors.size(); i++) {
        editors[i]->UnHighlightAll();
    }
}

void MainBook::DelAllBreakpointMarkers()
{
    std::vector<LEditor*> editors;
    GetAllEditors(editors, MainBook::kGetAll_IncludeDetached);
    for(size_t i = 0; i < editors.size(); i++) {
        editors[i]->DelAllBreakpointMarkers();
    }
}

void MainBook::SetViewEOL(bool visible)
{
    std::vector<LEditor*> editors;
    GetAllEditors(editors, MainBook::kGetAll_IncludeDetached);
    for(size_t i = 0; i < editors.size(); i++) {
        editors[i]->SetViewEOL(visible);
    }
}

void MainBook::HighlightWord(bool hl)
{
    std::vector<LEditor*> editors;
    GetAllEditors(editors, MainBook::kGetAll_IncludeDetached);
    for(size_t i = 0; i < editors.size(); i++) {
        editors[i]->HighlightWord(hl);
    }
}

void MainBook::ShowWhitespace(int ws)
{
    std::vector<LEditor*> editors;
    GetAllEditors(editors, MainBook::kGetAll_IncludeDetached);
    for(size_t i = 0; i < editors.size(); i++) {
        editors[i]->SetViewWhiteSpace(ws);
    }
}

void MainBook::UpdateColours()
{
    std::vector<LEditor*> editors;
    GetAllEditors(editors, MainBook::kGetAll_IncludeDetached);
    for(size_t i = 0; i < editors.size(); i++) {
        editors[i]->UpdateColours();
    }
}

void MainBook::UpdateBreakpoints()
{
    std::vector<LEditor*> editors;
    GetAllEditors(editors, MainBook::kGetAll_IncludeDetached);
    for(size_t i = 0; i < editors.size(); i++) {
        editors[i]->UpdateBreakpoints();
    }
    ManagerST::Get()->GetBreakpointsMgr()->RefreshBreakpointMarkers();
}

void MainBook::MarkEditorReadOnly(LEditor* editor)
{
    if(!editor) { return; }

    bool readOnly = (!editor->IsEditable()) || ::IsFileReadOnly(editor->GetFileName());
    if(readOnly && editor->GetModify()) {
        // an attempt to mark a modified file as read-only
        // ask the user to save his changes before
        ::wxMessageBox(_("Please save your changes before marking the file as read only"), "CodeLite",
                       wxOK | wxCENTER | wxICON_WARNING, this);
        return;
    }

#if !CL_USE_NATIVEBOOK
    wxBitmap lockBmp = ::clGetManager()->GetStdIcons()->LoadBitmap("lock");
    for(size_t i = 0; i < m_book->GetPageCount(); i++) {
        wxBitmap orig_bmp = editor->GetEditorBitmap();
        if(editor == m_book->GetPage(i)) {
            m_book->SetPageBitmap(i, readOnly ? lockBmp : orig_bmp);
            break;
        }
    }
#endif
}

long MainBook::GetBookStyle() { return 0; }

bool MainBook::DoSelectPage(wxWindow* win)
{
    LEditor* editor = dynamic_cast<LEditor*>(win);
    if(editor) {
        editor->SetActive();
        m_quickFindBar->SetEditor(editor);

    } else {
        m_quickFindBar->ShowForPlugins();
    }

    // Remove context menu if needed
    DoHandleFrameMenu(editor);

    if(!editor) {
        clMainFrame::Get()->SetFrameTitle(NULL);
        clMainFrame::Get()->GetStatusBar()->SetLinePosColumn(wxEmptyString);
        UpdateNavBar(NULL);
        SendCmdEvent(wxEVT_CMD_PAGE_CHANGED, win);

    } else {
        // if(editor->GetContext()->GetName() == wxT("C++")) {
        //     if(clMainFrame::Get()->GetMenuBar()->FindMenu(wxT("C++")) == wxNOT_FOUND) {
        //         clMainFrame::Get()->GetMenuBar()->Append(wxXmlResource::Get()->LoadMenu(wxT("editor_right_click")),
        //                                                  wxT("C++"));
        //     }
        // }
        wxCommandEvent event(wxEVT_ACTIVE_EDITOR_CHANGED);
        event.SetClientData((void*)dynamic_cast<IEditor*>(editor));
        EventNotifier::Get()->AddPendingEvent(event);
        UpdateNavBar(editor);
    }

    return true;
}

void MainBook::ShowMessage(const wxString& message, bool showHideButton, const wxBitmap& bmp, const ButtonDetails& btn1,
                           const ButtonDetails& btn2, const ButtonDetails& btn3, const CheckboxDetails& cb)
{
    m_messagePane->ShowMessage(message, showHideButton, bmp, btn1, btn2, btn3, cb);
    clMainFrame::Get()->SendSizeEvent();
}

void MainBook::OnPageChanged(wxBookCtrlEvent& e)
{
    e.Skip();
    int newSel = e.GetSelection();
    if(newSel != wxNOT_FOUND && m_reloadingDoRaise) {
        wxWindow* win = m_book->GetPage((size_t)newSel);
        if(win) { SelectPage(win); }
    }

    // Cancel any tooltip
    LEditor::Vec_t editors;
    GetAllEditors(editors, MainBook::kGetAll_IncludeDetached);
    for(size_t i = 0; i < editors.size(); ++i) {
        // Cancel any calltip when switching from the editor
        editors.at(i)->DoCancelCalltip();
    }
    DoUpdateNotebookTheme();
}

void MainBook::DoUpdateNotebookTheme()
{
    size_t initialStyle = m_book->GetStyle();
    size_t style = m_book->GetStyle();
    if(EditorConfigST::Get()->GetOptions()->IsTabColourMatchesTheme()) {
        // Update theme
        IEditor* editor = GetActiveEditor();
        if(editor) {
            wxColour bgColour = editor->GetCtrl()->StyleGetBackground(0);
            if(DrawingUtils::IsDark(bgColour) && !(m_book->GetStyle() & kNotebook_DarkTabs)) {
                style &= ~kNotebook_LightTabs;
                style |= kNotebook_DarkTabs;
            } else if(!DrawingUtils::IsDark(bgColour) && !(m_book->GetStyle() & kNotebook_LightTabs)) {
                style &= ~kNotebook_DarkTabs;
                style |= kNotebook_LightTabs;
            }
        }
    } else if(EditorConfigST::Get()->GetOptions()->IsTabColourDark()) {
        style &= ~kNotebook_LightTabs;
        style |= kNotebook_DarkTabs;
    } else {
        style &= ~kNotebook_DarkTabs;
        style |= kNotebook_LightTabs;
    }

    if(!EditorConfigST::Get()->GetOptions()->IsTabHasXButton()) {
        style &= ~(kNotebook_CloseButtonOnActiveTab | kNotebook_CloseButtonOnActiveTabFireEvent);
    } else {
        style |= (kNotebook_CloseButtonOnActiveTab | kNotebook_CloseButtonOnActiveTabFireEvent);
    }

    if(initialStyle != style) { m_book->SetStyle(style); }
}

wxWindow* MainBook::GetCurrentPage() { return m_book->GetCurrentPage(); }
int MainBook::GetCurrentPageIndex() { return m_book->GetSelection(); }

void MainBook::OnClosePage(wxBookCtrlEvent& e)
{
    clWindowUpdateLocker locker(this);
    int where = e.GetSelection();
    if(where == wxNOT_FOUND) { return; }
    wxWindow* page = m_book->GetPage((size_t)where);
    if(page) { ClosePage(page); }
}

void MainBook::DoPositionFindBar(int where)
{
    clWindowUpdateLocker locker(this);
    // the find bar is already placed on the MainBook, detach it
    GetSizer()->Detach(m_quickFindBar);

    bool placeAtBottom = EditorConfigST::Get()->GetOptions()->GetFindBarAtBottom();
    size_t itemCount = GetSizer()->GetItemCount();
    for(size_t i = 0; i < itemCount; ++i) {
        wxSizerItem* sizerItem = GetSizer()->GetItem(i);
        if(!sizerItem) continue;
        if(sizerItem->GetWindow() == m_book) {
            // we found the main book
            if(placeAtBottom) {
                GetSizer()->Insert(i + 1, m_quickFindBar, 0, wxTOP | wxBOTTOM | wxEXPAND);
            } else {
                GetSizer()->Insert(i, m_quickFindBar, 0, wxTOP | wxBOTTOM | wxEXPAND);
            }
            break;
        }
    }
    GetSizer()->Layout();
}

void MainBook::OnDebugEnded(clDebugEvent& e) { e.Skip(); }

void MainBook::DoHandleFrameMenu(LEditor* editor) { wxUnusedVar(editor); }

void MainBook::OnPageChanging(wxBookCtrlEvent& e)
{
    LEditor* editor = GetActiveEditor();
    if(editor) { editor->CallTipCancel(); }
#if HAS_LIBCLANG
    ClangCodeCompletion::Instance()->CancelCodeComplete();
#endif
    e.Skip();
}

void MainBook::SetViewWordWrap(bool b)
{
    std::vector<LEditor*> editors;
    GetAllEditors(editors, MainBook::kGetAll_Default);
    for(size_t i = 0; i < editors.size(); i++) {
        editors[i]->SetWrapMode(b ? wxSTC_WRAP_WORD : wxSTC_WRAP_NONE);
    }
}

void MainBook::OnInitDone(wxCommandEvent& e) { e.Skip(); }

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
#if !USE_AUI_NOTEBOOK
    if(GetActiveEditor()) {
        LEditor* editor = GetActiveEditor();
        m_book->RemovePage(m_book->GetSelection(), true);
        EditorFrame* frame = new EditorFrame(clMainFrame::Get(), editor);
        // Move it to the same position as the main frame
        frame->Move(EventNotifier::Get()->TopFrame()->GetPosition());
        // And show it
        frame->Show();
        frame->Raise();
        m_detachedEditors.push_back(frame);
    }
#endif
}

void MainBook::OnDetachedEditorClosed(clCommandEvent& e)
{
    e.Skip();
    IEditor* editor = reinterpret_cast<IEditor*>(e.GetClientData());
    DoEraseDetachedEditor(editor);

    wxString alternateText = (editor && editor->IsModified()) ? editor->GetCtrl()->GetText() : "";
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

void MainBook::OnWorkspaceReloadEnded(clCommandEvent& e)
{
    e.Skip();
    m_isWorkspaceReloading = false;
}

void MainBook::OnWorkspaceReloadStarted(clCommandEvent& e)
{
    e.Skip();
    m_isWorkspaceReloading = true;
}

void MainBook::ClosePageVoid(wxWindow* win) { ClosePage(win); }

void MainBook::CloseAllButThisVoid(wxWindow* win) { CloseAllButThis(win); }

void MainBook::CloseAllVoid(bool cancellable) { CloseAll(cancellable); }

FilesModifiedDlg* MainBook::GetFilesModifiedDlg()
{
    if(!m_filesModifiedDlg) m_filesModifiedDlg = new FilesModifiedDlg(clMainFrame::Get());
    return m_filesModifiedDlg;
}

void MainBook::CreateSession(SessionEntry& session, wxArrayInt* excludeArr)
{
    std::vector<LEditor*> editors;
    GetAllEditors(editors, kGetAll_RetainOrder);

    // Remove editors which belong to the SFTP
    std::vector<LEditor*> editorsTmp;
    std::for_each(editors.begin(), editors.end(), [&](LEditor* editor) {
        IEditor* ieditor = dynamic_cast<IEditor*>(editor);
        if(ieditor->GetClientData("sftp") == NULL) { editorsTmp.push_back(editor); }
    });

    editors.swap(editorsTmp);

    session.SetSelectedTab(0);
    std::vector<TabInfo> vTabInfoArr;
    for(size_t i = 0; i < editors.size(); i++) {

        if(excludeArr && (excludeArr->GetCount() > i) && (!excludeArr->Item(i))) {
            // If we're saving only selected editors, and this isn't one of them...
            continue;
        }

        if(editors[i] == GetActiveEditor()) { session.SetSelectedTab(vTabInfoArr.size()); }
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
}

void MainBook::ShowTabBar(bool b) { wxUnusedVar(b); }

void MainBook::CloseTabsToTheRight(wxWindow* win)
{
    wxString text;

    // clWindowUpdateLocker locker(this);

    // Get list of tabs to close
    std::vector<wxWindow*> windows;
    bool currentWinFound(false);
    for(size_t i = 0; i < m_book->GetPageCount(); ++i) {
        if(currentWinFound) {
            windows.push_back(m_book->GetPage(i));
        } else {
            if(m_book->GetPage(i) == win) { currentWinFound = true; }
        }
    }

    // start from right to left
    if(windows.empty()) return;

    std::vector<wxWindow*> tabsToClose;
    for(int i = (int)(windows.size() - 1); i >= 0; --i) {
        if(windows.at(i) == win) { break; }
        tabsToClose.push_back(windows.at(i));
    }

    if(tabsToClose.empty()) return;

    for(size_t i = 0; i < tabsToClose.size(); ++i) {
        ClosePage(tabsToClose.at(i));
    }

#ifdef __WXMAC__
    m_book->GetSizer()->Layout();
#endif
}

void MainBook::OnNavigating(wxBookCtrlEvent& e)
{
    if(m_book->GetPageCount() == 0) return;
    NotebookNavigationDlg dlg(EventNotifier::Get()->TopFrame(), m_book);
    if(dlg.ShowModal() == wxID_OK && dlg.GetSelection() != wxNOT_FOUND) { m_book->SetSelection(dlg.GetSelection()); }
}

void MainBook::OnThemeChanged(wxCommandEvent& e)
{
    e.Skip();
    DoUpdateNotebookTheme();
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
    wxWindow* tabCtrl = static_cast<wxWindow*>(e.GetEventObject());
    if((e.GetSelection() == m_book->GetSelection()) &&
#if USE_AUI_NOTEBOOK
       (tabCtrl == m_book)
#else
       (tabCtrl->GetParent() == m_book)
#endif
    ) {
        // we only show context menu for the active tab
        e.Skip(false);
        wxMenu* contextMenu = wxXmlResource::Get()->LoadMenu(wxT("editor_tab_right_click"));

        // Notify the plugins about the tab label context menu
        clContextMenuEvent event(wxEVT_CONTEXT_MENU_TAB_LABEL);
        event.SetMenu(contextMenu);
        EventNotifier::Get()->ProcessEvent(event);

        contextMenu = event.GetMenu();
        tabCtrl->PopupMenu(contextMenu);
        wxDELETE(contextMenu);
    }
}

bool MainBook::ClosePage(IEditor* editor, bool notify)
{
    wxWindow* page = dynamic_cast<wxWindow*>(editor->GetCtrl());
    if(!page) return false;
    int pos = m_book->GetPageIndex(page);

#if USE_AUI_NOTEBOOK
    if(notify) {
        return (pos != wxNOT_FOUND) && (m_book->DeletePage(pos));
    } else {
        if(pos == wxNOT_FOUND) { return false; }
        wxWindow* win = m_book->GetPage(pos);
        if(win == nullptr) { return false; }
        bool res = m_book->RemovePage(pos);
        if(res) { win->Destroy(); }
        return res;
    }
#else
    return (pos != wxNOT_FOUND) && (m_book->DeletePage(pos, notify));
#endif
}

void MainBook::GetDetachedTabs(clTab::Vec_t& tabs)
{
    // Make sure that modified detached editors are also enabling the "Save" and "Save All" button
    const EditorFrame::List_t& detachedEditors = GetDetachedEditors();
    std::for_each(detachedEditors.begin(), detachedEditors.end(), [&](EditorFrame* fr) {
        clTab tabInfo;
        tabInfo.bitmap = wxNullBitmap;
        tabInfo.filename = fr->GetEditor()->GetFileName();
        tabInfo.isFile = true;
        tabInfo.isModified = fr->GetEditor()->IsModified();
        tabInfo.text = fr->GetEditor()->GetFileName().GetFullPath();
        tabInfo.window = fr->GetEditor();
        tabs.push_back(tabInfo);
    });
}

void MainBook::DoOpenFile(const wxString& filename, const wxString& content)
{
    LEditor* editor = OpenFile(filename);
    if(editor && !content.IsEmpty()) { editor->SetText(content); }
}

void MainBook::OnCacheUpdated(clCommandEvent& e)
{
    e.Skip();
    UpdateNavBar(GetActiveEditor());
}

void MainBook::OnUpdateNavigationBar(clCodeCompletionEvent& e)
{
    e.Skip();
    IEditor* editor = dynamic_cast<IEditor*>(e.GetEditor());
    CHECK_PTR_RET(editor);

    LEditor* activeEditor = GetActiveEditor();
    if(editor != activeEditor) return;

    // This event is no longer valid
    if(activeEditor->GetCurrentLine() != e.GetLineNumber()) return;

    FileExtManager::FileType ft = FileExtManager::GetTypeFromExtension(editor->GetFileName());
    if((ft != FileExtManager::TypeSourceC) && (ft != FileExtManager::TypeSourceCpp) &&
       (ft != FileExtManager::TypeHeader))
        return;

    // A C++ file :)
    e.Skip(false);
    UpdateNavBar(activeEditor);
}

void MainBook::OnColoursAndFontsChanged(clCommandEvent& e)
{
    e.Skip();
    DoUpdateEditorsThemes();
}

void MainBook::DoUpdateEditorsThemes()
{
    std::vector<LEditor*> editors;
    GetAllEditors(editors, MainBook::kGetAll_IncludeDetached);
    for(size_t i = 0; i < editors.size(); i++) {
        editors[i]->SetSyntaxHighlight(editors[i]->GetContext()->GetName());
    }
}

void MainBook::OnNavigationBarMenuShowing(clContextMenuEvent& e)
{
    e.Skip();
    m_currentNavBarTags.clear();
    IEditor* editor = GetActiveEditor(true);
    if(m_navBar->IsShown() && editor) {
        TagEntryPtrVector_t tags;
        if(!TagsManagerST::Get()->GetFileCache()->Find(editor->GetFileName(), tags,
                                                       clCxxFileCacheSymbols::kFunctions) ||
           tags.empty()) {
            return;
        }
        wxMenu* menu = e.GetMenu();
        std::for_each(tags.begin(), tags.end(), [&](TagEntryPtr tag) {
            wxString fullname = tag->GetFullDisplayName();
            menu->Append(wxID_ANY, fullname);
            m_currentNavBarTags.insert({ fullname, tag });
        });
    }
}

void MainBook::OnNavigationBarMenuSelectionMade(clCommandEvent& e)
{
    e.Skip();
    const wxString& selection = e.GetString();
    if(m_currentNavBarTags.count(selection) == 0) { return; }

    TagEntryPtr tag = m_currentNavBarTags[selection];
    // Ours to handle
    e.Skip(false);

    IEditor* editor = GetActiveEditor(true);
    if(!editor) { return; }

    editor->FindAndSelect(tag->GetPattern(), tag->GetName(), editor->PosFromLine(tag->GetLine() - 1), nullptr);
}
