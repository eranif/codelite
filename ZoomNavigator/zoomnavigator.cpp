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

#include "zoomnavigator.h"

#include "bookmark_manager.h"
#include "event_notifier.h"
#include "globals.h"
#include "znSettingsDlg.h"
#include "zn_config_item.h"

#include <wx/menu.h>
#include <wx/msgdlg.h>
#include <wx/stc/stc.h>
#include <wx/wupdlock.h>
#include <wx/xrc/xmlres.h>

#define CHECK_CONDITION(cond) \
    if (!cond)                \
        return;

const wxString ZOOM_PANE_TITLE(_("Zoom"));

// Define the plugin entry point
CL_PLUGIN_API IPlugin* CreatePlugin(IManager* manager)
{
    return new ZoomNavigator(manager);
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

namespace
{
/// @[param]in ctrl the control
/// @[param]in marker_mask marker type to check
/// @[param]out lines
size_t GetMarkers(wxStyledTextCtrl* ctrl, int marker_mask, std::vector<int>* lines)
{
    int nFoundLine = 0;
    while (true) {
        nFoundLine = ctrl->MarkerNext(nFoundLine, marker_mask);
        if (nFoundLine == wxNOT_FOUND) {
            break;
        }
        lines->push_back(nFoundLine);
        ++nFoundLine;
    }
    return lines->size();
}
} // namespace

ZoomNavigator::ZoomNavigator(IManager* manager)
    : IPlugin(manager)
    , mgr(manager)
    , m_zoompane(NULL)
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
    EventNotifier::Get()->Bind(wxEVT_INIT_DONE, &ZoomNavigator::OnInitDone, this);
    EventNotifier::Get()->Bind(wxEVT_FILE_SAVED, &ZoomNavigator::OnFileSaved, this);
    EventNotifier::Get()->Bind(wxEVT_ZN_SETTINGS_UPDATED, &ZoomNavigator::OnSettingsChanged, this);

    m_topWindow->Connect(XRCID("zn_settings"), wxEVT_COMMAND_MENU_SELECTED,
                         wxCommandEventHandler(ZoomNavigator::OnSettings), NULL, this);

    m_timer = new wxTimer(this);
    Bind(wxEVT_TIMER, &ZoomNavigator::OnTimer, this, m_timer->GetId());
    m_timer->Start(100);

    DoInitialize();
}

ZoomNavigator::~ZoomNavigator() {}

void ZoomNavigator::UnPlug()
{
    EventNotifier::Get()->Unbind(wxEVT_ZN_SETTINGS_UPDATED, &ZoomNavigator::OnSettingsChanged, this);
    EventNotifier::Get()->Unbind(wxEVT_INIT_DONE, &ZoomNavigator::OnInitDone, this);
    EventNotifier::Get()->Unbind(wxEVT_FILE_SAVED, &ZoomNavigator::OnFileSaved, this);

    m_topWindow->Disconnect(wxEVT_IDLE, wxIdleEventHandler(ZoomNavigator::OnIdle), NULL, this);
    m_topWindow->Disconnect(XRCID("zn_settings"), wxEVT_COMMAND_MENU_SELECTED,
                            wxCommandEventHandler(ZoomNavigator::OnSettings), NULL, this);
    // cancel the timer
    Unbind(wxEVT_TIMER, &ZoomNavigator::OnTimer, this, m_timer->GetId());
    m_timer->Stop();
    wxDELETE(m_timer);

    // Remove the tab if it's actually docked in the workspace pane
    if (!m_mgr->BookDeletePage(PaneId::SIDE_BAR, m_zoompane)) {
        m_zoompane->Destroy();
    }
    m_zoompane = nullptr;
}

void ZoomNavigator::CreateToolBar(clToolBarGeneric* toolbar) { wxUnusedVar(toolbar); }

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
    if (m_config->ReadItem(&data)) {
        m_enabled = data.IsEnabled();
    }

    // create tab (possibly detached)
    auto book = m_mgr->BookGet(PaneId::SIDE_BAR);
    m_zoompane = new wxPanel(book);
    m_mgr->BookAddPage(PaneId::SIDE_BAR, m_zoompane, ZOOM_PANE_TITLE, "zoom-button");
    m_mgr->AddWorkspaceTab(ZOOM_PANE_TITLE);

    m_text = new ZoomText(m_zoompane);
    m_text->Bind(wxEVT_LEFT_DOWN, &ZoomNavigator::OnPreviewClicked, this);
    m_text->Bind(wxEVT_LEFT_DCLICK, &ZoomNavigator::OnPreviewClicked, this);
    m_text->SetCursor(wxCURSOR_POINT_LEFT);
    wxBoxSizer* bs = new wxBoxSizer(wxVERTICAL);
    bs->Add(m_text, 1, wxEXPAND, 0);
    m_zoompane->SetSizer(bs);
}

void ZoomNavigator::DoUpdate()
{
    // sanity tests
    CHECK_CONDITION(m_enabled);
    CHECK_CONDITION(!m_mgr->IsShutdownInProgress());

    IEditor* curEditor = m_mgr->GetActiveEditor();
    if (!curEditor && !m_text->IsEmpty()) {
        DoCleanup();
    }
    CHECK_CONDITION(curEditor);

    wxStyledTextCtrl* stc = curEditor->GetCtrl();
    CHECK_CONDITION(stc);
    CHECK_CONDITION(stc->IsShown());

    // locate any error and warning markers in the main editor and duplicate then into the zoomed view
    std::vector<int> error_lines;
    std::vector<int> warning_lines;
    m_text->DeleteAllMarkers();
    if (GetMarkers(stc, mmt_error, &error_lines)) {
        m_text->UpdateMarkers(error_lines, ZoomText::MARKER_ERROR);
    }

    if (GetMarkers(stc, mmt_warning, &warning_lines)) {
        m_text->UpdateMarkers(warning_lines, ZoomText::MARKER_WARNING);
    }

    if (curEditor->GetFileName().GetFullPath() != m_curfile) {
        SetEditorText(curEditor);
    }

    int first = stc->GetFirstVisibleLine();
    int last = stc->LinesOnScreen() + first;

    if (m_markerFirstLine != first || m_markerLastLine != last) {
        PatchUpHighlights(first, last);
        SetZoomTextScrollPosToMiddle(stc);
    }
}

void ZoomNavigator::SetEditorText(IEditor* editor)
{
    m_curfile.Clear();
    m_text->UpdateText(editor);
    if (editor) {
        m_curfile = editor->GetFileName().GetFullPath();
        m_text->UpdateLexer(editor);
    }
}

void ZoomNavigator::SetZoomTextScrollPosToMiddle(wxStyledTextCtrl* stc)
{
    // make the middle line of editor text centered in the zoomview
    int first = stc->GetFirstVisibleLine() + (stc->LinesOnScreen() / 2);

    // we want to make 'first' centered
    int numLinesOnScreen = m_text->LinesOnScreen();
    int linesAboveIt = numLinesOnScreen / 2;

    first = first - linesAboveIt;
    if (first < 0)
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
    if (pos == wxSTC_INVALID_POSITION) {
        return;
    }
    int first = m_text->LineFromPosition(pos);
    int nLinesOnScreen = curEditor->GetCtrl()->LinesOnScreen();
    first -= (nLinesOnScreen / 2);
    if (first < 0)
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
    if (m_config->ReadItem(&data)) {
        m_enabled = data.IsEnabled();

        if (!m_enabled) {
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

    if (e.GetString() == m_curfile) {
        // Update the file content
        m_curfile.Clear();
        m_markerFirstLine = m_markerLastLine = wxNOT_FOUND; // forces a scrolling
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

void ZoomNavigator::OnIdle(wxIdleEvent& e) { e.Skip(); }

void ZoomNavigator::OnTimer(wxTimerEvent& event)
{
    wxUnusedVar(event);
    DoUpdate();
}
