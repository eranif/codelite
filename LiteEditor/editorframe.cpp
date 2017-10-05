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

#include "editorframe.h"
#include "cl_editor.h"
#include "plugin.h"
#include "event_notifier.h"
#include "quickfindbar.h"
#include "bookmark_manager.h"
#include <wx/xrc/xmlres.h>
#include "my_menu_bar.h"
#include "manager.h"
#include "mainbook.h"

wxDEFINE_EVENT(wxEVT_DETACHED_EDITOR_CLOSED, clCommandEvent);

EditorFrame::EditorFrame(wxWindow* parent, LEditor* editor)
    : EditorFrameBase(parent)
    , m_editor(editor)
{
    m_editor->Reparent(m_mainPanel);
    m_mainPanel->GetSizer()->Add(m_editor, 1, wxEXPAND | wxALL, 2);
    // Notebook::RemovePage hides the detached tab
    if(!m_editor->IsShown()) {
        m_editor->Show();
    }
    // Load the menubar from XRC and set this frame's menubar to it.
    wxMenuBar* mb = wxXmlResource::Get()->LoadMenuBar(wxT("main_menu"));

    // Under wxGTK < 2.9.4 we need this wrapper class to avoid warnings on ubuntu when codelite exits
    m_myMenuBar = new MyMenuBar();
    m_myMenuBar->Set(mb);
    SetMenuBar(mb);

    // Set a find control for this editor
    m_findBar = new QuickFindBar(m_mainPanel);
    m_findBar->SetEditor(m_editor);
    m_mainPanel->GetSizer()->Add(m_findBar, 0, wxEXPAND | wxALL, 2);
    m_findBar->Hide();
    m_toolbar->SetDropdownMenu(XRCID("toggle_bookmark"), BookmarkManager::Get().CreateBookmarksSubmenu(NULL));

    m_mainPanel->Layout();
    SetTitle(m_editor->GetFileName().GetFullPath());

    // Update the accelerator table for this frame
    ManagerST::Get()->UpdateMenuAccelerators(this);
    SetSize(600, 600);
    CentreOnScreen();
}

EditorFrame::~EditorFrame()
{
// this will make sure that the main menu bar's member m_widget is freed before the we enter wxMenuBar destructor
// see this wxWidgets bug report for more details:
//  http://trac.wxwidgets.org/ticket/14292
#if defined(__WXGTK__) && wxVERSION_NUMBER < 2904
    delete m_myMenuBar;
#endif

    clCommandEvent evntInternalClosed(wxEVT_DETACHED_EDITOR_CLOSED);
    evntInternalClosed.SetClientData((IEditor*)m_editor);
    evntInternalClosed.SetFileName(m_editor->GetFileName().GetFullPath());
    EventNotifier::Get()->ProcessEvent(evntInternalClosed);

    // Send the traditional plugin event notifying that this editor is about to be destroyed
    wxCommandEvent eventClose(wxEVT_EDITOR_CLOSING);
    eventClose.SetClientData((IEditor*)m_editor);
    EventNotifier::Get()->ProcessEvent(eventClose);

    m_editor = NULL;
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

bool EditorFrame::ConfirmClose() { return !(m_editor->IsModified() && !MainBook::AskUserToSave(m_editor)); }
