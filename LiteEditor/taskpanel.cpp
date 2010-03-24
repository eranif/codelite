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
#include "tasks_find_what_dlg.h"
#include <wx/tglbtn.h>
#include "frame.h"
#include "manager.h"
#include "macros.h"
#include "taskpanel.h"

BEGIN_EVENT_TABLE(TaskPanel, FindResultsTab)
    EVT_BUTTON(XRCID("search"),       TaskPanel::OnSearch)
	EVT_BUTTON(XRCID("find_what"),    TaskPanel::OnFindWhat)
    EVT_UPDATE_UI(XRCID("search"),    TaskPanel::OnSearchUI)
END_EVENT_TABLE()

TaskPanel::TaskPanel(wxWindow* parent, wxWindowID id, const wxString &name)
    : FindResultsTab(parent, id, name)
    , m_scope(NULL)
    , m_filter(NULL)
{
    wxArrayString scopes;
    scopes.Add(SEARCH_IN_PROJECT);
    scopes.Add(SEARCH_IN_WORKSPACE);
    scopes.Add(SEARCH_IN_CURR_FILE_PROJECT);

    wxArrayString filters;
    filters.Add(wxT("C/C++ Sources"));
    m_extensions.Add(wxT("*.c;*.cpp;*.cxx;*.cc;*.h;*.hpp;*.hxx;*.hh;*.inl;*.inc;*.hh"));
    filters.Add(wxT("All Files"));
    m_extensions.Add(wxT("*.*"));

    wxBoxSizer *horzSizer = new wxBoxSizer(wxHORIZONTAL);

    wxStaticText *text = new wxStaticText(this, wxID_ANY, wxT("Search Tasks in:"));
    horzSizer->Add(text, 0, wxRIGHT|wxLEFT|wxALIGN_CENTER_VERTICAL, 5);

    m_scope = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, scopes);
    m_scope->SetSelection(0);
    horzSizer->Add(m_scope, 1, wxRIGHT|wxLEFT|wxALIGN_CENTER_VERTICAL, 2);

    m_filter = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, filters);
    m_filter->SetSelection(0);
    horzSizer->Add(m_filter, 1, wxRIGHT|wxLEFT|wxALIGN_CENTER_VERTICAL, 2);

    wxButton *btn = new wxButton(this, XRCID("search"), wxT("&Search"));
    horzSizer->Add(btn, 0, wxRIGHT|wxLEFT|wxALIGN_CENTER_VERTICAL, 5);

	m_findWhat = new wxButton(this, XRCID("find_what"), _("Find What..."));
	horzSizer->Add(m_findWhat, 0, wxRIGHT|wxLEFT|wxALIGN_CENTER_VERTICAL, 2);

	wxBoxSizer *vertSizer = new wxBoxSizer(wxVERTICAL);
	vertSizer->Add(horzSizer, 0, wxEXPAND|wxTOP|wxBOTTOM);

	// grab the base class scintilla and put our sizer in its place
	wxSizer *mainSizer = GetSizer();
	mainSizer->Detach(m_sci);
	vertSizer->Add(m_sci, 1, wxEXPAND | wxALL, 1);

#ifdef __WXMAC__
	mainSizer->Insert(0, vertSizer, 1, wxEXPAND | wxALL, 1);
#else
	mainSizer->Add(vertSizer, 1, wxEXPAND | wxALL, 1);
#endif

	mainSizer->Layout();
}

TaskPanel::~TaskPanel()
{
}

SearchData TaskPanel::DoGetSearchData()
{
	SearchData data;
    data.SetDisplayScope(true);
    data.SetRegularExpression(true);
    data.SetMatchCase(false);
    data.SetMatchWholeWord(false);
    data.SetEncoding(wxT("ISO-8859-1"));
    data.SetOwner(this);

	wxString sfind;

	// Load all info from disk
	TasksPanelData d;
	EditorConfigST::Get()->ReadObject(wxT("TasksPanelData"), &d);

	std::map<wxString, wxString>::const_iterator iter = d.GetTasks().begin();
	for(; iter != d.GetTasks().end(); iter++){
		wxString name  = iter->first;
		wxString regex = iter->second;
		bool enabled   = (d.GetEnabledItems().Index(iter->first) != wxNOT_FOUND);

		regex.Trim().Trim(false);
		wxRegEx re(regex);
		if(enabled && !regex.IsEmpty() && re.IsValid())
			sfind << wxT("(") << regex << wxT(")|");
	}

	if(sfind.empty() == false)
		sfind.RemoveLast();

    data.SetFindString(sfind);

	wxString rootDir = m_scope->GetStringSelection();
	wxArrayString rootDirs;
	rootDirs.push_back(rootDir);
    data.SetRootDirs(rootDirs);
    wxArrayString files;
	if (rootDir == SEARCH_IN_WORKSPACE) {
		ManagerST::Get()->GetWorkspaceFiles(files);
	} else if (rootDir == SEARCH_IN_PROJECT) {
		ManagerST::Get()->GetProjectFiles(ManagerST::Get()->GetActiveProjectName(), files);
	} else if (rootDir == SEARCH_IN_CURR_FILE_PROJECT) {
		wxString project = ManagerST::Get()->GetActiveProjectName();
		if (Frame::Get()->GetMainBook()->GetActiveEditor()) {
			wxFileName activeFile = Frame::Get()->GetMainBook()->GetActiveEditor()->GetFileName();
			project = ManagerST::Get()->GetProjectNameByFile(activeFile.GetFullPath());
		}
		ManagerST::Get()->GetProjectFiles(project, files);
	}
    data.SetFiles(files);
    data.SetExtensions(m_extensions[m_filter->GetSelection()]);

	return data;
}

void TaskPanel::OnSearch(wxCommandEvent& e)
{
	wxUnusedVar(e);
    SearchData sd = DoGetSearchData();
    SearchThreadST::Get()->PerformSearch(sd);
}

void TaskPanel::OnSearchUI(wxUpdateUIEvent& e)
{
    e.Enable(true);
}

void TaskPanel::OnRepeatOutput(wxCommandEvent& e)
{
	OnSearch(e);
}

void TaskPanel::OnFindWhat(wxCommandEvent& e)
{
	TasksFindWhatDlg dlg(this);
	dlg.ShowModal();
}
