//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : workspace_pane.cpp
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
#include <wx/app.h>
#include <wx/wupdlock.h>
#include <wx/xrc/xmlres.h>
#include "parse_thread.h"
#include "editor_config.h"
#include "configuration_manager_dlg.h"
#include "detachedpanesinfo.h"
#include "dockablepanemenumanager.h"
#include "dockablepane.h"
#include "manager.h"
#include "frame.h"
#include "cl_editor.h"
#include "notebook_ex.h"
#include "cpp_symbol_tree.h"
#include "windowstack.h"
#include "macros.h"
#include "fileview.h"
#include "openwindowspanel.h"
#include "fileexplorer.h"
#include "workspacetab.h"
#include "tabgroupspane.h"
#include "workspace_pane.h"
#include "cl_config.h"
#include "cl_aui_dock_art.h"

#ifdef __WXGTK20__
// We need this ugly hack to workaround a gtk2-wxGTK name-clash
// See http://trac.wxwidgets.org/ticket/10883
#define GSocket GlibGSocket
#include <gtk/gtk.h>
#undef GSocket
#endif

WorkspacePane::WorkspacePane(wxWindow *parent, const wxString &caption, wxAuiManager *mgr)
    : wxPanel(parent)
    , m_caption(caption)
    , m_mgr(mgr)
{
    CreateGUIControls();
    Connect();
}

WorkspacePane::~WorkspacePane()
{
}

#define IS_DETACHED(name) (detachedPanes.Index(name) != wxNOT_FOUND) ? true : false

void WorkspacePane::CreateGUIControls()
{
    wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(mainSizer);

    // add notebook for tabs
    long bookStyle = wxVB_LEFT | wxAUI_NB_WINDOWLIST_BUTTON | wxBORDER_NONE;
    m_book = new Notebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, bookStyle);
    
    // Calculate the widest tab (the one with the 'Workspace' label)
    int xx, yy;
    wxFont fnt = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
    wxWindow::GetTextExtent(_("Workspace"), &xx, &yy, NULL, NULL, &fnt);

    mainSizer->Add(m_book, 1, wxEXPAND | wxALL, 0);

    // Add the parsing progress controls
    m_staticText = new wxStaticText(this, wxID_ANY, _("Parsing workspace..."));
    mainSizer->Add(m_staticText, 0, wxEXPAND|wxALL, 2);

    m_parsingProgress = new wxGauge(this, wxID_ANY, 100, wxDefaultPosition, wxSize(-1, 15), wxGA_HORIZONTAL|wxGA_SMOOTH);
    mainSizer->Add(m_parsingProgress, 0, wxEXPAND|wxALL, 1);
    m_parsingProgress->Hide();
    m_staticText->Hide();

    // create tabs (possibly detached)
    DetachedPanesInfo dpi;
    EditorConfigST::Get()->ReadObject(wxT("DetachedPanesList"), &dpi);


    wxArrayString detachedPanes;
    detachedPanes = dpi.GetPanes();

    // Add the workspace tab
    wxString  name;

    name = _("Workspace");
    if(IS_DETACHED(name)) {
        DockablePane *cp = new DockablePane(GetParent(), m_book, name, wxNullBitmap, wxSize(200, 200));
        m_workspaceTab = new WorkspaceTab(cp, name);
        cp->SetChildNoReparent(m_workspaceTab);
    } else {
        m_workspaceTab = new WorkspaceTab(m_book, name);
        m_book->AddPage(m_workspaceTab, name, true, wxNullBitmap);
    }

    // Add the explorer tab
    name = _("Explorer");
    if(IS_DETACHED(name)) {
        DockablePane *cp = new DockablePane(GetParent(), m_book, name, wxNullBitmap, wxSize(200, 200));
        m_explorer = new FileExplorer(cp, name);
        cp->SetChildNoReparent(m_explorer);
    } else {
        m_explorer = new FileExplorer(m_book, name);
        m_book->AddPage(m_explorer, name, false);
    }

    // Add the Open Windows Panel (Tabs)
    name = _("Tabs");
    if(IS_DETACHED(name)) {
        DockablePane *cp = new DockablePane(GetParent(), m_book,  name, wxNullBitmap, wxSize(200, 200));
        m_openWindowsPane = new OpenWindowsPanel(cp, name);
        cp->SetChildNoReparent(m_openWindowsPane);
    } else {
        m_openWindowsPane = new OpenWindowsPanel(m_book, name);
        m_book->AddPage(m_openWindowsPane, name, false);
    }

    // Add the Tabgroups tab
    name = _("Tabgroups");
    if(IS_DETACHED(name)) {
        DockablePane *cp = new DockablePane(GetParent(), m_book,  name, wxNullBitmap, wxSize(200, 200));
        m_TabgroupsPane = new TabgroupsPane(cp, name);
        cp->SetChildNoReparent(m_TabgroupsPane);
    } else {
        m_TabgroupsPane = new TabgroupsPane(m_book, name);
        m_book->AddPage(m_TabgroupsPane, name, false);
    }

    if (m_book->GetPageCount() > 0) {
        m_book->SetSelection((size_t)0);
    }
    UpdateTabs();
    m_mgr->Update();
}

void WorkspacePane::Connect()
{
}

void WorkspacePane::ClearProgress()
{
    m_parsingProgress->SetValue(0);
    m_parsingProgress->Hide();

    m_staticText->SetLabel(_("Parsing workspace..."));
    m_staticText->Hide();
    Layout();
}

void WorkspacePane::UpdateProgress(int val)
{
    if(m_parsingProgress->IsShown() == false) {
        m_parsingProgress->Show();
        m_staticText->Show();
        Layout();
    }

    m_staticText->SetLabel(wxString::Format(_("Parsing workspace: %d%% completed"), val));
    m_parsingProgress->SetValue(val);
    m_parsingProgress->Update();
}

void WorkspacePane::UpdateTabs()
{
    long flags = View_Show_Default;
    EditorConfigST::Get()->GetLongValue(wxT("view_workspace_view"), flags);

    DoShowTab(flags & View_Show_Workspace_Tab, _("Workspace"));
    DoShowTab(flags & View_Show_Explorer_Tab,  _("Explorer"));
    DoShowTab(flags & View_Show_Tabs_Tab,      _("Tabs"));
    DoShowTab(flags & View_Show_Tabgroups_Tab, _("Tabgroups"));
}
typedef struct {
    wxString text;
    wxWindow* win;
    wxBitmap bmp;
} tagTabInfo;

#include "file_logger.h"
void WorkspacePane::ApplySavedTabOrder() const
{
    

    wxArrayString tabs;
    int index = -1;
    if ( !clConfig::Get().GetWorkspaceTabOrder(tabs, index) )
        return;

    // There are (currently) 4 'standard' panes and a variable number of plugin ones
    // NB Since we're only dealing with panes currently in the notebook, this shouldn't
    // be broken by floating panes or non-loaded plugins
    std::vector<tagTabInfo> vTempstore;
    for (size_t t=0; t < tabs.GetCount(); ++t) {
        wxString title = tabs.Item(t);
        if (title.empty()) {
            continue;
        }
        for (size_t n=0; n < m_book->GetPageCount(); ++n) {
            if (title == m_book->GetPageText(n)) {
                tagTabInfo Tab;
                Tab.text = title;
                Tab.win  = m_book->GetPage(n);
                Tab.bmp  = m_book->GetPageBitmap(n);
                
                vTempstore.push_back(Tab);
                m_book->RemovePage(n);
                break;
            }
        }
        // If we reach here without finding title, presumably that tab is no longer available and will just be ignored
    }

    // All the matched tabs are now stored in the vector. Any left in m_book are presumably new additions
    // Now prepend the ordered tabs, so that any additions will effectively be appended
    for (size_t n=0; n < vTempstore.size(); ++n) {
        m_book->InsertPage(n, vTempstore.at(n).win, vTempstore.at(n).text, false, vTempstore.at(n).bmp);
    }

//wxPrintf("After load");for (size_t n=0; n < m_book->GetPageCount(); ++n)  CL_DEBUG1(wxString::Format("Tab %i:  %zs",(int)n,m_book->GetPageText(n)));

    // Restore any saved last selection
    // NB: this doesn't actually work atm: the selection is set correctly, but presumably something else changes is later
    // I've left the code in case anyone ever has time/inclination to fix it
    if ((index >= 0) && (index < (int)m_book->GetPageCount())) {
        m_book->SetSelection(index);
    }

    m_mgr->Update();
}

void WorkspacePane::SaveWorkspaceViewTabOrder() const
{
    wxArrayString panes = m_book->GetPagesTextInOrder();
    clConfig::Get().SetWorkspaceTabOrder( panes, m_book->GetSelection() );
}

void WorkspacePane::DoShowTab(bool show, const wxString& title)
{
    if(!show) {
        for(size_t i=0; i<m_book->GetPageCount(); i++) {
            if(m_book->GetPageText(i) == title) {
                // we've got a match
                m_book->RemovePage(i);
                wxWindow* win = DoGetControlByName(title);
                if(win) {
                    win->Show(false);
                }
                break;
            }
        }
    } else {
        for(size_t i=0; i<m_book->GetPageCount(); i++) {
            if(m_book->GetPageText(i) == title) {
                // requested to add a page which already exists
                return;
            }
        }

        // Fetch the list of detached panes
        // If the mainframe is NULL, read the
        // list from the disk, otherwise use the
        // dockable pane menu

        // Read it from the disk
        DetachedPanesInfo dpi;
        EditorConfigST::Get()->ReadObject(wxT("DetachedPanesList"), &dpi);
        wxArrayString detachedPanes;
        detachedPanes = dpi.GetPanes();

        if(IS_DETACHED(title)) return;

        wxWindow* win = DoGetControlByName(title);
        if(win) {
            win->Show(true);
            m_book->InsertPage(0, win, title, true);
        }
    }
}

wxWindow* WorkspacePane::DoGetControlByName(const wxString& title)
{
    if(title == _("Explorer"))
        return m_explorer;
    else if(title == _("Workspace"))
        return m_workspaceTab;
    else if(title == _("Tabs"))
        return m_openWindowsPane;
    else if(title == _("Tabgroups"))
        return m_TabgroupsPane;
    return NULL;
}

bool WorkspacePane::IsTabVisible(int flag)
{
    wxWindow *win (NULL);
    wxString  title;

    switch(flag) {
    case View_Show_Workspace_Tab:
        title = _("Workspace");
        win = DoGetControlByName(_("Workspace"));
        break;
    case View_Show_Explorer_Tab:
        title = _("Explorer");
        win = DoGetControlByName(_("Explorer"));
        break;
    case View_Show_Tabs_Tab:
        title = _("Tabs");
        win = DoGetControlByName(_("Tabs"));
        break;
    case View_Show_Tabgroups_Tab:
        title = _("Tabgroups");
        win = DoGetControlByName(_("Tabgroups"));
        break;
    }

    if(!win || title.IsEmpty())
        return false;

    // if the control exists in the notebook, return true
    for(size_t i=0; i<m_book->GetPageCount(); i++) {
        if(m_book->GetPageText(i) == title) {
            return true;
        }
    }

    return win && win->IsShown();
}
