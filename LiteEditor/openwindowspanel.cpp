//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : openwindowspanel.cpp
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
#include <wx/xrc/xmlres.h>
#include "globals.h"
#include <wx/clntdata.h>
#include "event_notifier.h"
#include "openwindowspanel.h"
#include "cl_config.h"
#include "globals.h"
#include "pluginmanager.h"
#include <wx/wupdlock.h>
#include "macros.h"
#include <wx/filename.h>
#include <algorithm>
#include <wx/stc/stc.h>
#include "imanager.h"

BEGIN_EVENT_TABLE(OpenWindowsPanel, OpenWindowsPanelBase)
EVT_MENU(XRCID("wxID_CLOSE_SELECTED"), OpenWindowsPanel::OnCloseSelectedFiles)
EVT_MENU(XRCID("wxID_SAVE_SELECTED"), OpenWindowsPanel::OnSaveSelectedFiles)
END_EVENT_TABLE()

struct TabSorter
{
    bool operator()(const clTab& t1, const clTab& t2)
    {
        wxString file1, file2;
        if(t1.isFile) {
            file1 = t1.filename.GetFullName().Lower();
        } else {
            file1 = t1.text;
        }

        if(t2.isFile) {
            file2 = t2.filename.GetFullName().Lower();
        } else {
            file2 = t2.text;
        }
        return file1.CmpNoCase(file2) < 0;
    }
};

struct TabClientData : public wxClientData
{
    clTab tab;
    bool isModified;
    TabClientData() {}
    TabClientData(const clTab& t)
        : tab(t)
        , isModified(false)
    {
    }
    virtual ~TabClientData() {}
    bool IsFile() const { return tab.isFile; }
};

OpenWindowsPanel::OpenWindowsPanel(wxWindow* parent, const wxString& caption)
    : OpenWindowsPanelBase(parent)
    , m_caption(caption)
    , m_mgr(PluginManager::Get())
    , m_initDone(false)
    , m_workspaceClosing(false)
    , m_workspaceOpened(false)
{
    m_bitmaps = clGetManager()->GetStdIcons()->MakeStandardMimeMap();

    clConfig cfg;
    m_auibar->ToggleTool(XRCID("TabsSortTool"), cfg.Read(kConfigTabsPaneSortAlphabetically, true));

    EventNotifier::Get()->Connect(wxEVT_INIT_DONE, wxCommandEventHandler(OpenWindowsPanel::OnInitDone), NULL, this);
    Bind(wxEVT_IDLE, &OpenWindowsPanel::OnIdle, this);
}

OpenWindowsPanel::~OpenWindowsPanel()
{
    // clear list now, or wxGTK seems to crash on exit
    Clear();
    EventNotifier::Get()->Disconnect(wxEVT_INIT_DONE, wxCommandEventHandler(OpenWindowsPanel::OnInitDone), NULL, this);
    EventNotifier::Get()->Disconnect(
        wxEVT_ACTIVE_EDITOR_CHANGED, wxCommandEventHandler(OpenWindowsPanel::OnActiveEditorChanged), NULL, this);
    EventNotifier::Get()->Disconnect(
        wxEVT_CMD_PAGE_CHANGED, wxCommandEventHandler(OpenWindowsPanel::OnActivePageChanged), NULL, this);
    EventNotifier::Get()->Disconnect(
        wxEVT_ALL_EDITORS_CLOSED, wxCommandEventHandler(OpenWindowsPanel::OnAllEditorsClosed), NULL, this);
    EventNotifier::Get()->Unbind(wxEVT_EDITOR_MODIFIED, &OpenWindowsPanel::OnEditorModified, this);
    EventNotifier::Get()->Unbind(wxEVT_FILE_SAVED, &OpenWindowsPanel::OnEditorSaved, this);
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_CLOSED, &OpenWindowsPanel::OnWorkspaceClosed, this);
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_CLOSING, &OpenWindowsPanel::OnWorkspaceClosing, this);
    Unbind(wxEVT_IDLE, &OpenWindowsPanel::OnIdle, this);
}

#define CHECK_WORKSPACE_CLOSING() \
    if(m_workspaceClosing) return

void OpenWindowsPanel::OnActivePageChanged(wxCommandEvent& e)
{
    e.Skip();
    CHECK_WORKSPACE_CLOSING();

    PopulateView();
    DoSelectItem((wxWindow*)e.GetClientData());
}

void OpenWindowsPanel::OnActiveEditorChanged(wxCommandEvent& e)
{
    e.Skip();
    CHECK_WORKSPACE_CLOSING();

    PopulateView();
    if(m_mgr->GetActiveEditor()) {
        DoSelectItem(m_mgr->GetActiveEditor());
    }
}

void OpenWindowsPanel::OnAllEditorsClosed(wxCommandEvent& e)
{
    e.Skip();
    CHECK_WORKSPACE_CLOSING();

    Clear();
}

void OpenWindowsPanel::OnCloseSelectedFiles(wxCommandEvent& e)
{
    wxUnusedVar(e);
    wxDataViewItemArray items;
    m_dvListCtrl->GetSelections(items);

    if(items.IsEmpty()) return;

    wxArrayString paths;  // list of files
    wxArrayString others; // list of non files editors
    for(size_t i = 0; i < items.GetCount(); ++i) {
        TabClientData* data = reinterpret_cast<TabClientData*>(m_dvListCtrl->GetItemData(items.Item(i)));
        if(data->IsFile()) {
            paths.Add(data->tab.filename.GetFullPath());
        } else {
            others.Add(data->tab.text);
        }

        // Delete the item data
        wxDELETE(data);

        // Remove the item from the list
        int row = m_dvListCtrl->ItemToRow(items.Item(i));
        m_dvListCtrl->DeleteItem(row);
    }

    // Close the files
    for(size_t i = 0; i < paths.GetCount(); ++i) {
        wxFileName fn(paths.Item(i));
        m_mgr->ClosePage(fn);
    }

    // Close non file tabs
    for(size_t i = 0; i < others.GetCount(); ++i) {
        m_mgr->ClosePage(others.Item(i));
    }
}

void OpenWindowsPanel::OnSaveSelectedFiles(wxCommandEvent& e)
{
    wxUnusedVar(e);
    wxDataViewItemArray items;
    m_dvListCtrl->GetSelections(items);

    if(items.IsEmpty()) return;

    for(size_t i = 0; i < items.GetCount(); ++i) {
        DoSaveItem(items.Item(i));
    }
}

void OpenWindowsPanel::DoSaveItem(wxDataViewItem item)
{
    TabClientData* data = reinterpret_cast<TabClientData*>(m_dvListCtrl->GetItemData(item));
    if(data) {
        if(data->IsFile()) {
            IEditor* editor = m_mgr->FindEditor(data->tab.filename.GetFullPath());
            if(editor) {
                editor->Save();
            }
        }
    }
}

void OpenWindowsPanel::DoCloseItem(wxDataViewItem item)
{
    TabClientData* data = reinterpret_cast<TabClientData*>(m_dvListCtrl->GetItemData(item));
    if(data) {
        if(data->IsFile()) {
            m_mgr->ClosePage(data->tab.filename);
        } else {
            m_mgr->ClosePage(data->tab.text);
        }
    }
}

void OpenWindowsPanel::DoSelectItem(IEditor* editor)
{
    CHECK_PTR_RET(editor);
    for(int i = 0; i < m_dvListCtrl->GetItemCount(); ++i) {
        wxDataViewItem item = m_dvListCtrl->RowToItem(i);
        TabClientData* data = reinterpret_cast<TabClientData*>(m_dvListCtrl->GetItemData(item));
        if(data->IsFile()) {
            if(editor->GetFileName() == data->tab.filename) {
                m_dvListCtrl->Select(item);
                break;
            }
        }
    }
}

void OpenWindowsPanel::OnSortItems(wxCommandEvent& event)
{
    if(event.IsChecked()) {
        SortAlphabetically();
    } else {
        SortByEditorOrder();
    }

    IEditor* editor = m_mgr->GetActiveEditor();
    DoSelectItem(editor);

    clConfig::Get().Write(kConfigTabsPaneSortAlphabetically, event.IsChecked());
}

void OpenWindowsPanel::OnSortItemsUpdateUI(wxUpdateUIEvent& event) { event.Enable(m_dvListCtrl->GetItemCount()); }

void OpenWindowsPanel::SortAlphabetically()
{
    wxWindowUpdateLocker locker(m_dvListCtrl);
    clTab::Vec_t tabs;
    m_mgr->GetAllTabs(tabs);

    // Sort editors
    std::sort(tabs.begin(), tabs.end(), TabSorter());
    Clear();

    clTab::Vec_t::iterator iter = tabs.begin();
    for(; iter != tabs.end(); ++iter) {
        AppendEditor(*iter);
    }
}

void OpenWindowsPanel::SortByEditorOrder()
{
    wxWindowUpdateLocker locker(m_dvListCtrl);
    clTab::Vec_t tabs;
    m_mgr->GetAllTabs(tabs);

    // Sort editors
    Clear();

    clTab::Vec_t::iterator iter = tabs.begin();
    for(; iter != tabs.end(); ++iter) {
        AppendEditor(*iter);
    }
}

void OpenWindowsPanel::OnTabActivated(wxDataViewEvent& event)
{
    event.Skip();
    CHECK_WORKSPACE_CLOSING();

    wxDataViewItem item = event.GetItem();
    CHECK_ITEM_RET(item);

    TabClientData* data = reinterpret_cast<TabClientData*>(m_dvListCtrl->GetItemData(item));
    if(data->IsFile()) {
        m_mgr->OpenFile(data->tab.filename.GetFullPath());
    } else {
        m_mgr->SelectPage(m_mgr->FindPage(data->tab.text));
    }
}

void OpenWindowsPanel::OnTabSelected(wxDataViewEvent& event)
{
    event.Skip();
    CHECK_WORKSPACE_CLOSING();

    if(::wxGetKeyState(WXK_CONTROL)) {
        // multiple selection in progress
        return;
    }

    wxDataViewItem item = event.GetItem();
    CHECK_ITEM_RET(item);
    TabClientData* data = reinterpret_cast<TabClientData*>(m_dvListCtrl->GetItemData(item));
    if(data->IsFile()) {
        m_mgr->OpenFile(data->tab.filename.GetFullPath());
    } else {
        wxWindow* page = m_mgr->FindPage(data->tab.text);
        if(page) {
            m_mgr->SelectPage(page);
        }
    }
}

void OpenWindowsPanel::AppendEditor(const clTab& tab)
{
    TabClientData* data = new TabClientData(tab);
    wxVariant value = PrepareValue(tab);

    // the row index is the same as the row count (before we add the new entry)
    int itemIndex = m_dvListCtrl->GetItemCount();

    wxVector<wxVariant> cols;
    cols.push_back(value);
    m_dvListCtrl->AppendItem(cols, (wxUIntPtr)data);
    if(tab.isFile) {
        m_editors.insert(std::make_pair(tab.filename.GetFullPath(), m_dvListCtrl->RowToItem(itemIndex)));
    }
}

wxString OpenWindowsPanel::GetEditorPath(wxDataViewItem item)
{
    TabClientData* data = reinterpret_cast<TabClientData*>(m_dvListCtrl->GetItemData(item));
    return data->tab.text;
}

void OpenWindowsPanel::Clear()
{
    for(int i = 0; i < m_dvListCtrl->GetItemCount(); ++i) {
        wxDataViewItem item = m_dvListCtrl->RowToItem(i);
        TabClientData* data = reinterpret_cast<TabClientData*>(m_dvListCtrl->GetItemData(item));
        wxDELETE(data);
    }
    m_dvListCtrl->DeleteAllItems();
    m_editors.clear();
}

void OpenWindowsPanel::PopulateView()
{
    if(m_auibar->GetToolToggled(XRCID("TabsSortTool"))) {
        SortAlphabetically();
    } else {
        SortByEditorOrder();
    }
}

void OpenWindowsPanel::OnMenu(wxDataViewEvent& event)
{
    wxDataViewItemArray items;
    m_dvListCtrl->GetSelections(items);
    if(items.IsEmpty()) return;

    wxMenu* menu;
    if(items.GetCount() == 1) {
        // single item
        menu = wxXmlResource::Get()->LoadMenu(wxT("editor_tab_right_click"));
    } else {
        menu = wxXmlResource::Get()->LoadMenu(wxT("tabs_multi_sels_menu"));
    }
    PopupMenu(menu);
    wxDELETE(menu);
}

bool OpenWindowsPanel::IsEditor(wxDataViewItem item) const
{
    TabClientData* data = reinterpret_cast<TabClientData*>(m_dvListCtrl->GetItemData(item));
    return data->IsFile();
}

void OpenWindowsPanel::DoSelectItem(wxWindow* win)
{
    CHECK_PTR_RET(win);
    for(int i = 0; i < m_dvListCtrl->GetItemCount(); ++i) {
        wxDataViewItem item = m_dvListCtrl->RowToItem(i);
        TabClientData* data = reinterpret_cast<TabClientData*>(m_dvListCtrl->GetItemData(item));
        if(data->tab.window == win) {
            m_dvListCtrl->Select(item);
            break;
        }
    }
}

void OpenWindowsPanel::OnInitDone(wxCommandEvent& event)
{
    event.Skip();
    m_initDone = true;

    // Connect the events only after the initialization has completed
    EventNotifier::Get()->Connect(
        wxEVT_ACTIVE_EDITOR_CHANGED, wxCommandEventHandler(OpenWindowsPanel::OnActiveEditorChanged), NULL, this);
    EventNotifier::Get()->Connect(
        wxEVT_CMD_PAGE_CHANGED, wxCommandEventHandler(OpenWindowsPanel::OnActiveEditorChanged), NULL, this);
    EventNotifier::Get()->Connect(
        wxEVT_ALL_EDITORS_CLOSED, wxCommandEventHandler(OpenWindowsPanel::OnAllEditorsClosed), NULL, this);

    EventNotifier::Get()->Bind(wxEVT_EDITOR_MODIFIED, &OpenWindowsPanel::OnEditorModified, this);
    EventNotifier::Get()->Bind(wxEVT_FILE_SAVED, &OpenWindowsPanel::OnEditorSaved, this);
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_CLOSED, &OpenWindowsPanel::OnWorkspaceClosed, this);
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_CLOSING, &OpenWindowsPanel::OnWorkspaceClosing, this);
}

void OpenWindowsPanel::OnIdle(wxIdleEvent& event)
{
    event.Skip();
    static bool once = false;
    if(!once && m_initDone && m_mgr->GetPageCount() && m_dvListCtrl->GetItemCount() == 0) {
        // populate the control
        once = true;
        PopulateView();
    }
}

void OpenWindowsPanel::OnEditorModified(clCommandEvent& event)
{
    event.Skip();
    if(!m_initDone) return;
    IEditor* editor = m_mgr->FindEditor(event.GetFileName());
    if(editor) {
        DoMarkModify(event.GetFileName(), editor->IsModified());
    }
}

void OpenWindowsPanel::OnEditorSaved(clCommandEvent& event)
{
    event.Skip();
    if(!m_initDone) return;
    DoMarkModify(event.GetFileName(), false);
}

void OpenWindowsPanel::DoMarkModify(const wxString& filename, bool b)
{
    std::map<wxString, wxDataViewItem>::iterator iter = m_editors.find(filename);
    if(iter == m_editors.end()) return;
    wxDataViewItem item = iter->second;

    wxBitmap bmp;
    TabClientData* cd = reinterpret_cast<TabClientData*>(m_dvListCtrl->GetItemData(item));
    const clTab& tab = cd->tab;
    wxVariant value = PrepareValue(tab);
    m_dvListCtrl->SetValue(value, m_dvListCtrl->ItemToRow(item), 0);
    m_dvListCtrl->Refresh();
}

wxVariant OpenWindowsPanel::PrepareValue(const clTab& tab)
{
    wxString title;
    wxStyledTextCtrl* editor(NULL);
    if(tab.isFile) {
        title = tab.filename.GetFullName();
        editor = dynamic_cast<wxStyledTextCtrl*>(tab.window);
    } else {
        title = tab.text;
    }

    FileExtManager::FileType ft = FileExtManager::GetType(title, FileExtManager::TypeText);
    wxBitmap bmp;

    // If the tab had an icon, use it, otherwise, use a bitmap by the file type
    if(tab.bitmap.IsOk()) {
        bmp = tab.bitmap;
    } else if(m_bitmaps.count(ft)) {
        bmp = m_bitmaps.find(ft)->second;
    } else {
        bmp = m_bitmaps.find(FileExtManager::TypeText)->second;
    }

    if(editor && editor->GetModify()) {
        title.Prepend("*");
    }

    wxVariant value = ::MakeIconText(title, bmp);
    return value;
}

void OpenWindowsPanel::OnWorkspaceClosed(wxCommandEvent& event)
{
    event.Skip();
    m_workspaceClosing = false;
}

void OpenWindowsPanel::OnWorkspaceClosing(wxCommandEvent& event)
{
    event.Skip();
    Clear();
    m_workspaceClosing = true;
}
