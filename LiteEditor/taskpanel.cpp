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
#include <wx/tglbtn.h>
#include "frame.h"
#include "manager.h"
#include "macros.h"
#include "taskpanel.h"


BEGIN_EVENT_TABLE(TaskPanel, FindResultsTab)
    EVT_TOGGLEBUTTON(wxID_ANY,        TaskPanel::OnToggle)
    
    EVT_BUTTON(XRCID("search"),       TaskPanel::OnSearch)
    EVT_BUTTON(XRCID("customize"),    TaskPanel::OnCustomize)
    
    EVT_UPDATE_UI(XRCID("search"),    TaskPanel::OnSearchUI)
    EVT_UPDATE_UI(XRCID("customize"), TaskPanel::OnCustomizeUI)
END_EVENT_TABLE()

TaskPanel::TaskPanel(wxWindow* parent, wxWindowID id, const wxString &name)
    : FindResultsTab(parent, id, name, 1)
    , m_scope(NULL)
    , m_filter(NULL)
{
    // TODO: could load some of the following data (tasks, scopes, filters) from EditorConfig:
    
    wxArrayString tasks;
    tasks.Add(wxT("TODO"));
    tasks.Add(wxT("FIXME"));
    tasks.Add(wxT("BUG"));
    tasks.Add(wxT("ATTN"));
    
    wxArrayString scopes;
    scopes.Add(SEARCH_IN_PROJECT);
    scopes.Add(SEARCH_IN_WORKSPACE);
    scopes.Add(SEARCH_IN_CURR_FILE_PROJECT);
    
    wxArrayString filters;
    filters.Add(wxT("C/C++ Sources"));
    m_extensions.Add(wxT("*.c;*.cpp;*.cxx;*.cc;*.h;*.hpp;*.hxx;*.hh;*.inl;*.inc"));
    filters.Add(wxT("All Files"));
    m_extensions.Add(wxT("*.*"));
    
    wxBoxSizer *horzSizer = new wxBoxSizer(wxHORIZONTAL);
    
    wxStaticText *text = new wxStaticText(this, wxID_ANY, wxT("Find:"));
    horzSizer->Add(text, 0, wxRIGHT|wxLEFT|wxALIGN_CENTER_VERTICAL, 5);
    
    for (size_t i = 0; i < sizeof(tasks)/sizeof(tasks[0]); i++) {
        wxToggleButton *btn = new wxToggleButton(this, wxID_ANY, tasks[i], wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
        btn->SetValue(true);
        btn->SetBackgroundColour(wxColor(wxT("GOLD")));
        m_task.push_back(btn);
        horzSizer->Add(btn, 0, wxRIGHT|wxLEFT|wxALIGN_CENTER_VERTICAL, 2);
    }
    
    text = new wxStaticText(this, wxID_ANY, wxT("In:"));
    horzSizer->Add(text, 0, wxRIGHT|wxLEFT|wxALIGN_CENTER_VERTICAL, 5);
    
    m_scope = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, scopes);
    m_scope->SetSelection(0);
    horzSizer->Add(m_scope, 1, wxRIGHT|wxLEFT|wxALIGN_CENTER_VERTICAL, 2);
    
    m_filter = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, filters);
    m_filter->SetSelection(0);
    horzSizer->Add(m_filter, 1, wxRIGHT|wxLEFT|wxALIGN_CENTER_VERTICAL, 2);
    
    wxButton *btn = new wxButton(this, XRCID("search"), wxT("&Search"));
    horzSizer->Add(btn, 0, wxRIGHT|wxLEFT|wxALIGN_CENTER_VERTICAL, 5);
    
    btn = new wxButton(this, XRCID("customize"), wxT("&Customize"));
    horzSizer->Add(btn, 0, wxRIGHT|wxLEFT|wxALIGN_CENTER_VERTICAL, 2);
    
	wxBoxSizer *vertSizer = new wxBoxSizer(wxVERTICAL);
	vertSizer->Add(horzSizer, 0, wxEXPAND|wxTOP|wxBOTTOM);

	// grab the base class scintilla and put our sizer in its place
	wxSizer *mainSizer = GetSizer();
	mainSizer->Detach(m_sci);
	vertSizer->Add(m_sci, 1, wxEXPAND | wxALL, 1);

	mainSizer->Add(vertSizer, 1, wxEXPAND | wxALL, 1);
	mainSizer->Layout();
}

TaskPanel::~TaskPanel()
{
}

void TaskPanel::DoSetSearchData()
{
    m_data.SetDisplayScope(true);
    m_data.SetRegularExpression(true);
    m_data.SetMatchCase(false);
    m_data.SetMatchWholeWord(false);
    m_data.SetUseEditorFontConfig(true);
    m_data.SetOutputTab(0);
    m_data.SetOwner(this);
    
    wxString find = wxT("/[/*]\\s*(?:");
    for (size_t i = 0; i < m_task.size(); i++) {
        if (m_task[i]->GetValue()) {
            find << m_task[i]->GetLabelText() << wxT('|');
        }
    }
    if (find.Last() == wxT('(')) {
        // fallback
        find << wxT("TODO|ATTN|FIXME|BUG|");
    }
    find.Last() = wxT(')');
    find << wxT("(?=:)");
    m_data.SetFindString(find);
    
    m_data.SetRootDir(m_scope->GetStringSelection());
    wxArrayString files;
	if (m_scope->GetStringSelection() == SEARCH_IN_WORKSPACE) {
		ManagerST::Get()->GetWorkspaceFiles(files);
	}else if (m_scope->GetStringSelection() == SEARCH_IN_PROJECT) {
		ManagerST::Get()->GetProjectFiles(ManagerST::Get()->GetActiveProjectName(), files);
	}else if (m_scope->GetStringSelection() == SEARCH_IN_CURR_FILE_PROJECT) {
		wxString project = ManagerST::Get()->GetActiveProjectName();
		if (Frame::Get()->GetMainBook()->GetActiveEditor()) {
			wxFileName activeFile = Frame::Get()->GetMainBook()->GetActiveEditor()->GetFileName();
			project = ManagerST::Get()->GetProjectNameByFile(activeFile.GetFullPath());
		}
		ManagerST::Get()->GetProjectFiles(project, files);
	}
    m_data.SetFiles(files);
    
    m_data.SetExtensions(m_extensions[m_filter->GetSelection()]);
}

void TaskPanel::OnToggle(wxCommandEvent &e)
{
    wxToggleButton *btn = (wxToggleButton*) e.GetEventObject();
    btn->SetBackgroundColour(e.IsChecked() ? wxColor(wxT("GOLD")) : wxNullColour);
    btn->Refresh();
}

void TaskPanel::OnSearch(wxCommandEvent& e)
{
    DoSetSearchData();
    OnRepeatOutput(e);
}

void TaskPanel::OnSearchUI(wxUpdateUIEvent& e)
{
    bool any = false;
    for (size_t i = 0; i < m_task.size() && !any; i++) {
        any = m_task[i]->GetValue();
    }
    e.Enable(m_recv == NULL && any);
}

void TaskPanel::OnCustomize(wxCommandEvent& e)
{
    LoadFindInFilesData();
    DoSetSearchData();
    m_find->SetSearchData(m_data);
    LEditor *editor = Frame::Get()->GetMainBook()->GetActiveEditor();
    if (editor) {
        // remove selection so it doesn't clobber the find-string in the dialog
        editor->SetSelection(-1,-1);
    }
    OnFindInFiles(e);
}

void TaskPanel::OnCustomizeUI(wxUpdateUIEvent& e)
{
    OnSearchUI(e);
}
