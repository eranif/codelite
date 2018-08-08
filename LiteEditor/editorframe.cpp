//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : editorframe.cpp
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

#include "bookmark_manager.h"
#include "cl_editor.h"
#include "editorframe.h"
#include "event_notifier.h"
#include "mainbook.h"
#include "manager.h"
#include "my_menu_bar.h"
#include "plugin.h"
#include "quickfindbar.h"
#include <wx/msgdlg.h>
#include <wx/xrc/xmlres.h>

wxDEFINE_EVENT(wxEVT_DETACHED_EDITOR_CLOSED, clCommandEvent);

EditorFrame::EditorFrame(wxWindow* parent, clEditor* editor, size_t notebookStyle)
    : EditorFrameBase(parent)
    , m_myMenuBar(NULL)
{
    m_book = new Notebook(m_mainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                          notebookStyle | kNotebook_AllowForeignDnD);
    m_book->Bind(wxEVT_BOOK_PAGE_CLOSED, &EditorFrame::OnPageClosed, this);
    m_book->Bind(wxEVT_BOOK_PAGE_CLOSING, &EditorFrame::OnPageClosing, this);

    m_mainPanel->GetSizer()->Add(m_book, 1, wxEXPAND);
    editor->Reparent(m_book);
    m_book->AddPage(editor, editor->GetFileName().GetFullName(), true);
    // Notebook::RemovePage hides the detached tab
    if(!editor->IsShown()) { editor->Show(); }
    
    // Set a find control for this editor
    m_findBar = new QuickFindBar(m_mainPanel);
    m_findBar->SetEditor(editor);
    m_mainPanel->GetSizer()->Add(m_findBar, 0, wxEXPAND | wxALL, 2);
    m_findBar->Hide();
    m_toolbar->SetDropdownMenu(XRCID("toggle_bookmark"), BookmarkManager::Get().CreateBookmarksSubmenu(NULL));

    m_mainPanel->Layout();
    SetTitle(editor->GetFileName().GetFullPath());

    // Update the accelerator table for this frame
    //ManagerST::Get()->UpdateMenuAccelerators(this);
    SetSize(600, 600);
    CentreOnScreen();
}

EditorFrame::~EditorFrame()
{
// this will make sure that the main menu bar's member m_widget is freed before the we enter wxMenuBar destructor
// see this wxWidgets bug report for more details:
//  http://trac.wxwidgets.org/ticket/14292
#if defined(__WXGTK__) && wxVERSION_NUMBER < 2904
    wxDELETE(m_myMenuBar);
#endif

    for(size_t i = 0; i < m_book->GetPageCount(); ++i) {
        clEditor* editor = dynamic_cast<clEditor*>(m_book->GetPage(i));
        if(!editor) continue;
        DoCloseEditor(editor);
    }
}

void EditorFrame::OnClose(wxCommandEvent& event)
{
    wxUnusedVar(event);
    Destroy();
}

void EditorFrame::OnCloseUI(wxUpdateUIEvent& event) { event.Enable(true); }

void EditorFrame::OnFind(wxCommandEvent& event)
{
    if(!m_findBar->IsShown()) {
        m_findBar->Show();
        m_mainPanel->GetSizer()->Layout();
    }
}

void EditorFrame::OnFindUI(wxUpdateUIEvent& event) { event.Enable(true); }

void EditorFrame::OnCloseWindow(wxCloseEvent& event) { event.Skip(); }

void EditorFrame::GetAllEditors(std::vector<clEditor*>& editors)
{
    for(size_t i = 0; i < m_book->GetPageCount(); ++i) {
        clEditor* editor = dynamic_cast<clEditor*>(m_book->GetPage(i));
        if(!editor) continue;
        editors.push_back(editor);
    }
}

clEditor* EditorFrame::GetEditor() const { return GetEditor(m_book->GetSelection()); }

void EditorFrame::OnPageClosed(wxBookCtrlEvent& event)
{
    event.Skip();
    if(m_book->GetPageCount() == 0) {
        Close();
        m_findBar->SetEditor(NULL);
    } else {
        clEditor* editor = reinterpret_cast<clEditor*>(m_book->GetCurrentPage());
        if(editor) {
            m_findBar->SetEditor(editor->GetCtrl());
        } else {
            m_findBar->SetEditor(NULL);
        }
    }
}

void EditorFrame::OnPageClosing(wxBookCtrlEvent& event)
{
    // Are we closing a modified editor?
    clEditor* editor = GetEditor(event.GetSelection());
    if(!editor) return;

    event.Skip();
    if(editor->IsModified()) {
        // Ask the user
        if(::wxMessageBox(wxString() << editor->GetFileName().GetFullName() << " is modified, continue?", "CodeLite",
                          wxICON_WARNING | wxYES_NO | wxCANCEL | wxCANCEL_DEFAULT, this) != wxYES) {
            event.Skip(false);
            event.Veto();
            return;
        }
    }

    // Perform the cleanup needed before the page is destroyed
    DoCloseEditor(editor);
}

clEditor* EditorFrame::GetEditor(size_t index) const
{
    if(index >= m_book->GetPageCount()) return nullptr;
    return dynamic_cast<clEditor*>(m_book->GetPage(index));
}

void EditorFrame::DoCloseEditor(clEditor* editor)
{
    clCommandEvent evntInternalClosed(wxEVT_DETACHED_EDITOR_CLOSED);
    evntInternalClosed.SetClientData((IEditor*)editor);
    evntInternalClosed.SetFileName(editor->GetFileName().GetFullPath());
    EventNotifier::Get()->ProcessEvent(evntInternalClosed);

    // Send the traditional plugin event notifying that this editor is about to be destroyed
    wxCommandEvent eventClose(wxEVT_EDITOR_CLOSING);
    eventClose.SetClientData((IEditor*)editor);
    EventNotifier::Get()->ProcessEvent(eventClose);
}
