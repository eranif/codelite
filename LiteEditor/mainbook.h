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
#include "filehistory.h"

#ifdef CL_USE_CUSTOM_BOOK
#undef CL_USE_CUSTOM_BOOK
#endif
#define CL_USE_CUSTOM_BOOK 1

#ifdef CL_USE_CUSTOM_BOOK
#    include "custom_notebook.h"
#else
#    include <wx/notebook.h>
class NativeBook;
#endif


class MainBook : public wxPanel
{
private:
	FileHistory   m_recentFiles;
	NavBar       *m_navBar;

#ifdef CL_USE_CUSTOM_BOOK
	Notebook     *m_book;
#else
	NativeBook   *m_book;
#endif

	QuickFindBar *m_quickFindBar;
	wxWindow     *m_currentPage;

	std::set<wxWindow*> m_detachedTabs;

	void CreateGuiControls();
	void ConnectEvents    ();

	void OnMouseDClick       (wxMouseEvent      &e);
	void OnFocus             (wxFocusEvent      &e);
	void OnPaneClosed        (wxAuiManagerEvent &e);
	void OnProjectFileAdded  (wxCommandEvent    &e);
	void OnProjectFileRemoved(wxCommandEvent    &e);
	void OnWorkspaceLoaded   (wxCommandEvent    &e);
	void OnWorkspaceClosed   (wxCommandEvent    &e);

#ifdef CL_USE_CUSTOM_BOOK
	void OnPageClosing       (NotebookEvent     &e);
	void OnPageClosed        (NotebookEvent     &e);
#else
	void OnPageClosing       (wxNotebookEvent   &e);
	void OnPageClosed        (wxNotebookEvent   &e);
#endif

	bool AskUserToSave(LEditor *editor);

public:
	MainBook(wxWindow *parent);
	~MainBook();

	void ClearFileHistory();
	void GetRecentlyOpenedFiles(wxArrayString &files);
	FileHistory &GetRecentlyOpenedFilesClass() {
		return m_recentFiles;
	}

	void ShowQuickBar (bool s = true)       {
		m_quickFindBar->Show(s);
	}
	void ShowNavBar   (bool s = true);
	void UpdateNavBar (LEditor *editor);
	bool IsNavBarShown()                    {
		return m_navBar->IsShown();
	}

	void SaveSession   (SessionEntry &session);
	void RestoreSession(SessionEntry &session);

	LEditor *GetActiveEditor();
	void     GetAllEditors  (std::vector<LEditor*> &editors);
	LEditor *FindEditor     (const wxString &fileName);
	bool     CloseEditor    (const wxString &fileName) {
		return ClosePage(FindEditor(fileName));
	}

	wxWindow *GetCurrentPage() {
		return m_currentPage;
	}
	wxWindow *FindPage      (const wxString &text);

	LEditor *NewEditor();

	LEditor *OpenFile(const wxString &file_name, const wxString &projectName = wxEmptyString,
	                  int lineno = wxNOT_FOUND, long position = wxNOT_FOUND, bool addjump = true);
	LEditor *OpenFile(const BrowseRecord &rec) {
		return OpenFile(rec.filename, rec.project, rec.lineno, rec.position, false);
	}

	bool AddPage   (wxWindow *win, const wxString &text, const wxBitmap &bmp = wxNullBitmap, bool selected = false);
	bool SelectPage(wxWindow *win);

	bool DetachPage(wxWindow *win);
	bool DockPage  (wxWindow *win);
	bool IsDetached(wxWindow *win);

	bool UserSelectFiles(std::vector<std::pair<wxFileName,bool> > &files, const wxString &title, const wxString &caption,
	                     bool cancellable = true);

	bool SaveAll(bool askUser, bool includeUntitled);

	void ReloadExternallyModified();

	bool ClosePage      (const wxString &text) {
		return ClosePage(FindPage(text));
	}
	bool ClosePage      (wxWindow *win);
	bool CloseAllButThis(wxWindow *win);
	bool CloseAll       (bool cancellable);

	wxString GetPageTitle(wxWindow *win);
	void     SetPageTitle(wxWindow *page, const wxString &name);
	long     GetBookStyle();

	// TODO: replace these functions with event handlers
	void ApplySettingsChanges   ();
	void UnHighlightAll         ();
	void DelAllBreakpointMarkers();
	void SetViewEOL             (bool visible);
	void HighlightWord          (bool hl);
	void ShowWhitespace         (int ws);
	void UpdateColours          ();
	void UpdateBreakpoints      ();
	void MarkEditorReadOnly		(LEditor *editor, bool ro);
};

#ifndef CL_USE_CUSTOM_BOOK
class NativeBook : public wxNotebook
{
	wxMenu *m_contextMenu;

public:
	static const size_t npos = static_cast<size_t>(-1);

public:
	NativeBook(wxWindow *win)
			: wxNotebook(win, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0)
			, m_contextMenu(NULL) {}
	virtual ~NativeBook() {}

	// Compatibility API from Notebook to wxNotebook
	size_t    GetPageIndex      (wxWindow *win);

	void      SetRightClickMenu (wxMenu *menu) {
		m_contextMenu = menu;
	}

	wxWindow *GetTabContainer   () {
		return this;
	}

	bool RemovePage(size_t pos, bool) {
		return wxNotebook::RemovePage(pos);
	}

	void AddPage(wxWindow *win, const wxString &text, const wxString &WXUNUSED(tooltip), const wxBitmap &WXUNUSED(bmp), bool selected = false) {
		wxNotebook::AddPage(win, text, selected, wxNOT_FOUND);
	}

	void DeleteAllPages(bool) {
		wxNotebook::DeleteAllPages();
	}

	void SetPageText(size_t index, const wxString &text, const wxString &tooltip) {
		wxUnusedVar(tooltip);
		wxNotebook::SetPageText(index, text);
	}

	void SetPageBitmap(size_t index, const wxBitmap &bmp) {
		wxUnusedVar(index);
		wxUnusedVar(bmp);
	}

	long GetBookStyle() const {
		return 0;
	}
};

#endif //CL_USE_CUSTOM_BOOK

#endif //MAINBOOK_H
