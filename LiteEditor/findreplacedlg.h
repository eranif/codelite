//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : findreplacedlg.h
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
#ifndef FIND_REPLACE_DLG_H
#define FIND_REPLACE_DLG_H

#include "cl_config.h"
#include <wx/dialog.h>

class wxTextCtrl;
class wxCheckBox;
class wxButton;
class wxStaticText;

#define wxFRD_MATCHCASE (1 << 0)
#define wxFRD_MATCHWHOLEWORD (1 << 1)
#define wxFRD_REGULAREXPRESSION (1 << 2)
#define wxFRD_SEARCHUP (1 << 3)
#define wxFRD_WRAPSEARCH (1 << 4)
#define wxFRD_SELECTIONONLY (1 << 5)
#define wxFRD_DISPLAYSCOPE (1 << 6)
#define wxFRD_SAVE_BEFORE_SEARCH (1 << 7)
#define wxFRD_SKIP_COMMENTS (1 << 8)
#define wxFRD_SKIP_STRINGS (1 << 9)
#define wxFRD_COLOUR_COMMENTS (1 << 10)
#define wxFRD_SEPARATETAB_DISPLAY (1 << 11)
#define wxFRD_ENABLE_PIPE_SUPPORT (1 << 12)

#define FIND_DLG 0
#define REPLACE_DLG 1

#ifndef WXDLLIMPEXP_LE
#ifdef WXMAKINGDLL
#define WXDLLIMPEXP_LE WXIMPORT
#elif defined(WXUSINGDLL)
#define WXDLLIMPEXP_LE WXEXPORT
#else
#define WXDLLIMPEXP_LE
#endif // WXDLLIMPEXP_LE
#endif

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_LE, wxEVT_FRD_FIND_NEXT, wxCommandEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_LE, wxEVT_FRD_CLOSE, wxCommandEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_LE, wxEVT_FRD_REPLACE, wxCommandEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_LE, wxEVT_FRD_REPLACEALL, wxCommandEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_LE, wxEVT_FRD_BOOKMARKALL, wxCommandEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_LE, wxEVT_FRD_CLEARBOOKMARKS, wxCommandEvent);

class FindReplaceData : public clConfigItem
{
    wxArrayString m_replaceString;
    wxArrayString m_findString;
    size_t m_flags;
    wxString m_paths;
    wxString m_encoding;
    wxArrayString m_fileMask;
    wxString m_selectedMask;

protected:
    void TruncateArray(wxArrayString& arr, size_t maxSize);

public:
    /**
     * @brief
     * @param json
     */
    virtual void FromJSON(const JSONItem& json);
    /**
     * @brief
     * @return
     */
    virtual JSONItem ToJSON() const;
    FindReplaceData();
    virtual ~FindReplaceData() {}

    wxString GetFindString() const;
    wxString GetReplaceString() const;
    void SetFindString(const wxString& str);
    void SetReplaceString(const wxString& str);

    wxArrayString GetFindStringArr() const;

    wxArrayString GetReplaceStringArr() const;

    void SetEncoding(const wxString& encoding) { this->m_encoding = encoding; }
    void SetFileMask(const wxArrayString& fileMask) { this->m_fileMask = fileMask; }
    void SetFlags(size_t flags) { this->m_flags = flags; }
    void SetSearchPaths(const wxString& searchPaths);
    void SetSelectedMask(const wxString& selectedMask) { this->m_selectedMask = selectedMask; }
    const wxString& GetEncoding() const { return m_encoding; }
    const wxArrayString& GetFileMask() const { return m_fileMask; }
    size_t GetFlags() const { return m_flags; }
    const wxString& GetSearchPaths() const { return m_paths; }
    const wxString& GetSelectedMask() const { return m_selectedMask; }
};

class wxStaticText;
class wxComboBox;
class wxGridBagSizer;
class wxStaticBoxSizer;

enum frd_showzero { frd_dontshowzeros, frd_showzeros };

class FindReplaceDialog : public wxDialog
{
    wxEvtHandler* m_owner;

    FindReplaceData m_data;

    // Options
    wxComboBox* m_findString;
    wxComboBox* m_replaceString;
    wxCheckBox* m_matchCase;
    wxCheckBox* m_matchWholeWord;
    wxCheckBox* m_regualrExpression;
    wxCheckBox* m_searchUp;
    wxCheckBox* m_selectionOnly;

    // Buttons
    wxButton* m_find;
    wxButton* m_replace;
    wxButton* m_replaceAll;
    wxButton* m_markAll;
    wxButton* m_clearBookmarks;
    wxButton* m_cancel;
    wxStaticText* m_replacementsMsg;
    wxStaticText* m_replaceWithLabel;
    wxGridBagSizer* gbSizer;
    wxStaticBoxSizer* sz;
    int m_kind;
    unsigned int m_replacedCount;

public:
    virtual ~FindReplaceDialog();
    FindReplaceDialog();
    FindReplaceDialog(wxWindow* parent, const FindReplaceData& data, wxWindowID id = wxID_ANY,
                      const wxString& caption = _("Find / Replace"), const wxPoint& pos = wxDefaultPosition,
                      const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

    // Creation
    bool Create(wxWindow* parent, const FindReplaceData& data, wxWindowID id = wxID_ANY,
                const wxString& caption = _("Find / Replace"), const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

    // Return the data
    FindReplaceData& GetData() { return m_data; }

    virtual bool Show(int kind);

    void SetEventOwner(wxEvtHandler* owner) { m_owner = owner; }
    wxEvtHandler* GetEventOwner() const { return m_owner; }

    // Set the replacements message
    void SetReplacementsMessage(enum frd_showzero showzero = frd_showzeros);
    unsigned int GetReplacedCount() { return m_replacedCount; }
    void IncReplacedCount() { ++m_replacedCount; }
    void ResetReplacedCount() { m_replacedCount = 0; }
    void ResetSelectionOnlyFlag();
    void SetFindReplaceData(FindReplaceData& data, bool focus);

protected:
    void ShowReplaceControls(bool show);
    void CreateGUIControls();
    void ConnectEvents();
    void OnClick(wxCommandEvent& event);
    void OnFindEvent(wxCommandEvent& event);
    void OnSelectionOnlyUI(wxUpdateUIEvent& event);
    void SendEvent(wxEventType type);

    void OnKeyDown(wxKeyEvent& event);
    DECLARE_EVENT_TABLE()
    void OnClose(wxCloseEvent& event);
};

#endif // FIND_REPLACE_DLG_H
