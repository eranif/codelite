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
#include "openwindowspanel.h"

#include "ColoursAndFontsManager.h"
#include "clToolBar.h"
#include "clToolBarButtonBase.h"
#include "cl_config.h"
#include "editor_config.h"
#include "event_notifier.h"
#include "globals.h"
#include "imanager.h"
#include "macros.h"
#include "pluginmanager.h"

#include <algorithm>
#include <wx/clntdata.h>
#include <wx/filename.h>
#include <wx/stc/stc.h>
#include <wx/wupdlock.h>
#include <wx/xrc/xmlres.h>

BEGIN_EVENT_TABLE(OpenWindowsPanel, OpenWindowsPanelBase)
EVT_MENU(XRCID("wxID_CLOSE_SELECTED"), OpenWindowsPanel::OnCloseSelectedFiles)
EVT_MENU(XRCID("wxID_SAVE_SELECTED"), OpenWindowsPanel::OnSaveSelectedFiles)
END_EVENT_TABLE()

struct TabClientData : public wxClientData {
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
    // Disable the built in sorting, this control will take care of it
    m_dvListCtrl->SetSortFunction(nullptr);
    m_dvListCtrl->SetBitmaps(clGetManager()->GetStdIcons()->GetStandardMimeBitmapListPtr());
    m_toolbar = new clToolBarGeneric(this);
    auto images = m_toolbar->GetBitmapsCreateIfNeeded();
    m_toolbar->AddToggleButton(wxID_SORT_ASCENDING, images->Add("sort"), _("Sort"));
    m_toolbar->Realize();
    GetSizer()->Insert(0, m_toolbar, 0, wxEXPAND);

    m_toolbar->Bind(wxEVT_TOOL, &OpenWindowsPanel::OnSortItems, this, wxID_SORT_ASCENDING);
    m_toolbar->Bind(wxEVT_UPDATE_UI, &OpenWindowsPanel::OnSortItemsUpdateUI, this, wxID_SORT_ASCENDING);

    clConfig cfg;
    m_toolbar->ToggleTool(XRCID("TabsSortTool"), cfg.Read(kConfigTabsPaneSortAlphabetically, true));

    EventNotifier::Get()->Connect(wxEVT_INIT_DONE, wxCommandEventHandler(OpenWindowsPanel::OnInitDone), NULL, this);
    EventNotifier::Get()->Bind(wxEVT_BITMAPS_UPDATED, &OpenWindowsPanel::OnThemeChanged, this);
    Bind(wxEVT_IDLE, &OpenWindowsPanel::OnIdle, this);
}

OpenWindowsPanel::~OpenWindowsPanel()
{
    m_toolbar->Unbind(wxEVT_TOOL, &OpenWindowsPanel::OnSortItems, this, wxID_SORT_ASCENDING);
    m_toolbar->Unbind(wxEVT_UPDATE_UI, &OpenWindowsPanel::OnSortItemsUpdateUI, this, wxID_SORT_ASCENDING);
    EventNotifier::Get()->Unbind(wxEVT_BITMAPS_UPDATED, &OpenWindowsPanel::OnThemeChanged, this);

    // clear list now, or wxGTK seems to crash on exit
    Clear();
    EventNotifier::Get()->Disconnect(wxEVT_INIT_DONE, wxCommandEventHandler(OpenWindowsPanel::OnInitDone), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_ACTIVE_EDITOR_CHANGED,
                                     wxCommandEventHandler(OpenWindowsPanel::OnActiveEditorChanged), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_CMD_PAGE_CHANGED,
                                     wxCommandEventHandler(OpenWindowsPanel::OnActivePageChanged), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_ALL_EDITORS_CLOSED,
                                     wxCommandEventHandler(OpenWindowsPanel::OnAllEditorsClosed), NULL, this);
    EventNotifier::Get()->Unbind(wxEVT_EDITOR_MODIFIED, &OpenWindowsPanel::OnEditorModified, this);
    EventNotifier::Get()->Unbind(wxEVT_FILE_SAVED, &OpenWindowsPanel::OnEditorSaved, this);
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_CLOSED, &OpenWindowsPanel::OnWorkspaceClosed, this);
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_CLOSING, &OpenWindowsPanel::OnWorkspaceClosing, this);
    Unbind(wxEVT_IDLE, &OpenWindowsPanel::OnIdle, this);
}

#define CHECK_WORKSPACE_CLOSING() \
    if(m_workspaceClosing)        \
    return

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

    if(items.IsEmpty()) {
        return;
    }

    // Close the page, it will be removed from the UI later
    std::vector<wxFileName> files;
    wxArrayString pages;
    for(size_t i = 0; i < items.GetCount(); ++i) {
        TabClientData* data = reinterpret_cast<TabClientData*>(m_dvListCtrl->GetItemData(items.Item(i)));
        if(data && data->IsFile()) {
            files.push_back(data->tab.filename);
        } else if(data) {
            pages.push_back(data->tab.text);
        }
    }

    // Close the files
    for(size_t i = 0; i < files.size(); ++i) {
        m_mgr->ClosePage(files[i]);
    }

    // Close non files editors
    for(size_t i = 0; i < pages.size(); ++i) {
        m_mgr->ClosePage(pages.Item(i));
    }
}

void OpenWindowsPanel::OnSaveSelectedFiles(wxCommandEvent& e)
{
    wxUnusedVar(e);
    wxDataViewItemArray items;
    m_dvListCtrl->GetSelections(items);

    if(items.IsEmpty())
        return;

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
    for(size_t i = 0; i < m_dvListCtrl->GetItemCount(); ++i) {
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
    m_sortItems = event.IsChecked();
    if(m_sortItems) {
        SortAlphabetically();
    } else {
        SortByEditorOrder();
    }

    IEditor* editor = m_mgr->GetActiveEditor();
    DoSelectItem(editor);

    clConfig::Get().Write(kConfigTabsPaneSortAlphabetically, event.IsChecked());
}

void OpenWindowsPanel::OnSortItemsUpdateUI(wxUpdateUIEvent& event)
{
    event.Enable(m_dvListCtrl->GetItemCount());
    event.Check(m_sortItems);
}

void OpenWindowsPanel::SortAlphabetically()
{
    wxWindowUpdateLocker locker(m_dvListCtrl);
    clTab::Vec_t tabs;
    m_mgr->GetAllTabs(tabs);

    // Sort editors
    std::sort(tabs.begin(), tabs.end(), [&](const clTab& t1, const clTab& t2) {
        wxString file1 = GetDisplayName(t1);
        wxString file2 = GetDisplayName(t2);
        return file1.CmpNoCase(file2) < 0;
    });
    Clear();

    auto iter = tabs.begin();
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
        m_mgr->OpenFile(data->tab.filename.GetFullPath(), wxEmptyString, wxNOT_FOUND, OF_None);
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
    bool is_modified = false;
    wxVariant value = PrepareValue(tab, &is_modified);

    // the row index is the same as the row count (before we add the new entry)
    int itemIndex = m_dvListCtrl->GetItemCount();

    wxVector<wxVariant> cols;
    cols.push_back(value);
    auto item = m_dvListCtrl->AppendItem(cols, (wxUIntPtr)data);
    if(tab.isFile) {
        m_editors.insert({ tab.filename.GetFullPath(), m_dvListCtrl->RowToItem(itemIndex) });
    }

    bool saved_before = false;
    auto editor = clGetManager()->FindEditor(tab.filename.GetFullPath());
    saved_before = editor && editor->GetCtrl()->CanUndo();
    MarkItemModified(item, is_modified, saved_before);
}

wxString OpenWindowsPanel::GetEditorPath(wxDataViewItem item)
{
    TabClientData* data = reinterpret_cast<TabClientData*>(m_dvListCtrl->GetItemData(item));
    return data->tab.text;
}

void OpenWindowsPanel::Clear()
{
    for(size_t i = 0; i < m_dvListCtrl->GetItemCount(); ++i) {
        wxDataViewItem item = m_dvListCtrl->RowToItem(i);
        TabClientData* data = reinterpret_cast<TabClientData*>(m_dvListCtrl->GetItemData(item));
        wxDELETE(data);
    }
    m_dvListCtrl->DeleteAllItems();
    m_editors.clear();
}

void OpenWindowsPanel::PopulateView()
{
    if(m_sortItems) {
        SortAlphabetically();
    } else {
        SortByEditorOrder();
    }
}

void OpenWindowsPanel::OnMenu(wxDataViewEvent& event)
{
    wxDataViewItemArray items;
    m_dvListCtrl->GetSelections(items);
    if(items.IsEmpty())
        return;

    wxMenu* menu;
    if(items.GetCount() == 1) {
        // single item
        menu = wxXmlResource::Get()->LoadMenu(wxT("editor_tab_right_click"));
    } else {
        menu = wxXmlResource::Get()->LoadMenu(wxT("tabs_multi_sels_menu"));
    }
    m_dvListCtrl->PopupMenu(menu);
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
    for(size_t i = 0; i < m_dvListCtrl->GetItemCount(); ++i) {
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
    EventNotifier::Get()->Connect(wxEVT_ACTIVE_EDITOR_CHANGED,
                                  wxCommandEventHandler(OpenWindowsPanel::OnActiveEditorChanged), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_CMD_PAGE_CHANGED,
                                  wxCommandEventHandler(OpenWindowsPanel::OnActiveEditorChanged), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_ALL_EDITORS_CLOSED, wxCommandEventHandler(OpenWindowsPanel::OnAllEditorsClosed),
                                  NULL, this);

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
    if(!m_initDone)
        return;
    IEditor* editor = m_mgr->FindEditor(event.GetFileName());
    if(editor) {
        DoMarkModify(editor, event.GetFileName(), editor->IsEditorModified());
    }
}

void OpenWindowsPanel::OnEditorSaved(clCommandEvent& event)
{
    event.Skip();
    if(!m_initDone)
        return;

    DoMarkModify(clGetManager()->FindEditor(event.GetFileName()), event.GetFileName(), false);
}

void OpenWindowsPanel::DoMarkModify(IEditor* editor, const wxString& filename, bool b)
{
    std::map<wxString, wxDataViewItem>::iterator iter = m_editors.find(filename);
    if(iter == m_editors.end())
        return;

    wxDataViewItem item = iter->second;

    wxBitmap bmp;
    TabClientData* cd = reinterpret_cast<TabClientData*>(m_dvListCtrl->GetItemData(item));
    const clTab& tab = cd->tab;
    bool is_modified = false;
    wxVariant value = PrepareValue(tab, &is_modified);
    m_dvListCtrl->SetValue(value, m_dvListCtrl->ItemToRow(item), 0);

    // if we can do "undo" -> the file was saved
    MarkItemModified(item, is_modified, editor && editor->GetCtrl()->CanUndo());
    m_dvListCtrl->Refresh();
}

wxVariant OpenWindowsPanel::PrepareValue(const clTab& tab, bool* isModified)
{
    wxString title;
    wxStyledTextCtrl* editor = nullptr;
    *isModified = false;
    title = GetDisplayName(tab);

    if(tab.isFile) {
        auto i_editor = clGetManager()->FindEditor(tab.filename.GetFullPath());
        if(i_editor) {
            editor = i_editor->GetCtrl();
        }
    }

    FileExtManager::FileType ft = FileExtManager::GetType(title, FileExtManager::TypeText);
    int imgId = clGetManager()->GetStdIcons()->GetMimeImageId(ft);
    if(editor && editor->GetModify()) {
        *isModified = true;
        title.Prepend("*");
    }

    wxVariant value = ::MakeBitmapIndexText(title, imgId);
    return value;
}

void OpenWindowsPanel::OnWorkspaceClosed(clWorkspaceEvent& event)
{
    event.Skip();
    m_workspaceClosing = false;
}

void OpenWindowsPanel::OnWorkspaceClosing(clWorkspaceEvent& event)
{
    event.Skip();
    Clear();
    m_workspaceClosing = true;
}

void OpenWindowsPanel::MarkItemModified(const wxDataViewItem& item, bool b, bool saved_before)
{
    auto lexer = ColoursAndFontsManager::Get().GetLexer("diff");
    const auto& colours = m_dvListCtrl->GetColours();
    if(b) {
        m_dvListCtrl->SetItemTextColour(item, lexer->GetProperty(wxSTC_DIFF_DELETED).GetFgColour());
    } else {
        wxColour item_colour = colours.GetItemTextColour();
        if(saved_before) {
            item_colour = lexer->GetProperty(wxSTC_DIFF_ADDED).GetFgColour();
        }
        m_dvListCtrl->SetItemTextColour(item, item_colour);
    }
}

void OpenWindowsPanel::OnThemeChanged(clCommandEvent& event)
{
    event.Skip();
    m_dvListCtrl->SetBitmaps(clGetManager()->GetStdIcons()->GetStandardMimeBitmapListPtr());
    PopulateView();
    m_dvListCtrl->Refresh();
}

wxString OpenWindowsPanel::GetDisplayName(const clTab& tab) const
{
    wxString title;
    if(tab.isFile) {
        const wxFileName& fn = tab.filename;
        if(fn.GetDirCount() && EditorConfigST::Get()->GetOptions()->IsTabShowPath()) {
            title = fn.GetDirs().Last() + wxFileName::GetPathSeparator() + fn.GetFullName();
        } else {
            title = tab.filename.GetFullName();
        }
    } else {
        title = tab.text;
    }
    return title;
}