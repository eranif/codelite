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
#include "ctags_manager.h"
#include "frame.h"
#include "manager.h"
#include "close_all_dlg.h"
#include "filechecklist.h"
#include "editor_config.h"
#include "mainbook.h"
#include <wx/aui/auibook.h>

MainBook::MainBook(wxWindow *parent)
		: wxPanel       (parent)
		, m_navBar      (NULL)
		, m_book        (NULL)
		, m_quickFindBar(NULL)
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
	m_book = new clAuiBook(this);

	m_book->SetRightClickMenu(wxXmlResource::Get()->LoadMenu(wxT("editor_tab_right_click")));
	sz->Add(m_book, 1, wxEXPAND);

	m_quickFindBar = new QuickFindBar(this);
	sz->Add(m_quickFindBar, 0, wxTOP|wxBOTTOM|wxEXPAND);

	sz->Layout();
}

void MainBook::ConnectEvents()
{
	m_book->Connect(wxEVT_COMMAND_AUINOTEBOOK_BG_DCLICK, wxAuiNotebookEventHandler(MainBook::OnMouseDClick), NULL, this);

	m_book->Connect(wxEVT_COMMAND_AUINOTEBOOK_PAGE_CLOSE,  wxAuiNotebookEventHandler(MainBook::OnPageClosing),  NULL, this);
	m_book->Connect(wxEVT_COMMAND_AUINOTEBOOK_PAGE_CLOSED, wxAuiNotebookEventHandler(MainBook::OnPageClosed),   NULL, this);

	wxTheApp->Connect(wxEVT_WORKSPACE_LOADED,  wxCommandEventHandler(MainBook::OnWorkspaceLoaded),    NULL, this);
	wxTheApp->Connect(wxEVT_PROJ_FILE_ADDED,   wxCommandEventHandler(MainBook::OnProjectFileAdded),   NULL, this);
	wxTheApp->Connect(wxEVT_PROJ_FILE_REMOVED, wxCommandEventHandler(MainBook::OnProjectFileRemoved), NULL, this);
	wxTheApp->Connect(wxEVT_WORKSPACE_CLOSED,  wxCommandEventHandler(MainBook::OnWorkspaceClosed),    NULL, this);

}

MainBook::~MainBook()
{
}

void MainBook::OnMouseDClick(wxAuiNotebookEvent& e)
{
	wxUnusedVar(e);
	NewEditor();
}

void MainBook::OnFocus(wxFocusEvent &e)
{
	// we use here DoSelectPage so the the selection wont change
	if (!SelectPage(dynamic_cast<wxWindow*>(e.GetEventObject()))) {
		e.Skip();
	}
	m_quickFindBar->SetEditor(GetActiveEditor());
}

void MainBook::OnPageClosing(wxAuiNotebookEvent &e)
{
	int sel = m_book->GetSelection();
	if ( sel == wxNOT_FOUND ) {
		return;
	}

	LEditor *editor = dynamic_cast<LEditor*>(m_book->GetPage(e.GetSelection()));
	if (!editor) {
		; // the page is not an editor
	} else if (AskUserToSave(editor)) {
		SendCmdEvent(wxEVT_EDITOR_CLOSING, (IEditor*)editor);
	} else {
		e.Veto();
	}
}

void MainBook::OnPageClosed(wxAuiNotebookEvent &e)
{
	int sel = m_book->GetSelection();
	if ( sel == wxNOT_FOUND ) {
		return;
	}

	SelectPage(m_book->GetPage((size_t)sel));
	m_quickFindBar->SetEditor(GetActiveEditor());

	// any editors left open?
	LEditor *editor = NULL;
	for (size_t i = 0; i < m_book->GetPageCount() && editor == NULL; i++) {
		editor = dynamic_cast<LEditor*>(m_book->GetPage(i));
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

void MainBook::ClearFileHistory()
{
	size_t count = m_recentFiles.GetCount();
	for ( size_t i=0; i<count; i++ ) {
		m_recentFiles.RemoveFileFromHistory ( 0 );
	}
	wxArrayString files;
	EditorConfigST::Get()->SetRecentlyOpenedFies ( files );
}

void MainBook::GetRecentlyOpenedFiles ( wxArrayString &files )
{
	EditorConfigST::Get()->GetRecentlyOpenedFies ( files );
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
	wxWindow *win = GetCurrentPage();
	if ( win ) {
		return dynamic_cast<LEditor*>( win );
	}
	return NULL;
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
}

LEditor *MainBook::FindEditor(const wxString &fileName)
{
	for (size_t i = 0; i < m_book->GetPageCount(); i++) {
		LEditor *editor = dynamic_cast<LEditor*>(m_book->GetPage(i));
		if (editor && editor->GetFileName().GetFullPath().CmpNoCase(fileName) == 0)
			return editor;
	}
	return NULL;
}

wxWindow *MainBook::FindPage(const wxString &text)
{
	for (size_t i = 0; i < m_book->GetPageCount(); i++) {
		LEditor *editor = dynamic_cast<LEditor*>(m_book->GetPage(i));
		if (editor && editor->GetFileName().GetFullPath().CmpNoCase(text) == 0) {
			return editor;
		}

		if (m_book->GetPageText(i) == text)
			return m_book->GetPage(i);
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
	editor->SetActive();
	return editor;
}

LEditor *MainBook::OpenFile(const wxString &file_name, const wxString &projectName, int lineno, long position, bool addjump)
{
	wxFileName fileName(file_name);
	fileName.MakeAbsolute();

	wxString projName = projectName;
	if (projName.IsEmpty()) {
		// try to match a project name to the file. otherwise, CC may not work
		projName = ManagerST::Get()->GetProjectNameByFile(fileName.GetFullPath());
	}

	LEditor* editor = GetActiveEditor();
	BrowseRecord jumpfrom = editor ? editor->CreateBrowseRecord() : BrowseRecord();

	editor = FindEditor(fileName.GetFullPath());
	if (editor) {
		editor->SetProject(projName);
	} else if (fileName.IsOk() == false) {
		wxLogMessage(wxT("Invalid file name: ") + fileName.GetFullPath());
		return NULL;
	} else if (!fileName.FileExists()) {
		wxLogMessage(wxT("File: ") + fileName.GetFullPath() + wxT(" does not exist!"));
		return NULL;
	} else {
		editor = new LEditor(m_book);
		editor->Create(projName, fileName);
		AddPage(editor, fileName.GetFullName());

		// mark the editor as read only if needed
		MarkEditorReadOnly(editor, IsFileReadOnly(editor->GetFileName()));

		if (position == wxNOT_FOUND && lineno == wxNOT_FOUND && editor->GetContext()->GetName() == wxT("C++")) {
			// try to find something interesting in the file to put the caret at
			// for now, just skip past initial blank lines and comments
			for (lineno = 0; lineno < editor->GetLineCount(); lineno++) {
				switch (editor->GetStyleAt(editor->PositionFromLine(lineno))) {
				case wxSCI_C_DEFAULT:
				case wxSCI_C_COMMENT:
				case wxSCI_C_COMMENTDOC:
				case wxSCI_C_COMMENTLINE:
				case wxSCI_C_COMMENTLINEDOC:
					continue;
				}
				// if we got here, it's a line to stop on
				break;
			}
			if (lineno == editor->GetLineCount()) {
				lineno = 1; // makes sure a navigation record gets saved
			}
		}
	}

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

	// Add this file to the history. Don't check for uniqueness:
	// if it's already on the list, wxFileHistory will move it to the top
	// Also, sync between the history object and the configuration file
	m_recentFiles.AddFileToHistory ( fileName.GetFullPath() );
	wxArrayString files;
	m_recentFiles.GetFiles ( files );
	EditorConfigST::Get()->SetRecentlyOpenedFies ( files );

	if (addjump) {
		BrowseRecord jumpto = editor->CreateBrowseRecord();
		NavMgr::Get()->AddJump(jumpfrom, jumpto);
	}
	return editor;
}

bool MainBook::AddPage(wxWindow *win, const wxString &text, const wxBitmap &bmp, bool selected)
{
	if (m_book->GetPageIndex(win) != wxNOT_FOUND)
		return false;
	win->Connect(wxEVT_SET_FOCUS, wxFocusEventHandler(MainBook::OnFocus), NULL, this);

	m_book->AddPage(win, text, selected);
	return true;
}

bool MainBook::SelectPage(wxWindow *win)
{
	int index = m_book->GetPageIndex(win);
	if ( index == wxNOT_FOUND )
		return false;

	return DoSelectPage( win );
}

bool MainBook::UserSelectFiles(std::vector<std::pair<wxFileName,bool> > &files, const wxString &title,
                               const wxString &caption, bool cancellable)
{
	if (files.empty())
		return true;

	FileCheckList dlg(Frame::Get(), wxID_ANY, title);
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
	if ( !page ) {
		return false;
	}

	int pos = m_book->GetPageIndex(page);
	return pos != wxNOT_FOUND && m_book->DeletePage(pos);
}

bool MainBook::CloseAllButThis(wxWindow *page)
{
	// TODO: handle case where page is detached
	wxString text;
	int pos = m_book->GetPageIndex(page);
	if (pos != wxNOT_FOUND) {
		text = m_book->GetPageText(pos);
		m_book->RemovePage(pos);
	}
	bool res = CloseAll(true);
	if (pos != wxNOT_FOUND) {
		m_book->AddPage(page, text, true);
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

	while ( m_book->GetPageCount() > 0 ) {
		m_book->DeletePage(0);
	}
	Frame::Get()->SetFrameTitle(NULL);
	return true;
}

wxString MainBook::GetPageTitle(wxWindow *page)
{
	int selection = m_book->GetPageIndex(page);
	if (selection != wxNOT_FOUND)
		return m_book->GetPageText(selection);

	return wxEmptyString;
}

void MainBook::SetPageTitle ( wxWindow *page, const wxString &name )
{
	int selection = m_book->GetPageIndex(page);
	if (selection != wxNOT_FOUND) {
		m_book->SetPageText(selection, name);
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
	ManagerST::Get()->GetBreakpointsMgr()->RefreshBreakpointMarkers();
}

void MainBook::MarkEditorReadOnly(LEditor* editor, bool ro)
{
	if (!editor) {
		return;
	}

	for (size_t i = 0; i < m_book->GetPageCount(); i++) {
		if (editor == m_book->GetPage(i)) {
			m_book->SetPageBitmap(i, ro ? wxXmlResource::Get()->LoadBitmap(wxT("read_only")) : wxNullBitmap );
			break;
		}
	}
}

long MainBook::GetBookStyle()
{
	return 0;
}

bool MainBook::DoSelectPage(wxWindow* win)
{
	LEditor *editor = dynamic_cast<LEditor*>(win);
	if ( editor ) {
		editor->SetActive();
	}

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

wxWindow* MainBook::GetCurrentPage()
{
	if ( m_book->GetPageCount() == 0 ) {
		return NULL;
	}

	int sel = m_book->GetSelection();
	if(sel == wxNOT_FOUND) {
		return NULL;
	}
	return m_book->GetPage((size_t)sel);
}

// ----------------------------------------------------------------
// clAuiBook
// ----------------------------------------------------------------

clAuiBook::clAuiBook(wxWindow* win)
	: wxAuiNotebook(win, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_NB_DEFAULT_STYLE | wxAUI_NB_WINDOWLIST_BUTTON | wxNO_BORDER)
	, m_menu(NULL)
{
	Connect(wxEVT_COMMAND_AUINOTEBOOK_TAB_RIGHT_DOWN, wxAuiNotebookEventHandler(clAuiBook::OnRightDown), NULL, this);
}

clAuiBook::~clAuiBook()
{
}

void clAuiBook::SetRightClickMenu(wxMenu* menu)
{
	m_menu = menu;
}

void clAuiBook::OnRightDown(wxAuiNotebookEvent& e)
{
	if ( e.GetSelection() == wxNOT_FOUND ) {
		return;
	}

	// make sure that the clicked window is the selected one
	if ( e.GetSelection() != GetSelection() ){
		SetSelection(e.GetSelection());
	}

	PopupMenu( m_menu );
}
