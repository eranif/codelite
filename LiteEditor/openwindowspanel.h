//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : openwindowspanel.h
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
#ifndef __openwindowspanel__
#define __openwindowspanel__

#include "Notebook.h"
#include "bitmap_loader.h"
#include "clToolBar.h"
#include "cl_command_event.h"
#include "imanager.h"
#include "openwindowspanelbase.h"

#include <map>

class IEditor;
class OpenWindowsPanel : public OpenWindowsPanelBase
{
    wxString m_caption;
    IManager* m_mgr;
    bool m_initDone;
    std::map<wxString, wxDataViewItem> m_editors;
    bool m_workspaceClosing;
    bool m_workspaceOpened;
    clToolBarGeneric* m_toolbar;
    bool m_sortItems = false;

protected:
    wxString GetEditorPath(wxDataViewItem item);
    bool IsEditor(wxDataViewItem item) const;
    void DoSaveItem(wxDataViewItem item);
    void DoCloseItem(wxDataViewItem item);
    void DoSelectItem(IEditor* editor);
    void DoSelectItem(wxWindow* win);
    void SortAlphabetically();
    void SortByEditorOrder();
    void AppendEditor(const clTab& tab);
    void Clear();
    void PopulateView();
    void DoMarkModify(IEditor* editor, const wxString& filename, bool b);
    wxVariant PrepareValue(const clTab& tab, bool* isModified);
    wxString GetDisplayName(const clTab& tab) const;
    void MarkItemModified(const wxDataViewItem& item, bool b, bool saved_before);
    void OnThemeChanged(clCommandEvent& event);

protected:
    virtual void OnMenu(wxDataViewEvent& event);
    virtual void OnTabActivated(wxDataViewEvent& event);
    virtual void OnTabSelected(wxDataViewEvent& event);
    void OnInitDone(wxCommandEvent& event);

    // Handlers for OpenWindowsPanelBase events.
    void OnSortItems(wxCommandEvent& event);
    void OnSortItemsUpdateUI(wxUpdateUIEvent& event);

    // Handlers for workspace events
    void OnActiveEditorChanged(wxCommandEvent& e);
    void OnActivePageChanged(wxCommandEvent& e);
    void OnAllEditorsClosed(wxCommandEvent& e);

    void OnCloseSelectedFiles(wxCommandEvent& e);
    void OnSaveSelectedFiles(wxCommandEvent& e);
    void OnIdle(wxIdleEvent& event);
    void OnEditorModified(clCommandEvent& event);
    void OnEditorSaved(clCommandEvent& event);
    void OnWorkspaceClosed(clWorkspaceEvent& event);
    void OnWorkspaceClosing(clWorkspaceEvent& event);

    DECLARE_EVENT_TABLE()

public:
    OpenWindowsPanel(wxWindow* parent, const wxString& caption);
    ~OpenWindowsPanel();
    const wxString& GetCaption() const { return m_caption; }
};

#endif // __openwindowspanel__
