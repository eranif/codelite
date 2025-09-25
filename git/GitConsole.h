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

#include "CustomControls/IndicatorPanel.hpp"
#include "bitmap_loader.h"
#include "clGenericSTCStyler.h"
#include "clToolBar.h"
#include "clWorkspaceEvent.hpp"
#include "gitui.h"
#include "wxTerminalCtrl/wxTerminalOutputCtrl.hpp"

#include <wx/dataview.h>

class GitPlugin;
class GitConsole : public GitConsoleBase
{
public:
    GitConsole(wxWindow* parent, GitPlugin* git);
    virtual ~GitConsole();
    void AddText(const wxString& text);
    void AddLine(const wxString& line);
    void PrintPrompt();
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
    void ShowProgress(const wxString& message);
    void HideProgress();
    void UpdateProgress(unsigned long current, const wxString& message);
    bool IsProgressShown() const;
    void ShowLog();
    void SetIndent(size_t depth = 1) { m_indent = depth; }

protected:
    wxString GetIndent() const { return wxString(' ', (m_indent * 4)); }
    virtual void OnUnversionedFileActivated(wxDataViewEvent& event);
    virtual void OnUnversionedFileContextMenu(wxDataViewEvent& event);
    virtual void OnUpdateUI(wxUpdateUIEvent& event);
    virtual void OnAddUnversionedFilesUI(wxUpdateUIEvent& event);
    virtual void OnResetFileUI(wxUpdateUIEvent& event);
    virtual void OnStclogStcChange(wxStyledTextEvent& event);
    virtual void OnApplyPatch(wxCommandEvent& event);
    virtual void OnFileActivated(wxDataViewEvent& event);
    virtual void OnItemSelectedUI(wxUpdateUIEvent& event);
    virtual void OnResetFile(wxCommandEvent& event);
    virtual void OnAddUnstagedFiles(wxCommandEvent& event);
    virtual void OnContextMenu(wxDataViewEvent& event);
    virtual void OnStopGitProcessUI(wxUpdateUIEvent& event);
    virtual void OnClearGitLogUI(wxUpdateUIEvent& event);
    virtual void OnClearGitLog(wxCommandEvent& event);
    virtual void OnGenerateReleaseNotesUI(wxUpdateUIEvent& event);
    virtual void OnGenerateReleaseNotes(wxCommandEvent& event);
    void OnGenerateReleaseNotesDone();
    virtual void OnStopGitProcess(wxCommandEvent& event);
    virtual void OnOpenUnversionedFiles(wxCommandEvent& event);
    virtual void OnAddUnversionedFiles(wxCommandEvent& event);
    void OnSysColoursChanged(clCommandEvent& event);

    void OnOpenFile(wxCommandEvent& e);
    void OnWorkspaceClosed(clWorkspaceEvent& e);
    void OnConfigurationChanged(wxCommandEvent& e);
    wxString GetPrompt() const;
    bool IsPatternFound(const wxString& buffer, const std::unordered_set<wxString>& m) const;
    bool HasAnsiEscapeSequences(const wxString& buffer) const;

    void OnGitPullDropdown(wxAuiToolBarEvent& event) { DoOnDropdown("git_pull", event.GetToolId(), event); }
    void OnGitRebaseDropdown(wxAuiToolBarEvent& event) { DoOnDropdown("git_rebase", event.GetToolId(), event); }

    void DoOnDropdown(const wxString& commandName, int id, const wxAuiToolBarEvent& event);
    void OnDropDownMenuEvent(wxCommandEvent& e);
    void Clear();
    void OnOutputViewTabChanged(clCommandEvent& event);

private:
    wxArrayString GetSelectedUnversionedFiles() const;
    wxArrayString GetSelectedModifiedFiles() const;

    GitPlugin* m_git = nullptr;
    bool m_isVerbose = false;
    BitmapLoader* m_bitmapLoader = nullptr;
    wxBitmap m_modifiedBmp;
    wxBitmap m_untrackedBmp;
    wxBitmap m_folderBmp;
    wxBitmap m_newBmp;
    wxBitmap m_deleteBmp;
    size_t m_indent = 0;
    std::unordered_set<wxString> m_errorPatterns;
    std::unordered_set<wxString> m_successPatterns;
    std::unordered_set<wxString> m_warningPatterns;
    wxString m_buffer;
    wxTerminalOutputCtrl* m_log_view{nullptr};
    IndicatorPanel* m_statusBar{nullptr};

    struct ReleaseNotesGenerationState {
        // used for detecting loop of the model.
        std::vector<std::string> last_tokens;
        std::string response;
        size_t tokens_count{0};
        bool is_multi_requests{false};
        size_t total_batch_count{1};
        size_t current_batch{1};
    } m_releaseNodesGenState;
};
#endif // GITCONSOLE_H
