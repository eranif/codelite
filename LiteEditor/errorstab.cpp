//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : buidltab.cpp
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
#include "errorstab.h"
#include "editor_config.h"


ErrorsTab::ErrorsTab(BuildTab *bt, wxWindow *parent, wxWindowID id, const wxString &name)
    : OutputTabWindow(parent, id, name)
    , m_bt(bt)
{
    m_tb->DeleteTool(XRCID("clear_all_output"));
    m_tb->AddSeparator();

    m_tb->AddCheckTool(XRCID("show_errors"), wxT("Errors"), wxXmlResource::Get()->LoadBitmap(wxT("project_conflict")), wxNullBitmap, wxT("Show build errors"));
    m_tb->ToggleTool(XRCID("show_errors"), true);
    m_tb->Connect(XRCID("show_errors"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(ErrorsTab::OnRedisplayLines), NULL, this);

    m_tb->AddCheckTool(XRCID("show_warnings"), wxT("Warnings"), wxXmlResource::Get()->LoadBitmap(wxT("help_icon")), wxNullBitmap, wxT("Show build warnings"));
    m_tb->ToggleTool(XRCID("show_warnings"), true);
    m_tb->Connect(XRCID("show_warnings"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(ErrorsTab::OnRedisplayLines), NULL, this);

    m_tb->AddCheckTool(XRCID("show_build_lines"), wxT("Build"), wxXmlResource::Get()->LoadBitmap(wxT("todo")), wxNullBitmap, wxT("Show build status lines"));
    m_tb->ToggleTool(XRCID("show_build_lines"), false);
    m_tb->Connect(XRCID("show_build_lines"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(ErrorsTab::OnRedisplayLines), NULL, this);
	m_tb->Realize();
}

ErrorsTab::~ErrorsTab()
{
}

void ErrorsTab::ClearLines()
{
    Clear();
    m_lineMap.clear();
	BuildTabSettingsData options;
	EditorConfigST::Get()->ReadObject(wxT("build_tab_settings"), &options);
    BuildTab::SetStyles(m_sci, options);
}

bool ErrorsTab::IsShowing(int linecolor)
{
    switch (linecolor) {
        case wxSCI_LEX_GCC_BUILDING:
            return m_tb->GetToolState(XRCID("show_build_lines"));
        case wxSCI_LEX_GCC_ERROR:
            return m_tb->GetToolState(XRCID("show_errors"));
        case wxSCI_LEX_GCC_WARNING:
            return m_tb->GetToolState(XRCID("show_warnings"));
    }
    return false;
}

void ErrorsTab::AppendLine(int line)
{
    std::map<int,BuildTab::LineInfo>::iterator i = m_bt->m_lineInfo.find(line);
    if (i != m_bt->m_lineInfo.end() && IsShowing(i->second.linecolor)) {
        m_lineMap[m_sci->GetLineCount()-1] = line;
        AppendText(i->second.linetext);
    }
}

void ErrorsTab::MarkLine(int line)
{
    std::map<int,BuildTab::LineInfo>::iterator i = m_bt->m_lineInfo.find(line);
    if (i == m_bt->m_lineInfo.end() || !IsShowing(i->second.linecolor))
        return;
    for (std::map<int,int>::iterator j = m_lineMap.begin(); j != m_lineMap.end(); j++) {
        if (j->second == line) {
            m_sci->MarkerDeleteAll(0x7);
            m_sci->MarkerAdd(j->first, 0x7);
            m_sci->SetCurrentPos(m_sci->PositionFromLine(j->first));
            m_sci->SetSelection(-1, m_sci->GetCurrentPos());
            m_sci->EnsureCaretVisible();
        }
    }
}

void ErrorsTab::OnRedisplayLines(wxCommandEvent& e)
{
    wxUnusedVar(e);

    int marked = -1;
    ClearLines();
    for (int i = 0; i < m_bt->m_sci->GetLineCount(); i++) {
        AppendLine(i);
        if (m_bt->m_sci->MarkerGet(i) & 1<<0x7) {
            marked = i;
        }
    }
    if (marked >= 0) {
        MarkLine(marked);
    }
}

void ErrorsTab::OnHotspotClicked(wxScintillaEvent &e)
{
	OnMouseDClick(e);
}

void ErrorsTab::OnMouseDClick(wxScintillaEvent &e)
{
    m_sci->SetSelection(-1, m_sci->GetCurrentPos());
    std::map<int,int>::iterator i = m_lineMap.find(m_sci->LineFromPosition(e.GetPosition()));
    if (i != m_lineMap.end()) {
        m_bt->DoMarkAndOpenFile(m_bt->m_lineInfo.find(i->second), true);
    }
}

