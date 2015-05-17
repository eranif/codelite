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
#include "clTreeCtrlPanel.h"
#include "clFileOrFolderDropTarget.h"
#include "codelite_events.h"
#include "clWorkspaceView.h"

FileExplorer::FileExplorer(wxWindow* parent, const wxString& caption)
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(250, 300))
    , m_caption(caption)
{
    CreateGUIControls();
    m_themeHelper = new ThemeHandlerHelper(this);
    SetDropTarget(new clFileOrFolderDropTarget(this));
    Bind(wxEVT_DND_FOLDER_DROPPED, &FileExplorer::OnFolderDropped, this);
}

FileExplorer::~FileExplorer()
{
    wxDELETE(m_themeHelper);
    Unbind(wxEVT_DND_FOLDER_DROPPED, &FileExplorer::OnFolderDropped, this);
}

void FileExplorer::CreateGUIControls()
{
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(mainSizer);
    m_view = new clTreeCtrlPanel(this);
    mainSizer->Add(m_view, 1, wxEXPAND | wxALL, 2);
    Layout();
}

void FileExplorer::OnFolderDropped(clCommandEvent& event)
{
    const wxArrayString& folders = event.GetStrings();
    for(size_t i = 0; i < folders.size(); ++i) {
        m_view->AddFolder(folders.Item(i));
    }
    size_t index = clGetManager()->GetWorkspacePaneNotebook()->GetPageIndex(_("Explorer"));
    if(index != wxString::npos) {
        clGetManager()->GetWorkspacePaneNotebook()->ChangeSelection(index);
    }
}

void FileExplorer::OpenFolder(const wxString& path)
{
    m_view->AddFolder(path);
}

// void FileExplorer::OnCollapseAll(wxCommandEvent& e)
//{
//    wxUnusedVar(e);
//    m_fileTree->Tree()->ReCreateTree();
//}
//
// void FileExplorer::OnGoHome(wxCommandEvent& e)
//{
//    wxUnusedVar(e);
//    m_fileTree->ClearSelections();
//    m_fileTree->Tree()->ExpandPath(::wxGetCwd());
//}
//
// void FileExplorer::OnLinkEditor(wxCommandEvent& e)
//{
//    m_isLinkedToEditor = !m_isLinkedToEditor;
//    EditorConfigST::Get()->SetInteger(wxT("LinkFileExplorerToEditor"), m_isLinkedToEditor ? 1 : 0);
//    if(m_isLinkedToEditor) {
//        OnActiveEditorChanged(e);
//    }
//}
//
// void FileExplorer::OnShowFile(wxCommandEvent& e)
//{
//    LEditor* editor = clMainFrame::Get()->GetMainBook()->GetActiveEditor();
//    if(editor && editor->GetFileName().FileExists()) {
//        m_fileTree->ClearSelections();
//        m_fileTree->Tree()->ExpandPath(editor->GetFileName().GetFullPath());
//        ManagerST::Get()->ShowWorkspacePane(m_caption);
//    }
//    e.Skip();
//}
//
// void FileExplorer::OnShowFileUI(wxUpdateUIEvent& e)
//{
//    LEditor* editor = clMainFrame::Get()->GetMainBook()->GetActiveEditor();
//    e.Enable(editor && editor->GetFileName().FileExists());
//}
//
// void FileExplorer::OnActiveEditorChanged(wxCommandEvent& e)
//{
//    e.Skip();
//    if(m_isLinkedToEditor) {
//        LEditor* editor = clMainFrame::Get()->GetMainBook()->GetActiveEditor();
//        if(editor && editor->GetFileName().FileExists()) {
//            m_fileTree->ClearSelections();
//            CL_DEBUG1(" ===> [Explorer] Expand to path for " + editor->GetFileName().GetFullPath());
//            m_fileTree->Tree()->ExpandPath(editor->GetFileName().GetFullPath());
//            CL_DEBUG1(" <=== [Explorer] Expand to path for " + editor->GetFileName().GetFullPath());
//        }
//    }
//}
//
// void FileExplorer::OnWorkspaceLoaded(wxCommandEvent& e)
//{
//    e.Skip();
//    wxUnusedVar(e);
//    if(m_isLinkedToEditor) {
//        m_fileTree->Tree()->ExpandPath(wxGetCwd());
//    }
//}
//
// void FileExplorer::OnBookmark(wxAuiToolBarEvent& event)
//{
//    wxAuiToolBar* tb = dynamic_cast<wxAuiToolBar*>(event.GetEventObject());
//    if(event.IsDropDownClicked()) {
//        // Show the drop down menu
//        wxArrayString folders = clConfig::Get().Read(kConfigFileExplorerBookmarks, wxArrayString());
//        if(folders.IsEmpty()) return;
//
//        // Remove non existing folders from the bookmark
//        wxArrayString existingFolders;
//        for(size_t i=0; i<folders.GetCount(); ++i) {
//            if(wxFileName::DirExists(folders.Item(i))) {
//                existingFolders.Add(folders.Item(i));
//            }
//        }
//
//        // Update the list
//        clConfig::Get().Write(kConfigFileExplorerBookmarks, folders);
//
//        folders.swap(existingFolders);
//        if(folders.IsEmpty()) return;
//
//        std::map<int, wxString> entries;
//        wxMenu menu;
//        for(size_t i = 0; i < folders.GetCount(); ++i) {
//            int id = ::wxNewId();
//            entries.insert(std::make_pair(id, folders.Item(i)));
//            menu.Append(id, folders.Item(i));
//        }
//
//        wxPoint pt = event.GetItemRect().GetBottomLeft();
//        pt.y++;
//
//        int sel = tb->GetPopupMenuSelectionFromUser(menu, pt);
//        if(entries.count(sel)) {
//            wxString path = entries.find(sel)->second;
//            m_fileTree->ClearSelections();
//            m_fileTree->GetGenericDirCtrl()->ExpandPath(path);
//            m_fileTree->GetGenericDirCtrl()->SelectPath(path);
//        }
//
//    } else {
//        wxArrayString paths;
//        m_fileTree->GetSelectedDirectories(paths);
//        if(paths.IsEmpty()) return;
//
//        wxArrayString folders = clConfig::Get().Read(kConfigFileExplorerBookmarks, wxArrayString());
//        for(size_t i = 0; i < paths.GetCount(); ++i) {
//            if(folders.Index(paths.Item(i)) == wxNOT_FOUND) {
//                folders.Add(paths.Item(i));
//            }
//        }
//        folders.Sort();
//        clConfig::Get().Write(kConfigFileExplorerBookmarks, folders);
//    }
//}
//
// void FileExplorer::OnGotoFolder(wxCommandEvent& event)
//{
//    // Quickly jump to folder
//    OpenFolderDlg dlg(EventNotifier::Get()->TopFrame());
//    if(dlg.ShowModal() == wxID_OK) {
//        wxString path = dlg.GetTextCtrlFolder()->GetValue();
//        if(wxFileName::DirExists(path)) {
//            m_fileTree->ClearSelections();
//            m_fileTree->GetGenericDirCtrl()->ExpandPath(path);
//            m_fileTree->GetGenericDirCtrl()->SelectPath(path);
//        }
//    }
//}
//
// void FileExplorer::OnFindInFiles(wxCommandEvent& event)
//{
//    wxArrayString folders, files;
//    m_fileTree->GetSelections(folders, files);
//    if(folders.IsEmpty() && files.IsEmpty()) return;
//
//    // Prepare a folder list from both arrays
//    for(size_t i=0; i<files.size(); ++i) {
//        wxFileName fn(files.Item(i));
//        if(folders.Index(fn.GetPath()) == wxNOT_FOUND) {
//            folders.Add(fn.GetPath());
//        }
//    }
//
//    ::clGetManager()->OpenFindInFileForPaths(folders);
//}
//
