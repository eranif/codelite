//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : fileexplorer.cpp
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
#include "wx/xrc/xmlres.h"
#include "fileexplorer.h"
#include "wx/sizer.h"
#include "wx/tokenzr.h"
#include "event_notifier.h"

#include "macros.h"
#include "globals.h"
#include "plugin.h"
#include "editor_config.h"
#include "manager.h"
#include "workspace_pane.h"
#include "frame.h"
#include "FileExplorerTab.h"
#include "file_logger.h"

FileExplorer::FileExplorer(wxWindow *parent, const wxString &caption)
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(250, 300))
    , m_caption(caption)
    , m_isLinkedToEditor(false)
{
    long link(0);
    EditorConfigST::Get()->GetLongValue(wxT("LinkFileExplorerToEditor"), link);
    m_isLinkedToEditor = link ? true : false;
    CreateGUIControls();
    m_themeHelper = new ThemeHandlerHelper(this);
}

FileExplorer::~FileExplorer()
{
    wxDELETE(m_themeHelper);
    wxTheApp->Disconnect(XRCID("show_in_explorer"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(FileExplorer::OnShowFile), NULL, this);
    wxTheApp->Disconnect(XRCID("show_in_explorer"), wxEVT_UPDATE_UI, wxUpdateUIEventHandler(FileExplorer::OnShowFileUI), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_WORKSPACE_LOADED, wxCommandEventHandler(FileExplorer::OnWorkspaceLoaded), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_ACTIVE_EDITOR_CHANGED, wxCommandEventHandler(FileExplorer::OnActiveEditorChanged), NULL, this);
}

void FileExplorer::CreateGUIControls()
{
    wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(mainSizer);

    wxToolBar *tb = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT|wxTB_HORIZONTAL|wxTB_NODIVIDER);
    mainSizer->Add(tb, 0, wxEXPAND);
    
    m_fileTree = new FileExplorerTab(this);
    mainSizer->Add(m_fileTree, 1, wxEXPAND|wxALL, 1);

    tb->AddTool(XRCID("link_editor"), wxEmptyString, wxXmlResource::Get()->LoadBitmap(wxT("link_editor")), _("Link Editor"), wxITEM_CHECK);
    tb->ToggleTool(XRCID("link_editor"), m_isLinkedToEditor);
    tb->AddTool(XRCID("collapse_all"), wxEmptyString, wxXmlResource::Get()->LoadBitmap(wxT("collapse")), _("Collapse All"), wxITEM_NORMAL);
    tb->AddTool(XRCID("go_home"), wxEmptyString, wxXmlResource::Get()->LoadBitmap(wxT("gohome")), _("Goto Current Directory"), wxITEM_NORMAL);
    tb->Realize();

    Connect( XRCID("link_editor"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( FileExplorer::OnLinkEditor ));
    Connect( XRCID("collapse_all"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( FileExplorer::OnCollapseAll ));
    Connect( XRCID("go_home"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( FileExplorer::OnGoHome ));

    mainSizer->Layout();

    wxTheApp->Connect(XRCID("show_in_explorer"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(FileExplorer::OnShowFile), NULL, this);
    wxTheApp->Connect(XRCID("show_in_explorer"), wxEVT_UPDATE_UI, wxUpdateUIEventHandler(FileExplorer::OnShowFileUI), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_WORKSPACE_LOADED, wxCommandEventHandler(FileExplorer::OnWorkspaceLoaded), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_ACTIVE_EDITOR_CHANGED, wxCommandEventHandler(FileExplorer::OnActiveEditorChanged), NULL, this);
}

void FileExplorer::OnCollapseAll(wxCommandEvent &e)
{
    wxUnusedVar(e);
    m_fileTree->Tree()->ReCreateTree();
}

void FileExplorer::OnGoHome(wxCommandEvent &e)
{
    wxUnusedVar(e);
    m_fileTree->ClearSelections();
    m_fileTree->Tree()->ExpandPath( ::wxGetCwd() );
}

void FileExplorer::OnLinkEditor(wxCommandEvent &e)
{
    m_isLinkedToEditor = !m_isLinkedToEditor;
    EditorConfigST::Get()->SaveLongValue(wxT("LinkFileExplorerToEditor"), m_isLinkedToEditor ? 1 : 0);
    if (m_isLinkedToEditor) {
        OnActiveEditorChanged(e);
    }
}

void FileExplorer::OnShowFile(wxCommandEvent& e)
{
    LEditor *editor = clMainFrame::Get()->GetMainBook()->GetActiveEditor();
    if (editor && editor->GetFileName().FileExists()) {
        m_fileTree->ClearSelections();
        m_fileTree->Tree()->ExpandPath(editor->GetFileName().GetFullPath());
        ManagerST::Get()->ShowWorkspacePane(m_caption);
    }
    e.Skip();
}

void FileExplorer::OnShowFileUI(wxUpdateUIEvent& e)
{
    LEditor *editor = clMainFrame::Get()->GetMainBook()->GetActiveEditor();
    e.Enable(editor && editor->GetFileName().FileExists());
}

void FileExplorer::OnActiveEditorChanged(wxCommandEvent& e)
{
    e.Skip();
    if (m_isLinkedToEditor) {
        LEditor *editor = clMainFrame::Get()->GetMainBook()->GetActiveEditor();
        if (editor && editor->GetFileName().FileExists()) {
            m_fileTree->ClearSelections();
CL_DEBUG1(" ===> [Explorer] Expand to path for " + editor->GetFileName().GetFullPath() );
            m_fileTree->Tree()->ExpandPath(editor->GetFileName().GetFullPath());
CL_DEBUG1(" <=== [Explorer] Expand to path for " + editor->GetFileName().GetFullPath() );
        }
    }
}

void FileExplorer::OnWorkspaceLoaded(wxCommandEvent& e)
{
    e.Skip();
    wxUnusedVar(e);
    if (m_isLinkedToEditor) {
        m_fileTree->Tree()->ExpandPath( wxGetCwd() );
    }
}
