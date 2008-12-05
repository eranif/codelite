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
#include <wx/xrc/xmlres.h>
#include "globals.h"
#include "frame.h"
#include "manager.h"
#include "custom_tabcontainer.h"
#include "close_all_dlg.h"
#include "filechecklist.h"
#include "mainbook.h"

MainBook::MainBook(wxWindow *parent)
		: wxPanel       (parent)
		, m_navBar      (NULL)
		, m_book        (NULL)
		, m_quickFindBar(NULL)
		, m_currentPage (NULL)
{
	CreateGuiControls();
	ConnectEvents();
}

void MainBook::CreateGuiControls()
{
	wxBoxSizer *sz = new wxBoxSizer(wxVERTICAL);
	SetSizer(sz);

	m_navBar = new NavBar(this);
	sz->Add(m_navBar, 0, wxEXPAND);

	long style = wxVB_TOP|wxVB_HAS_X|wxVB_BORDER|wxVB_TAB_DECORATION|wxVB_MOUSE_MIDDLE_CLOSE_TAB;
	m_book = new Notebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, style);
	m_book->GetTabContainer()->Connect(wxEVT_LEFT_DCLICK, wxMouseEventHandler(MainBook::OnMouseDClick), NULL, this);
	m_book->SetRightClickMenu(wxXmlResource::Get()->LoadMenu(wxT("editor_tab_right_click")));
	sz->Add(m_book, 1, wxEXPAND);

	m_quickFindBar = new QuickFindBar(this);
	sz->Add(m_quickFindBar, 0, wxEXPAND);

	sz->Layout();
}

void MainBook::ConnectEvents()
{
	m_book->GetTabContainer()->Connect(wxEVT_LEFT_DCLICK, wxMouseEventHandler(MainBook::OnMouseDClick), NULL, this);

	m_book->Connect(wxEVT_COMMAND_BOOK_PAGE_CLOSING,  NotebookEventHandler(MainBook::OnPageClosing),  NULL, this);
	m_book->Connect(wxEVT_COMMAND_BOOK_PAGE_CLOSED,   NotebookEventHandler(MainBook::OnPageClosed),   NULL, this);

	wxTheApp->Connect(wxEVT_AUI_PANE_CLOSE, wxAuiManagerEventHandler(MainBook::OnPaneClosed), NULL, this);

	wxTheApp->Connect(wxEVT_WORKSPACE_LOADED,  wxCommandEventHandler(MainBook::OnWorkspaceLoaded),    NULL, this);
	wxTheApp->Connect(wxEVT_PROJ_FILE_ADDED,   wxCommandEventHandler(MainBook::OnProjectFileAdded),   NULL, this);
	wxTheApp->Connect(wxEVT_PROJ_FILE_REMOVED, wxCommandEventHandler(MainBook::OnProjectFileRemoved), NULL, this);
	wxTheApp->Connect(wxEVT_WORKSPACE_CLOSED,  wxCommandEventHandler(MainBook::OnWorkspaceClosed),    NULL, this);
}

MainBook::~MainBook()
{
}

void MainBook::OnMouseDClick(wxMouseEvent& e)
{
	wxUnusedVar(e);
	NewEditor();
}

void MainBook::OnFocus(wxFocusEvent &e)
{
	if (!SelectPage(dynamic_cast<wxWindow*>(e.GetEventObject()))) {
		e.Skip();
	}
}

void MainBook::OnPaneClosed(wxAuiManagerEvent &e)
{
	if (!DockPage(e.GetPane()->window)) {
		e.Skip();
	}
}

void MainBook::OnPageClosing(NotebookEvent &e)
{
	LEditor *editor = dynamic_cast<LEditor*>(m_book->GetPage(e.GetSelection()));
	if (!editor) {
		; // the page is not an editor
	} else if (AskUserToSave(editor)) {
		SendCmdEvent(wxEVT_EDITOR_CLOSING, (IEditor*)editor);
	} else {
		e.Veto();
	}
}

void MainBook::OnPageClosed(NotebookEvent &e)
{
	SelectPage(m_book->GetCurrentPage());

	// any editors left open?
	LEditor *editor = NULL;
	for (size_t i = 0; i < m_book->GetPageCount() && editor == NULL; i++) {
		editor = dynamic_cast<LEditor*>(m_book->GetPage(i));
	}
	for (std::set<wxWindow*>::iterator i = m_detachedTabs.begin(); i != m_detachedTabs.end() && editor == NULL; i++) {
		editor = dynamic_cast<LEditor*>(*i);
	}
	if (editor == NULL) {
		SendCmdEvent(wxEVT_ALL_EDITORS_CLOSED);
		ShowQuickBar(false);
	}
}

void MainBook::OnProjectFileAdded(wxCommandEvent &e)
{
	e.Skip();
	wxArrayString *files = (wxArrayString*) e.GetClientData();
	if (!files)
		return;
	for (size_t i = 0; i < files->GetCount(); i++) {
		LEditor *editor = FindEditor(files->Item(i));
		if (editor) {
			wxString fileName = editor->GetFileName().GetFullPath();
			if (files->Index(fileName) != wxNOT_FOUND) {
				editor->SetProject(ManagerST::Get()->GetProjectNameByFile(fileName));
			}
		}
	}
}

void MainBook::OnProjectFileRemoved(wxCommandEvent &e)
{
	e.Skip();
	wxArrayString *files = (wxArrayString*) e.GetClientData();
	if (!files)
		return;
	for (size_t i = 0; i < files->GetCount(); i++) {
		LEditor *editor = FindEditor(files->Item(i));
		if (editor && files->Index(editor->GetFileName().GetFullPath()) != wxNOT_FOUND) {
			editor->SetProject(wxEmptyString);
		}
	}
}

void MainBook::OnWorkspaceLoaded(wxCommandEvent &e)
{
	e.Skip();
	CloseAll(false); // get ready for session to be restored by clearing out existing pages
}

void MainBook::OnWorkspaceClosed(wxCommandEvent &e)
{
	e.Skip();
	CloseAll(false); // make sure no unsaved files
}

bool MainBook::AskUserToSave(LEditor *editor)
{
	if (!editor || !editor->GetModify())
		return true;

	// unsaved changes
	wxString msg;
	msg << wxT("Save changes to '") << editor->GetFileName().GetFullName() << wxT("' ?");
	long style = wxYES_NO;
	if (!ManagerST::Get()->IsShutdownInProgress()) {
		style |= wxCANCEL;
	}

	int answer = wxMessageBox(msg, wxT("Confirm"), style);
	switch (answer) {
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

void MainBook::UpdateNavBar(LEditor *editor)
{
	if (m_navBar->IsShown())  {
		TagEntryPtr tag = NULL;
		if (editor && !editor->GetProject().IsEmpty()) {
			tag = TagsManagerST::Get()->FunctionFromFileLine(editor->GetFileName(), editor->GetCurrentLine()+1);
		}
		m_navBar->UpdateScope(tag);
	}
}

void MainBook::ShowNavBar(bool s)
{
	Freeze();
	m_navBar->DoShow(s);
	UpdateNavBar(GetActiveEditor());
	Thaw();
	Refresh();
}

void MainBook::SaveSession(SessionEntry &session)
{
	std::vector<LEditor*> editors;
	GetAllEditors(editors);

	session.SetSelectedTab(0);
	std::vector<TabInfo> vTabInfoArr;
	for (size_t i = 0; i < editors.size(); i++) {
		if (editors[i] == GetActiveEditor()) {
			session.SetSelectedTab(vTabInfoArr.size());
		}
		TabInfo oTabInfo;
		oTabInfo.SetFileName(editors[i]->GetFileName().GetFullPath());
		oTabInfo.SetFirstVisibleLine(editors[i]->GetFirstVisibleLine());
		oTabInfo.SetCurrentLine(editors[i]->GetCurrentLine());
		wxArrayString astrBookmarks;
		for (int nLine = 0; (nLine = editors[i]->MarkerNext(nLine, 128)) >= 0; nLine++) {
			astrBookmarks.Add(wxString::Format(wxT("%d"), nLine));
		}
		oTabInfo.SetBookmarks(astrBookmarks);
		vTabInfoArr.push_back(oTabInfo);
	}
	session.SetTabInfoArr(vTabInfoArr);
}

void MainBook::RestoreSession(SessionEntry &session)
{
	size_t sel = session.GetSelectedTab();
	const std::vector<TabInfo> &vTabInfoArr = session.GetTabInfoArr();
	for (size_t i = 0; i < vTabInfoArr.size(); i++) {
		const TabInfo &ti = vTabInfoArr[i];
		LEditor *editor = OpenFile(ti.GetFileName());
		if (!editor) {
			if (i < sel) {
				// have to adjust selected tab number because couldn't open tab
				sel--;
			}
			continue;
		}
		editor->ScrollToLine(ti.GetFirstVisibleLine());
		editor->GotoLine(ti.GetCurrentLine());
		const wxArrayString &astrBookmarks = ti.GetBookmarks();
		for (size_t i = 0; i < astrBookmarks.GetCount(); i++) {
			long nLine = 0;
			if (astrBookmarks.Item(i).ToLong(&nLine)) {
				editor->MarkerAdd(nLine, 0x7);
			}
		}
	}
	SelectPage(m_book->GetPage(sel));
}

LEditor *MainBook::GetActiveEditor()
{
	return dynamic_cast<LEditor*>(GetCurrentPage());
}

void MainBook::GetAllEditors(std::vector<LEditor*> &editors)
{
	editors.clear();
	for (size_t i = 0; i < m_book->GetPageCount(); i++) {
		LEditor *editor = dynamic_cast<LEditor*>(m_book->GetPage(i));
		if (editor) {
			editors.push_back(editor);
		}
	}
	for (std::set<wxWindow*>::iterator i = m_detachedTabs.begin(); i != m_detachedTabs.end(); i++) {
		LEditor *editor = dynamic_cast<LEditor*>(*i);
		if (editor) {
			editors.push_back(editor);
		}
	}
}

LEditor *MainBook::FindEditor(const wxString &fileName)
{
	for (size_t i = 0; i < m_book->GetPageCount(); i++) {
		LEditor *editor = dynamic_cast<LEditor*>(m_book->GetPage(i));
		if (editor && editor->GetFileName().GetFullPath() == fileName)
			return editor;
	}
	for (std::set<wxWindow*>::iterator i = m_detachedTabs.begin(); i != m_detachedTabs.end(); i++) {
		LEditor *editor = dynamic_cast<LEditor*>(*i);
		if (editor && editor->GetFileName().GetFullPath() == fileName)
			return editor;
	}
	return NULL;
}

wxWindow *MainBook::FindPage(const wxString &text)
{
	for (size_t i = 0; i < m_book->GetPageCount(); i++) {
		if (m_book->GetPageText(i) == text)
			return m_book->GetPage(i);
	}
	for (std::set<wxWindow*>::iterator i = m_detachedTabs.begin(); i != m_detachedTabs.end(); i++) {
		if (Frame::Get()->GetDockingManager().GetPane(*i).caption == text)
			return *i;
	}
	return NULL;
}

LEditor *MainBook::NewEditor()
{
	static int fileCounter = 0;

	wxString fileNameStr(wxT("Untitled"));
	fileNameStr << ++fileCounter;
	wxFileName fileName(fileNameStr);

	LEditor *editor = new LEditor(m_book);
	editor->SetFileName(fileName);
	AddPage(editor, fileName.GetFullName(), wxNullBitmap, true);
	return editor;
}

LEditor *MainBook::OpenFile(const wxString &file_name, const wxString &projectName, int lineno, long position)
{
	wxFileName fileName(file_name);
	fileName.MakeAbsolute();

	wxString projName = projectName;
	if (projName.IsEmpty()) {
		// try to match a project name to the file. otherwise, CC may not work
		projName = ManagerST::Get()->GetProjectNameByFile(fileName.GetFullPath());
	}

	LEditor* editor = FindEditor(fileName.GetFullPath());
	if (editor) {
		editor->SetProject(projName);
	} else if (fileName.IsOk() == false) {
		wxLogMessage(wxT("Invalid file name: ") + fileName.GetFullPath());
	} else if (!fileName.FileExists()) {
		wxLogMessage(wxT("File: ") + fileName.GetFullPath() + wxT(" does not exist!"));
	} else {
		editor = new LEditor(m_book);
		editor->Create(projName, fileName);
		AddPage(editor, fileName.GetFullName());
	}

	if (editor) {
		if (position != wxNOT_FOUND) {
			editor->SetCaretAt(position);
		} else if (lineno != wxNOT_FOUND) {
			editor->GotoLine(lineno);
		}
		editor->EnsureCaretVisible();
		if (GetActiveEditor() == editor) {
			editor->SetActive();
		} else {
			SelectPage(editor);
		}
		ManagerST::Get()->AddToRecentlyOpenedFiles(fileName.GetFullPath());
	}

	return editor;
}

bool MainBook::AddPage(wxWindow *win, const wxString &text, const wxBitmap &bmp, bool selected)
{
	if (m_book->GetPageIndex(win) != Notebook::npos || m_detachedTabs.find(win) != m_detachedTabs.end())
		return false;
	win->Connect(wxEVT_SET_FOCUS, wxFocusEventHandler(MainBook::OnFocus), NULL, this);
	m_book->AddPage(win, text, bmp, selected);
	return true;
}

bool MainBook::SelectPage(wxWindow *win)
{
	size_t index = m_book->GetPageIndex(win);
	std::set<wxWindow*>::iterator i = m_detachedTabs.find(win);

	if (index != Notebook::npos) {
		m_book->SetSelection(index);
	} else  if (i != m_detachedTabs.end()) {
		// anything to do?
	} else if (win == NULL) {
		// OK -- deselecting any page
	} else
		return false;

	m_currentPage = win;

	LEditor *editor = dynamic_cast<LEditor*>(win);

	// FIXME: move special context-specific menu handling to the Context classes?
	// it could be done inside the existing ContextXXX::SetActive() method.
	if (!editor || editor->GetContext()->GetName() != wxT("C++")) {
		int idx = Frame::Get()->GetMenuBar()->FindMenu(wxT("C++"));
		if ( idx != wxNOT_FOUND ) {
			delete Frame::Get()->GetMenuBar()->Remove(idx);
		}
	}

	if (!editor) {
		Frame::Get()->SetFrameTitle(NULL);
		Frame::Get()->SetStatusMessage(wxEmptyString, 2); // clear line & column indicator
		Frame::Get()->SetStatusMessage(wxEmptyString, 3); // clear end-of-line mode indicator
		UpdateNavBar(NULL);
	} else {
		editor->SetActive();
		if (editor->GetContext()->GetName() == wxT("C++")) {
			if (Frame::Get()->GetMenuBar()->FindMenu(wxT("C++")) == wxNOT_FOUND) {
				Frame::Get()->GetMenuBar()->Append(wxXmlResource::Get()->LoadMenu(wxT("editor_right_click")), wxT("C++"));
			}
		}
		SendCmdEvent(wxEVT_ACTIVE_EDITOR_CHANGED, (IEditor*)editor);
	}

	return true;
}

bool MainBook::DetachPage(wxWindow* win)
{
	if (IsDetached(win))
		return true;
	size_t pos = m_book->GetPageIndex(win);
	if (pos == Notebook::npos)
		return false;
	wxAuiPaneInfo info = wxAuiPaneInfo().Name(m_book->GetPageText(pos)).Caption(m_book->GetPageText(pos))
	                     .BestSize(win->GetSize()).Float();
	m_book->RemovePage(pos, false);
	Frame::Get()->GetDockingManager().AddPane(win, info);
	Frame::Get()->GetDockingManager().Update();
	m_detachedTabs.insert(win);
	wxAuiFloatingFrame *frm = dynamic_cast<wxAuiFloatingFrame*>(win->GetParent());
	if (frm) {
		wxAcceleratorTable *acclTable = Frame::Get()->GetAcceleratorTable();
		((wxFrame*)frm)->SetAcceleratorTable(*acclTable);
	}
	return true;
}

bool MainBook::DockPage(wxWindow* win)
{
	std::set<wxWindow*>::iterator i = m_detachedTabs.find(win);
	if (i == m_detachedTabs.end())
		return false;
	m_detachedTabs.erase(i);
	wxAuiPaneInfo info = Frame::Get()->GetDockingManager().GetPane(win);
	Frame::Get()->GetDockingManager().DetachPane(win);
	Frame::Get()->GetDockingManager().Update();
	m_book->AddPage(win, info.caption);
	return true;
}

bool MainBook::IsDetached(wxWindow* win)
{
	return m_detachedTabs.find(win) != m_detachedTabs.end();
}

bool MainBook::UserSelectFiles(std::vector<std::pair<wxFileName,bool> > &files, const wxString &title,
                               const wxString &caption, bool cancellable)
{
	if (files.empty())
		return true;

	FileCheckList dlg(this, wxID_ANY, title);
	dlg.SetCaption(caption);
	dlg.SetFiles(files);
	dlg.SetCancellable(cancellable);
	bool res = dlg.ShowModal() == wxID_OK;
	files = dlg.GetFiles();
	return res;
}

bool MainBook::SaveAll(bool askUser, bool includeUntitled)
{
	std::vector<LEditor*> editors;
	GetAllEditors(editors);

	std::vector<std::pair<wxFileName, bool> > files;
	size_t n = 0;
	for (size_t i = 0; i < editors.size(); i++) {
		if (!editors[i]->GetModify())
			continue;
		if (!includeUntitled && editors[i]->GetFileName().GetFullPath().StartsWith(wxT("Untitled")))
			continue; //don't save new documents that have not been saved to disk yet
		files.push_back(std::make_pair(editors[i]->GetFileName(), true));
		editors[n++] = editors[i];
	}
	editors.resize(n);

	bool res = !askUser || UserSelectFiles(files, wxT("Save Modified Files"),
	                                       wxT("Some files are modified.\nChoose the files you would like to save."));
	if (res) {
		for (size_t i = 0; i < files.size(); i++) {
			if (files[i].second) {
				editors[i]->SaveFile();
			}
		}
	}
	return res;
}

void MainBook::ReloadExternallyModified()
{
	std::vector<LEditor*> editors;
	GetAllEditors(editors);

	// filter list of editors for any whose files have been modified
	std::vector<std::pair<wxFileName, bool> > files;
	size_t n = 0;
	for (size_t i = 0; i < editors.size(); i++) {
		time_t diskTime = editors[i]->GetFileLastModifiedTime();
		time_t editTime = editors[i]->GetEditorLastModifiedTime();
		if (diskTime > editTime) {
			files.push_back(std::make_pair(editors[i]->GetFileName(), !editors[i]->GetModify()));
			// update editor last mod time so that we don't keep bugging the user over the same file,
			// unless it gets changed again
			editors[i]->SetEditorLastModifiedTime(diskTime);
			editors[n++] = editors[i];
		}
	}
	editors.resize(n);

	UserSelectFiles(files, wxT("Reload Modified Files"),
	                wxT("Files have been modified outside the editor.\nChoose which files you would like to reload."), false);
	std::vector<wxFileName> filesToRetag;
	for (size_t i = 0; i < files.size(); i++) {
		if (files[i].second) {
			editors[i]->ReloadFile();
			filesToRetag.push_back(files[i].first);
		}
	}
	if (!filesToRetag.empty()) {
		TagsManagerST::Get()->RetagFiles(filesToRetag);
		SendCmdEvent(wxEVT_FILE_RETAGGED, (void*)&filesToRetag);
	}
}

bool MainBook::ClosePage(wxWindow *page)
{
	DockPage(page);
	size_t pos = m_book->GetPageIndex(page);
	return pos != Notebook::npos && m_book->DeletePage(pos);
}

bool MainBook::CloseAllButThis(wxWindow *page)
{
	// TODO: handle case where page is detached
	wxString text;
	size_t pos = m_book->GetPageIndex(page);
	if (pos != Notebook::npos) {
		text = m_book->GetPageText(pos);
		m_book->RemovePage(pos, false);
	}
	bool res = CloseAll(true);
	if (pos != Notebook::npos) {
		m_book->AddPage(page, text, wxNullBitmap, true);
	}
	return res;
}

bool MainBook::CloseAll(bool cancellable)
{
	std::vector<LEditor*> editors;
	GetAllEditors(editors);

	// filter list of editors for any that need to be saved
	std::vector<std::pair<wxFileName, bool> > files;
	size_t n = 0;
	for (size_t i = 0; i < editors.size(); i++) {
		if (editors[i]->GetModify()) {
			files.push_back(std::make_pair(editors[i]->GetFileName(), true));
			editors[n++] = editors[i];
		}
	}
	editors.resize(n);

	if (!UserSelectFiles(files, wxT("Save Modified Files"),
	                     wxT("Some files are modified.\nChoose the files you would like to save."), cancellable))
		return false;

	for (size_t i = 0; i < files.size(); i++) {
		if (files[i].second) {
			editors[i]->SaveFile();
		} else {
			editors[i]->SetSavePoint();
		}
	}

	while (!m_detachedTabs.empty()) {
		DockPage(*m_detachedTabs.begin());
	}
	m_book->DeleteAllPages(true);
	return true;
}

wxString MainBook::GetPageTitle(wxWindow *page)
{
	size_t selection = m_book->GetPageIndex(page);
	if (selection != Notebook::npos)
		return m_book->GetPageText(selection);
	std::set<wxWindow*>::iterator i = m_detachedTabs.find(page);
	if (i != m_detachedTabs.end()) {
		return Frame::Get()->GetDockingManager().GetPane(page).caption;
	}
	return wxEmptyString;
}

void MainBook::SetPageTitle ( wxWindow *page, const wxString &name )
{
	size_t selection = m_book->GetPageIndex(page);
	if (selection != Notebook::npos) {
		m_book->SetPageText(selection, name);
	}
	std::set<wxWindow*>::iterator i = m_detachedTabs.find(page);
	if (i != m_detachedTabs.end()) {
		wxAuiManager &mgr = Frame::Get()->GetDockingManager();
		wxAuiPaneInfo &info = mgr.GetPane(page);
		if (info.IsOk()) {
			info.caption = info.name = name;
			if (info.frame != NULL) {
				info.frame->SetTitle(name);
			}
			mgr.Update();
		}
	}
}

void MainBook::ApplySettingsChanges()
{
	std::vector<LEditor*> editors;
	GetAllEditors(editors);
	for (size_t i = 0; i < editors.size(); i++) {
		editors[i]->SetSyntaxHighlight(editors[i]->GetContext()->GetName());
	}
}

void MainBook::UnHighlightAll()
{
	std::vector<LEditor*> editors;
	GetAllEditors(editors);
	for (size_t i = 0; i < editors.size(); i++) {
		editors[i]->UnHighlightAll();
	}
}

void MainBook::DelAllBreakpointMarkers()
{
	std::vector<LEditor*> editors;
	GetAllEditors(editors);
	for (size_t i = 0; i < editors.size(); i++) {
		editors[i]->DelAllBreakpointMarkers();
	}
}

void MainBook::SetViewEOL(bool visible)
{
	std::vector<LEditor*> editors;
	GetAllEditors(editors);
	for (size_t i = 0; i < editors.size(); i++) {
		editors[i]->SetViewEOL(visible);
	}
}

void MainBook::HighlightWord(bool hl)
{
	std::vector<LEditor*> editors;
	GetAllEditors(editors);
	for (size_t i = 0; i < editors.size(); i++) {
		editors[i]->HighlightWord(hl);
	}
}

void MainBook::ShowWhitespace(int ws)
{
	std::vector<LEditor*> editors;
	GetAllEditors(editors);
	for (size_t i = 0; i < editors.size(); i++) {
		editors[i]->SetViewWhiteSpace(ws);
	}
}

void MainBook::UpdateColours()
{
	std::vector<LEditor*> editors;
	GetAllEditors(editors);
	for (size_t i = 0; i < editors.size(); i++) {
		editors[i]->UpdateColours();
	}
}

void MainBook::UpdateBreakpoints()
{
	std::vector<LEditor*> editors;
	GetAllEditors(editors);
	for (size_t i = 0; i < editors.size(); i++) {
		editors[i]->UpdateBreakpoints();
	}
}
