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
#include "mainbook.h"

MainBook::MainBook(wxWindow *parent)
    : wxPanel( parent )
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
    
	m_book->Connect(wxEVT_COMMAND_BOOK_PAGE_CHANGING, NotebookEventHandler(MainBook::OnPageChanging), NULL, this);
	m_book->Connect(wxEVT_COMMAND_BOOK_PAGE_CHANGED,  NotebookEventHandler(MainBook::OnPageChanged),  NULL, this);
	m_book->Connect(wxEVT_COMMAND_BOOK_PAGE_CLOSING,  NotebookEventHandler(MainBook::OnPageClosing),  NULL, this);
	m_book->Connect(wxEVT_COMMAND_BOOK_PAGE_CLOSED,   NotebookEventHandler(MainBook::OnPageClosed),   NULL, this);
    
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

void MainBook::OnPageChanging(NotebookEvent &e)
{
	return;
}

void MainBook::OnPageChanged(NotebookEvent &e)
{
    LEditor *editor = GetActiveEditor();
    
    if (!editor) {
        Frame::Get()->RemoveCppMenu();
        Frame::Get()->SetFrameTitle(NULL);
        Frame::Get()->SetStatusMessage(wxEmptyString, 2); // clear line & column indicator
        Frame::Get()->SetStatusMessage(wxEmptyString, 3); // clear end-of-line mode indicator
        UpdateNavBar(NULL);
        ShowQuickBar(false);
        return;
    }
    
    if (editor->GetContext()->GetName() == wxT("C++")) {
        Frame::Get()->AddCppMenu();
    } else {
        Frame::Get()->RemoveCppMenu();
    }
    editor->SetActive();
    SendCmdEvent(wxEVT_ACTIVE_EDITOR_CHANGED, (IEditor*)editor);
}

void MainBook::OnPageClosing(NotebookEvent &e)
{
    LEditor *editor = dynamic_cast<LEditor*>(m_book->GetPage(e.GetSelection()));
    if (!editor)
        return;
	if (editor->GetModify()) {
		// unsaved changes
		wxString msg;
		msg << wxT("Save changes to '") << editor->GetFileName().GetFullName() << wxT("' ?");
		long style = wxYES_NO;
		if (!ManagerST::Get()->IsShutdownInProgress()) {
			style |= wxCANCEL;
		}
		int answer = wxMessageBox(msg, wxT("Confirm"), style);
		if (answer == wxCANCEL || answer == wxYES && !editor->SaveFile()) {
			e.Veto();
			return;
		}
	}
	SendCmdEvent(wxEVT_EDITOR_CLOSING, (IEditor*)editor);
}

void MainBook::OnPageClosed(NotebookEvent &e)
{
    if (GetCurrentPage() == NULL) {
        // we don't get a "page changed" event from notebook when the last page
        // is closed, so we'll simulate changing to a "NULL" page.  This will
        // cause the frame indicators to update correctly.
        OnPageChanged(e);
    }
    
    // any editors left open?
    LEditor *editor = NULL;
    for (size_t i = 0; i < m_book->GetPageCount() && editor == NULL; i++) {
        editor = dynamic_cast<LEditor*>(m_book->GetPage(i));
    }
    if (editor == NULL) {
        SendCmdEvent(wxEVT_ALL_EDITORS_CLOSED);
    }
}

void MainBook::OnWorkspaceLoaded(wxCommandEvent &e)
{
    e.Skip();
    CloseAll(); // get ready for session to be restored by clearing out existing pages
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

void MainBook::OnWorkspaceClosed(wxCommandEvent &e)
{
    e.Skip();
    CloseAll();
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
    session.SetSelectedTab(0);
	std::vector<TabInfo> vTabInfoArr;
	for (size_t i = 0; i < m_book->GetPageCount(); i++) {
		LEditor *editor = dynamic_cast<LEditor*>(m_book->GetPage(i));
        if (!editor)
            continue;
        if (editor == GetActiveEditor()) {
            session.SetSelectedTab(vTabInfoArr.size());
       }
        TabInfo oTabInfo;
        oTabInfo.SetFileName(editor->GetFileName().GetFullPath());
        oTabInfo.SetFirstVisibleLine(editor->GetFirstVisibleLine());
        oTabInfo.SetCurrentLine(editor->GetCurrentLine());
        wxArrayString astrBookmarks;
        for (int nLine = 0; (nLine = editor->MarkerNext(nLine, 128)) >= 0; nLine++) {
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
    m_book->SetSelection(sel);
}

LEditor *MainBook::GetActiveEditor()
{
    return dynamic_cast<LEditor*>(GetCurrentPage());
}

LEditor *MainBook::FindEditor(const wxString &fileName)
{
	for (size_t i = 0; i < m_book->GetPageCount(); i++) {
		LEditor *editor = dynamic_cast<LEditor*>(m_book->GetPage(i));
		if (editor && editor->GetFileName().GetFullPath() == fileName) 
			return editor;
	}
	return NULL;
}

wxWindow *MainBook::GetCurrentPage()
{
    return m_book->GetPage(m_book->GetSelection());
}

wxWindow *MainBook::FindPage(const wxString &text)
{
    for (size_t i = 0; i < m_book->GetPageCount(); i++) {
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
	m_book->AddPage(editor, fileName.GetFullName());    
	m_book->SetSelection(m_book->GetPageIndex(editor));
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
		m_book->AddPage(editor, fileName.GetFullName());
	}

    if (editor) {
       if (position != wxNOT_FOUND) {
            editor->SetCaretAt(position);
        } else if (lineno != wxNOT_FOUND) {
            editor->GotoLine(lineno);
        }
        editor->EnsureCaretVisible();
        m_book->SetSelection(m_book->GetPageIndex(editor));
        ManagerST::Get()->AddToRecentlyOpenedFiles(fileName.GetFullPath());
    }
    
	return editor;
}

bool MainBook::AddPage(wxWindow *win, const wxString &text, const wxBitmap &bmp, bool selected)
{
    if (m_book->GetPageIndex(win) != Notebook::npos)
        return false;
    m_book->AddPage(win, text, bmp, selected);
    return true;
}

bool MainBook::SelectPage(wxWindow *win)
{
    size_t index = m_book->GetPageIndex(win);
    if (index == Notebook::npos)
        return false;
    m_book->SetSelection(index);
    return true;
}

void MainBook::SaveAll(bool includeUntitled)
{
	for (size_t i = 0; i < m_book->GetPageCount(); i++) {
		LEditor* editor = dynamic_cast<LEditor*>(m_book->GetPage(i));
		if (!editor || !editor->GetModify())
			continue;
		if (!includeUntitled && editor->GetFileName().GetFullPath().StartsWith(wxT("Untitled")))
			continue; //don't save new documents that have not been saved to disk yet
        editor->SaveFile();
	}
}

void MainBook::ReloadExternallyModified()
{
    std::vector<wxFileName> filesToRetag;
	for (size_t i = 0; i < m_book->GetPageCount(); i++) {
		LEditor *editor = dynamic_cast<LEditor*>(m_book->GetPage(i));
		if (!editor)
            continue;
		time_t diskTime = editor->GetFileLastModifiedTime();
		time_t editTime = editor->GetEditorLastModifiedTime();
        if (diskTime <= editTime) 
            continue;
        
        // update time so that we don't keep bugging the user for this file, unless it changes again
        editor->SetEditorLastModifiedTime(diskTime);

        wxString msg;
        msg << wxT("The File '");
        msg << editor->GetFileName().GetFullName();
        msg << wxT("' was modified\n");
        msg << wxT("outside of the editor, would you like to reload it?");
        if (wxMessageBox(msg, wxT("Confirm"), wxYES_NO | wxCANCEL | wxICON_QUESTION) != wxYES)
            continue;
            
        editor->ReloadFile();
        if (!editor->GetProject().IsEmpty()) {
            filesToRetag.push_back(editor->GetFileName());
        }
	}
	if (filesToRetag.empty() == false) {
		TagsManagerST::Get()->RetagFiles(filesToRetag);
		SendCmdEvent(wxEVT_FILE_RETAGGED, (void*)&filesToRetag);
	}    
}

bool MainBook::ClosePage(wxWindow *page)
{
    size_t pos = m_book->GetPageIndex(page);
    return pos != Notebook::npos && m_book->DeletePage(pos);
}

void MainBook::CloseAllButThis(wxWindow *page)
{
	while (m_book->DeletePage(m_book->GetPage(0) == page ? 1 : 0)) {}
}

void MainBook::CloseAll()
{
    // determine if we need to ask user how to deal with unsaved files
    bool modifyDetected = false;
    for (size_t i=0; i < m_book->GetPageCount() && !modifyDetected; i++) {
        LEditor *editor = dynamic_cast<LEditor*>(m_book->GetPage(i));
        if (editor) {
            modifyDetected = editor->GetModify();
        }
    }
    int retCode = CLOSEALL_DISCARDALL;
    if (modifyDetected) {
        CloseAllDialog *dlg = new CloseAllDialog(Frame::Get());
        retCode = dlg->ShowModal();
        dlg->Destroy();
    }
    switch ( retCode ) {
        case CLOSEALL_SAVEALL: 
            SaveAll();
            break;
        case CLOSEALL_DISCARDALL:
            for (size_t i = 0; i < m_book->GetPageCount(); i++) {
                LEditor* editor = dynamic_cast<LEditor*>(m_book->GetPage(i));
                if (editor && editor->GetModify()) {
                    editor->SetSavePoint();
                }
            }
            break;
    }
    m_book->DeleteAllPages(true);
}

wxString MainBook::GetPageTitle(wxWindow *page)
{
    size_t selection = m_book->GetPageIndex(page);
    return selection != Notebook::npos ? m_book->GetPageText(selection) : wxString(wxEmptyString);
}

void MainBook::SetPageTitle ( wxWindow *page, const wxString &name )
{
    size_t selection = m_book->GetPageIndex(page);
    if (selection != Notebook::npos) {
        m_book->SetPageText(selection, name);
    }
}

void MainBook::ApplySettingsChanges()
{
    for (size_t i = 0; i < m_book->GetPageCount(); i++) {
        LEditor *editor = dynamic_cast<LEditor*>(m_book->GetPage(i));
        if (editor) {
            editor->SetSyntaxHighlight(editor->GetContext()->GetName());
        }
    }
}

void MainBook::UnHighlightAll()
{
    for (size_t i = 0; i < m_book->GetPageCount(); i++) {
        LEditor *editor = dynamic_cast<LEditor*>(m_book->GetPage(i));
        if (editor) {
            editor->UnHighlightAll();
        }
    }
}

void MainBook::DelAllBreakpointMarkers()
{
    for (size_t i = 0; i < m_book->GetPageCount(); i++) {
        LEditor *editor = dynamic_cast<LEditor*>(m_book->GetPage(i));
        if (editor) {
            editor->DelAllBreakpointMarkers();
        }
    }
}

void MainBook::SetViewEOL(bool visible)
{
    for (size_t i=0; i<GetNotebook()->GetPageCount(); i++) {
        LEditor *editor = dynamic_cast<LEditor*>(m_book->GetPage(i));
        if (editor) {
            editor->SetViewEOL(visible);
        }
    }
}

void MainBook::HighlightWord(bool hl)
{
    for (size_t i = 0; i < m_book->GetPageCount(); i++) {
        LEditor *editor = dynamic_cast<LEditor*>(m_book->GetPage(i));
        if (editor) {
            editor->HighlightWord(hl);
        }
    }
}

void MainBook::ShowWhitespace(int ws)
{
    for (size_t i = 0; i < m_book->GetPageCount(); i++) {
        LEditor *editor = dynamic_cast<LEditor*>(m_book->GetPage(i));
        if (editor) {
            editor->SetViewWhiteSpace(ws);
        }
    }    
}

void MainBook::UpdateColours()
{
    for (size_t i = 0; i < m_book->GetPageCount(); i++) {
        LEditor *editor = dynamic_cast<LEditor*>(m_book->GetPage(i));
        if (editor) {
            editor->UpdateColours();
        }
    }
}

void MainBook::UpdateBreakpoints()
{
    for (size_t i = 0; i < m_book->GetPageCount(); i++) {
        LEditor *editor = dynamic_cast<LEditor*>(m_book->GetPage(i));
        if (editor) {
            editor->UpdateBreakpoints();
        }
    }
}
