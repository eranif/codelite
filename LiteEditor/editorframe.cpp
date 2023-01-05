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

#include "bookmark_manager.h"
#include "cl_editor.h"
#include "event_notifier.h"
#include "frame.h"
#include "mainbook.h"
#include "manager.h"
#include "plugin.h"
#include "quickfindbar.h"

#include <wx/msgdlg.h>
#include <wx/xrc/xmlres.h>

wxDEFINE_EVENT(wxEVT_DETACHED_EDITOR_CLOSED, clCommandEvent);

EditorFrame::EditorFrame(wxWindow* parent, clEditor* editor, size_t notebookStyle)
    : EditorFrameBase(parent)
    , m_editor(editor)
{
    m_editor->Reparent(m_mainPanel);
    m_mainPanel->GetSizer()->Add(editor, 1, wxEXPAND);

    // Notebook::RemovePage hides the detached tab
    if(!editor->IsShown()) {
        editor->Show();
    }

    // Set a find control for this editor
    m_findBar = new QuickFindBar(m_mainPanel);
    m_findBar->SetEditor(editor);
    m_mainPanel->GetSizer()->Add(m_findBar, 0, wxEXPAND | wxALL, 2);
    m_findBar->Hide();
    m_toolbar->SetDropdownMenu(XRCID("toggle_bookmark"), BookmarkManager::Get().CreateBookmarksSubmenu(NULL));

    m_toolbar->SetMiniToolBar(false);
    auto images = m_toolbar->GetBitmapsCreateIfNeeded();
    m_toolbar->AddTool(XRCID("file-save"), _("Save"), images->Add("file_save"));
    m_toolbar->AddTool(XRCID("file-close"), _("Close"), images->Add("file_close"));
    m_toolbar->AddTool(XRCID("reload_file"), _("Reload"), images->Add("file_reload"));
    m_toolbar->AddTool(XRCID("show-find-bar"), _("Find"), images->Add("cscope"));
    m_toolbar->AddSpacer();
    m_toolbar->AddTool(wxID_UNDO, _("Undo"), images->Add("undo"));
    m_toolbar->AddTool(wxID_REDO, _("Redo"), images->Add("redo"));
    m_toolbar->Realize();

    m_toolbar->Bind(wxEVT_TOOL, &EditorFrame::OnSave, this, XRCID("file-save"));
    m_toolbar->Bind(wxEVT_UPDATE_UI, &EditorFrame::OnSaveUI, this, XRCID("file-save"));
    m_toolbar->Bind(wxEVT_TOOL, &EditorFrame::OnClose, this, XRCID("file-close"));
    m_toolbar->Bind(wxEVT_TOOL, &EditorFrame::OnEdit, this, wxID_UNDO);
    m_toolbar->Bind(wxEVT_TOOL, &EditorFrame::OnEdit, this, wxID_REDO);
    m_toolbar->Bind(wxEVT_TOOL, &EditorFrame::OnFind, this, XRCID("show-find-bar"));
    m_toolbar->Bind(wxEVT_TOOL, &EditorFrame::OnReload, this, XRCID("reload_file"));

    m_mainPanel->Layout();
    SetTitle(editor->GetFileName().GetFullPath());
    SetSize(600, 600);
    CentreOnScreen();
}

EditorFrame::~EditorFrame() { DoCloseEditor(m_editor); }

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

void EditorFrame::OnEdit(wxCommandEvent& event) { m_editor->OnMenuCommand(event); }

void EditorFrame::OnSave(wxCommandEvent& event)
{
    m_editor->SaveFile();
    SetTitle(m_editor->GetFileName().GetFullPath());
}

void EditorFrame::OnSaveUI(wxUpdateUIEvent& event) { event.Enable(m_editor && m_editor->GetModify()); }

void EditorFrame::OnUndo(wxCommandEvent& event) { m_editor->GetCtrl()->Undo(); }

void EditorFrame::OnRedo(wxCommandEvent& event) { m_editor->GetCtrl()->Redo(); }

void EditorFrame::OnUndoUI(wxUpdateUIEvent& event) { event.Enable(m_editor && m_editor->CanUndo()); }

void EditorFrame::OnRedoUI(wxUpdateUIEvent& event) { event.Enable(m_editor && m_editor->CanRedo()); }

void EditorFrame::OnReload(wxCommandEvent& event)
{
    wxUnusedVar(event);
    m_editor->ReloadFromDisk();
}
