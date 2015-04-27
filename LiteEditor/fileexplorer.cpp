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
#include "FileExplorerTabToolBar.h"
#include "cl_config.h"
#include "OpenFolderDlg.h"
#include "globals.h"
#include <wx/arrstr.h>

FileExplorer::FileExplorer(wxWindow* parent, const wxString& caption)
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(250, 300))
    , m_caption(caption)
    , m_isLinkedToEditor(false)
{
    long link = EditorConfigST::Get()->GetInteger(wxT("LinkFileExplorerToEditor"), 0);
    m_isLinkedToEditor = link ? true : false;
    CreateGUIControls();
    m_themeHelper = new ThemeHandlerHelper(this);
}

FileExplorer::~FileExplorer()
{
    wxDELETE(m_themeHelper);
    wxTheApp->Disconnect(XRCID("show_in_explorer"),
                         wxEVT_COMMAND_MENU_SELECTED,
                         wxCommandEventHandler(FileExplorer::OnShowFile),
                         NULL,
                         this);
    wxTheApp->Disconnect(
        XRCID("show_in_explorer"), wxEVT_UPDATE_UI, wxUpdateUIEventHandler(FileExplorer::OnShowFileUI), NULL, this);
    EventNotifier::Get()->Disconnect(
        wxEVT_WORKSPACE_LOADED, wxCommandEventHandler(FileExplorer::OnWorkspaceLoaded), NULL, this);
    EventNotifier::Get()->Disconnect(
        wxEVT_ACTIVE_EDITOR_CHANGED, wxCommandEventHandler(FileExplorer::OnActiveEditorChanged), NULL, this);
    Disconnect(XRCID("link_editor"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(FileExplorer::OnLinkEditor));
    Disconnect(XRCID("collapse_all"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(FileExplorer::OnCollapseAll));
    Disconnect(XRCID("go_home"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(FileExplorer::OnGoHome));
    Unbind(wxEVT_AUITOOLBAR_TOOL_DROPDOWN,
           &FileExplorer::OnBookmark,
           this,
           FileExplorerTabToolBar::ID_TOOL_EXPLORER_BOOKMARKS);
    Unbind(wxEVT_MENU, &FileExplorer::OnGotoFolder, this, FileExplorerTabToolBar::ID_TOOL_GOTO_FOLDER);
    Unbind(wxEVT_MENU, &FileExplorer::OnFindInFiles, this, FileExplorerTabToolBar::ID_TOOL_FIND_IN_FILES);
}

void FileExplorer::CreateGUIControls()
{
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(mainSizer);

    m_fileTree = new FileExplorerTab(this);
    FileExplorerTabToolBar* tb = new FileExplorerTabToolBar(this, m_fileTree);
    mainSizer->Add(tb, 0, wxEXPAND);
    mainSizer->Add(m_fileTree, 1, wxEXPAND | wxALL, 1);
    tb->ToggleTool(XRCID("link_editor"), m_isLinkedToEditor);

    Connect(XRCID("link_editor"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(FileExplorer::OnLinkEditor));
    Connect(XRCID("collapse_all"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(FileExplorer::OnCollapseAll));
    Connect(XRCID("go_home"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(FileExplorer::OnGoHome));
    Bind(wxEVT_AUITOOLBAR_TOOL_DROPDOWN,
         &FileExplorer::OnBookmark,
         this,
         FileExplorerTabToolBar::ID_TOOL_EXPLORER_BOOKMARKS);
    Bind(wxEVT_MENU, &FileExplorer::OnGotoFolder, this, FileExplorerTabToolBar::ID_TOOL_GOTO_FOLDER);
    Bind(wxEVT_MENU, &FileExplorer::OnFindInFiles, this, FileExplorerTabToolBar::ID_TOOL_FIND_IN_FILES);
    mainSizer->Layout();

    wxTheApp->Connect(XRCID("show_in_explorer"),
                      wxEVT_COMMAND_MENU_SELECTED,
                      wxCommandEventHandler(FileExplorer::OnShowFile),
                      NULL,
                      this);
    wxTheApp->Connect(
        XRCID("show_in_explorer"), wxEVT_UPDATE_UI, wxUpdateUIEventHandler(FileExplorer::OnShowFileUI), NULL, this);
    EventNotifier::Get()->Connect(
        wxEVT_WORKSPACE_LOADED, wxCommandEventHandler(FileExplorer::OnWorkspaceLoaded), NULL, this);
    EventNotifier::Get()->Connect(
        wxEVT_ACTIVE_EDITOR_CHANGED, wxCommandEventHandler(FileExplorer::OnActiveEditorChanged), NULL, this);
}

void FileExplorer::OnCollapseAll(wxCommandEvent& e)
{
    wxUnusedVar(e);
    m_fileTree->Tree()->ReCreateTree();
}

void FileExplorer::OnGoHome(wxCommandEvent& e)
{
    wxUnusedVar(e);
    m_fileTree->ClearSelections();
    m_fileTree->Tree()->ExpandPath(::wxGetCwd());
}

void FileExplorer::OnLinkEditor(wxCommandEvent& e)
{
    m_isLinkedToEditor = !m_isLinkedToEditor;
    EditorConfigST::Get()->SetInteger(wxT("LinkFileExplorerToEditor"), m_isLinkedToEditor ? 1 : 0);
    if(m_isLinkedToEditor) {
        OnActiveEditorChanged(e);
    }
}

void FileExplorer::OnShowFile(wxCommandEvent& e)
{
    LEditor* editor = clMainFrame::Get()->GetMainBook()->GetActiveEditor();
    if(editor && editor->GetFileName().FileExists()) {
        m_fileTree->ClearSelections();
        m_fileTree->Tree()->ExpandPath(editor->GetFileName().GetFullPath());
        ManagerST::Get()->ShowWorkspacePane(m_caption);
    }
    e.Skip();
}

void FileExplorer::OnShowFileUI(wxUpdateUIEvent& e)
{
    LEditor* editor = clMainFrame::Get()->GetMainBook()->GetActiveEditor();
    e.Enable(editor && editor->GetFileName().FileExists());
}

void FileExplorer::OnActiveEditorChanged(wxCommandEvent& e)
{
    e.Skip();
    if(m_isLinkedToEditor) {
        LEditor* editor = clMainFrame::Get()->GetMainBook()->GetActiveEditor();
        if(editor && editor->GetFileName().FileExists()) {
            m_fileTree->ClearSelections();
            CL_DEBUG1(" ===> [Explorer] Expand to path for " + editor->GetFileName().GetFullPath());
            m_fileTree->Tree()->ExpandPath(editor->GetFileName().GetFullPath());
            CL_DEBUG1(" <=== [Explorer] Expand to path for " + editor->GetFileName().GetFullPath());
        }
    }
}

void FileExplorer::OnWorkspaceLoaded(wxCommandEvent& e)
{
    e.Skip();
    wxUnusedVar(e);
    if(m_isLinkedToEditor) {
        m_fileTree->Tree()->ExpandPath(wxGetCwd());
    }
}

void FileExplorer::OnBookmark(wxAuiToolBarEvent& event)
{
    wxAuiToolBar* tb = dynamic_cast<wxAuiToolBar*>(event.GetEventObject());
    if(event.IsDropDownClicked()) {
        // Show the drop down menu
        wxArrayString folders = clConfig::Get().Read(kConfigFileExplorerBookmarks, wxArrayString());
        if(folders.IsEmpty()) return;
    
        // Remove non existing folders from the bookmark
        wxArrayString existingFolders;
        for(size_t i=0; i<folders.GetCount(); ++i) {
            if(wxFileName::DirExists(folders.Item(i))) {
                existingFolders.Add(folders.Item(i));
            }
        }
        
        // Update the list
        clConfig::Get().Write(kConfigFileExplorerBookmarks, folders);
        
        folders.swap(existingFolders);
        if(folders.IsEmpty()) return;
        
        std::map<int, wxString> entries;
        wxMenu menu;
        for(size_t i = 0; i < folders.GetCount(); ++i) {
            int id = ::wxNewId();
            entries.insert(std::make_pair(id, folders.Item(i)));
            menu.Append(id, folders.Item(i));
        }

        wxPoint pt = event.GetItemRect().GetBottomLeft();
        pt.y++;

        int sel = tb->GetPopupMenuSelectionFromUser(menu, pt);
        if(entries.count(sel)) {
            wxString path = entries.find(sel)->second;
            m_fileTree->ClearSelections();
            m_fileTree->GetGenericDirCtrl()->ExpandPath(path);
            m_fileTree->GetGenericDirCtrl()->SelectPath(path);
        }

    } else {
        wxArrayString paths;
        m_fileTree->GetSelectedDirectories(paths);
        if(paths.IsEmpty()) return;

        wxArrayString folders = clConfig::Get().Read(kConfigFileExplorerBookmarks, wxArrayString());
        for(size_t i = 0; i < paths.GetCount(); ++i) {
            if(folders.Index(paths.Item(i)) == wxNOT_FOUND) {
                folders.Add(paths.Item(i));
            }
        }
        folders.Sort();
        clConfig::Get().Write(kConfigFileExplorerBookmarks, folders);
    }
}

void FileExplorer::OnGotoFolder(wxCommandEvent& event)
{
    // Quickly jump to folder
    OpenFolderDlg dlg(EventNotifier::Get()->TopFrame());
    if(dlg.ShowModal() == wxID_OK) {
        wxString path = dlg.GetTextCtrlFolder()->GetValue();
        if(wxFileName::DirExists(path)) {
            m_fileTree->ClearSelections();
            m_fileTree->GetGenericDirCtrl()->ExpandPath(path);
            m_fileTree->GetGenericDirCtrl()->SelectPath(path);
        }
    }
}

void FileExplorer::OnFindInFiles(wxCommandEvent& event)
{
    wxArrayString folders, files;
    m_fileTree->GetSelections(folders, files);
    if(folders.IsEmpty() && files.IsEmpty()) return;
    
    // Prepare a folder list from both arrays
    for(size_t i=0; i<files.size(); ++i) {
        wxFileName fn(files.Item(i));
        if(folders.Index(fn.GetPath()) == wxNOT_FOUND) {
            folders.Add(fn.GetPath());
        }
    }
    
    ::clGetManager()->OpenFindInFileForPaths(folders);
}
