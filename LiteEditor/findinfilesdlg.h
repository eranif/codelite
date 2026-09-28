//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : findinfilesdlg.h
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
#ifndef FIND_IN_FILES_DLG_H
#define FIND_IN_FILES_DLG_H

#include "findinfiles_dlg.hpp"
#include "macros.h"
#include "search_thread.h"
#include "sessionmanager.h"

class FindInFilesDialog : public FindInFilesDialogBase
{
    FindInFilesSession m_data;
    wxArrayString m_pluginFileMask;
    bool m_transient = false;
    wxWindow* m_handler = nullptr;
    bool m_oldRegexValue;
    bool m_userChangedRegexManually = false;
    bool m_presetSearch = false;

protected:
    void OnRegex(wxCommandEvent& event) override;
    void OnATTN(wxCommandEvent& event) override;
    void OnBUG(wxCommandEvent& event) override;
    void OnFIXME(wxCommandEvent& event) override;
    void OnTODO(wxCommandEvent& event) override;
    void OnFindEnter(wxCommandEvent& event) override;
    void OnReplaceEnter(wxCommandEvent& event) override;
    wxArrayString GetPathsAsArray() const;
    void SetPresets();

protected:
    virtual void OnLookInKeyDown(wxKeyEvent& event);
    void OnReplaceUI(wxUpdateUIEvent& event) override;
    void OnButtonClose(wxCommandEvent& event) override;
    void OnFind(wxCommandEvent& event) override;
    void OnReplace(wxCommandEvent& event) override;
    void DoSearch();
    void DoSearchReplace();
    SearchData DoGetSearchData();
    void DoSaveOpenFiles();
    void DoAddProjectFiles(const wxString& projectName, wxArrayString& files);
    void DoSelectAll();

    // Set new search paths
    void DoAppendSearchPath(const wxString& path);

    // Event Handlers
    virtual void OnClose(wxCloseEvent& event);
    void OnAddPath(wxCommandEvent& event) override;

    void OnFindWhatUI(wxUpdateUIEvent& event) override;

    void OnUseDiffColourForCommentsUI(wxUpdateUIEvent& event);
    size_t GetSearchFlags();
    void SaveFindReplaceData();

public:
    FindInFilesDialog(wxWindow* parent, wxWindow* handler = nullptr);
    ~FindInFilesDialog() override;
    void SetSearchPaths(const wxString& paths, bool transient = false);
    void SetFileMask(const wxString& mask);
    int ShowDialog();
};

#endif // FIND_IN_FILES_DLG_H
