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
#include "notebook_ex.h"
#include <wx/panel.h>

// Forward Declarations
class FileViewTree;
class OpenWindowsPanel;
class FileExplorer;
class WorkspaceTab;
class TabgroupsPane;
class wxGauge;

class WorkspacePane : public wxPanel
{
private:
	wxString          m_caption;
	wxAuiManager     *m_mgr;
	wxGauge          *m_parsingProgress;
	wxStaticText     *m_staticText;
	Notebook         *m_book;
	TabgroupsPane    *m_TabgroupsPane;
	OpenWindowsPanel *m_openWindowsPane;
	FileExplorer     *m_explorer;
	WorkspaceTab     *m_workspaceTab;

	void CreateGUIControls();
    void Connect();
	void DoShowTab(bool show, const wxString &title);
	wxWindow* DoGetControlByName(const wxString &title);
	
public:
	WorkspacePane(wxWindow *parent, const wxString &caption, wxAuiManager *mgr);
	~WorkspacePane();

	void UpdateProgress(int val);
	void ClearProgress ();
	void UpdateTabs();
	bool IsTabVisible(int flag);
	
	// Getters
	const wxString &GetCaption      () const    { return m_caption;      }
	Notebook       *GetNotebook     ()          { return m_book;         }
    WorkspaceTab   *GetWorkspaceTab ()          { return m_workspaceTab; }
	FileExplorer   *GetFileExplorer ()          { return m_explorer;     }
	TabgroupsPane  *GetTabgroupsTab()          { return m_TabgroupsPane;}
};

#endif // WORKSPACE_PANE_H

