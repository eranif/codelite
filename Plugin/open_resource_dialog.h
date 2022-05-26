//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : open_resource_dialog.h
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

#ifndef __open_resource_dialog__
#define __open_resource_dialog__

#include "LSP/LSPEvent.h"
#include "LSP/basic_types.h"
#include "clAnagram.h"
#include "cl_command_event.h"
#include "codelite_exports.h"
#include "entry.h"
#include "fileextmanager.h"
#include "openresourcedialogbase.h"
#include "wxStringHash.h"

#include <vector>
#include <wx/arrstr.h>
#include <wx/event.h>
#include <wx/timer.h>

class IManager;
class wxTimer;

class WXDLLIMPEXP_SDK OpenResourceDialogItemData : public wxClientData
{
public:
    wxString m_file;
    int m_line = 0;
    int m_column = wxNOT_FOUND;
    wxString m_pattern;
    wxString m_name;
    wxString m_scope;
    bool m_impl;

public:
    OpenResourceDialogItemData()
        : m_file("")
        , m_line(wxNOT_FOUND)
        , m_pattern("")
        , m_name("")
        , m_scope("")
        , m_impl(false)
    {
    }

    OpenResourceDialogItemData(const wxString& file, int line, const wxString& pattern, const wxString& name,
                               const wxString& scope)
        : m_file(file)
        , m_line(line)
        , m_pattern(pattern)
        , m_name(name)
        , m_scope(scope)
        , m_impl(false)
    {
    }

    virtual ~OpenResourceDialogItemData() {}

    bool IsOk() const;
};

namespace std
{
template <> struct hash<LSP::eSymbolKind> {
    std::size_t operator()(LSP::eSymbolKind sk) const { return static_cast<size_t>(sk); }
};
} // namespace std

class WXDLLIMPEXP_SDK OpenResourceDialog : public OpenResourceDialogBase
{
    IManager* m_manager;
    std::unordered_multimap<wxString, wxString> m_files;
    std::unordered_map<LSP::eSymbolKind, int> m_fileTypeHash;
    wxTimer* m_timer;
    bool m_needRefresh;
    wxArrayString m_filters;
    wxArrayString m_userFilters;
    long m_lineNumber = wxNOT_FOUND;
    long m_column = wxNOT_FOUND;

protected:
    virtual void OnEnter(wxCommandEvent& event);
    virtual void OnEntrySelected(wxDataViewEvent& event);
    virtual void OnEntryActivated(wxDataViewEvent& event);
    virtual void OnCheckboxfilesCheckboxClicked(wxCommandEvent& event);
    virtual void OnCheckboxshowsymbolsCheckboxClicked(wxCommandEvent& event);
    void OnWorkspaceSymbols(LSPEvent& event);

    void DoPopulateList();
    void DoPopulateWorkspaceFile();
    bool MatchesFilter(const wxString& name);
    void DoPopulateTags(const std::vector<LSP::SymbolInformation>& symbols);
    void DoSelectItem(const wxDataViewItem& item);
    void Clear();
    void DoAppendLine(const wxString& name, const wxString& fullname, bool boldFont,
                      OpenResourceDialogItemData* clientData, int imgid);
    int DoGetTagImg(const LSP::SymbolInformation& symbol);
    OpenResourceDialogItemData* GetItemData(const wxDataViewItem& item) const;
    void OnSelectAllText();

protected:
    // Handlers for OpenResourceDialogBase events.
    void OnText(wxCommandEvent& event);
    void OnUsePartialMatching(wxCommandEvent& event);
    void OnKeyDown(wxKeyEvent& event);
    void OnOK(wxCommandEvent& event);
    void OnOKUI(wxUpdateUIEvent& event);
    void OnTimer(wxTimerEvent& event);

    DECLARE_EVENT_TABLE()

    void GetLineAndColumnFromFilter(const wxString& filter, wxString& modFilter, long& lineNumber, long& column);

public:
    /** Constructor */
    OpenResourceDialog(wxWindow* parent, IManager* manager, const wxString& initialSelection);
    virtual ~OpenResourceDialog();

    std::vector<OpenResourceDialogItemData*> GetSelections() const;
    wxArrayString& GetFilters() { return m_filters; }

    /**
     * \brief helper method for opening the selection
     * \param selection
     * \return
     */
    static void OpenSelection(const OpenResourceDialogItemData& selection, IManager* manager);
};

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_OPEN_RESOURCE_FILE_SELECTED, clCommandEvent);
#endif // __open_resource_dialog__
