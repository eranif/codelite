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

#include <set>
#include <wx/panel.h>
#include "sessionmanager.h"
#include "navbar.h"
#include "quickfindbar.h"
#include "notebook_ex.h"
#include "filehistory.h"
#include "message_pane.h"
#include "cl_command_event.h"

enum OF_extra { OF_None = 0x00000001, OF_AddJump = 0x00000002, OF_PlaceNextToCurrent = 0x00000004 };

class MessagePane;
class MainBook : public wxPanel
{

private:
    FileHistory   m_recentFiles;
    NavBar       *m_navBar;
    Notebook     *m_book;
    QuickFindBar *m_quickFindBar;
    MessagePane  *m_messagePane;
    bool          m_useBuffereLimit;

private:
    void CreateGuiControls();
    void ConnectEvents    ();

    void OnMouseDClick        (NotebookEvent     &e);
    void OnPageClosing        (NotebookEvent     &e);
    void OnPageClosed         (NotebookEvent     &e);
    void OnPageChanged        (NotebookEvent     &e);
    void OnClosePage          (NotebookEvent     &e);
    void OnPageChanging       (NotebookEvent     &e);
    void OnProjectFileAdded   (clCommandEvent& e);
    void OnProjectFileRemoved (wxCommandEvent    &e);
    void OnWorkspaceLoaded    (wxCommandEvent    &e);
    void OnWorkspaceClosed    (wxCommandEvent    &e);
    void OnDebugEnded         (wxCommandEvent    &e);
    void OnStringHighlight    (wxCommandEvent    &e);
    void OnInitDone           (wxCommandEvent    &e);

    bool AskUserToSave(LEditor *editor);
    bool DoSelectPage (wxWindow *win  );
    void DoPositionFindBar(int where);
    void DoHandleFrameMenu(LEditor *editor);
public:
    MainBook(wxWindow *parent);
    ~MainBook();
    
    void ClearFileHistory();
    void GetRecentlyOpenedFiles(wxArrayString &files);
    FileHistory &GetRecentlyOpenedFilesClass() {
        return m_recentFiles;
    }

    void ShowQuickBarForPlugins() {
        m_quickFindBar->ShowForPlugins();
    }

    void ShowQuickBar (bool s = true)           {
        m_quickFindBar->Show(s);
    }
    void ShowQuickBar (const wxString &findWhat) {
        m_quickFindBar->Show(findWhat);
    }
    void ShowMessage  (const wxString &message, bool showHideButton = true, const wxBitmap &bmp = wxNullBitmap, const ButtonDetails &btn1 = ButtonDetails(), const ButtonDetails &btn2 = ButtonDetails(), const ButtonDetails &btn3 = ButtonDetails(), const CheckboxDetails &cb = CheckboxDetails());

    void ShowNavBar   (bool s = true);
    void UpdateNavBar (LEditor *editor);
    bool IsNavBarShown()                    {
        return m_navBar->IsShown();
    }

    void SaveSession   (SessionEntry &session, wxArrayInt& intArr);
    void RestoreSession(SessionEntry &session);

    LEditor *GetActiveEditor();
    void     GetAllEditors  (std::vector<LEditor*> &editors, bool retain_order = false);
    LEditor *FindEditor     (const wxString &fileName);
    bool     CloseEditor    (const wxString &fileName) {
        return ClosePage(FindEditor(fileName));
    }

    wxWindow *GetCurrentPage();
    wxWindow *GetPage(size_t page);
    size_t   GetPageCount() const;
    wxWindow *FindPage      (const wxString &text);

    LEditor *NewEditor();

    LEditor *OpenFile(const wxString &file_name, const wxString &projectName = wxEmptyString,
                      int lineno = wxNOT_FOUND, long position = wxNOT_FOUND, OF_extra extra = OF_AddJump, bool preserveSelection = true);
    LEditor *OpenFile(const BrowseRecord &rec) {
        return OpenFile(rec.filename, rec.project, rec.lineno, rec.position, OF_None, false);
    }

    bool AddPage   (wxWindow *win, const wxString &text, const wxBitmap &bmp = wxNullBitmap, bool selected = false, size_t insert_at_index = wxNOT_FOUND);
    bool SelectPage(wxWindow *win);

    bool UserSelectFiles(std::vector<std::pair<wxFileName,bool> > &files, const wxString &title, const wxString &caption,
                         bool cancellable = true);

    bool SaveAll(bool askUser, bool includeUntitled);

    void ReloadExternallyModified(bool prompt);

    bool ClosePage (const wxString &text);

    bool ClosePage      (wxWindow *win);
    bool CloseAllButThis(wxWindow *win);
    bool CloseAll       (bool cancellable);

    wxString GetPageTitle(wxWindow *win) const;
    void     SetPageTitle(wxWindow *page, const wxString &name);
    long     GetBookStyle();

    void ApplySettingsChanges   ();
    void UnHighlightAll         ();
    void DelAllBreakpointMarkers();
    void SetViewEOL             (bool visible);
    void SetViewWordWrap        (bool b);
    void HighlightWord          (bool hl);
    void ShowWhitespace         (int ws);
    void UpdateColours          ();
    void UpdateBreakpoints      ();
    void MarkEditorReadOnly     (LEditor *editor, bool ro);

    void SetUseBuffereLimit(bool useBuffereLimit) {
        this->m_useBuffereLimit = useBuffereLimit;
    }
    bool GetUseBuffereLimit() const {
        return m_useBuffereLimit;
    }
};

#endif //MAINBOOK_H
