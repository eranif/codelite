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
#include "event_notifier.h"
#include "fileexplorer.h"
#include "wx/sizer.h"
#include "wx/tokenzr.h"
#include "wx/xrc/xmlres.h"

#include "OpenFolderDlg.h"
#include "clFileOrFolderDropTarget.h"
#include "clToolBarButton.h"
#include "clTreeCtrlPanel.h"
#include "clWorkspaceView.h"
#include "cl_config.h"
#include "codelite_events.h"
#include "editor_config.h"
#include "file_logger.h"
#include "frame.h"
#include "globals.h"
#include "macros.h"
#include "manager.h"
#include "plugin.h"
#include "workspace_pane.h"
#include <wx/arrstr.h>
#include <wx/dirdlg.h>

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
    BitmapLoader* bmpLoader = clGetManager()->GetStdIcons();
    clToolBarButton* button =
        new clToolBarButton(m_view->GetToolBar(), wxID_OPEN, bmpLoader->LoadBitmap("folder"), _("Open folder"));
    m_view->GetToolBar()->Add(button);
    m_view->GetToolBar()->Realize();
    m_view->GetToolBar()->Bind(wxEVT_TOOL, &FileExplorer::OnOpenFolder, this, wxID_OPEN);
    // For the file explorer we use the standard configuration tool
    m_view->SetConfig(&clConfig::Get());
    m_view->SetViewName(_("File Explorer"));
    mainSizer->Add(m_view, 1, wxEXPAND | wxALL, 0);
    Layout();
}

void FileExplorer::OnFolderDropped(clCommandEvent& event)
{
    const wxArrayString& folders = event.GetStrings();
    for(size_t i = 0; i < folders.size(); ++i) {
        m_view->AddFolder(folders.Item(i));
    }
    size_t index = clGetManager()->GetWorkspacePaneNotebook()->GetPageIndex(_("Explorer"));
    if(index != wxString::npos) { clGetManager()->GetWorkspacePaneNotebook()->ChangeSelection(index); }
}

void FileExplorer::OpenFolder(const wxString& path) { m_view->AddFolder(path); }

void FileExplorer::OnOpenFolder(wxCommandEvent& event)
{
    wxString path = ::wxDirSelector(_("Select folder to open"));
    if(path.IsEmpty()) { return; }
    OpenFolder(path);
}
