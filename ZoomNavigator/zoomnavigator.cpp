//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : zoomnavigator.cpp
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

//	Copyright: 2013 Brandon Captain
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.

#include "detachedpanesinfo.h"
#include "dockablepane.h"
#include "event_notifier.h"
#include "znSettingsDlg.h"
#include "zn_config_item.h"
#include "zoomnavigator.h"
#include <wx/menu.h>
#include <wx/msgdlg.h>
#include <wx/stc/stc.h>
#include <wx/wupdlock.h>
#include <wx/xrc/xmlres.h>

static ZoomNavigator* thePlugin = NULL;
#define CHECK_CONDITION(cond) \
    if(!cond)                 \
        return;

const wxString ZOOM_PANE_TITLE(_("Zoom Navigator"));

// Define the plugin entry point
CL_PLUGIN_API IPlugin* CreatePlugin(IManager* manager)
{
    if(thePlugin == 0) {
        thePlugin = new ZoomNavigator(manager);
    }
    return thePlugin;
}

CL_PLUGIN_API PluginInfo* GetPluginInfo()
{
    static PluginInfo info;
    info.SetAuthor(wxT("Brandon Captain"));
    info.SetName(wxT("ZoomNavigator"));
    info.SetDescription(_("A dockable pane that shows a zoomed-out view of your code."));
    info.SetVersion(wxT("v1.0"));
    return &info;
}

CL_PLUGIN_API int GetPluginInterfaceVersion() { return PLUGIN_INTERFACE_VERSION; }

ZoomNavigator::ZoomNavigator(IManager* manager)
    : IPlugin(manager)
    , mgr(manager)
    , zoompane(NULL)
    , m_topWindow(NULL)
    , m_text(NULL)
    , m_markerFirstLine(wxNOT_FOUND)
    , m_markerLastLine(wxNOT_FOUND)
    , m_enabled(false)
    , m_lastLine(wxNOT_FOUND)
    , m_startupCompleted(false)
{
    m_config = new clConfig("zoom-navigator.conf");
    m_longName = _("Zoom Navigator");
    m_shortName = wxT("ZoomNavigator");
    m_topWindow = m_mgr->GetTheApp();

    m_topWindow->Connect(wxEVT_IDLE, wxIdleEventHandler(ZoomNavigator::OnIdle), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_INIT_DONE, wxCommandEventHandler(ZoomNavigator::OnInitDone), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_FILE_SAVED, clCommandEventHandler(ZoomNavigator::OnFileSaved), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_ZN_SETTINGS_UPDATED, wxCommandEventHandler(ZoomNavigator::OnSettingsChanged),
                                  NULL, this);
    m_topWindow->Connect(XRCID("zn_settings"), wxEVT_COMMAND_MENU_SELECTED,
                         wxCommandEventHandler(ZoomNavigator::OnSettings), NULL, this);
    EventNotifier::Get()->Bind(wxEVT_SHOW_WORKSPACE_TAB, &ZoomNavigator::OnToggleTab, this);
    DoInitialize();
}

ZoomNavigator::~ZoomNavigator() {}

void ZoomNavigator::UnPlug()
{
    EventNotifier::Get()->Disconnect(wxEVT_INIT_DONE, wxCommandEventHandler(ZoomNavigator::OnInitDone), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_ZN_SETTINGS_UPDATED, wxCommandEventHandler(ZoomNavigator::OnSettingsChanged),
                                     NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_FILE_SAVED, clCommandEventHandler(ZoomNavigator::OnFileSaved), NULL, this);

    m_topWindow->Disconnect(wxEVT_IDLE, wxIdleEventHandler(ZoomNavigator::OnIdle), NULL, this);
    m_topWindow->Disconnect(XRCID("zn_settings"), wxEVT_COMMAND_MENU_SELECTED,
                            wxCommandEventHandler(ZoomNavigator::OnSettings), NULL, this);
    EventNotifier::Get()->Unbind(wxEVT_SHOW_WORKSPACE_TAB, &ZoomNavigator::OnToggleTab, this);

    // Remove the tab if it's actually docked in the workspace pane
    int index(wxNOT_FOUND);
    index = m_mgr->GetWorkspacePaneNotebook()->GetPageIndex(zoompane);
    if(index != wxNOT_FOUND) {
        m_mgr->GetWorkspacePaneNotebook()->RemovePage(index);
    }
    zoompane->Destroy();
}

void ZoomNavigator::CreateToolBar(clToolBar* toolbar) { wxUnusedVar(toolbar); }

void ZoomNavigator::CreatePluginMenu(wxMenu* pluginsMenu)
{
    wxMenu* menu = new wxMenu();
    wxMenuItem* item(NULL);
    item = new wxMenuItem(menu, XRCID("zn_settings"), _("Settings"), _("Settings"), wxITEM_NORMAL);
    menu->Append(item);
    pluginsMenu->Append(wxID_ANY, _("Zoom Navigator"), menu);
}

void ZoomNavigator::OnShowHideClick(wxCommandEvent& e) {}

void ZoomNavigator::DoInitialize()
{
    znConfigItem data;
    if(m_config->ReadItem(&data)) {
        m_enabled = data.IsEnabled();
    }

    // create tab (possibly detached)
    Notebook* book = m_mgr->GetWorkspacePaneNotebook();
    if(IsZoomPaneDetached()) {
        // Make the window child of the main panel (which is the grand parent of the notebook)
        DockablePane* cp = new DockablePane(book->GetParent()->GetParent(), book, ZOOM_PANE_TITLE, false, wxNOT_FOUND,
                                            wxSize(200, 200));
        zoompane = new wxPanel(cp);
        cp->SetChildNoReparent(zoompane);

    } else {
        zoompane = new wxPanel(book);
        book->AddPage(zoompane, ZOOM_PANE_TITLE, false, wxNOT_FOUND);
    }
    m_mgr->AddWorkspaceTab(ZOOM_PANE_TITLE);

    m_text = new ZoomText(zoompane);
    m_text->Connect(wxEVT_LEFT_DOWN, wxMouseEventHandler(ZoomNavigator::OnPreviewClicked), NULL, this);
    m_text->Connect(wxEVT_LEFT_DCLICK, wxMouseEventHandler(ZoomNavigator::OnPreviewClicked), NULL, this);
    m_text->SetCursor(wxCURSOR_POINT_LEFT);

    wxBoxSizer* bs = new wxBoxSizer(wxVERTICAL);
    bs->Add(m_text, 1, wxEXPAND, 0);
    wxCheckBox* cbEnablePlugin = new wxCheckBox(zoompane, wxID_ANY, _("Enable plugin"));
    cbEnablePlugin->SetValue(data.IsEnabled());
    bs->Add(cbEnablePlugin, 0, wxEXPAND);

    cbEnablePlugin->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(ZoomNavigator::OnEnablePlugin), NULL,
                            this);
    zoompane->SetSizer(bs);
}

bool ZoomNavigator::IsZoomPaneDetached()
{
    DetachedPanesInfo dpi;
    m_mgr->GetConfigTool()->ReadObject(wxT("DetachedPanesList"), &dpi);
    wxArrayString detachedPanes = dpi.GetPanes();
    return detachedPanes.Index(ZOOM_PANE_TITLE) != wxNOT_FOUND;
}

void ZoomNavigator::DoUpdate()
{
    // sanity tests
    CHECK_CONDITION(m_enabled);
    CHECK_CONDITION(!m_mgr->IsShutdownInProgress());

    IEditor* curEditor = m_mgr->GetActiveEditor();
    if(!curEditor && !m_text->IsEmpty()) {
        DoCleanup();
    }
    CHECK_CONDITION(curEditor);

    wxStyledTextCtrl* stc = curEditor->GetCtrl();
    CHECK_CONDITION(stc);

    if(curEditor->GetFileName().GetFullPath() != m_curfile) {
        SetEditorText(curEditor);
    }

    int first = stc->GetFirstVisibleLine();
    int last = stc->LinesOnScreen() + first;

    if(m_markerFirstLine != first || m_markerLastLine != last) {
        PatchUpHighlights(first, last);
        SetZoomTextScrollPosToMiddle(stc);
    }
}

void ZoomNavigator::SetEditorText(IEditor* editor)
{
    m_curfile.Clear();
    m_text->UpdateText(editor);
    if(editor) {
        m_curfile = editor->GetFileName().GetFullPath();
        m_text->UpdateLexer(editor);
    }
}

void ZoomNavigator::SetZoomTextScrollPosToMiddle(wxStyledTextCtrl* stc)
{
    int first;
    first = stc->GetFirstVisibleLine();

    // we want to make 'first' centered
    int numLinesOnScreen = m_text->LinesOnScreen();
    int linesAboveIt = numLinesOnScreen / 2;

    first = first - linesAboveIt;
    if(first < 0)
        first = 0;

    m_text->SetFirstVisibleLine(first);
    m_text->ClearSelections();
}

void ZoomNavigator::PatchUpHighlights(const int first, const int last)
{
    m_text->HighlightLines(first, last);
    m_markerFirstLine = first;
    m_markerLastLine = last;
}

void ZoomNavigator::HookPopupMenu(wxMenu* menu, MenuType type) {}

void ZoomNavigator::OnPreviewClicked(wxMouseEvent& e)
{
    IEditor* curEditor = m_mgr->GetActiveEditor();

    // user clicked on the preview
    CHECK_CONDITION(m_startupCompleted);
    CHECK_CONDITION(curEditor);
    CHECK_CONDITION(m_enabled);

    // the first line is taken from the preview
    int pos = m_text->PositionFromPoint(e.GetPosition());
    if(pos == wxSTC_INVALID_POSITION) {
        return;
    }
    int first = m_text->LineFromPosition(pos);
    int nLinesOnScreen = curEditor->GetCtrl()->LinesOnScreen();
    first -= (nLinesOnScreen / 2);
    if(first < 0)
        first = 0;

    // however, the last line is set according to the actual editor
    int last = nLinesOnScreen + first;

    PatchUpHighlights(first, last);
    curEditor->GetCtrl()->SetFirstVisibleLine(first);
    curEditor->SetCaretAt(curEditor->PosFromLine(first + (nLinesOnScreen / 2)));

    // reset the from/last members to avoid unwanted movements in the 'OnTimer' function
    m_markerFirstLine = curEditor->GetCtrl()->GetFirstVisibleLine();
    m_markerLastLine = m_markerFirstLine + curEditor->GetCtrl()->LinesOnScreen();
}

void ZoomNavigator::DoCleanup()
{
    SetEditorText(NULL);
    m_markerFirstLine = wxNOT_FOUND;
    m_markerLastLine = wxNOT_FOUND;
    m_text->UpdateLexer(NULL);
}

void ZoomNavigator::OnSettings(wxCommandEvent& e)
{
    znSettingsDlg dlg(wxTheApp->GetTopWindow());
    dlg.ShowModal();
}

void ZoomNavigator::OnSettingsChanged(wxCommandEvent& e)
{
    e.Skip();
    m_config->Reload();
    znConfigItem data;
    if(m_config->ReadItem(&data)) {
        m_enabled = data.IsEnabled();

        if(!m_enabled) {
            // Clear selection
            m_text->UpdateText(NULL);

        } else {
            DoCleanup();
            DoUpdate();
        }
    }
}

void ZoomNavigator::OnFileSaved(clCommandEvent& e)
{
    e.Skip();

    if(e.GetString() == m_curfile) {
        // Update the file content
        m_curfile.Clear();
        DoUpdate();
    }
}

void ZoomNavigator::OnWorkspaceClosed(wxCommandEvent& e)
{
    e.Skip();
    DoCleanup();
}

void ZoomNavigator::OnEnablePlugin(wxCommandEvent& e)
{
    znConfigItem data;
    m_config->Reload();
    m_config->ReadItem(&data);
    data.SetEnabled(e.IsChecked());
    m_config->WriteItem(&data);

    // Notify about the settings changed
    wxCommandEvent evt(wxEVT_ZN_SETTINGS_UPDATED);
    EventNotifier::Get()->AddPendingEvent(evt);
}

void ZoomNavigator::OnInitDone(wxCommandEvent& e)
{
    e.Skip();
    m_startupCompleted = true;
    m_text->Startup();
}

void ZoomNavigator::OnIdle(wxIdleEvent& e)
{
    e.Skip();
    DoUpdate();
}

void ZoomNavigator::OnToggleTab(clCommandEvent& event)
{
    if(event.GetString() != ZOOM_PANE_TITLE) {
        event.Skip();
        return;
    }

    if(event.IsSelected()) {
        // show it
        m_mgr->GetWorkspacePaneNotebook()->AddPage(zoompane, ZOOM_PANE_TITLE, true);
    } else {
        int where = m_mgr->GetWorkspacePaneNotebook()->GetPageIndex(ZOOM_PANE_TITLE);
        if(where != wxNOT_FOUND) {
            m_mgr->GetWorkspacePaneNotebook()->RemovePage(where);
        }
    }
}
