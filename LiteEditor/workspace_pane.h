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

#include "wx/panel.h"
#include "tag_tree.h"

#include "wx/filename.h"

class FileViewTree;
class SymbolTree;
class CppSymbolTree;
class WindowStack;
class OpenWindowsPanel;
class wxComboBox;
class FileExplorer;
class WorkspaceTab;
class Notebook;
class wxAuiManager;

class WorkspacePane : public wxPanel 
{
public:
	static const wxString SYMBOL_VIEW;
	static const wxString FILE_VIEW;
	
	Notebook *m_book;
	wxString m_caption;
	WindowStack *m_winStack;
	OpenWindowsPanel *m_openWindowsPane;
	FileExplorer *m_explorer;
	WorkspaceTab *m_workspaceTab;
	wxComboBox *m_workspaceConfig;	
	wxAuiManager *m_mgr;
	
private:
	void CreateGUIControls();
	CppSymbolTree *GetTreeByFilename(const wxFileName &filename);
	
public:
	WorkspacePane(wxWindow *parent, const wxString &caption, wxAuiManager *mgr);
	virtual ~WorkspacePane();
	
	//-----------------------------------------------
	// Operations
	//-----------------------------------------------
	void BuildSymbolTree(const wxFileName &filename);
	void BuildFileTree();
	void DisplaySymbolTree(const wxFileName &filename);
	void DeleteSymbolTree(const wxFileName &filename);
	void DeleteAllSymbolTrees();
	void CollpaseAll();
	void OnConfigurationManager(wxCommandEvent &e);
	void OnConfigurationManagerUI(wxUpdateUIEvent &e);

	// Return the index of the given tab by name
	int CaptionToIndex(const wxString &caption);

	//-----------------------------------------------
	// Setters/Getters
	//-----------------------------------------------
	Notebook *GetNotebook() { return m_book; }
	
	SymbolTree *GetSymbolTree(const wxString &fileName = wxEmptyString);
	FileViewTree *GetFileViewTree();
	OpenWindowsPanel *GetOpenedWindows() {return m_openWindowsPane;}
	const wxString &GetCaption() const {return m_caption;}
	wxComboBox *GetConfigCombBox();
    WorkspaceTab *GetWorkspaceTab(){return m_workspaceTab;}
	FileExplorer *GetFileExplorer(){return m_explorer;}
	
	DECLARE_EVENT_TABLE()
	virtual void OnEraseBg(wxEraseEvent &e);
	virtual void OnPaint(wxPaintEvent &e);
	virtual void OnSize(wxSizeEvent &e);
	
};

#endif // WORKSPACE_PANE_H

