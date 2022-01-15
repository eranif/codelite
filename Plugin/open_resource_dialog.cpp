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

#include "ColoursAndFontsManager.h"
#include "ServiceProviderManager.h"
#include "bitmap_loader.h"
#include "clFileSystemWorkspace.hpp"
#include "clWorkspaceManager.h"
#include "ctags_manager.h"
#include "editor_config.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "fileutils.h"
#include "globals.h"
#include "ieditor.h"
#include "imanager.h"
#include "macros.h"
#include "project.h"
#include "window_locker.h"
#include "windowattrmanager.h"
#include "workspace.h"

#include <algorithm>
#include <codelite_events.h>
#include <vector>
#include <wx/event.h>
#include <wx/filefn.h>
#include <wx/imaglist.h>
#include <wx/wupdlock.h>
#include <wx/xrc/xmlres.h>

BEGIN_EVENT_TABLE(OpenResourceDialog, OpenResourceDialogBase)
EVT_TIMER(XRCID("OR_TIMER"), OpenResourceDialog::OnTimer)
END_EVENT_TABLE()

wxDEFINE_EVENT(wxEVT_OPEN_RESOURCE_FILE_SELECTED, clCommandEvent);

OpenResourceDialog::OpenResourceDialog(wxWindow* parent, IManager* manager, const wxString& initialSelection)
    : OpenResourceDialogBase(parent)
    , m_manager(manager)
    , m_needRefresh(false)
    , m_lineNumber(wxNOT_FOUND)
{
    m_dataview->SetSortFunction(nullptr);
    m_dataview->SetBitmaps(clGetManager()->GetStdIcons()->GetStandardMimeBitmapListPtr());
    auto lexer = ColoursAndFontsManager::Get().GetLexer("text");
    if(lexer) {
        m_dataview->SetDefaultFont(lexer->GetFontForSyle(0, m_dataview));
    }
    EventNotifier::Get()->Bind(wxEVT_LSP_WORKSPACE_SYMBOLS, &OpenResourceDialog::OnWorkspaceSymbols, this);

    // initialize the file-type hash
    m_fileTypeHash[LSP::kSK_Class] = BitmapLoader::kClass;
    m_fileTypeHash[LSP::kSK_Struct] = BitmapLoader::kStruct;
    m_fileTypeHash[LSP::kSK_Namespace] = BitmapLoader::kNamespace;
    m_fileTypeHash[LSP::kSK_Variable] = BitmapLoader::kMemberPublic;
    m_fileTypeHash[LSP::kSK_TypeParameter] = BitmapLoader::kTypedef;
    m_fileTypeHash[LSP::kSK_Property] = BitmapLoader::kMemberPublic;
    m_fileTypeHash[LSP::kSK_Function] = BitmapLoader::kFunctionPublic;
    m_fileTypeHash[LSP::kSK_Method] = BitmapLoader::kFunctionPublic;
    m_fileTypeHash[LSP::kSK_Constructor] = BitmapLoader::kFunctionPublic;
    m_fileTypeHash[LSP::kSK_TypeParameter] = BitmapLoader::kMacro;
    m_fileTypeHash[LSP::kSK_Enum] = BitmapLoader::kEnum;
    m_fileTypeHash[LSP::kSK_EnumMember] = BitmapLoader::kEnumerator;

    m_timer = new wxTimer(this, XRCID("OR_TIMER"));

    m_textCtrlResourceName->SetFocus();
    SetLabel(_("Open resource..."));
    SetName("OpenResourceDialog");

    // load all files from the workspace
    if(::clIsCxxWorkspaceOpened()) {
        if(m_manager->IsWorkspaceOpen()) {
            wxArrayString projects;
            m_manager->GetWorkspace()->GetProjectList(projects);

            for(size_t i = 0; i < projects.GetCount(); i++) {
                ProjectPtr p = m_manager->GetWorkspace()->GetProject(projects.Item(i));
                if(p) {
                    const Project::FilesMap_t& files = p->GetFiles();
                    // convert std::vector to wxArrayString
                    std::for_each(files.begin(), files.end(), [&](const Project::FilesMap_t::value_type& vt) {
                        wxFileName fn(vt.second->GetFilename());
                        m_files.insert(std::make_pair(fn.GetFullName(), fn.GetFullPath()));
                    });
                }
            }
        } else if(clFileSystemWorkspace::Get().IsOpen()) {
            const std::vector<wxFileName>& files = clFileSystemWorkspace::Get().GetFiles();
            for(const wxFileName& fn : files) {
                m_files.insert({ fn.GetFullName(), fn.GetFullPath() });
            }
        }
    } else if(clWorkspaceManager::Get().IsWorkspaceOpened()) {
        // the workspace API to get list of files
        wxArrayString files;
        clWorkspaceManager::Get().GetWorkspace()->GetWorkspaceFiles(files);
        wxStringSet_t unique_files;
        m_files.reserve(files.size());
        for(const auto& file : files) {
            if(unique_files.count(file) == 0) {
                unique_files.insert(file);
                wxFileName fn(file);
                // keep the file as-is do not "format" it by calling
                // fn.GetFullPath() since we might be on Windows and we display
                // Linux path style files
                m_files.insert({ fn.GetFullName(), file });
            }
        }
    }

    wxString lastStringTyped = clConfig::Get().Read("OpenResourceDialog/SearchString", wxString());
    // Set the initial selection
    // We use here 'SetValue' so an event will get fired and update the control
    if(!initialSelection.IsEmpty()) {
        m_textCtrlResourceName->SetValue(initialSelection);
        CallAfter(&OpenResourceDialog::OnSelectAllText);
    } else if(!lastStringTyped.IsEmpty()) {
        m_textCtrlResourceName->SetValue(lastStringTyped);
        CallAfter(&OpenResourceDialog::OnSelectAllText);
    }

    bool showFiles = clConfig::Get().Read("OpenResourceDialog/ShowFiles", true);
    bool showSymbols = clConfig::Get().Read("OpenResourceDialog/ShowSymbols", true);
    m_checkBoxFiles->SetValue(showFiles);
    m_checkBoxShowSymbols->SetValue(showSymbols);
    ::clSetDialogBestSizeAndPosition(this);
}

OpenResourceDialog::~OpenResourceDialog()
{
    m_timer->Stop();
    wxDELETE(m_timer);

    EventNotifier::Get()->Unbind(wxEVT_LSP_WORKSPACE_SYMBOLS, &OpenResourceDialog::OnWorkspaceSymbols, this);

    // Store current values
    clConfig::Get().Write("OpenResourceDialog/ShowFiles", m_checkBoxFiles->IsChecked());
    clConfig::Get().Write("OpenResourceDialog/ShowSymbols", m_checkBoxShowSymbols->IsChecked());
    clConfig::Get().Write("OpenResourceDialog/SearchString", m_textCtrlResourceName->GetValue());
}

void OpenResourceDialog::OnText(wxCommandEvent& event)
{
    event.Skip();
    m_timer->Stop();
    m_timer->Start(200, true);

    wxString filter = m_textCtrlResourceName->GetValue();
    filter.Trim().Trim(false);
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
    if(data) {
        EndModal(wxID_OK);
    }
}

void OpenResourceDialog::DoPopulateList()
{
    Clear();
    wxString name = m_textCtrlResourceName->GetValue();
    name.Trim().Trim(false);
    if(name.IsEmpty()) {
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
    for(size_t i = 0; i < m_userFilters.GetCount(); ++i) {
        m_userFilters.Item(i).MakeLower();
    }

    // Build the filter class
    if(m_checkBoxFiles->IsChecked()) {
        DoPopulateWorkspaceFile();
    }

    if(m_checkBoxShowSymbols->IsChecked() && (nLineNumber == -1)) {
        clCodeCompletionEvent workspace_symbols_event{ wxEVT_CC_WORKSPACE_SYMBOLS };
        workspace_symbols_event.SetString(name);

        // CC events are using ServiceProviderManager
        ServiceProviderManager::Get().ProcessEvent(workspace_symbols_event);
    }
}

void OpenResourceDialog::OnWorkspaceSymbols(LSPEvent& event) { DoPopulateTags(event.GetSymbolsInformation()); }

void OpenResourceDialog::DoPopulateTags(const vector<LSP::SymbolInformation>& symbols)
{
    // Next, add the tags
    if(m_userFilters.IsEmpty() || symbols.empty())
        return;

    for(const LSP::SymbolInformation& symbol : symbols) {
        if(!MatchesFilter(symbol.GetName())) {
            continue;
        }

        // keep the fullpath
        DoAppendLine(symbol.GetName(), symbol.GetContainerName(), false,
                     new OpenResourceDialogItemData(symbol.GetLocation().GetPath(),
                                                    symbol.GetLocation().GetRange().GetEnd().GetLine() + 1,
                                                    wxEmptyString, symbol.GetName(), symbol.GetContainerName()),
                     DoGetTagImg(symbol));
    }

    wxString filter = (m_userFilters.GetCount() == 1) ? m_userFilters.Item(0) : "";
    if(!filter.IsEmpty()) {
        wxDataViewItem matchedItem =
            m_dataview->FindNext(wxDataViewItem(nullptr), filter, 0,
                                 wxDV_SEARCH_ICASE | wxDV_SEARCH_METHOD_EXACT | wxDV_SEARCH_INCLUDE_CURRENT_ITEM);
        if(matchedItem.IsOk()) {
            DoSelectItem(matchedItem);
        }
    }
}

void OpenResourceDialog::DoPopulateWorkspaceFile()
{
    // do we need to include files?
    if(!m_filters.IsEmpty() && m_filters.Index(KIND_FILE) == wxNOT_FOUND)
        return;

    if(!m_userFilters.IsEmpty()) {

        std::unordered_multimap<wxString, wxString>::iterator iter = m_files.begin();
        const int maxFileSize = 100;
        int counter = 0;
        for(; (iter != m_files.end()) && (counter < maxFileSize); iter++) {
            const wxString& fullpath = iter->second;
            if(!MatchesFilter(fullpath))
                continue;

            wxFileName fn(iter->second);
            int imgId = clGetManager()->GetStdIcons()->GetMimeImageId(fn.GetFullName());
            DoAppendLine(fn.GetFullName(), iter->second, false,
                         new OpenResourceDialogItemData(iter->second, -1, wxT(""), fn.GetFullName(), wxT("")), imgId);
            ++counter;
        }
    }
}

void OpenResourceDialog::Clear()
{
    // list control does not own the client data, we need to free it ourselves
    m_dataview->DeleteAllItems([](wxUIntPtr ptr) {
        OpenResourceDialogItemData* cd = reinterpret_cast<OpenResourceDialogItemData*>(ptr);
        wxDELETE(cd);
    });
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
    if(EventNotifier::Get()->ProcessEvent(activateEvent))
        return;

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
    if(m_dataview->IsEmpty()) {
        return;
    }

    if(event.GetKeyCode() == WXK_DOWN || event.GetKeyCode() == WXK_UP || event.GetKeyCode() == WXK_NUMPAD_UP ||
       event.GetKeyCode() == WXK_NUMPAD_DOWN) {
        event.Skip(false);

        if(GetDataview()->GetSelectedItemsCount() == 0) {
            // Just select the first entry
            DoSelectItem(GetDataview()->RowToItem(0));
        } else {
            // fire char hook event to the DV control
            // so it will handle the keyboard movement itself
            wxKeyEvent charHook = event;
            charHook.SetEventObject(m_dataview);
            charHook.SetEventType(wxEVT_CHAR_HOOK);
            GetDataview()->GetEventHandler()->ProcessEvent(charHook);
        }

        // Set the focus back to the text control
        m_textCtrlResourceName->CallAfter(&wxTextCtrl::SetFocus);
    }
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

void OpenResourceDialog::DoAppendLine(const wxString& name, const wxString& fullname, bool boldFont,
                                      OpenResourceDialogItemData* clientData, int imgid)
{
    wxString prefix;
    clientData->m_impl = boldFont;
    wxVector<wxVariant> cols;
    cols.push_back(::MakeBitmapIndexText(prefix + name, imgid));
    cols.push_back(clientData->m_impl ? wxString(wxT("\u274C")) : wxString());
    cols.push_back(fullname);
    m_dataview->AppendItem(cols, (wxUIntPtr)clientData);
}

void OpenResourceDialog::OnTimer(wxTimerEvent& event)
{
    if(m_needRefresh) {
        DoPopulateList();
    }

    m_needRefresh = false;

    // If there is only 1 item in the resource window then highlight it.
    // This allows the user to hit ENTER immediately after to open the item, nice shortcut.
    if(m_dataview && m_dataview->GetItemCount() == 1) {
        DoSelectItem(m_dataview->RowToItem(0));
    }
}

int OpenResourceDialog::DoGetTagImg(const LSP::SymbolInformation& symbol)
{
    int imgId = BitmapLoader::kMemberPublic; // The default
    if(m_fileTypeHash.count(symbol.GetKind())) {
        imgId = m_fileTypeHash[symbol.GetKind()];
    }
    return clGetManager()->GetStdIcons()->GetImageIndex(imgId);
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
    if(items.IsEmpty()) {
        return selections;
    }

    selections.reserve(items.size());
    for(size_t i = 0; i < items.GetCount(); ++i) {
        OpenResourceDialogItemData* data = GetItemData(items.Item(i));
        if(data) {
            if(m_lineNumber != wxNOT_FOUND) {
                data->m_line = m_lineNumber;
            }

            if(m_column != wxNOT_FOUND) {
                data->m_column = m_column;
            }
            selections.push_back(data);
        }
    }
    return selections;
}

void OpenResourceDialog::GetLineAndColumnFromFilter(const wxString& filter, wxString& modFilter, long& lineNumber,
                                                    long& column)
{
    modFilter = filter;
    lineNumber = wxNOT_FOUND;
    column = wxNOT_FOUND;

    wxString tmpstr = filter;
    tmpstr.Replace("\\", "/");

    wxString remainder = filter.AfterLast('/');
    if(remainder.find(':') == wxString::npos) {
        return;
    }

    auto parts = ::wxStringTokenize(remainder, ":", wxTOKEN_STRTOK);
    // the first part is the name
    modFilter = parts.Item(0);
    parts.RemoveAt(0);

    if(!parts.empty()) {
        // line number
        parts.Item(0).ToCLong(&lineNumber);
        parts.RemoveAt(0);
    }

    if(!parts.empty()) {
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
