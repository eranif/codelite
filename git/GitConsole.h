//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : GitConsole.h
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

#ifndef GITCONSOLE_H
#define GITCONSOLE_H
#include "bitmap_loader.h"
#include "clGenericSTCStyler.h"
#include "gitui.h"
#include <wx/dataview.h>

class clToolBar;
class GitPlugin;
class GitConsole : public GitConsoleBase
{
    GitPlugin* m_git;
    bool m_isVerbose;
    BitmapLoader* m_bitmapLoader;
    wxBitmap m_modifiedBmp;
    wxBitmap m_untrackedBmp;
    wxBitmap m_folderBmp;
    wxBitmap m_newBmp;
    wxBitmap m_deleteBmp;
    clGenericSTCStyler::Ptr_t m_styler;
    size_t m_indent = 0;

public:
    GitConsole(wxWindow* parent, GitPlugin* git);
    virtual ~GitConsole();
    void AddRawText(const wxString& text);
    void AddText(const wxString& text);
    bool IsVerbose() const;
    void UpdateTreeView(const wxString& output);

    /**
     * @brief return true if there are any deleted/new/modified items
     * @return
     */
    bool IsDirty() const;

    //
    // Progress bar API
    //
    void ShowProgress(const wxString& message, bool pulse = false);
    void HideProgress();
    void UpdateProgress(unsigned long current, const wxString& message);
    bool IsProgressShown() const;
    void PulseProgress();
    void ShowLog();
    void SetIndent(size_t depth = 1) { m_indent = depth; }

protected:
    wxString GetIndent() const { return wxString(' ', (m_indent * 4)); }
    virtual void OnUnversionedFileActivated(wxDataViewEvent& event);
    virtual void OnUnversionedFileContextMenu(wxDataViewEvent& event);
    virtual void OnUpdateUI(wxUpdateUIEvent& event);
    virtual void OnStclogStcChange(wxStyledTextEvent& event);
    virtual void OnApplyPatch(wxCommandEvent& event);
    virtual void OnFileActivated(wxDataViewEvent& event);
    virtual void OnItemSelectedUI(wxUpdateUIEvent& event);
    virtual void OnResetFile(wxCommandEvent& event);
    virtual void OnContextMenu(wxDataViewEvent& event);
    virtual void OnStopGitProcessUI(wxUpdateUIEvent& event);
    virtual void OnClearGitLogUI(wxUpdateUIEvent& event);
    virtual void OnClearGitLog(wxCommandEvent& event);
    virtual void OnStopGitProcess(wxCommandEvent& event);
    virtual void OnOpenUnversionedFiles(wxCommandEvent& event);
    virtual void OnAddUnversionedFiles(wxCommandEvent& event);

    void OnOpenFile(wxCommandEvent& e);
    void OnCloseView(wxCommandEvent& e);
    void OnWorkspaceClosed(wxCommandEvent& e);
    void OnConfigurationChanged(wxCommandEvent& e);

    void OnGitPullDropdown(wxCommandEvent& event) { DoOnDropdown("git_pull", XRCID("git_pull")); }
    void OnGitRebaseDropdown(wxCommandEvent& event) { DoOnDropdown("git_rebase", XRCID("git_rebase")); }

    void DoOnDropdown(const wxString& commandName, int id);
    void OnDropDownMenuEvent(wxCommandEvent& e);
    void Clear();
    wxArrayString GetSelectedUnversionedFiles() const;
};
#endif // GITCONSOLE_H
