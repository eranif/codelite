//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : findinfilesdlg.cpp
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
#include "search_thread.h"
#include <wx/tokenzr.h>
#include <wx/fontmap.h>
#include "event_notifier.h"
#include "dirpicker.h"
#include "manager.h"
#include "frame.h"
#include "macros.h"
#include "findinfilesdlg.h"
#include "findresultstab.h"
#include "replaceinfilespanel.h"
#include "windowattrmanager.h"

FindInFilesDialog::FindInFilesDialog(wxWindow* parent, const wxString &dataName)
    : FindInFilesDialogBase(parent, wxID_ANY)
{
    m_data.SetName(dataName);

    // Store the find-in-files data
    clConfig::Get().ReadItem( &m_data );

    wxArrayString choices;
    size_t count = m_data.GetSearchPaths().GetCount();
    for (size_t i = 0; i < count; ++i) {
        choices.Add(m_data.GetSearchPaths().Item(i));
    }

    // add the default search paths
    if(choices.Index(wxGetTranslation(SEARCH_IN_WORKSPACE)) == wxNOT_FOUND)
        choices.Add(wxGetTranslation(SEARCH_IN_WORKSPACE));

    if(choices.Index(wxGetTranslation(SEARCH_IN_PROJECT)) == wxNOT_FOUND)
        choices.Add(wxGetTranslation(SEARCH_IN_PROJECT));

    if(choices.Index(wxGetTranslation(SEARCH_IN_CURR_FILE_PROJECT)) == wxNOT_FOUND)
        choices.Add(wxGetTranslation(SEARCH_IN_CURR_FILE_PROJECT));

    if(choices.Index(wxGetTranslation(SEARCH_IN_CURRENT_FILE)) == wxNOT_FOUND)
        choices.Add(wxGetTranslation(SEARCH_IN_CURRENT_FILE));
    
    if(choices.Index(wxGetTranslation(SEARCH_IN_OPEN_FILES)) == wxNOT_FOUND)
        choices.Add(wxGetTranslation(SEARCH_IN_OPEN_FILES));

    int initial = m_data.GetSearchScope();
    if ((initial == wxNOT_FOUND) || ((size_t)initial >= count)) {
        initial = 0;
    }

    m_dirPicker->SetValues(choices, initial);

    // Search for
    m_findString->Clear();
    m_findString->Append(m_data.GetFindStringArr());
    m_findString->SetValue(m_data.GetFindString());

    m_fileTypes->SetSelection(0);

    m_matchCase->SetValue(m_data.GetFlags() & wxFRD_MATCHCASE);
    m_matchWholeWord->SetValue(m_data.GetFlags() & wxFRD_MATCHWHOLEWORD);
    m_regualrExpression->SetValue(m_data.GetFlags() & wxFRD_REGULAREXPRESSION);
    m_printScope->SetValue(m_data.GetFlags() & wxFRD_DISPLAYSCOPE);
    m_checkBoxSaveFilesBeforeSearching->SetValue(m_data.GetFlags() & wxFRD_SAVE_BEFORE_SEARCH);
    m_checkBoxSeparateTab->SetValue(m_data.GetFlags() & wxFRD_SEPARATETAB_DISPLAY);
    m_checkBoxSkipMatchesFoundInComments->SetValue(m_data.GetFlags() & wxFRD_SKIP_COMMENTS);
    m_checkBoxSkipMatchesFoundInStrings->SetValue(m_data.GetFlags() & wxFRD_SKIP_STRINGS);
    m_checkBoxHighlighStringComments->SetValue(m_data.GetFlags() & wxFRD_COLOUR_COMMENTS);

    // Set encoding
    wxArrayString  astrEncodings;
    wxFontEncoding fontEnc;
    int            selection(0);

    size_t iEncCnt = wxFontMapper::GetSupportedEncodingsCount();
    for (size_t i = 0; i < iEncCnt; i++) {
        fontEnc = wxFontMapper::GetEncoding(i);
        if (wxFONTENCODING_SYSTEM == fontEnc) { // skip system, it is changed to UTF-8 in optionsconfig
            continue;
        }
        wxString encodingName = wxFontMapper::GetEncodingName(fontEnc);
        size_t pos = astrEncodings.Add(encodingName);

        if(m_data.GetEncoding() == encodingName) {
            selection = static_cast<int>(pos);
        }
    }

    m_choiceEncoding->Append(astrEncodings);
    if(m_choiceEncoding->IsEmpty() == false)
        m_choiceEncoding->SetSelection(selection);

    // Set the file mask
    DoSetFileMask();

    GetSizer()->Fit(this);

    WindowAttrManager::Load(this, "FindInFilesDialog", NULL);
    Centre();
}

FindInFilesDialog::~FindInFilesDialog()
{
    clConfig::Get().WriteItem( &m_data );
    WindowAttrManager::Save(this, "FindInFilesDialog", NULL);
}

void FindInFilesDialog::SetRootDir(const wxString &rootDir)
{
    m_dirPicker->SetPath(rootDir);
}

void FindInFilesDialog::DoSetFileMask()
{
    // First send an event to the plugins asking for an additional file mask
    clCommandEvent getFileMaskEvent(wxEVT_CMD_GET_FIND_IN_FILES_MASK, GetId());
    getFileMaskEvent.SetEventObject(this);
    EventNotifier::Get()->ProcessEvent(getFileMaskEvent);

    // Get the output
    wxArrayString fileTypes = m_data.GetFileMask();
    m_pluginFileMask = getFileMaskEvent.GetStrings();
    if( !fileTypes.IsEmpty() ) {
        
        m_fileTypes->Clear();
        m_fileTypes->Append(m_pluginFileMask);
        m_fileTypes->Append(fileTypes);
        
        int where = wxNOT_FOUND;
        if ( !m_pluginFileMask.IsEmpty() ) {
            where = 0;
            
        } else {
            where = m_fileTypes->FindString(m_data.GetSelectedMask());
            if(where == wxNOT_FOUND) {
                where = 0;
            }
        }
        m_fileTypes->SetSelection( where );
    }
}

void FindInFilesDialog::DoSearchReplace()
{
    SearchData data = DoGetSearchData();
    data.SetOwner(clMainFrame::Get()->GetOutputPane()->GetReplaceResultsTab());

    DoSaveOpenFiles();
    SearchThreadST::Get()->PerformSearch(data);

    DoSaveSearchPaths();
    Close();
}

void FindInFilesDialog::DoSearch()
{
    SearchData data = DoGetSearchData();
    data.SetOwner(clMainFrame::Get()->GetOutputPane()->GetFindResultsTab());

    // check to see if we require to save the files
    DoSaveOpenFiles();
    SearchThreadST::Get()->PerformSearch(data);

    DoSaveSearchPaths();
    Close();
}

SearchData FindInFilesDialog::DoGetSearchData()
{
    SearchData data;
    wxString findStr(m_data.GetFindString());
    if (m_findString->GetValue().IsEmpty() == false) {
        findStr = m_findString->GetValue();
    }

    data.SetFindString       (findStr);

    size_t flags = m_data.GetFlags();

    // If the 'Skip comments' is ON, remove the
    // 'colour comments' flag
    if(flags & wxFRD_SKIP_COMMENTS) {
        flags &= ~wxFRD_COLOUR_COMMENTS;
    }

    data.SetMatchCase        ((flags & wxFRD_MATCHCASE) != 0);
    data.SetMatchWholeWord   ((flags & wxFRD_MATCHWHOLEWORD) != 0);
    data.SetRegularExpression((flags & wxFRD_REGULAREXPRESSION) != 0);
    data.SetDisplayScope     ((flags & wxFRD_DISPLAYSCOPE) != 0);
    data.SetEncoding         (m_choiceEncoding->GetStringSelection());
    data.SetSkipComments     (flags & wxFRD_SKIP_COMMENTS);
    data.SetSkipStrings      (flags & wxFRD_SKIP_STRINGS);
    data.SetColourComments   ( flags & wxFRD_COLOUR_COMMENTS );
    wxArrayString rootDirs;
    for (size_t i = 0; i < m_listPaths->GetCount(); ++i) {
        rootDirs.push_back(m_listPaths->GetString(i));
    }
    if (rootDirs.IsEmpty()) {
        wxString dir = m_dirPicker->GetPath();
        if (dir.IsEmpty() == false) {
            rootDirs.push_back(dir);
        }
    }
    data.SetRootDirs(rootDirs);

    wxArrayString files;
    for (size_t i = 0; i < rootDirs.GetCount(); ++i) {
        const wxString& rootDir = rootDirs.Item(i);
        // Check both translations and otherwise: the history may contain either
        if ((rootDir == wxGetTranslation(SEARCH_IN_WORKSPACE)) || (rootDir == SEARCH_IN_WORKSPACE)) {
            ManagerST::Get()->GetWorkspaceFiles(files);

        } else if ((rootDir == wxGetTranslation(SEARCH_IN_PROJECT)) || (rootDir == SEARCH_IN_PROJECT)) {
            ManagerST::Get()->GetActiveProjectFiles(files);

        } else if ((rootDir == wxGetTranslation(SEARCH_IN_CURR_FILE_PROJECT)) || (rootDir == SEARCH_IN_CURR_FILE_PROJECT)) {
            ManagerST::Get()->GetActiveFileProjectFiles(files);

        } else if ((rootDir == wxGetTranslation(SEARCH_IN_CURRENT_FILE)) || (rootDir == SEARCH_IN_CURRENT_FILE)) {
            LEditor *editor = clMainFrame::Get()->GetMainBook()->GetActiveEditor();
            if(editor) {
                files.Add(editor->GetFileName().GetFullPath());
            }
        } else if ((rootDir == wxGetTranslation(SEARCH_IN_OPEN_FILES)) || (rootDir == SEARCH_IN_OPEN_FILES)) {
            std::vector<LEditor*> editors;
            clMainFrame::Get()->GetMainBook()->GetAllEditors(editors, MainBook::kGetAll_IncludeDetached );
            
            for (size_t n=0; n < editors.size(); ++n) {
                LEditor* editor = dynamic_cast<LEditor*>(*(editors.begin()+n));
                if (editor) {
                    files.Add(editor->GetFileName().GetFullPath());
                }
            }
        }
    }

    data.SetFiles(files);
    data.UseNewTab(m_checkBoxSeparateTab->IsChecked());
    data.SetExtensions(m_fileTypes->GetValue());
    return data;
}

void FindInFilesDialog::OnClick(wxCommandEvent &event)
{
    wxObject *btnClicked = event.GetEventObject();
    size_t flags = m_data.GetFlags();

    wxString findWhat = m_findString->GetValue();
    findWhat = findWhat.Trim().Trim(false);

    m_data.SetFindString( m_findString->GetValue() );
    m_data.SetEncoding  ( m_choiceEncoding->GetStringSelection() );

    wxString value = m_fileTypes->GetValue();
    value.Trim().Trim(false);

    wxArrayString fileMask = m_fileTypes->GetStrings();
    wxArrayString fileMaskFiltered;
    
    // Remove the plugins mask before we save it
    for(size_t i=0; i<fileMask.GetCount(); ++i) {
        if ( m_pluginFileMask.Index(fileMask.Item(i)) == wxNOT_FOUND ) {
            // not part of the plugin mask => keep it
            fileMaskFiltered.Add( fileMask.Item(i) );
        }
    }
    fileMaskFiltered.swap( fileMask );
    m_data.SetSearchScope(m_dirPicker->GetCurrentSelection());

    m_data.SetFileMask( fileMask );
    if(value.IsEmpty() == false)
        m_data.SetSelectedMask(value);

    if (btnClicked == m_stop) {
        SearchThreadST::Get()->StopSearch();

    } else if (btnClicked == m_find) {
        if ( findWhat.IsEmpty() ) {
            return;
        }
        DoSearch();

    } else if (btnClicked == m_replaceAll) {
        if ( findWhat.IsEmpty() ) {
            return;
        }
        DoSearchReplace();

    } else if (btnClicked == m_cancel) {
        Close();

    } else if (btnClicked == m_matchCase) {
        if (m_matchCase->IsChecked()) {
            flags |= wxFRD_MATCHCASE;
        } else {
            flags &= ~(wxFRD_MATCHCASE);
        }
    } else if (btnClicked == m_matchWholeWord) {
        if (m_matchWholeWord->IsChecked()) {
            flags |= wxFRD_MATCHWHOLEWORD;
        } else {
            flags &= ~(wxFRD_MATCHWHOLEWORD);
        }
    } else if (btnClicked == m_regualrExpression) {
        if (m_regualrExpression->IsChecked()) {
            flags |= wxFRD_REGULAREXPRESSION;
        } else {
            flags &= ~(wxFRD_REGULAREXPRESSION);
        }
    } else if (btnClicked == m_printScope) {
        if (m_printScope->IsChecked()) {
            flags |= wxFRD_DISPLAYSCOPE;
        } else {
            flags &= ~(wxFRD_DISPLAYSCOPE);
        }
    } else if (btnClicked == m_checkBoxSeparateTab) {
        if (m_checkBoxSeparateTab->IsChecked()) {
            flags |= wxFRD_SEPARATETAB_DISPLAY;
        } else {
            flags &= ~(wxFRD_SEPARATETAB_DISPLAY);
        }
    } else if (btnClicked == m_checkBoxSaveFilesBeforeSearching) {
        if (m_checkBoxSaveFilesBeforeSearching->IsChecked()) {
            flags |= wxFRD_SAVE_BEFORE_SEARCH;
        } else {
            flags &= ~(wxFRD_SAVE_BEFORE_SEARCH);
        }
    } else if (btnClicked == m_checkBoxSkipMatchesFoundInComments) {
        if(m_checkBoxSkipMatchesFoundInComments->IsChecked()) {
            flags |= wxFRD_SKIP_COMMENTS;
        } else {
            flags &= ~wxFRD_SKIP_COMMENTS;
        }

    } else if (btnClicked == m_checkBoxSkipMatchesFoundInStrings) {
        if(m_checkBoxSkipMatchesFoundInStrings->IsChecked()) {
            flags |= wxFRD_SKIP_STRINGS;
        } else {
            flags &= ~wxFRD_SKIP_STRINGS;
        }
    } else if (btnClicked == m_checkBoxHighlighStringComments) {
        if(m_checkBoxHighlighStringComments->IsChecked()) {
            flags |= wxFRD_COLOUR_COMMENTS;
        } else {
            flags &= ~wxFRD_COLOUR_COMMENTS;
        }
    }

    // Set the updated flags
    m_data.SetFlags(flags);
}

void FindInFilesDialog::OnClose(wxCloseEvent &e)
{
    Destroy();
}

void FindInFilesDialog::OnAddPath( wxCommandEvent& event )
{
    wxString path = m_dirPicker->GetPath();
    if (m_listPaths->FindString(path) == wxNOT_FOUND) {
        m_listPaths->Append(path);
    }
}

void FindInFilesDialog::OnRemovePath( wxCommandEvent& event )
{
    int sel = m_listPaths->GetSelection();
    if (sel != wxNOT_FOUND) {
        m_listPaths->Delete(sel);
    }
}

void FindInFilesDialog::OnClearPaths( wxCommandEvent& event )
{
    m_listPaths->Clear();
}

bool FindInFilesDialog::Show()
{
    bool res = IsShown() || wxDialog::Show();
    if (res) {

        // update the combobox
        m_findString->Clear();
        m_findString->Append(m_data.GetFindStringArr());
        DoSetFileMask();
        m_findString->SetValue(m_data.GetFindString());

        LEditor *editor = clMainFrame::Get()->GetMainBook()->GetActiveEditor();
        if (editor) {
            //if we have an open editor, and a selected text, make this text the search string
            wxString selText = editor->GetSelectedText();
            if (!selText.IsEmpty()) {
                m_findString->SetValue(selText);
            }
        }

        m_findString->SetSelection(-1, -1); // select all
        m_findString->SetFocus();
    }
    return res;
}

void FindInFilesDialog::DoSaveSearchPaths()
{
    wxArrayString paths = m_dirPicker->GetValues();
    m_data.SetSearchPaths(paths);
}

void FindInFilesDialog::DoSaveOpenFiles()
{
    if (m_checkBoxSaveFilesBeforeSearching->IsChecked()) {
        clMainFrame::Get()->GetMainBook()->SaveAll(false, false);
    }
}

void FindInFilesDialog::OnClearPathsUI(wxUpdateUIEvent& event)
{
    event.Enable(m_listPaths->IsEmpty() == false);
}

void FindInFilesDialog::OnRemovePathUI(wxUpdateUIEvent& event)
{
    event.Enable(m_listPaths->GetSelection() != wxNOT_FOUND);
}

void FindInFilesDialog::OnFindWhatUI(wxUpdateUIEvent& event)
{
    event.Enable(m_findString->GetValue().IsEmpty() == false);
}

void FindInFilesDialog::OnUseDiffColourForCommentsUI(wxUpdateUIEvent& event)
{
    event.Enable(m_checkBoxSkipMatchesFoundInComments->IsChecked() == false);
}
