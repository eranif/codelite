//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : taskpanel.cpp
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
#include "editor_config.h"
#include "taskspaneldata.h"
#include <wx/fontmap.h>
#include "tasks_find_what_dlg.h"
#include <wx/tglbtn.h>
#include "frame.h"
#include "manager.h"
#include "macros.h"
#include "taskpanel.h"
#include "clWorkspaceManager.h"
#include "event_notifier.h"
#include "clStrings.h"

BEGIN_EVENT_TABLE(TaskPanel, FindResultsTab)
EVT_BUTTON(wxID_FIND, TaskPanel::OnSearch)
EVT_BUTTON(XRCID("find_what"), TaskPanel::OnFindWhat)
EVT_UPDATE_UI(wxID_FIND, TaskPanel::OnSearchUI)
EVT_UPDATE_UI(XRCID("hold_pane_open"), TaskPanel::OnHoldOpenUpdateUI)
END_EVENT_TABLE()

TaskPanel::TaskPanel(wxWindow* parent, wxWindowID id, const wxString& name)
    : FindResultsTab(parent, id, name)
    , m_scope(NULL)
{
    wxArrayString filters;
    filters.Add(wxString(wxT("C/C++ ")) + _("Sources"));
    m_extensions.Add(wxT("*.c;*.cpp;*.cxx;*.cc;*.h;*.hpp;*.hxx;*.hh;*.inl;*.inc;*.hh;*.js;*.php;*.phtml"));
    filters.Add(_("All Files"));
    m_extensions.Add(wxT("*.*"));

    m_tb->DeleteById(XRCID("repeat_output"));
    m_tb->DeleteById(XRCID("recent_searches"));
    m_tb->Realize();

    wxBoxSizer* verticalPanelSizer = new wxBoxSizer(wxVERTICAL);

    wxButton* btn = new wxButton(this, wxID_FIND, _("&Search"));
    verticalPanelSizer->Add(btn, 0, wxEXPAND | wxALL, 5);

    m_findWhat = new wxButton(this, XRCID("find_what"), _("Find What..."));
    verticalPanelSizer->Add(m_findWhat, 0, wxEXPAND | wxALL, 5);
    verticalPanelSizer->Add(new wxStaticLine(this), 0, wxEXPAND | wxALL, 5);

    m_choiceEncoding = new wxChoice(this, wxID_ANY);
    verticalPanelSizer->Add(m_choiceEncoding, 0, wxEXPAND | wxALL, 5);
    m_choiceEncoding->SetToolTip(_("Encoding to use for the search"));

    TasksPanelData d;
    EditorConfigST::Get()->ReadObject(wxT("TasksPanelData"), &d);

    // Set encoding
    wxArrayString astrEncodings;
    wxFontEncoding fontEnc;
    int selection(0);

    size_t iEncCnt = wxFontMapper::GetSupportedEncodingsCount();
    for(size_t i = 0; i < iEncCnt; i++) {
        fontEnc = wxFontMapper::GetEncoding(i);
        if(wxFONTENCODING_SYSTEM == fontEnc) { // skip system, it is changed to UTF-8 in optionsconfig
            continue;
        }
        wxString encodingName = wxFontMapper::GetEncodingName(fontEnc);
        size_t pos = astrEncodings.Add(encodingName);

        if(d.GetEncoding() == encodingName) selection = static_cast<int>(pos);
    }

    m_choiceEncoding->Append(astrEncodings);
    if(m_choiceEncoding->IsEmpty() == false) m_choiceEncoding->SetSelection(selection);

    m_choiceEncoding->Connect(wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(TaskPanel::OnEncodingSelected), NULL,
                              this);

    wxBoxSizer* hSizer = new wxBoxSizer(wxHORIZONTAL);

    // grab the base class scintilla and put our sizer in its place
    wxSizer* mainSizer = m_vSizer;
    mainSizer->Detach(m_sci);
    hSizer->Add(m_sci, 1, wxEXPAND | wxALL, 1);
    hSizer->Add(verticalPanelSizer, 0, wxEXPAND | wxALL, 1);

    mainSizer->Add(hSizer, 1, wxEXPAND | wxALL, 1);
    mainSizer->Layout();
}

TaskPanel::~TaskPanel()
{
    m_choiceEncoding->Disconnect(wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(TaskPanel::OnEncodingSelected),
                                 NULL, this);
}

SearchData TaskPanel::DoGetSearchData()
{
    SearchData data;
    data.SetDisplayScope(true);
    data.SetRegularExpression(true);
    data.SetMatchCase(false);
    data.SetMatchWholeWord(false);
    data.SetEncoding(m_choiceEncoding->GetStringSelection());
    data.SetOwner(this);

    wxString sfind;

    // Load all info from disk
    TasksPanelData d;
    EditorConfigST::Get()->ReadObject(wxT("TasksPanelData"), &d);

    wxStringMap_t::const_iterator iter = d.GetTasks().begin();
    for(; iter != d.GetTasks().end(); iter++) {
        wxString name = iter->first;
        wxString regex = iter->second;
        bool enabled = (d.GetEnabledItems().Index(iter->first) != wxNOT_FOUND);

        regex.Trim().Trim(false);
        wxRegEx re(regex);
        if(enabled && !regex.IsEmpty() && re.IsValid()) sfind << wxT("(") << regex << wxT(")|");
    }

    if(sfind.empty() == false) sfind.RemoveLast();

    data.SetFindString(sfind);

    wxString rootDir = clWorkspaceManager::Get().GetWorkspace()->GetFileName().GetPath();
    wxArrayString rootDirs;
    rootDirs.push_back(rootDir);
    data.SetRootDirs(rootDirs);
    data.SetExtensions(wxT("*.*"));
    return data;
}

void TaskPanel::OnSearch(wxCommandEvent& e)
{
    wxUnusedVar(e);
    SearchData sd = DoGetSearchData();
    SearchThreadST::Get()->PerformSearch(sd);
}

void TaskPanel::OnSearchUI(wxUpdateUIEvent& e) { e.Enable(clWorkspaceManager::Get().IsWorkspaceOpened()); }

void TaskPanel::OnRepeatOutput(wxCommandEvent& e) { OnSearch(e); }

void TaskPanel::OnFindWhat(wxCommandEvent& e)
{
    TasksFindWhatDlg dlg(wxTheApp->GetTopWindow());
    dlg.ShowModal();
}

void TaskPanel::OnHoldOpenUpdateUI(wxUpdateUIEvent& e)
{
    int sel = clMainFrame::Get()->GetOutputPane()->GetNotebook()->GetSelection();
    if(clMainFrame::Get()->GetOutputPane()->GetNotebook()->GetPage(sel) != this) {
        return;
    }

    if(EditorConfigST::Get()->GetOptions()->GetHideOutpuPaneOnUserClick()) {
        e.Enable(true);
        e.Check(EditorConfigST::Get()->GetOptions()->GetHideOutputPaneNotIfTasks());

    } else {
        e.Enable(false);
        e.Check(false);
    }
}

void TaskPanel::OnEncodingSelected(wxCommandEvent& e)
{
    wxUnusedVar(e);

    TasksPanelData d;
    EditorConfigST::Get()->ReadObject(wxT("TasksPanelData"), &d);

    d.SetEncoding(m_choiceEncoding->GetStringSelection());
    EditorConfigST::Get()->WriteObject(wxT("TasksPanelData"), &d);
}

void TaskPanel::OnSearchStart(wxCommandEvent& e)
{
    m_searchInProgress = true;
    Clear();
    SetStyles(m_sci);
    SearchData* data = (SearchData*)e.GetClientData();
    wxDELETE(data);

    // Make sure that the Output view & the "Replace" tab
    // are visible

    clCommandEvent event(wxEVT_SHOW_OUTPUT_TAB);
    event.SetSelected(true).SetString(TASKS);
    EventNotifier::Get()->AddPendingEvent(event);
}
