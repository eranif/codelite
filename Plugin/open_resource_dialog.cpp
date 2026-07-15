//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : open_resource_dialog.cpp
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
#include "open_resource_dialog.h"

#include "FileSystemWorkspace/clFileSystemWorkspace.hpp"
#include "LSP/LSPManager.hpp"
#include "bitmap_loader.h"
#include "clWorkspaceManager.h"
#include "codelite_events.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "fileutils.h"
#include "globals.h"
#include "ieditor.h"
#include "imanager.h"
#include "macros.h"
#include "project.h"
#include "workspace.h"

#include <algorithm>
#include <vector>
#include <wx/event.h>
#include <wx/filefn.h>
#include <wx/imaglist.h>
#include <wx/wupdlock.h>
#include <wx/xrc/xmlres.h>

wxDEFINE_EVENT(wxEVT_OPEN_RESOURCE_FILE_SELECTED, clCommandEvent);

OpenResourceDialog::OpenResourceDialog(wxWindow* parent, IManager* manager, const wxString& initialSelection)
    : OpenResourceDialogBase(parent)
    , m_manager(manager)
    , m_needRefresh(false)
    , m_lineNumber(wxNOT_FOUND)
{
    EventNotifier::Get()->Bind(wxEVT_LSP_WORKSPACE_SYMBOLS, &OpenResourceDialog::OnWorkspaceSymbols, this);
#ifdef __WXMSW__
    m_dataview->SetAlternateRowColour(m_dataview->GetBackgroundColour().ChangeLightness(103));
#endif

    auto* images = clGetManager()->GetStdIcons();

    // initialize the file-type hash
    m_fileTypeHash[LSP::kSK_Class] = images->GetMimeBitmaps().GetBitmap(BitmapLoader::kClass, false);
    m_fileTypeHash[LSP::kSK_Struct] = images->GetMimeBitmaps().GetBitmap(BitmapLoader::kStruct, false);
    m_fileTypeHash[LSP::kSK_Namespace] = images->GetMimeBitmaps().GetBitmap(BitmapLoader::kNamespace, false);
    m_fileTypeHash[LSP::kSK_Variable] = images->GetMimeBitmaps().GetBitmap(BitmapLoader::kMemberPublic, false);
    m_fileTypeHash[LSP::kSK_TypeParameter] = images->GetMimeBitmaps().GetBitmap(BitmapLoader::kTypedef, false);
    m_fileTypeHash[LSP::kSK_Property] = images->GetMimeBitmaps().GetBitmap(BitmapLoader::kMemberPublic, false);
    m_fileTypeHash[LSP::kSK_Function] = images->GetMimeBitmaps().GetBitmap(BitmapLoader::kFunctionPublic, false);
    m_fileTypeHash[LSP::kSK_Method] = images->GetMimeBitmaps().GetBitmap(BitmapLoader::kFunctionPublic, false);
    m_fileTypeHash[LSP::kSK_Constructor] = images->GetMimeBitmaps().GetBitmap(BitmapLoader::kFunctionPublic, false);
    m_fileTypeHash[LSP::kSK_TypeParameter] = images->GetMimeBitmaps().GetBitmap(BitmapLoader::kMacro, false);
    m_fileTypeHash[LSP::kSK_Enum] = images->GetMimeBitmaps().GetBitmap(BitmapLoader::kEnum, false);
    m_fileTypeHash[LSP::kSK_EnumMember] = images->GetMimeBitmaps().GetBitmap(BitmapLoader::kEnumerator, false);

    m_timer.SetOwner(this);
    Bind(wxEVT_TIMER, &OpenResourceDialog::OnTimer, this, m_timer.GetId());

    m_textCtrlResourceName->SetFocus();
    SetLabel(_("Open resource..."));
    SetName("OpenResourceDialog");

    // load all files from the workspace
    if (::clIsCxxWorkspaceOpened()) {
        if (m_manager->IsWorkspaceOpen()) {
            wxArrayString projects;
            m_manager->GetWorkspace()->GetProjectList(projects);

            for (size_t i = 0; i < projects.GetCount(); i++) {
                ProjectPtr p = m_manager->GetWorkspace()->GetProject(projects.Item(i));
                if (p) {
                    const Project::FilesMap_t& files = p->GetFiles();
                    // convert std::vector to wxArrayString
                    for (const auto& p : files) {
                        wxFileName fn(p.second->GetFilename());
                        m_files.insert(std::make_pair(fn.GetFullName(), fn.GetFullPath()));
                    }
                }
            }
        } else if (clFileSystemWorkspace::Get().IsOpen()) {
            const std::vector<wxFileName>& files = clFileSystemWorkspace::Get().GetFiles();
            for (const wxFileName& fn : files) {
                m_files.insert({fn.GetFullName(), fn.GetFullPath()});
            }
        }
    } else if (clWorkspaceManager::Get().IsWorkspaceOpened()) {
        // the workspace API to get list of files
        wxArrayString files;
        clWorkspaceManager::Get().GetWorkspace()->GetWorkspaceFiles(files);
        wxStringSet_t unique_files;
        m_files.reserve(files.size());
        for (const auto& file : files) {
            if (unique_files.count(file) == 0) {
                unique_files.insert(file);
                wxFileName fn(file);
                // keep the file as-is do not "format" it by calling
                // fn.GetFullPath() since we might be on Windows and we display
                // Linux path style files
                m_files.insert({fn.GetFullName(), file});
            }
        }
    }

    wxString lastStringTyped = clConfig::Get().Read("OpenResourceDialog/SearchString", wxString());
    // Set the initial selection
    // We use here 'SetValue' so an event will get fired and update the control
    bool filter_results{false};
    if (!initialSelection.IsEmpty()) {
        m_textCtrlResourceName->ChangeValue(initialSelection);
        CallAfter(&OpenResourceDialog::OnSelectAllText);
        filter_results = true;
    } else if (!lastStringTyped.IsEmpty()) {
        m_textCtrlResourceName->ChangeValue(lastStringTyped);
        CallAfter(&OpenResourceDialog::OnSelectAllText);
        filter_results = true;
    }

    if (filter_results) {
        // This will start the timer
        wxCommandEvent dummEvent;
        OnText(dummEvent);
    }

    bool showFiles = clConfig::Get().Read("OpenResourceDialog/ShowFiles", true);
    bool showSymbols = clConfig::Get().Read("OpenResourceDialog/ShowSymbols", true);
    m_checkBoxFiles->SetValue(showFiles);
    m_checkBoxShowSymbols->SetValue(showSymbols);

    ::clSetDialogBestSizeAndPosition(*this);
    MSWSetWindowDarkTheme(this);
}

OpenResourceDialog::~OpenResourceDialog()
{
    m_timer.Stop();
    Unbind(wxEVT_TIMER, &OpenResourceDialog::OnTimer, this, m_timer.GetId());
    EventNotifier::Get()->Unbind(wxEVT_LSP_WORKSPACE_SYMBOLS, &OpenResourceDialog::OnWorkspaceSymbols, this);

    // Store current values
    clConfig::Get().Write("OpenResourceDialog/ShowFiles", m_checkBoxFiles->IsChecked());
    clConfig::Get().Write("OpenResourceDialog/ShowSymbols", m_checkBoxShowSymbols->IsChecked());
    clConfig::Get().Write("OpenResourceDialog/SearchString", m_textCtrlResourceName->GetValue());
}

void OpenResourceDialog::OnText(wxCommandEvent& event)
{
    event.Skip();
    if (m_timer.IsRunning())
        m_timer.Stop();
    m_timer.Start(200, true);

    wxString filter = m_textCtrlResourceName->GetValue();
    if (!filter.Trim().Trim(false).empty())
        m_needRefresh = true;
}

void OpenResourceDialog::OnUsePartialMatching(wxCommandEvent& event)
{
    wxUnusedVar(event);
    DoPopulateList();
    m_textCtrlResourceName->SetFocus();
}

void OpenResourceDialog::OnEntryActivated(wxDataViewEvent& event)
{
    CHECK_ITEM_RET(event.GetItem());
    OpenResourceDialogItemData* data =
        reinterpret_cast<OpenResourceDialogItemData*>(m_dataview->GetItemData(event.GetItem()));
    if (data) {
        EndModal(wxID_OK);
    }
}

void OpenResourceDialog::DoPopulateList()
{
    Clear();
    wxString name = m_textCtrlResourceName->GetValue();
    name.Trim().Trim(false);
    if (name.empty()) {
        return;
    }

    // First add the workspace files
    long nLineNumber;
    long nColumn;
    wxString modFilter;
    GetLineAndColumnFromFilter(name, modFilter, nLineNumber, nColumn);
    name.swap(modFilter);

    clDEBUG() << "Open resource:" << name << ":" << nLineNumber << ":" << nColumn << endl;
    m_lineNumber = nLineNumber;
    m_column = nColumn;

    // Prepare the user filter
    m_userFilters.Clear();
    m_userFilters = ::wxStringTokenize(name, " \t", wxTOKEN_STRTOK);
    for (size_t i = 0; i < m_userFilters.GetCount(); ++i) {
        m_userFilters.Item(i).MakeLower();
    }

    // Build the filter class
    if (m_checkBoxFiles->IsChecked()) {
        DoPopulateWorkspaceFile();
    }

    if (m_checkBoxShowSymbols->IsChecked() && (nLineNumber == -1)) {
        LSP::Manager::GetInstance().WorkspaceSymbols(name);
    }
}

void OpenResourceDialog::OnWorkspaceSymbols(LSPEvent& event) { DoPopulateTags(event.GetSymbolsInformation()); }

void OpenResourceDialog::DoPopulateTags(const std::vector<LSP::SymbolInformation>& symbols)
{
    // Next, add the tags
    if (m_userFilters.IsEmpty() || symbols.empty()) {
        return;
    }

    constexpr size_t kMaxSymbols = 200;
    wxWindowUpdateLocker locker{m_dataview};
    size_t itemsAdded{0};
    for (const LSP::SymbolInformation& symbol : symbols) {
        if (!MatchesFilter(symbol.GetName())) {
            continue;
        }

        // keep the fullpath
        DoAppendLine(symbol.GetName(),
                     symbol.GetContainerName(),
                     false,
                     new OpenResourceDialogItemData(symbol.GetLocation().GetPath(),
                                                    symbol.GetLocation().GetRange().GetEnd().GetLine() + 1,
                                                    wxEmptyString,
                                                    symbol.GetName(),
                                                    symbol.GetContainerName()),
                     DoGetTagImg(symbol));
        itemsAdded++;
        if (itemsAdded >= kMaxSymbols)
            break;
    }
}

void OpenResourceDialog::DoPopulateWorkspaceFile()
{
    // do we need to include files?
    if (!m_filters.IsEmpty() && m_filters.Index(KIND_FILE) == wxNOT_FOUND) {
        return;
    }

    if (!m_userFilters.empty()) {

        std::unordered_multimap<wxString, wxString>::iterator iter = m_files.begin();
        const int maxFileSize = 100;
        int counter = 0;
        for (; (iter != m_files.end()) && (counter < maxFileSize); iter++) {
            const wxString& fullpath = iter->second;
            if (!MatchesFilter(fullpath)) {
                continue;
            }

            wxFileName fn(iter->second);
            DoAppendLine(fn.GetFullName(),
                         iter->second,
                         false,
                         new OpenResourceDialogItemData(iter->second, -1, "", fn.GetFullName(), ""),
                         clGetManager()->GetStdIcons()->GetBitmapForFile(fn.GetFullName(), false));
            ++counter;
        }
    }
}

void OpenResourceDialog::Clear()
{
    for (auto row = 0; row < m_dataview->GetItemCount(); ++row) {
        auto* ptr = GetItemData(m_dataview->RowToItem(row));
        wxDELETE(ptr);
    }
    m_dataview->DeleteAllItems();
    m_userFilters.Clear();
}

void OpenResourceDialog::OpenSelection(const OpenResourceDialogItemData& selection, IManager* manager)
{
    wxUnusedVar(manager);

    // send event to the plugins to see if they want
    // to open this file
    wxString file_path = selection.m_file;
    clCommandEvent activateEvent(wxEVT_TREE_ITEM_FILE_ACTIVATED);
    activateEvent.SetFileName(file_path);
    if (EventNotifier::Get()->ProcessEvent(activateEvent)) {
        return;
    }

    clDEBUG() << "Opening editor:" << selection.m_file << ":" << selection.m_line << ":" << selection.m_column << endl;

    auto callback = [=](IEditor* editor) {
        editor->GetCtrl()->ClearSelections();
        editor->CenterLine(selection.m_line - 1, selection.m_column);
    };
    clGetManager()->OpenFileAndAsyncExecute(selection.m_file, std::move(callback));
}

void OpenResourceDialog::OnKeyDown(wxKeyEvent& event)
{
    event.Skip();
    if (m_dataview->GetItemCount() == 0) {
        return;
    }

    wxChar ch = event.GetUnicodeKey();
    wxChar key_code = event.GetKeyCode();
    int modifier_key = event.GetModifiers();
    bool control_down = (modifier_key == wxMOD_CONTROL);
    bool line_down =
        (key_code == WXK_DOWN) || (key_code == WXK_NUMPAD_DOWN) || (control_down && (ch == 'J' || ch == 'N'));
    bool page_down = (key_code == WXK_PAGEDOWN) || (control_down && (ch == 'D'));
    bool line_up = (key_code == WXK_UP) || (key_code == WXK_NUMPAD_UP) || (control_down && (ch == 'K' || ch == 'P'));
    bool page_up = (key_code == WXK_PAGEUP) || (control_down && (ch == 'U'));

    event.Skip(false);
    if (m_dataview->GetSelectedItemsCount() != 1) {
        // Just select the first entry
        DoSelectItem(m_dataview->RowToItem(0));
        event.Skip();
        return;
    }

    if (line_down || line_up || page_down || page_up) {
        int row = m_dataview->GetSelectedRow();
        if (line_down) {
            row++;
        } else if (line_up) {
            row--;
        } else if (page_down) {
            row += 10;
        } else if (page_up) {
            row -= 10;
        }

        row = std::clamp(row, 0, static_cast<int>(m_dataview->GetItemCount()));
        m_dataview->SelectRow(row);
        m_dataview->EnsureVisible(m_dataview->RowToItem(row));
        m_textCtrlResourceName->CallAfter(&wxDataViewListCtrl::SetFocus);
        return;
    }
    event.Skip();
}

void OpenResourceDialog::OnOK(wxCommandEvent& event) { event.Skip(); }

void OpenResourceDialog::OnOKUI(wxUpdateUIEvent& event) { event.Enable(m_dataview->GetSelectedItemsCount()); }

bool OpenResourceDialogItemData::IsOk() const { return m_file.IsEmpty() == false; }

void OpenResourceDialog::DoSelectItem(const wxDataViewItem& item)
{
    CHECK_ITEM_RET(item);
    GetDataview()->Select(item);
    GetDataview()->EnsureVisible(item);
    GetDataview()->Refresh();
}

void OpenResourceDialog::DoAppendLine(const wxString& name,
                                      const wxString& fullname,
                                      bool boldFont,
                                      OpenResourceDialogItemData* clientData,
                                      const wxBitmap& imgid)
{
    wxString prefix;
    clientData->m_impl = boldFont;
    wxVector<wxVariant> cols;

    wxDataViewIconText icontext(prefix + name, imgid);
    wxVariant iconTextVariant;
    iconTextVariant << icontext;
    cols.push_back(iconTextVariant);
    cols.push_back(clientData->m_impl ? wxString(wxT("\u274C")) : wxString());
    cols.push_back(fullname);
    m_dataview->AppendItem(cols, (wxUIntPtr)clientData);
}

void OpenResourceDialog::OnTimer([[maybe_unused]] wxTimerEvent& event)
{
    if (m_needRefresh) {
        DoPopulateList();
    }

    m_needRefresh = false;

    // If there is only 1 item in the resource window then highlight it.
    // This allows the user to hit ENTER immediately after to open the item, nice shortcut.
    if (m_dataview && m_dataview->GetItemCount() == 1) {
        DoSelectItem(m_dataview->RowToItem(0));
    }
}

const wxBitmap& OpenResourceDialog::DoGetTagImg(const LSP::SymbolInformation& symbol)
{
    if (m_fileTypeHash.count(symbol.GetKind())) {
        return m_fileTypeHash[symbol.GetKind()];
    }
    // The default
    return clGetManager()->GetStdIcons()->GetMimeBitmaps().GetBitmap(BitmapLoader::kMemberPublic, false);
}

bool OpenResourceDialog::MatchesFilter(const wxString& name)
{
    wxString mod_filter;
    long line_number, column_number;
    GetLineAndColumnFromFilter(m_textCtrlResourceName->GetValue(), mod_filter, line_number, column_number);
    return FileUtils::FuzzyMatch(mod_filter, name);
}

void OpenResourceDialog::OnCheckboxfilesCheckboxClicked(wxCommandEvent& event) { DoPopulateList(); }
void OpenResourceDialog::OnCheckboxshowsymbolsCheckboxClicked(wxCommandEvent& event) { DoPopulateList(); }

void OpenResourceDialog::OnEnter(wxCommandEvent& event) { CallAfter(&OpenResourceDialog::EndModal, wxID_OK); }

void OpenResourceDialog::OnEntrySelected(wxDataViewEvent& event) { event.Skip(); }

std::vector<OpenResourceDialogItemData*> OpenResourceDialog::GetSelections() const
{
    std::vector<OpenResourceDialogItemData*> selections;
    wxDataViewItemArray items;
    m_dataview->GetSelections(items);
    if (items.IsEmpty()) {
        return selections;
    }

    selections.reserve(items.size());
    for (size_t i = 0; i < items.GetCount(); ++i) {
        OpenResourceDialogItemData* data = GetItemData(items.Item(i));
        if (data) {
            if (m_lineNumber != wxNOT_FOUND) {
                data->m_line = m_lineNumber;
            }

            if (m_column != wxNOT_FOUND) {
                data->m_column = m_column;
            }
            selections.push_back(data);
        }
    }
    return selections;
}

void OpenResourceDialog::GetLineAndColumnFromFilter(const wxString& filter,
                                                    wxString& modFilter,
                                                    long& lineNumber,
                                                    long& column)
{
    modFilter = filter;
    lineNumber = wxNOT_FOUND;
    column = wxNOT_FOUND;

    wxString tmpstr = filter;
    tmpstr.Replace("\\", "/");

    const int sep_last = tmpstr.Find('/', true);
    const std::size_t col_first = tmpstr.find(':', (sep_last == wxNOT_FOUND ? 0 : sep_last));
    if (col_first == wxString::npos) {
        return;
    }

    modFilter = tmpstr.substr(0, col_first);
    wxString remainder = tmpstr.substr(col_first);

    auto parts = ::wxStringTokenize(remainder, ":", wxTOKEN_STRTOK);
    if (!parts.empty()) {
        // line number
        parts.Item(0).ToCLong(&lineNumber);
        parts.RemoveAt(0);
    }

    if (!parts.empty()) {
        // column number
        parts.Item(0).ToCLong(&column);
        parts.RemoveAt(0);
    }
}

OpenResourceDialogItemData* OpenResourceDialog::GetItemData(const wxDataViewItem& item) const
{
    return reinterpret_cast<OpenResourceDialogItemData*>(m_dataview->GetItemData(item));
}

void OpenResourceDialog::OnSelectAllText() { m_textCtrlResourceName->SelectAll(); }
