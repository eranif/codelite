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
#include "fileexplorertree.h"
#include "wx/sizer.h"
#include "wx/tokenzr.h"

#include "macros.h" 
#include "globals.h"
#include "plugin.h"
#include "editor_config.h"
#include "manager.h"
#include "workspace_pane.h"
#include "frame.h"

FileExplorer::FileExplorer(wxWindow *parent, const wxString &caption)
: wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(250, 300))
, m_caption(caption)
, m_isLinkedToEditor(false)
#ifdef __WXMSW__
#if wxUSE_FSVOLUME
, m_thread(this)
#endif
#endif
{
	long link(1);
	EditorConfigST::Get()->GetLongValue(wxT("LinkFileExplorerToEditor"), link);
	m_isLinkedToEditor = link ? true : false;
	CreateGUIControls();
}

FileExplorer::~FileExplorer()
{
}

void FileExplorer::CreateGUIControls()
{
	wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
	SetSizer(mainSizer);

	wxToolBar *tb = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT|wxTB_HORIZONTAL|wxTB_NODIVIDER);
	
	mainSizer->Add(tb, 0, wxEXPAND);
	
#ifdef __WXMSW__
#if wxUSE_FSVOLUME
	wxArrayString volumes;
	Connect(wxEVT_THREAD_VOLUME_COMPLETED, wxCommandEventHandler(FileExplorer::OnVolumes), NULL, this);
	
	m_thread.Create();
	m_thread.Run();
	
	m_volumes = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, volumes, 0 );
	mainSizer->Add(m_volumes, 0, wxEXPAND|wxALL, 1);
	ConnectChoice(m_volumes, FileExplorer::OnVolumeChanged);
#endif
#endif

	m_fileTree = new FileExplorerTree(this, wxID_ANY);
	m_fileTree->Connect(wxVDTC_ROOT_CHANGED, wxCommandEventHandler(FileExplorer::OnRootChanged), NULL, this);
	mainSizer->Add(m_fileTree, 1, wxEXPAND|wxALL, 1);
	
	tb->AddTool(XRCID("link_editor"), wxEmptyString, wxXmlResource::Get()->LoadBitmap(wxT("link_editor")), wxT("Link Editor"), wxITEM_CHECK);
	tb->ToggleTool(XRCID("link_editor"), m_isLinkedToEditor);
	tb->AddTool(XRCID("collapse_all"), wxEmptyString, wxXmlResource::Get()->LoadBitmap(wxT("collapse")), wxT("Collapse All"), wxITEM_NORMAL);
	tb->AddTool(XRCID("go_home"), wxEmptyString, wxXmlResource::Get()->LoadBitmap(wxT("gohome")), wxT("Goto Current Directory"), wxITEM_NORMAL);
	tb->Realize();
	
	Connect( XRCID("link_editor"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( FileExplorer::OnLinkEditor ));
	Connect( XRCID("collapse_all"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( FileExplorer::OnCollapseAll ));
	Connect( XRCID("go_home"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( FileExplorer::OnGoHome ));

	mainSizer->Layout();

    wxTheApp->Connect(XRCID("show_in_explorer"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(FileExplorer::OnShowFile), NULL, this);
    wxTheApp->Connect(XRCID("show_in_explorer"), wxEVT_UPDATE_UI, wxUpdateUIEventHandler(FileExplorer::OnShowFileUI), NULL, this);
    wxTheApp->Connect(wxEVT_WORKSPACE_LOADED, wxCommandEventHandler(FileExplorer::OnWorkspaceLoaded), NULL, this);
    wxTheApp->Connect(wxEVT_ACTIVE_EDITOR_CHANGED, wxCommandEventHandler(FileExplorer::OnActiveEditorChanged), NULL, this);
}

void FileExplorer::OnCollapseAll(wxCommandEvent &e)
{
	wxUnusedVar(e);
    m_fileTree->CollapseAll();
    wxTreeItemId root = m_fileTree->GetRootItem();
    if (root.IsOk()) {
        m_fileTree->Expand(m_fileTree->GetRootItem());
    }
	wxTreeItemId sel = m_fileTree->GetSelection();
	if (sel.IsOk()) {
		m_fileTree->EnsureVisible(sel);
    }
}

void FileExplorer::OnGoHome(wxCommandEvent &e)
{
	wxUnusedVar(e);
	m_fileTree->ExpandToPath(wxGetCwd());
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
    LEditor *editor = Frame::Get()->GetMainBook()->GetActiveEditor();
    if (editor && editor->GetFileName().FileExists()) {
        m_fileTree->ExpandToPath(editor->GetFileName());
        ManagerST::Get()->ShowWorkspacePane(m_caption);
    }
    e.Skip();
}

void FileExplorer::OnShowFileUI(wxUpdateUIEvent& e)
{
	LEditor *editor = Frame::Get()->GetMainBook()->GetActiveEditor();
	e.Enable(editor && editor->GetFileName().FileExists());
}

void FileExplorer::OnActiveEditorChanged(wxCommandEvent& e)
{
    e.Skip();
    if (m_isLinkedToEditor) {
        LEditor *editor = Frame::Get()->GetMainBook()->GetActiveEditor();
        if (editor && editor->GetFileName().FileExists()) {
            m_fileTree->ExpandToPath(editor->GetFileName());
        }
    }
}

void FileExplorer::OnWorkspaceLoaded(wxCommandEvent& e)
{
    e.Skip();
    wxUnusedVar(e);
    if (m_isLinkedToEditor) {
        m_fileTree->ExpandToPath(WorkspaceST::Get()->GetWorkspaceFileName().GetPath());
    }
}

void FileExplorer::OnRootChanged(wxCommandEvent &e)
{
	wxTreeItemId root = m_fileTree->GetRootItem();
	if(root.IsOk()){
		wxString vol = m_fileTree->GetItemText(root);
#ifdef __WXMSW__
#if wxUSE_FSVOLUME
        if(m_volumes->FindString(vol) == wxNOT_FOUND) {
            m_volumes->AppendString(vol);
        }
		m_volumes->SetStringSelection(vol);
#endif
#endif
        SendCmdEvent(wxEVT_FILE_EXP_INIT_DONE); //TODO: pass &vol?
	}
	e.Skip();
}

#ifdef __WXMSW__
#if wxUSE_FSVOLUME
void FileExplorer::OnVolumeChanged(wxCommandEvent &e)
{
	wxUnusedVar(e);
	m_fileTree->SetRootPath(m_volumes->GetStringSelection());
}

void FileExplorer::OnVolumes(wxCommandEvent &e)
{
	wxString curvol = m_volumes->GetStringSelection();
	wxArrayString volumes = wxStringTokenize(e.GetString(), wxT(";"), wxTOKEN_STRTOK);
	int where = volumes.Index(curvol);
	if(where != wxNOT_FOUND){
		volumes.RemoveAt((size_t)where);
	}
	m_volumes->Append(volumes);
}
#endif
#endif
