//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : abbreviationssettingsdlg.cpp
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

#include <wx/msgdlg.h>
#include <wx/textdlg.h>
#include "abbreviationentry.h"
#include "imanager.h"
#include "windowattrmanager.h"
#include "abbreviationssettingsdlg.h"
#include "cl_config.h"
#include <wx/filedlg.h>
#include <wx/dirdlg.h>
#include <macrosdlg.h>

AbbreviationsSettingsDlg::AbbreviationsSettingsDlg( wxWindow* parent, IManager *mgr )
    : AbbreviationsSettingsBase( parent )
    , m_mgr(mgr)
    , m_data()
    , m_dirty(false)
    , m_currSelection(wxNOT_FOUND)
    , m_config("abbreviations.conf")
{
    SetName("AbbreviationsSettingsDlg");
    WindowAttrManager::Load(this);
    if ( !m_config.ReadItem(&m_data) ) {
        // merge the data from the old configuration
        AbbreviationEntry data;
        m_mgr->GetConfigTool()->ReadObject(wxT("AbbreviationsData"), &data);

        m_data.SetAutoInsert( data.GetAutoInsert() );
        m_data.SetEntries( data.GetEntries() );
        m_config.WriteItem( &m_data );
    }
    DoPopulateItems();
}

AbbreviationsSettingsDlg::~AbbreviationsSettingsDlg()
{
    
}

void AbbreviationsSettingsDlg::OnItemSelected( wxCommandEvent& event )
{
    if(m_dirty) {
        DoSaveCurrent();
    }
    DoSelectItem(event.GetSelection());
}

void AbbreviationsSettingsDlg::OnNew(wxCommandEvent& e)
{
    if(m_dirty) {
        DoSaveCurrent();
    }

    wxString name = wxGetTextFromUser(_("Abbreviation Name:"), _("New abbreviation..."), wxT(""), this);
    if(name.IsEmpty() == false) {
        if( m_listBoxAbbreviations->FindString(name) != wxNOT_FOUND ) {
            wxMessageBox(wxString::Format(_("An abbreviation with this name already exists!")));
            return;
        }

        int where = m_listBoxAbbreviations->Append(name);
        m_listBoxAbbreviations->SetSelection(where);

        m_activeItemName = name;
        m_currSelection = where;

        m_textCtrlName->SetValue(name);
        m_stc->SetText("");
        m_stc->SetFocus();
    }
}

void AbbreviationsSettingsDlg::OnDelete(wxCommandEvent& event)
{
    if(m_activeItemName.IsEmpty() || m_currSelection == wxNOT_FOUND) {
        return;
    }

    if(wxMessageBox(wxString::Format(_("Are you sure you want to delete '%s'"), m_activeItemName.c_str()),
                    _("CodeLite"), wxYES_NO|wxCANCEL|wxCENTER|wxICON_QUESTION, this) != wxYES) {
        return;
    }

    // delete the entry from the configuration file
    DoDeleteEntry(m_activeItemName);
    
    // delete it
    m_listBoxAbbreviations->Delete((unsigned int) m_currSelection);
    m_stc->Clear();
    m_textCtrlName->Clear();

    // select the previous item in the list
    if(m_listBoxAbbreviations->IsEmpty() == false) {
        switch(m_currSelection) {
        case 0:
            m_currSelection = 0;
            m_activeItemName = m_listBoxAbbreviations->GetString(0);
            break;
        default:
            m_currSelection--;
            m_activeItemName = m_listBoxAbbreviations->GetString(m_currSelection);
            break;
        }
    } else {
        m_activeItemName.Empty();
        m_currSelection = wxNOT_FOUND;
    }

    if(m_currSelection != wxNOT_FOUND) {
        m_listBoxAbbreviations->SetSelection(m_currSelection);
        DoSelectItem(m_currSelection);
    }
    m_dirty = true;
}

void AbbreviationsSettingsDlg::OnDeleteUI(wxUpdateUIEvent& event)
{
    event.Enable(m_currSelection != wxNOT_FOUND && m_activeItemName.IsEmpty() == false);
}

void AbbreviationsSettingsDlg::OnSave(wxCommandEvent& event)
{
    wxUnusedVar(event);
    if(m_dirty) {
        DoSaveCurrent();
    }
    m_data.SetAutoInsert(m_checkBoxImmediateInsert->IsChecked());
    m_config.WriteItem( &m_data );
}

void AbbreviationsSettingsDlg::DoPopulateItems()
{
    m_listBoxAbbreviations->Clear();
    m_stc->ClearAll();
    wxStringMap_t entries = m_data.GetEntries();
    wxStringMap_t::iterator iter = entries.begin();
    for(; iter != entries.end(); ++iter) {
        m_listBoxAbbreviations->Append(iter->first);
    }

    if(m_listBoxAbbreviations->IsEmpty() == false) {
        m_listBoxAbbreviations->Select(0);
        DoSelectItem(0);
    }

    m_checkBoxImmediateInsert->SetValue(m_data.IsAutoInsert());
    m_dirty = false;

}

void AbbreviationsSettingsDlg::DoSelectItem(int item)
{
    if(item >= 0) {
        wxString name = m_listBoxAbbreviations->GetString(item);
        m_activeItemName = name;
        m_currSelection = item;

        m_textCtrlName->SetValue(name);

        wxStringMap_t entries = m_data.GetEntries();
        wxStringMap_t::iterator iter = entries.find(name);
        if(iter != entries.end()) {
            m_stc->SetText(iter->second);
        }
        m_dirty = false;
    }
}

void AbbreviationsSettingsDlg::DoSaveCurrent()
{
    if(m_currSelection == wxNOT_FOUND) {
        return;
    }

    // search for the old item
    wxStringMap_t entries = m_data.GetEntries();
    wxStringMap_t::iterator iter = entries.find(m_activeItemName);
    if(iter != entries.end()) {
        entries.erase( iter );
    }

    // insert the new item
    entries[m_textCtrlName->GetValue()] = m_stc->GetText();
    m_data.SetEntries(entries);

    m_activeItemName = m_textCtrlName->GetValue();

    // update the name in the list box
    m_listBoxAbbreviations->SetString((unsigned int) m_currSelection, m_activeItemName);
    m_dirty = false;

    m_textCtrlName->SetFocus();
}

void AbbreviationsSettingsDlg::DoDeleteEntry(const wxString &name)
{
    // search for the old item
    wxStringMap_t entries = m_data.GetEntries();
    wxStringMap_t::iterator iter = entries.find(name);
    if(iter != entries.end()) {
        entries.erase( iter );
    }
    m_data.SetEntries(entries);
}

void AbbreviationsSettingsDlg::OnSaveUI(wxUpdateUIEvent& event)
{
    event.Enable(m_dirty);
}

void AbbreviationsSettingsDlg::OnMarkDirty(wxStyledTextEvent& event)
{
    event.Skip();
    if(m_activeItemName.IsEmpty() == false) {
        m_dirty = true;
    }
}

void AbbreviationsSettingsDlg::OnExport(wxCommandEvent& event)
{
    wxString path = ::wxDirSelector();
    if ( path.IsEmpty() )
        return;
    
    // Construct the output file name
    wxFileName fn(path, "abbreviations.conf");
    if ( fn.FileExists() ) {
        if ( ::wxMessageBox(_("This folder already contains a file named 'abbreviations.conf' - would you like to overrite it?"), "wxCrafter", wxYES_NO|wxCANCEL|wxCENTER|wxICON_QUESTION) != wxYES )
            return;
    }
    m_config.Save( fn );
    ::wxMessageBox(_("Abbreviations were exported to '") + fn.GetFullPath() + _("'"), "wxCrafter", wxICON_INFORMATION|wxOK);
}

void AbbreviationsSettingsDlg::OnImport(wxCommandEvent& event)
{
    wxString path = ::wxFileSelector();
    if ( path.IsEmpty() )
        return;
    
    // load the imported configuration
    clConfig cfg(path);
    AbbreviationJSONEntry data, curData;
    if ( !cfg.ReadItem( &data ) ) {
        ::wxMessageBox(_("The file does not seem to contain a valid abbreviations entries"), "wxCrafter", wxOK|wxICON_WARNING|wxCENTER);
        return;
    }
    const wxStringMap_t &newEntries = data.GetEntries();
    const wxStringMap_t &curEntries = m_data.GetEntries();
    
    wxStringMap_t merged = m_config.MergeStringMaps(newEntries, curEntries);
    m_data.SetEntries( merged );
    m_config.WriteItem( &m_data );
    m_dirty = false;
    
    // Repopulate the abbreviations
    DoPopulateItems();
    
    ::wxMessageBox(_("Abbreviations imported successfully!"));
    
}
void AbbreviationsSettingsDlg::OnHelp(wxCommandEvent& event)
{
    MacrosDlg dlg(this, MacrosDlg::MacrosProject, NULL, NULL);
    dlg.ShowModal();
}

void AbbreviationsSettingsDlg::OnImmediateInsert(wxCommandEvent& event)
{
    m_dirty = true;
}
