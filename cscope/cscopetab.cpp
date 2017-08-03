//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : cscopetab.cpp
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
#include "csscopeconfdata.h"
#include "globals.h"
#include "plugin.h"
#include <wx/app.h>
#include <wx/log.h>
#include "cscopetab.h"
#include "drawingutils.h"
#include "cscopedbbuilderthread.h"
#include "imanager.h"
#include "fileextmanager.h"
#include "workspace.h"
#include "bitmap_loader.h"
#include <wx/treectrl.h>
#include <wx/imaglist.h>
#include <set>
#include "drawingutils.h"
#include "event_notifier.h"

CscopeTab::CscopeTab(wxWindow* parent, IManager* mgr)
    : CscopeTabBase(parent)
    , m_table(NULL)
    , m_mgr(mgr)
{
    m_styler.Reset(new clFindResultsStyler(m_stc));
    m_bitmaps = clGetManager()->GetStdIcons()->MakeStandardMimeMap();

    CScopeConfData data;
    m_mgr->GetConfigTool()->ReadObject(wxT("CscopeSettings"), &data);

    const wxString SearchScope[] = { wxTRANSLATE("Entire Workspace"), wxTRANSLATE("Active Project") };
    m_stringManager.AddStrings(
        sizeof(SearchScope) / sizeof(wxString), SearchScope, data.GetScanScope(), m_choiceSearchScope);

    wxFont defFont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
    m_font = wxFont(defFont.GetPointSize(), wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);

    m_checkBoxUpdateDb->SetValue(data.GetRebuildOption());
    m_checkBoxRevertedIndex->SetValue(data.GetBuildRevertedIndexOption());
    SetMessage(_("Ready"), 0);

    Clear(); // To make the Clear button UpdateUI work initially
    EventNotifier::Get()->Connect(wxEVT_CL_THEME_CHANGED, wxCommandEventHandler(CscopeTab::OnThemeChanged), NULL, this);
}

CscopeTab::~CscopeTab()
{
    EventNotifier::Get()->Disconnect(
        wxEVT_CL_THEME_CHANGED, wxCommandEventHandler(CscopeTab::OnThemeChanged), NULL, this);
}

void CscopeTab::Clear()
{
    FreeTable();
    m_stc->SetEditable(true);
    m_stc->ClearAll();
    m_stc->SetEditable(false);
    m_matchesInStc.clear();
}

void CscopeTab::BuildTable(CScopeResultTable_t* table)
{
    CHECK_PTR_RET(table);
    // Free the old table
    FreeTable();

    m_table = table;
    ClearText();
    m_matchesInStc.clear();
    m_styler->SetStyles(m_stc);

    wxStringSet_t insertedItems;
    CScopeResultTable_t::iterator iter = m_table->begin();
    for(; iter != m_table->end(); ++iter) {
        wxString file = iter->first;

        // Add line for the file
        AddFile(file);

        // Add the entries for this file
        CScopeEntryDataVec_t* vec = iter->second;
        for(size_t i = 0; i < vec->size(); ++i) {
            CscopeEntryData entry = vec->at(i);
            // Dont insert duplicate entries to the match view
            wxString display_string;
            display_string << _("Line: ") << entry.GetLine() << wxT(", ") << entry.GetScope() << wxT(", ")
                           << entry.GetPattern();
            if(insertedItems.count(display_string) == 0) {
                insertedItems.insert(display_string);
                int lineno = m_stc->GetLineCount() - 1; // STC line number *before* we add the result
                AddMatch(entry.GetLine(), entry.GetPattern());
                m_matchesInStc.insert(std::make_pair(lineno, entry));
            }
        }
    }
    FreeTable();
}

void CscopeTab::FreeTable()
{
    if(m_table) {
        CScopeResultTable_t::iterator iter = m_table->begin();
        for(; iter != m_table->end(); iter++) {
            // delete the vector
            delete iter->second;
        }
        m_table->clear();
        wxDELETE(m_table);
    }
}

void CscopeTab::SetMessage(const wxString& msg, int percent)
{
    if(m_mgr->GetStatusBar()) {
        m_mgr->GetStatusBar()->SetMessage(msg, 3);
    }
    m_gauge->SetValue(percent);
}

void CscopeTab::OnClearResults(wxCommandEvent& e)
{
    wxUnusedVar(e);
    SetMessage(_("Ready"), 0);
    Clear();
}

void CscopeTab::OnClearResultsUI(wxUpdateUIEvent& e)
{
    CHECK_CL_SHUTDOWN();
    e.Enable(!m_stc->IsEmpty());
}

void CscopeTab::OnChangeSearchScope(wxCommandEvent& e)
{
    CScopeConfData data;
    m_mgr->GetConfigTool()->ReadObject(wxT("CscopeSettings"), &data);
    // update the settings
    data.SetScanScope(m_stringManager.GetStringSelection());
    data.SetRebuildDbOption(m_checkBoxUpdateDb->IsChecked());
    data.SetBuildRevertedIndexOption(m_checkBoxRevertedIndex->IsChecked());
    // store the object
    m_mgr->GetConfigTool()->WriteObject(wxT("CscopeSettings"), &data);
}

void CscopeTab::OnCreateDB(wxCommandEvent& e)
{
    // There's no easy way afaict to reach the class Cscope direct, so...
    e.SetId(XRCID("cscope_create_db"));
    e.SetEventType(wxEVT_COMMAND_MENU_SELECTED);
    wxPostEvent(m_mgr->GetTheApp(), e);
}

void CscopeTab::OnWorkspaceOpenUI(wxUpdateUIEvent& e)
{
    CHECK_CL_SHUTDOWN();
    e.Enable(m_mgr->IsWorkspaceOpen());
}

void CscopeTab::OnThemeChanged(wxCommandEvent& e)
{
    e.Skip();
    m_styler->SetStyles(m_stc);
}

wxBitmap CscopeTab::GetBitmap(const wxString& filename) const
{
    wxFileName fn(filename);
    FileExtManager::FileType type = FileExtManager::GetType(filename);
    if(m_bitmaps.count(type) == 0) {
        type = FileExtManager::TypeText;
    }
    return m_bitmaps.find(type)->second;
}

void CscopeTab::ClearText()
{
    m_stc->SetEditable(true);
    m_stc->ClearAll();
    m_stc->SetEditable(false);
}

void CscopeTab::AddMatch(int line, const wxString& pattern)
{
    m_stc->SetEditable(true);
    wxString linenum = wxString::Format(wxT(" %5d: "), line);
    m_stc->AppendText(linenum + pattern + "\n");
    m_stc->SetEditable(false);
}

void CscopeTab::AddFile(const wxString& filename)
{
    m_stc->SetEditable(true);
    m_stc->AppendText(filename + "\n");
    m_stc->SetEditable(false);
}

void CscopeTab::OnHotspotClicked(wxStyledTextEvent& e)
{
    CHECK_PTR_RET(clCxxWorkspaceST::Get()->IsOpen());

    int clickedLine;
    int style = m_styler->HitTest(e, clickedLine);
    if(style == clFindResultsStyler::LEX_FIF_FILE || style == clFindResultsStyler::LEX_FIF_HEADER) {
        // Toggle
        m_stc->ToggleFold(clickedLine);
    } else {
        // Open the match
        std::map<int, CscopeEntryData>::const_iterator iter = m_matchesInStc.find(clickedLine);
        if(iter != m_matchesInStc.end()) {
            wxString wsp_path = clCxxWorkspaceST::Get()->GetPrivateFolder();
            wxFileName fn(iter->second.GetFile());
            if(!fn.MakeAbsolute(wsp_path)) {
                wxLogMessage(wxT("CScope: failed to convert file to absolute path"));
                return;
            }
            m_mgr->OpenFile(fn.GetFullPath(), "", iter->second.GetLine() - 1);

        // In theory this isn't needed as it happened in OpenFile()
        // In practice there's a timing issue: if the file needs to be loaded,
        // the CenterLine() call arrives too soon. So repeat it here, delayed.
        CallAfter(&CscopeTab::CenterEditorLine, iter->second.GetLine() - 1);
        }
    }
}

void CscopeTab::CenterEditorLine(int lineno)
{
    IEditor* editor = m_mgr->GetActiveEditor();
    if (editor) {
        editor->CenterLine(lineno);
    }
}
