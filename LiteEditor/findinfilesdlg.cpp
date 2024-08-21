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
#include "findinfilesdlg.h"

#include "ColoursAndFontsManager.h"
#include "FindInFilesLocationsDlg.h"
#include "StringUtils.h"
#include "clFilesCollector.h"
#include "clWorkspaceManager.h"
#include "dirpicker.h"
#include "event_notifier.h"
#include "findresultstab.h"
#include "frame.h"
#include "globals.h"
#include "lexer_configuration.h"
#include "macros.h"
#include "manager.h"
#include "project.h"
#include "replaceinfilespanel.h"
#include "search_thread.h"
#include "sessionmanager.h"
#include "windowattrmanager.h"
#include "workspace.h"
#include "wxStringHash.h"

#include <algorithm>
#include <wx/dirdlg.h>
#include <wx/fontmap.h>
#include <wx/tokenzr.h>
#include <wx/wupdlock.h>

namespace
{
const wxString RE_TODO = "(/[/\\*]+ *TODO)";
const wxString RE_BUG = "(/[/\\*]+ *BUG)";
const wxString RE_ATTN = "(/[/\\*]+ *ATTN)";
const wxString RE_FIXME = "(/[/\\*]+ *FIXME)";

void UpdateComboBox(clComboBox* cb, const wxArrayString& arr, const wxString& str)
{
    auto updated_arr = StringUtils::AppendAndMakeUnique(arr, str);
    cb->Clear();
    cb->Append(updated_arr);
    cb->SetStringSelection(str);
}
} // namespace

FindInFilesDialog::FindInFilesDialog(wxWindow* parent, wxWindow* handler)
    : FindInFilesDialogBase(parent, wxID_ANY)
    , m_handler(handler)
{
    // Load the find-in-files data
    SessionManager::Get().LoadFindInFilesSession(&m_data);
    auto lex = ColoursAndFontsManager::Get().GetLexer("default");
    lex->ApplyFont(m_findString);
    lex->ApplyFont(m_replaceString);
    lex->ApplyFont(m_fileTypes);
    lex->ApplyFont(m_comboBoxEncoding);

    // "Find"
    UpdateComboBox(m_findString, m_data.find_what_array, m_data.find_what);
    m_findString->Bind(
        wxEVT_MENU,
        [&](wxCommandEvent& e) {
            wxUnusedVar(e);
            m_findString->Clear();
        },
        wxID_CLEAR);

    m_findString->SetStringSelection(m_data.find_what);

    // "Replace"
    UpdateComboBox(m_replaceString, m_data.replace_with_array, m_data.replace_with);
    m_replaceString->Bind(
        wxEVT_MENU,
        [&](wxCommandEvent& e) {
            wxUnusedVar(e);
            m_replaceString->Clear();
        },
        wxID_CLEAR);

    // "Files"
    UpdateComboBox(m_fileTypes, m_data.files_array, m_data.files);
    m_fileTypes->Bind(
        wxEVT_MENU,
        [&](wxCommandEvent& e) {
            wxUnusedVar(e);
            m_fileTypes->Clear();
        },
        wxID_CLEAR);

    // "Where"
    UpdateComboBox(m_comboBoxWhere, m_data.where_array, m_data.where);
    m_comboBoxWhere->Bind(
        wxEVT_MENU,
        [&](wxCommandEvent& e) {
            wxUnusedVar(e);
            m_comboBoxWhere->Clear();
        },
        wxID_CLEAR);

    m_matchCase->SetValue(m_data.flags & wxFRD_MATCHCASE);
    m_matchWholeWord->SetValue(m_data.flags & wxFRD_MATCHWHOLEWORD);
    m_regualrExpression->SetValue(m_data.flags & wxFRD_REGULAREXPRESSION);
    m_checkBoxSaveFilesBeforeSearching->SetValue(m_data.flags & wxFRD_SAVE_BEFORE_SEARCH);
    m_checkBoxPipeForGrep->SetValue(m_data.flags & wxFRD_ENABLE_PIPE_SUPPORT);

    // keep the current regex value, in case user uses the presets and we want
    // to restore it back
    m_oldRegexValue = m_data.flags & wxFRD_REGULAREXPRESSION;

    // Set encoding
    static wxArrayString astrEncodings;
    static std::unordered_map<wxString, int> encodingMap;
    static int selection(0);

    if (astrEncodings.empty()) {
        wxFontEncoding fontEnc;
        size_t iEncCnt = wxFontMapper::GetSupportedEncodingsCount();
        astrEncodings.reserve(iEncCnt);
        for (size_t i = 0; i < iEncCnt; i++) {
            fontEnc = wxFontMapper::GetEncoding(i);
            if (wxFONTENCODING_SYSTEM == fontEnc) { // skip system, it is changed to UTF-8 in optionsconfig
                continue;
            }
            wxString encodingName = wxFontMapper::GetEncodingName(fontEnc);
            size_t pos = astrEncodings.Add(encodingName);
            encodingMap.insert({ encodingName, (int)pos });
            if (m_data.encoding == encodingName) {
                selection = static_cast<int>(pos);
            }
        }
    } else {
        selection = encodingMap.count(m_data.encoding) ? encodingMap.find(m_data.encoding)->second : 0;
    }

    m_comboBoxEncoding->Append(astrEncodings);
    if (m_comboBoxEncoding->GetCount()) {
        m_comboBoxEncoding->SetSelection(selection);
    }

    m_checkBoxFollowSymlinks->SetValue(!(m_data.files_scanner_flags & clFilesScanner::SF_DONT_FOLLOW_SYMLINKS));
    m_checkBoxIncludeHiddenFolders->SetValue(!(m_data.files_scanner_flags & clFilesScanner::SF_EXCLUDE_HIDDEN_DIRS));

    // Set the file mask
    DoSetFileMask();
    SetName("FindInFilesDialog");
    CallAfter(&FindInFilesDialog::DoSelectAll);

    // Fit the initial size and set it as the default minimum size
    GetSizer()->Fit(this);
    SetMinSize(GetSize());
    WindowAttrManager::Load(this);
    CenterOnParent();
}

FindInFilesDialog::~FindInFilesDialog() { SaveFindReplaceData(); }

void FindInFilesDialog::DoSetFileMask() {}

void FindInFilesDialog::DoSearchReplace()
{
    SearchData data = DoGetSearchData();
    // direct output results to the 'Replace' tab, unless a custom handler was provided
    data.SetOwner(m_handler ? m_handler : clMainFrame::Get()->GetOutputPane()->GetReplaceResultsTab());
    DoSaveOpenFiles();
    SearchThreadST::Get()->PerformSearch(data);
    SaveFindReplaceData();
    EndModal(wxID_OK);
}

void FindInFilesDialog::DoSearch()
{
    SearchData data = DoGetSearchData();
    // direct results to the 'Search' tab
    data.SetOwner(m_handler ? m_handler : clMainFrame::Get()->GetOutputPane()->GetFindResultsTab());

    // check to see if we require to save the files
    DoSaveOpenFiles();
    SearchThreadST::Get()->PerformSearch(data);
    SaveFindReplaceData();
    EndModal(wxID_OK);
}

SearchData FindInFilesDialog::DoGetSearchData()
{
    SearchData data;
    wxString findStr(m_data.find_what);
    if (!m_findString->GetValue().IsEmpty()) {
        findStr = m_findString->GetValue();
    }

    data.SetFindString(findStr);
    data.SetReplaceWith(m_replaceString->GetValue());

    m_data.flags = GetSearchFlags();
    size_t flags = m_data.flags;

    // If the 'Skip comments' is ON, remove the
    // 'colour comments' flag
    if (flags & wxFRD_SKIP_COMMENTS) {
        flags &= ~wxFRD_COLOUR_COMMENTS;
    }

    data.SetMatchCase((flags & wxFRD_MATCHCASE) != 0);
    data.SetMatchWholeWord((flags & wxFRD_MATCHWHOLEWORD) != 0);
    data.SetRegularExpression((flags & wxFRD_REGULAREXPRESSION) != 0);
    data.SetDisplayScope((flags & wxFRD_DISPLAYSCOPE) != 0);
    data.SetEncoding(m_comboBoxEncoding->GetStringSelection());
    data.SetSkipComments(flags & wxFRD_SKIP_COMMENTS);
    data.SetSkipStrings(flags & wxFRD_SKIP_STRINGS);
    data.SetColourComments(flags & wxFRD_COLOUR_COMMENTS);
    data.SetEnablePipeSupport(flags & wxFRD_ENABLE_PIPE_SUPPORT);

    size_t search_flags = clFilesScanner::SF_DEFAULT;
    if (m_checkBoxFollowSymlinks->IsChecked()) {
        search_flags &= ~clFilesScanner::SF_DONT_FOLLOW_SYMLINKS;
    }
    if (m_checkBoxIncludeHiddenFolders->IsChecked()) {
        search_flags &= ~clFilesScanner::SF_EXCLUDE_HIDDEN_DIRS;
    }
    data.SetFileScannerFlags(search_flags);

    // for persisntency
    m_data.files_scanner_flags = search_flags;

    wxArrayString searchWhere = GetPathsAsArray();
    wxArrayString files;
    wxArrayString rootDirs;
    wxArrayString excludePattern;

    for (size_t i = 0; i < searchWhere.GetCount(); ++i) {
        const wxString& rootDir = searchWhere.Item(i);
        if (rootDir.StartsWith("-")) {
            // An exclude pattern
            excludePattern.Add(rootDir.Mid(1));
        } else if (rootDir.StartsWith("F: ")) {
            // a file
            files.Add(rootDir.Mid(3));
        } else if (rootDir.StartsWith("W: ")) {
            // Add all project files
            wxArrayString projects;
            clCxxWorkspaceST::Get()->GetProjectList(projects);
            for (size_t n = 0; n < projects.size(); ++n) {
                DoAddProjectFiles(projects.Item(n), files);
            }
        } else if (rootDir.StartsWith("V: ")) {
            // a virtual folder
            if (clCxxWorkspaceST::Get()->IsOpen()) {
                wxString vd = rootDir.Mid(3);
                vd.Replace("/", ":"); // Virtual directory is expecting colons as the separator
                wxString projectName = vd.BeforeFirst(':');
                vd = vd.AfterFirst(':');
                ProjectPtr p = clCxxWorkspaceST::Get()->GetProject(projectName);
                if (p) {
                    wxArrayString vdFiles;
                    p->GetFilesByVirtualDir(vd, vdFiles, true);
                    files.insert(files.end(), vdFiles.begin(), vdFiles.end());
                }
            }
        } else if (rootDir.StartsWith("P: ")) {
            // Project
            wxString projectName = rootDir.Mid(3);
            DoAddProjectFiles(projectName, files);

        } else if ((rootDir == wxGetTranslation(SEARCH_IN_WORKSPACE_FOLDER)) ||
                   (rootDir == SEARCH_IN_WORKSPACE_FOLDER)) {
            if (!clWorkspaceManager::Get().IsWorkspaceOpened()) {
                continue;
            }
            // Add the workspace folder
            rootDirs.Add(clWorkspaceManager::Get().GetWorkspace()->GetDir());

        } else if ((rootDir == wxGetTranslation(SEARCH_IN_WORKSPACE)) || (rootDir == SEARCH_IN_WORKSPACE)) {
            if (!clWorkspaceManager::Get().IsWorkspaceOpened()) {
                continue;
            }
            clWorkspaceManager::Get().GetWorkspace()->GetWorkspaceFiles(files);

        } else if ((rootDir == wxGetTranslation(SEARCH_IN_PROJECT)) || (rootDir == SEARCH_IN_PROJECT)) {
            if (!clWorkspaceManager::Get().IsWorkspaceOpened())
                continue;
            if (clWorkspaceManager::Get().GetWorkspace()->IsProjectSupported()) {
                // get the active project files
                clWorkspaceManager::Get().GetWorkspace()->GetProjectFiles("", files);
            } else {
                // search the entire workspace
                clWorkspaceManager::Get().GetWorkspace()->GetWorkspaceFiles(files);
            }

        } else if ((rootDir == wxGetTranslation(SEARCH_IN_CURR_FILE_PROJECT)) ||
                   (rootDir == SEARCH_IN_CURR_FILE_PROJECT)) {

            if (!clWorkspaceManager::Get().IsWorkspaceOpened())
                continue;
            IEditor* editor = clGetManager()->GetActiveEditor();
            if (!editor)
                continue;

            if (clWorkspaceManager::Get().GetWorkspace()->IsProjectSupported()) {
                wxString projectName =
                    clWorkspaceManager::Get().GetWorkspace()->GetProjectFromFile(editor->GetFileName());
                clWorkspaceManager::Get().GetWorkspace()->GetProjectFiles(projectName, files);
            } else {
                // search the entire workspace
                clWorkspaceManager::Get().GetWorkspace()->GetWorkspaceFiles(files);
            }

        } else if ((rootDir == wxGetTranslation(SEARCH_IN_CURRENT_FILE)) || (rootDir == SEARCH_IN_CURRENT_FILE)) {
            clEditor* editor = clMainFrame::Get()->GetMainBook()->GetActiveEditor();
            if (editor) {
                files.Add(editor->GetFileName().GetFullPath());
            }
        } else if ((rootDir == wxGetTranslation(SEARCH_IN_OPEN_FILES)) || (rootDir == SEARCH_IN_OPEN_FILES)) {
            std::vector<clEditor*> editors;
            clMainFrame::Get()->GetMainBook()->GetAllEditors(editors, MainBook::kGetAll_Default);

            for (size_t n = 0; n < editors.size(); ++n) {
                clEditor* editor = dynamic_cast<clEditor*>(*(editors.begin() + n));
                if (editor) {
                    files.Add(editor->GetFileName().GetFullPath());
                }
            }
        } else if (wxFileName::DirExists(searchWhere.Item(i))) {
            rootDirs.Add(searchWhere.Item(i));
        }
    }

    // Remove duplicates
    wxStringSet_t filesSet;
    wxArrayString uniqueFiles;
    // Unique files may contain up to files.size() elements
    uniqueFiles.Alloc(files.size());
    for (const wxString& file : files) {
        if (filesSet.count(file) == 0) {
            filesSet.insert(file);
            uniqueFiles.Add(file);
        }
    }

    // Release unused memory
    uniqueFiles.Shrink();
    files.swap(uniqueFiles);

    data.SetFiles(files);       // list of files
    data.SetRootDirs(rootDirs); // folders
    data.UseNewTab(false);
    data.SetExcludePatterns(excludePattern);
    data.SetExtensions(m_fileTypes->GetValue());
    return data;
}

void FindInFilesDialog::OnClose(wxCloseEvent& e) { Destroy(); }

void FindInFilesDialog::OnAddPath(wxCommandEvent& event)
{
    // Show a popup menu
    wxMenu menu;
    int firstItem = 8994;
    menu.Append(firstItem + 6, _("Add Folder..."));
    menu.AppendSeparator();
    menu.Append(firstItem + 0, wxGetTranslation(SEARCH_IN_WORKSPACE_FOLDER));
    menu.Append(firstItem + 1, wxGetTranslation(SEARCH_IN_WORKSPACE));
    menu.Append(firstItem + 2, wxGetTranslation(SEARCH_IN_PROJECT));
    menu.Append(firstItem + 3, wxGetTranslation(SEARCH_IN_CURR_FILE_PROJECT));
    menu.Append(firstItem + 4, wxGetTranslation(SEARCH_IN_CURRENT_FILE));
    menu.Append(firstItem + 5, wxGetTranslation(SEARCH_IN_OPEN_FILES));
    menu.AppendSeparator();
    menu.Append(firstItem + 7, _("Add Exclude Pattern"));

    std::map<int, wxString> options;
    options.insert(std::make_pair(firstItem + 0, wxGetTranslation(SEARCH_IN_WORKSPACE_FOLDER)));
    options.insert(std::make_pair(firstItem + 1, wxGetTranslation(SEARCH_IN_WORKSPACE)));
    options.insert(std::make_pair(firstItem + 2, wxGetTranslation(SEARCH_IN_PROJECT)));
    options.insert(std::make_pair(firstItem + 3, wxGetTranslation(SEARCH_IN_CURR_FILE_PROJECT)));
    options.insert(std::make_pair(firstItem + 4, wxGetTranslation(SEARCH_IN_CURRENT_FILE)));
    options.insert(std::make_pair(firstItem + 5, wxGetTranslation(SEARCH_IN_OPEN_FILES)));

    // Menu will be shown in client coordinates
    wxRect size = m_btnAddPath->GetSize();
    wxPoint menuPos(0, size.GetHeight());
    int selection = m_btnAddPath->GetPopupMenuSelectionFromUser(menu, menuPos);

    if (selection == wxID_NONE)
        return;

    if (selection == (firstItem + 7)) {
        // Add exclude pattern
        wxString current_content = m_comboBoxWhere->GetValue();
        current_content.Trim();
        if (!current_content.empty() && !current_content.EndsWith(";")) {
            current_content << ";";
        }

        int selection_start = current_content.length();
        current_content << "-*PATTERN*";
        selection_start += 1; // skip the `-`
        int selection_end = current_content.length();
        m_comboBoxWhere->SetValue(current_content);
        m_comboBoxWhere->CallAfter(&clComboBox::SetFocus);

    } else if (selection == (firstItem + 6)) {
        wxString folder = ::wxDirSelector();
        if (folder.IsEmpty())
            return;
        DoAppendSearchPath(folder);

    } else if (options.count(selection)) {
        DoAppendSearchPath(options.find(selection)->second);
    }
}

int FindInFilesDialog::ShowDialog()
{
    // Update the combobox
    m_findString->Clear();
    m_findString->Append(m_data.find_what_array);
    m_findString->SetValue(m_data.find_what);

    clEditor* editor = clMainFrame::Get()->GetMainBook()->GetActiveEditor();
    if (editor) {
        // if we have an open editor, and a selected text, make this text the search string
        wxString selText = editor->GetSelectedText();
        if (!selText.IsEmpty()) {
            m_findString->SetValue(selText);
        }
    }

    m_findString->CallAfter(&wxTextCtrl::SetFocus);
    return wxDialog::ShowModal();
}

void FindInFilesDialog::DoSaveOpenFiles()
{
    if (m_checkBoxSaveFilesBeforeSearching->IsChecked()) {
        clMainFrame::Get()->GetMainBook()->SaveAll(false, false);
    }
}

void FindInFilesDialog::OnFindWhatUI(wxUpdateUIEvent& event)
{
    wxArrayString paths = GetPathsAsArray();
    event.Enable(!m_findString->GetValue().IsEmpty() && !paths.IsEmpty());
}

void FindInFilesDialog::OnUseDiffColourForCommentsUI(wxUpdateUIEvent& event)
{
    // event.Enable(m_checkBoxSkipMatchesFoundInComments->IsChecked() == false);
}

void FindInFilesDialog::OnFind(wxCommandEvent& event)
{
    wxUnusedVar(event);
    DoSearch();
}

void FindInFilesDialog::OnReplace(wxCommandEvent& event)
{
    wxUnusedVar(event);
    DoSearchReplace();
}

void FindInFilesDialog::OnButtonClose(wxCommandEvent& event) { EndModal(wxID_CANCEL); }

size_t FindInFilesDialog::GetSearchFlags()
{
    size_t flags = 0;
    if (m_matchCase->IsChecked())
        flags |= wxFRD_MATCHCASE;
    if (m_matchWholeWord->IsChecked())
        flags |= wxFRD_MATCHWHOLEWORD;
    if (m_regualrExpression->IsChecked())
        flags |= wxFRD_REGULAREXPRESSION;
    if (m_checkBoxSaveFilesBeforeSearching->IsChecked())
        flags |= wxFRD_SAVE_BEFORE_SEARCH;
    if (m_checkBoxPipeForGrep->IsChecked())
        flags |= wxFRD_ENABLE_PIPE_SUPPORT;
    return flags;
}

void FindInFilesDialog::SetSearchPaths(const wxString& paths, bool transient)
{
    wxArrayString arr = ::wxStringTokenize(paths, ";\n", wxTOKEN_STRTOK);
    wxString current_value = ::wxJoin(arr, ';');
    m_comboBoxWhere->SetValue(current_value);
    m_transient = transient;
}

void FindInFilesDialog::DoAppendSearchPath(const wxString& path)
{
    wxString current_value = m_comboBoxWhere->GetValue();
    wxArrayString paths = ::wxStringTokenize(current_value, ";\n", wxTOKEN_STRTOK);
    if (paths.Index(path) != wxNOT_FOUND) {
        return;
    }
    paths.Add(path);
    current_value = ::wxJoin(paths, ';');
    m_comboBoxWhere->SetValue(current_value);
}

void FindInFilesDialog::OnReplaceUI(wxUpdateUIEvent& event)
{
    wxArrayString paths = GetPathsAsArray();
    event.Enable(!m_findString->GetValue().IsEmpty() && !paths.IsEmpty());
}

void FindInFilesDialog::OnLookInKeyDown(wxKeyEvent& event) { event.Skip(); }

void FindInFilesDialog::DoAddProjectFiles(const wxString& projectName, wxArrayString& files)
{
    if (!clCxxWorkspaceST::Get()->IsOpen()) {
        return;
    }
    ProjectPtr p = clCxxWorkspaceST::Get()->GetProject(projectName);
    if (p) {
        const Project::FilesMap_t& filesMap = p->GetFiles();
        if (!filesMap.empty()) {
            wxArrayString tmpArr;
            tmpArr.Alloc(filesMap.size());
            std::for_each(filesMap.begin(), filesMap.end(),
                          [&](const Project::FilesMap_t::value_type& vt) { tmpArr.Add(vt.second->GetFilename()); });
            files.insert(files.end(), tmpArr.begin(), tmpArr.end());
        }
    }
}

wxArrayString FindInFilesDialog::GetPathsAsArray() const
{
    wxString str = m_comboBoxWhere->GetValue();
    wxArrayString arr = ::wxStringTokenize(str, ";", wxTOKEN_STRTOK);
    return arr;
}

namespace
{
wxArrayString GetComboBoxStrings(clComboBox* cb)
{
    wxArrayString arr = cb->GetStrings();
    wxString value = cb->GetValue();
    int loc = arr.Index(value);
    if (loc != wxNOT_FOUND) {
        arr.RemoveAt(loc);
    }
    arr.Insert(value, 0);
    return arr;
}
} // namespace

void FindInFilesDialog::SaveFindReplaceData()
{
    if (m_presetSearch && !m_userChangedRegexManually) {
        // restore the regex value
        m_regualrExpression->SetValue(m_oldRegexValue);
    }

    m_data.flags = GetSearchFlags();
    m_data.find_what_array = StringUtils::AppendAndMakeUnique(m_findString->GetStrings(), m_findString->GetValue());
    m_data.find_what = m_findString->GetValue();

    m_data.replace_with_array =
        StringUtils::AppendAndMakeUnique(m_replaceString->GetStrings(), m_replaceString->GetValue());
    m_data.replace_with = m_replaceString->GetValue();

    m_data.encoding = m_comboBoxEncoding->GetStringSelection();

    wxArrayString masks = GetComboBoxStrings(m_fileTypes);
    wxString value = m_fileTypes->GetValue();

    m_data.files = value;
    m_data.files_array = masks;

    // store the "Where"
    if (!m_transient) {
        wxArrayString where_arr =
            StringUtils::AppendAndMakeUnique(m_comboBoxWhere->GetStrings(), m_comboBoxWhere->GetValue());
        m_data.where_array = where_arr;
        m_data.where = m_comboBoxWhere->GetValue();
    }

    size_t search_flags = clFilesScanner::SF_DEFAULT;
    if (m_checkBoxFollowSymlinks->IsChecked()) {
        search_flags &= ~clFilesScanner::SF_DONT_FOLLOW_SYMLINKS;
    }
    if (m_checkBoxIncludeHiddenFolders->IsChecked()) {
        search_flags &= ~clFilesScanner::SF_EXCLUDE_HIDDEN_DIRS;
    }
    m_data.files_scanner_flags = search_flags;

    // save the session
    SessionManager::Get().SaveFindInFilesSession(m_data);
}

void FindInFilesDialog::SetFileMask(const wxString& mask)
{
    if (!mask.IsEmpty()) {
        int where = m_fileTypes->FindString(mask);
        if (where == wxNOT_FOUND) {
            // Add it
            where = m_fileTypes->Append(mask);
        }
        m_fileTypes->SetSelection(where);
    }
}

void FindInFilesDialog::OnFindEnter(wxCommandEvent& event)
{
    event.Skip();
    OnFind(event);
}

void FindInFilesDialog::OnReplaceEnter(wxCommandEvent& event) { OnReplace(event); }

void FindInFilesDialog::DoSelectAll() { m_findString->SelectAll(); }

void FindInFilesDialog::OnATTN(wxCommandEvent& event)
{
    m_presetSearch = true;
    SetPresets();
}

void FindInFilesDialog::OnBUG(wxCommandEvent& event)
{
    m_presetSearch = true;
    SetPresets();
}

void FindInFilesDialog::OnFIXME(wxCommandEvent& event)
{
    m_presetSearch = true;
    SetPresets();
}

void FindInFilesDialog::OnTODO(wxCommandEvent& event)
{
    m_presetSearch = true;
    SetPresets();
}

void FindInFilesDialog::OnRegex(wxCommandEvent& event)
{
    event.Skip();
    m_userChangedRegexManually = true;
}

void FindInFilesDialog::SetPresets()
{
    wxString pattern;
    if (m_checkBoxATTN->IsChecked()) {
        pattern << RE_ATTN << "|";
    }

    if (m_checkBoxBUG->IsChecked()) {
        pattern << RE_BUG << "|";
    }

    if (m_checkBoxFIXME->IsChecked()) {
        pattern << RE_FIXME << "|";
    }

    if (m_checkBoxTODO->IsChecked()) {
        pattern << RE_TODO << "|";
    }

    if (!pattern.empty()) {
        // remove the trailing pipe
        pattern.RemoveLast();
    }

    m_findString->SetValue(pattern);
    if (!m_regualrExpression->IsChecked()) {
        // enable regex
        m_regualrExpression->SetValue(true);
        m_presetSearch = true;
    }
}
