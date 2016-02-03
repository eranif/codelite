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

CscopeTab::CscopeTab( wxWindow* parent, IManager *mgr )
    : CscopeTabBase( parent )
    , m_table(NULL)
    , m_mgr(mgr)
{
    m_bitmaps = clGetManager()->GetStdIcons()->MakeStandardMimeMap();

    CScopeConfData data;
    m_mgr->GetConfigTool()->ReadObject(wxT("CscopeSettings"), &data);

    const wxString SearchScope[] = { wxTRANSLATE("Entire Workspace"), wxTRANSLATE("Active Project") };
    m_stringManager.AddStrings(sizeof(SearchScope)/sizeof(wxString), SearchScope, data.GetScanScope(), m_choiceSearchScope);

    wxFont defFont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
    m_font = wxFont( defFont.GetPointSize(), wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);

    m_checkBoxUpdateDb->SetValue(data.GetRebuildOption());
    m_checkBoxRevertedIndex->SetValue(data.GetBuildRevertedIndexOption());
    SetMessage(_("Ready"), 0);

    Clear(); // To make the Clear button UpdateUI work initially
    EventNotifier::Get()->Connect(wxEVT_CL_THEME_CHANGED, wxCommandEventHandler(CscopeTab::OnThemeChanged), NULL, this);
}

CscopeTab::~CscopeTab()
{
    EventNotifier::Get()->Disconnect(wxEVT_CL_THEME_CHANGED, wxCommandEventHandler(CscopeTab::OnThemeChanged), NULL, this);
}

void CscopeTab::OnItemActivated(wxDataViewEvent& event)
{
    event.Skip();
    DoItemActivated( event.GetItem() );
}

void CscopeTab::Clear()
{
    if (m_table) {
        //free the old table
        FreeTable();
    }
    m_dataviewModel->Clear();
}

void CscopeTab::BuildTable(CScopeResultTable_t *table)
{
    if ( !table ) {
        return;
    }

    if (m_table) {
        // Free the old table
        FreeTable();
    }

    m_table = table;
    m_dataviewModel->Clear();

    wxStringSet_t insertedItems;
    CScopeResultTable_t::iterator iter = m_table->begin();
    for (; iter != m_table->end(); ++iter ) {
        wxString file = iter->first;

        wxVector<wxVariant> cols;
        cols.push_back( CScoptViewResultsModel::CreateIconTextVariant(file, GetBitmap(file)) );
        cols.push_back(wxString());
        cols.push_back(wxString());
        wxDataViewItem fileItem = m_dataviewModel->AppendItem( wxDataViewItem(0), cols, NULL);

        // Add the entries for this file
        CScopeEntryDataVec_t* vec = iter->second;
        for ( size_t i=0; i<vec->size(); ++i ) {
            CscopeEntryData entry = vec->at(i);
            // Dont insert duplicate entries to the match view
            wxString display_string;
            display_string << _("Line: ") << entry.GetLine() << wxT(", ") << entry.GetScope() << wxT(", ") << entry.GetPattern();
            if(insertedItems.find(display_string) == insertedItems.end()) {
                insertedItems.insert(display_string);
                cols.clear();
                cols.push_back( CScoptViewResultsModel::CreateIconTextVariant(entry.GetScope(), wxNullBitmap) );
                cols.push_back( (wxString() << entry.GetLine()) );
                cols.push_back( (wxString() << entry.GetPattern()) );
                m_dataviewModel->AppendItem( fileItem, cols, new CscopeTabClientData(entry) );
            }
        }
    }
    FreeTable();
}

void CscopeTab::FreeTable()
{
    if (m_table) {
        CScopeResultTable_t::iterator iter = m_table->begin();
        for (; iter != m_table->end(); iter++ ) {
            //delete the vector
            delete iter->second;
        }
        m_table->clear();
        wxDELETE(m_table);
    }
}

void CscopeTab::DoItemActivated(const wxDataViewItem& item )
{
    CscopeTabClientData *data = dynamic_cast<CscopeTabClientData*>(m_dataviewModel->GetClientObject(item));
    if (data) {
        wxString wsp_path = clCxxWorkspaceST::Get()->GetPrivateFolder();
        //a single entry was activated, open the file
        //convert the file path to absolut path. We do it here, to improve performance
        wxFileName fn(data->GetEntry().GetFile());

        if ( !fn.MakeAbsolute(wsp_path) ) {
            wxLogMessage(wxT("failed to convert file to absolute path"));
        }

        if(m_mgr->OpenFile(fn.GetFullPath(), wxEmptyString, data->GetEntry().GetLine()-1)) {
            IEditor *editor = m_mgr->GetActiveEditor();
            if( editor && editor->GetFileName().GetFullPath() == fn.GetFullPath() && !GetFindWhat().IsEmpty()) {
                // We can't use data->GetEntry().GetPattern() as the line to search for as any appended comments have been truncated
                // For some reason LEditor::DoFindAndSelect checks it against the whole current line
                // and won't believe a match unless their lengths are the same
                int line = data->GetEntry().GetLine() - 1;
                int start = editor->PosFromLine(line);	// PosFromLine() returns the line start position
                int end = editor->LineEnd(line);
                wxString searchline(editor->GetTextRange(start, end));
                // Find and select the entry in the file
                editor->FindAndSelectV(searchline, GetFindWhat(), start); // The async version of FindAndSelect()
                editor->DelayedSetActive(); // We need to SetActive() editor. At least in wxGTK, this won't work synchronously
            }
        }
    } else {
        // Parent item, expand it
        m_dataview->Expand( item );
    }
}

void CscopeTab::SetMessage(const wxString &msg, int percent)
{
    m_statusMessage->SetLabel(msg);
    m_gauge->SetValue(percent);
}

void CscopeTab::OnClearResults(wxCommandEvent &e)
{
    wxUnusedVar(e);
    SetMessage(_("Ready"), 0);
    Clear();
}

void CscopeTab::OnClearResultsUI(wxUpdateUIEvent& e)
{
    CHECK_CL_SHUTDOWN();
    e.Enable( !m_dataviewModel->IsEmpty() );
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

void CscopeTab::OnCreateDB(wxCommandEvent &e)
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
    m_dataview->SetBackgroundColour( DrawingUtils::GetOutputPaneBgColour() );
    m_dataview->SetForegroundColour( DrawingUtils::GetOutputPaneFgColour());
}

wxBitmap CscopeTab::GetBitmap(const wxString& filename) const
{
    wxFileName fn(filename);
    FileExtManager::FileType type = FileExtManager::GetType(filename);
    if ( m_bitmaps.count(type) == 0 ) {
        type = FileExtManager::TypeText;
    }
    return m_bitmaps.find(type)->second;
}
void CscopeTab::OnItemSelected(wxDataViewEvent& event)
{
    // Expand parent items on selection
    CscopeTabClientData *data = dynamic_cast<CscopeTabClientData*>(m_dataviewModel->GetClientObject(event.GetItem()));
    if ( !data ) {
        // Parent item, expand it
        if ( m_dataview->IsExpanded( event.GetItem() ) ) {
            m_dataview->Collapse( event.GetItem() );
        } else {
            m_dataview->Expand( event.GetItem() );
        }
        
    } else {
        event.Skip();
    }
}
