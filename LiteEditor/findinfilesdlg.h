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

#include "findinfiles_dlg.h"
#include "findreplacedlg.h"
#include "search_thread.h"
#include "macros.h"

class FindInFilesDialog : public FindInFilesDialogBase
{
    FindReplaceData& m_data;
    wxArrayString m_pluginFileMask;
    bool m_transient = false;

protected:
    wxArrayString GetPathsAsArray() const;

protected:
    virtual void OnLookInKeyDown(wxKeyEvent& event);
    virtual void OnReplaceUI(wxUpdateUIEvent& event);
    virtual void OnButtonClose(wxCommandEvent& event);
    virtual void OnFind(wxCommandEvent& event);
    virtual void OnReplace(wxCommandEvent& event);
    void DoSearch();
    void DoSearchReplace();
    void DoSaveSearchPaths();
    SearchData DoGetSearchData();
    void DoSaveOpenFiles();
    void DoSetFileMask();
    void DoAddProjectFiles(const wxString& projectName, wxArrayString& files);

    // Set new search paths
    void DoSetSearchPaths(const wxString& paths);

    // Event Handlers
    virtual void OnClose(wxCloseEvent& event);
    virtual void OnAddPath(wxCommandEvent& event);

    virtual void OnFindWhatUI(wxUpdateUIEvent& event);

    void OnUseDiffColourForCommentsUI(wxUpdateUIEvent& event);
    size_t GetSearchFlags();
    void BuildFindReplaceData();

public:
    FindInFilesDialog(wxWindow* parent, FindReplaceData& data);
    virtual ~FindInFilesDialog();
    void SetSearchPaths(const wxString& paths, bool transient = false);
    void SetFileMask(const wxString& mask);
    int ShowDialog();
};

#endif // FIND_IN_FILES_DLG_H
