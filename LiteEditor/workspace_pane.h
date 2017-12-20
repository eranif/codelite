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

#include "Notebook.h"
#include "clAuiCaptionEnabler.h"
#include "clTabRenderer.h"
#include "cl_defs.h"
#include <map>
#include <wx/bitmap.h>
#include <wx/filename.h>
#include <wx/panel.h>
#include <wx/timer.h>

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
    wxString m_caption;
    wxAuiManager* m_mgr;
    wxGauge* m_parsingProgress;
    wxStaticText* m_staticText;
    Notebook* m_book;
    TabgroupsPane* m_TabgroupsPane;

#ifndef __WXOSX__
    OpenWindowsPanel* m_openWindowsPane;
#endif

    FileExplorer* m_explorer;
    WorkspaceTab* m_workspaceTab;
    clAuiCaptionEnabler m_captionEnabler;

protected:
    struct Tab {
        wxString m_label;
        wxWindow* m_window;
        wxBitmap m_bmp;

        Tab(const wxString& label, wxWindow* win, const wxBitmap& bmp = wxNullBitmap)
            : m_label(label)
            , m_window(win)
            , m_bmp(bmp)
        {
        }

        Tab()
            : m_window(NULL)
        {
        }
    };

protected:
    std::map<wxString, Tab> m_tabs;

protected:
    void CreateGUIControls();
    void DoShowTab(bool show, const wxString& title);
    wxWindow* DoGetControlByName(const wxString& title);
    void OnInitDone(wxCommandEvent& event);
    void OnSettingsChanged(wxCommandEvent& event);
    void OnToggleWorkspaceTab(clCommandEvent& event);
    
#if !USE_AUI_NOTEBOOK
    clTabRenderer::Ptr_t GetNotebookRenderer();
#endif

public:
    WorkspacePane(wxWindow* parent, const wxString& caption, wxAuiManager* mgr);
    ~WorkspacePane();

    void UpdateProgress(int val);
    void ClearProgress();
    void ApplySavedTabOrder() const;
    void SaveWorkspaceViewTabOrder() const;
    bool IsTabVisible(int flag);

    // Getters
    const wxString& GetCaption() const { return m_caption; }
    Notebook* GetNotebook() { return m_book; }
    WorkspaceTab* GetWorkspaceTab() { return m_workspaceTab; }
    FileExplorer* GetFileExplorer() { return m_explorer; }
    TabgroupsPane* GetTabgroupsTab() { return m_TabgroupsPane; }
    /**
     * @brief set an active tab by its title
     * @param tabTitle the tab to select
     */
    void SelectTab(const wxString& tabTitle);
};

#endif // WORKSPACE_PANE_H
