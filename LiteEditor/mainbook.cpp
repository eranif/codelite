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
#include <wx/wupdlock.h>
#include "manager.h"
#include "close_all_dlg.h"
#include "filechecklist.h"
#include "editor_config.h"
#include "mainbook.h"
#include "message_pane.h"

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

	long style = wxVB_TOP|wxVB_HAS_X|wxVB_MOUSE_MIDDLE_CLOSE_TAB|wxVB_PASS_FOCUS|wxVB_NODND;
	// load the notebook style from the configuration settings
	m_book = new Notebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, style);
	m_book->SetRightClickMenu(wxXmlResource::Get()->LoadMenu(wxT("editor_tab_right_click")));

	sz->Add(m_book, 1, wxEXPAND);

	m_quickFindBar = new QuickFindBar(this);
	sz->Add(m_quickFindBar, 0, wxTOP|wxBOTTOM|wxEXPAND);

	m_messagePane = new MessagePane(this);
	sz->Insert(0, m_messagePane, 0, wxALL|wxEXPAND, 5, NULL);

	sz->Layout();
}

void MainBook::ConnectEvents()
{
	m_book->Connect(wxEVT_LEFT_DCLICK, wxMouseEventHandler(MainBook::OnMouseDClick), NULL, this);

	m_book->Connect(wxEVT_COMMAND_BOOK_PAGE_CLOSING,         NotebookEventHandler(MainBook::OnPageClosing),  NULL, this);
	m_book->Connect(wxEVT_COMMAND_BOOK_PAGE_CLOSED,          NotebookEventHandler(MainBook::OnPageClosed),   NULL, this);
	m_book->Connect(wxEVT_COMMAND_BOOK_PAGE_CHANGED,         NotebookEventHandler(MainBook::OnPageChanged),  NULL, this);
	m_book->Connect(wxEVT_COMMAND_BOOK_PAGE_X_CLICKED,       NotebookEventHandler(MainBook::OnClosePage),    NULL, this);
	m_book->Connect(wxEVT_COMMAND_BOOK_PAGE_MIDDLE_CLICKED,  NotebookEventHandler(MainBook::OnClosePage),    NULL, this);
	m_book->Connect(wxEVT_COMMAND_BOOK_SWAP_PAGES,           NotebookEventHandler(MainBook::OnSwapPages),    NULL, this);

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
	int where = m_book->HitTest(e.GetPosition());
	if(where == wxNOT_FOUND) {

		// This seems to be working only under Mac/Linux...
		NewEditor();

	} else {

		// Maximize the current editor
		wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, XRCID("toggle_panes"));
		evt.SetEventObject(this);
		Frame::Get()->GetEventHandler()->AddPendingEvent(evt);
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
	m_quickFindBar->SetEditor(GetActiveEditor());

	// any editors left open?
	LEditor *editor = NULL;
	for (size_t i = 0; i < m_book->GetPageCount() && editor == NULL; i++) {
		editor = dynamic_cast<LEditor*>(m_book->GetPage(i));
	}

	if (m_book->GetPageCount() == 0) {
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
	EditorConfigST::Get()->SetRecentItems( files, wxT("RecentFiles") );
}

void MainBook::GetRecentlyOpenedFiles ( wxArrayString &files )
{
	EditorConfigST::Get()->GetRecentItems( files, wxT("RecentFiles") );
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

void MainBook::SaveSession(SessionEntry &session, wxArrayInt& intArr)
{
	std::vector<LEditor*> editors;
	GetAllEditors(editors);

	session.SetSelectedTab(0);
	std::vector<TabInfo> vTabInfoArr;
	for (size_t i = 0; i < editors.size(); i++) {
		if ( (intArr.GetCount() > i) && (!intArr.Item(i)) ) {
			// If we're saving only selected editors, and this isn't one of them...
			continue;
		}
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
		editor->EnsureCaretVisible();

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
	if ( !GetCurrentPage() ) {
		return NULL;
	}
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
}

LEditor *MainBook::FindEditor(const wxString &fileName)
{
	wxString unixStyleFile(fileName);
#ifdef __WXMSW__
	unixStyleFile.Replace(wxT("\\"), wxT("/"));
#endif

	for (size_t i = 0; i < m_book->GetPageCount(); i++) {
		LEditor *editor = dynamic_cast<LEditor*>(m_book->GetPage(i));
		if (editor) {
			wxString unixStyleFile(editor->GetFileName().GetFullPath());
			wxString nativeFile   (editor->GetFileName().GetFullPath());
#ifdef __WXMSW__
			unixStyleFile.Replace(wxT("\\"), wxT("/"));
#endif
			if(nativeFile.CmpNoCase(fileName) == 0 || unixStyleFile.CmpNoCase(fileName) == 0) {
				return editor;
			}
		}
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

static bool IsFileExists(const wxFileName &filename) {
#ifdef __WXMSW__
	wxString drive  = filename.GetVolume();
	if(drive.Length()>1)
		return false;

	return filename.FileExists();
#else
	return filename.FileExists();
#endif
}

LEditor *MainBook::OpenFile(const wxString &file_name, const wxString &projectName, int lineno, long position, bool addjump)
{
	wxWindowUpdateLocker locker(this);

	wxFileName fileName(file_name);
	fileName.MakeAbsolute();

	if(IsFileExists(fileName) == false) {
		wxLogMessage(wxT("Failed to open: %s: No such file or directory"), fileName.GetFullPath().c_str());
		return NULL;
	}

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
		editor->SetSyntaxHighlight();

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
		editor->EnsureVisible(lineno);
	}
	editor->EnsureCaretVisible();
	if (GetActiveEditor() == editor) {
		editor->SetActive();
	} else {
		SelectPage(editor);
	}

#ifdef __WXGTK__
	editor->ScrollToColumn(0);
#endif

	// Add this file to the history. Don't check for uniqueness:
	// if it's already on the list, wxFileHistory will move it to the top
	// Also, sync between the history object and the configuration file
	m_recentFiles.AddFileToHistory ( fileName.GetFullPath() );
	wxArrayString files;
	m_recentFiles.GetFiles ( files );
	EditorConfigST::Get()->SetRecentItems( files, wxT("RecentFiles") );

	if (addjump) {
		BrowseRecord jumpto = editor->CreateBrowseRecord();
		NavMgr::Get()->AddJump(jumpfrom, jumpto);
	}
	return editor;
}

bool MainBook::AddPage(wxWindow *win, const wxString &text, const wxBitmap &bmp, bool selected)
{
	if (m_book->GetPageIndex(win) != Notebook::npos)
		return false;

	long MaxBuffers(15);
	EditorConfigST::Get()->GetLongValue(wxT("MaxOpenedTabs"), MaxBuffers);

	wxWindowUpdateLocker locker(m_book);

	if( (long)(m_book->GetPageCount()) >= MaxBuffers ) {
		// We have reached the limit of the number of open buffers
		// Close the last used buffer
		const wxArrayPtrVoid &arr = m_book->GetHistory();
		if ( arr.GetCount() ) {
			// We got at least one page, close the last used
			wxWindow *tab = static_cast<wxWindow*>(arr.Item(arr.GetCount()-1));
			ClosePage(tab);
		}
	}

	m_book->AddPage(win, text, selected);
	return true;
}

bool MainBook::SelectPage(wxWindow *win)
{
	size_t index = m_book->GetPageIndex(win);
	if (index != Notebook::npos && m_book->GetSelection() != index) {
		m_book->SetSelection(index);
	}

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

void MainBook::ReloadExternallyModified(bool prompt)
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

	if(prompt) {
		UserSelectFiles(files, wxT("Reload Modified Files"), wxT("Files have been modified outside the editor.\nChoose which files you would like to reload."), false);
	}

	std::vector<wxFileName> filesToRetag;
	for (size_t i = 0; i < files.size(); i++) {
		if (files[i].second) {
			editors[i]->ReloadFile();
			filesToRetag.push_back(files[i].first);
		}
	}
	if (filesToRetag.size() > 1) {
		TagsManagerST::Get()->RetagFiles(filesToRetag, true);
		SendCmdEvent(wxEVT_FILE_RETAGGED, (void*)&filesToRetag);

	} else if (filesToRetag.size() == 1) {
		ManagerST::Get()->RetagFile(filesToRetag.at(0).GetFullPath());
		SendCmdEvent(wxEVT_FILE_RETAGGED, (void*)&filesToRetag);

	}
}

bool MainBook::ClosePage(wxWindow *page)
{
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
//		LEditor *editor = dynamic_cast<LEditor*>(page);
//		m_book->AddPage(page, text, editor ? editor->GetFileName().GetFullPath() : text, wxNullBitmap, true);
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

	m_book->DeleteAllPages(ManagerST::Get()->IsShutdownInProgress() ? false : true);
	return true;
}

wxString MainBook::GetPageTitle(wxWindow *page)
{
	size_t selection = m_book->GetPageIndex(page);
	if (selection != Notebook::npos)
		return m_book->GetPageText(selection);
	return wxEmptyString;
}

void MainBook::SetPageTitle ( wxWindow *page, const wxString &name )
{
	size_t selection = m_book->GetPageIndex(page);
	if (selection != Notebook::npos) {
		//LEditor *editor = dynamic_cast<LEditor*>(page);
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
//			m_book->SetPageBitmap(i, ro ? wxXmlResource::Get()->LoadBitmap(wxT("read_only")) : wxNullBitmap );

			// TODO ::  Mark this editor as read only
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
		m_quickFindBar->SetEditor( editor );
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
		Frame::Get()->SetStatusMessage(wxEmptyString, 1); // clear line & column indicator
		UpdateNavBar(NULL);
	} else {
		if (editor->GetContext()->GetName() == wxT("C++")) {
			if (Frame::Get()->GetMenuBar()->FindMenu(wxT("C++")) == wxNOT_FOUND) {
				Frame::Get()->GetMenuBar()->Append(wxXmlResource::Get()->LoadMenu(wxT("editor_right_click")), wxT("C++"));
			}
		}
		SendCmdEvent(wxEVT_ACTIVE_EDITOR_CHANGED, (IEditor*)editor);
	}

	return true;
}

void MainBook::ShowMessage(const wxString &message, bool showHideButton, const wxBitmap &bmp, const ButtonDetails &btn1, const ButtonDetails &btn2, const ButtonDetails &btn3)
{
	m_messagePane->ShowMessage(message, showHideButton, bmp, btn1, btn2, btn3);
}

void MainBook::OnPageChanged(NotebookEvent& e)
{
	int newSel = e.GetSelection();
	if(newSel != wxNOT_FOUND) {
		LEditor *editor = dynamic_cast<LEditor*>(m_book->GetPage((size_t)newSel));
		if(editor) {
			SelectPage(editor);
		}
	}
	e.Skip();
}

wxWindow* MainBook::GetCurrentPage()
{
	return m_book->GetCurrentPage();
}


void MainBook::OnClosePage(NotebookEvent& e)
{
	wxWindowUpdateLocker locker( this );
	int where = e.GetSelection();
	if(where == wxNOT_FOUND) {
		return;
	}
	wxWindow *page = m_book->GetPage((size_t)where);
	if(page)
		ClosePage(page);
}

void MainBook::OnSwapPages(NotebookEvent& e)
{
	int startPos = e.GetOldSelection();
	int endPos   = e.GetSelection();

	// Sanity
	if(startPos < 0 || endPos < 0)
		return;

	wxWindowUpdateLocker locker( this );

	// We are dropping on another tab, remove the source tab from its current location, and place it
	// on the new location
	wxWindow *page  = m_book->GetPage     ((size_t)startPos);
	wxString  txt   = m_book->GetPageText ((size_t)startPos);
	int       imgId = m_book->GetPageImage((size_t)startPos);

	if(endPos > startPos) {

		// we are moving our tab to the right
		m_book->RemovePage(startPos, false);

		if((size_t)endPos == m_book->GetPageCount()) {
			m_book->AddPage(page, txt, true, imgId);
		} else {
			m_book->InsertPage((size_t)endPos, page, txt, true, imgId);
		}

	} else {

		// we are moving our tab to the right
		m_book->RemovePage((size_t)startPos, false);
		m_book->InsertPage((size_t)endPos, page, txt, true, imgId);

	}
}
