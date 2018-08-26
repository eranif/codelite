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

#include "clAnagram.h"
#include "codelite_exports.h"
#include "entry.h"
#include "openresourcedialogbase.h"
#include "wxStringHash.h"
#include <vector>
#include <wx/arrstr.h>
#include <wx/timer.h>

class IManager;
class wxTimer;

class WXDLLIMPEXP_SDK OpenResourceDialogItemData : public wxClientData
{
public:
    wxString m_file;
    int m_line;
    wxString m_pattern;
    wxString m_name;
    wxString m_scope;
    bool m_impl;

public:
    OpenResourceDialogItemData()
        : m_file(wxT(""))
        , m_line(wxNOT_FOUND)
        , m_pattern(wxT(""))
        , m_name(wxT(""))
        , m_scope(wxT(""))
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

/** Implementing OpenResourceDialogBase */
class WXDLLIMPEXP_SDK OpenResourceDialog : public OpenResourceDialogBase
{
    IManager* m_manager;
    std::unordered_multimap<wxString, wxString> m_files;
    wxTimer* m_timer;
    bool m_needRefresh;
    std::unordered_map<wxString, wxBitmap> m_tagImgMap;
    wxArrayString m_filters;
    wxArrayString m_userFilters;
    long m_lineNumber;

protected:
    virtual void OnEnter(wxCommandEvent& event);
    virtual void OnEntrySelected(wxDataViewEvent& event);
    virtual void OnEntryActivated(wxDataViewEvent& event);
    virtual void OnCheckboxfilesCheckboxClicked(wxCommandEvent& event);
    virtual void OnCheckboxshowsymbolsCheckboxClicked(wxCommandEvent& event);
    void DoPopulateList();
    void DoPopulateWorkspaceFile();
    bool MatchesFilter(const wxString& name);
    void DoPopulateTags();
    void DoSelectItem(const wxDataViewItem& item);
    void Clear();
    wxDataViewItem DoAppendLine(const wxString& name, const wxString& fullname, bool boldFont,
                                OpenResourceDialogItemData* clientData, const wxBitmap& bmp);
    wxBitmap DoGetTagImg(TagEntryPtr tag);

protected:
    // Handlers for OpenResourceDialogBase events.
    void OnText(wxCommandEvent& event);
    void OnUsePartialMatching(wxCommandEvent& event);
    void OnKeyDown(wxKeyEvent& event);
    void OnOK(wxCommandEvent& event);
    void OnOKUI(wxUpdateUIEvent& event);
    void OnTimer(wxTimerEvent& event);

    DECLARE_EVENT_TABLE()

    void GetLineNumberFromFilter(const wxString& filter, wxString& modFilter, long& lineNumber);

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

#endif // __open_resource_dialog__
