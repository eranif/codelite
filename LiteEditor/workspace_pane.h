//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : workspace_pane.h              
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
#ifndef WORKSPACE_PANE_H
#define WORKSPACE_PANE_H

#include <wx/filename.h>
#include <wx/panel.h>

// Forward Declarations
class Notebook;
class FileViewTree;
class WindowStack;
class OpenWindowsPanel;
class FileExplorer;
class WorkspaceTab;

class WorkspacePane : public wxPanel 
{
private:
	wxString          m_caption;
	wxComboBox       *m_workspaceConfig;	
	wxAuiManager     *m_mgr;
    
	Notebook         *m_book;
	WindowStack      *m_winStack;
	OpenWindowsPanel *m_openWindowsPane;
	FileExplorer     *m_explorer;
	WorkspaceTab     *m_workspaceTab;
	
	void CreateGUIControls();
    void Connect();
    void ShowCurrentOutline();
    
    // Workspace event handlers
    void OnWorkspaceConfig    (wxCommandEvent &e);
    void OnWorkspaceClosed    (wxCommandEvent &e);
    void OnFileRetagged       (wxCommandEvent &e);
    void OnProjectFileAdded   (wxCommandEvent &e);
    void OnProjectFileRemoved (wxCommandEvent &e);
    void OnSymbolsUpdated     (wxCommandEvent &e);
    void OnSymbolsDeleted     (wxCommandEvent &e);
    void OnSymbolsAdded       (wxCommandEvent &e);
    void OnActiveEditorChanged(wxCommandEvent &e);
    void OnEditorClosing      (wxCommandEvent &e);
    void OnAllEditorsClosed   (wxCommandEvent &e);
    
	// Configuration mgr handlers
	void OnConfigurationManager      (wxCommandEvent  &e);
	void OnConfigurationManagerUI    (wxUpdateUIEvent &e);
	void OnConfigurationManagerChoice(wxCommandEvent &event);
	
public:
	WorkspacePane(wxWindow *parent, const wxString &caption, wxAuiManager *mgr);
	~WorkspacePane();
	
	// Getters
	const wxString &GetCaption      () const    { return m_caption;      }
	Notebook       *GetNotebook     ()          { return m_book;         }
    WorkspaceTab   *GetWorkspaceTab ()          { return m_workspaceTab; }
	FileExplorer   *GetFileExplorer ()          { return m_explorer;     }
};

#endif // WORKSPACE_PANE_H

