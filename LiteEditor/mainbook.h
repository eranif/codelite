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

#include <wx/panel.h>
#include "sessionmanager.h"
#include "navbar.h"
#include "quickfindbar.h"
#include "custom_notebook.h"

class MainBook : public wxPanel
{
private:
	NavBar       *m_navBar;
	Notebook     *m_book;
	QuickFindBar *m_quickFindBar;
	
    void CreateGuiControls();
    void ConnectEvents    ();

    void OnMouseDClick       (wxMouseEvent   &e);
    void OnPageChanging      (NotebookEvent  &e);
    void OnPageChanged       (NotebookEvent  &e);
    void OnPageClosing       (NotebookEvent  &e);
    void OnPageClosed        (NotebookEvent  &e);
    void OnProjectFileAdded  (wxCommandEvent &e);
	void OnProjectFileRemoved(wxCommandEvent &e);
    void OnWorkspaceLoaded   (wxCommandEvent &e);
    void OnWorkspaceClosed   (wxCommandEvent &e);
    
	bool AskUserToSave(LEditor *editor);
	
public:
	MainBook(wxWindow *parent);
	~MainBook();
    
	void ShowQuickBar (bool s = true)       { m_quickFindBar->DoShow(s); }
	void ShowNavBar   (bool s = true);
	void UpdateNavBar (LEditor *editor);
	bool IsNavBarShown()                    { return m_navBar->IsShown(); }

    void SaveSession   (SessionEntry &session);
    void RestoreSession(SessionEntry &session);
     
    LEditor *GetActiveEditor();
	LEditor *FindEditor     (const wxString &fileName);
    bool     CloseEditor    (const wxString &fileName) { return ClosePage(FindEditor(fileName)); }
    
    wxWindow *GetCurrentPage();
    wxWindow *FindPage      (const wxString &text);
    
    LEditor *NewEditor();
    
	LEditor *OpenFile(const wxString &file_name, const wxString &projectName = wxEmptyString,
	                  int lineno = wxNOT_FOUND, long position = wxNOT_FOUND);
	LEditor *OpenFile(const BrowseRecord &rec)
        { return OpenFile(rec.filename, rec.project, rec.lineno, rec.position); }

    bool AddPage   (wxWindow *win, const wxString &text, const wxBitmap &bmp = wxNullBitmap, bool selected = false);
    bool SelectPage(wxWindow *win);

	bool SaveAll(bool askUser, bool includeUntitled);
    
    void ReloadExternallyModified();

    bool ClosePage      (const wxString &text) { return ClosePage(FindPage(text)); }
    bool ClosePage      (wxWindow *win);
	void CloseAllButThis(wxWindow *win);
	void CloseAll       ();

	wxString GetPageTitle(wxWindow *win);
	void     SetPageTitle(wxWindow *page, const wxString &name);
    
    // TODO: replace these functions with event handlers
	void ApplySettingsChanges   ();
    void UnHighlightAll         ();
    void DelAllBreakpointMarkers();
    void SetViewEOL             (bool visible);
    void HighlightWord          (bool hl);
    void ShowWhitespace         (int ws);
    void UpdateColours          ();
    void UpdateBreakpoints      ();
};

#endif //MAINBOOK_H
