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

#pragma once

#include "CustomControls/IndicatorPanel.hpp"
#include "ai/ResponseCollector.hpp"
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
    virtual void OnAIAvailableUI(wxUpdateUIEvent& event);
    /**
     * @brief Generates release notes by fetching git log between two commits and using an LLM to create formatted
     * notes.
     *
     * This function displays a dialog to get two commit identifiers from the user, fetches the git log
     * between those commits, and then uses a language model (LLM) to generate release notes based on the
     * commit history. The generated release notes are streamed to a newly created file and styled as Markdown.
     *
     * The function handles the following workflow:
     * - Shows a dialog to collect commit IDs, token limits, and formatting options
     * - Fetches git log history between the specified commits in chunks
     * - Constructs prompts from a template using the git history
     * - Sends prompts to the LLM manager for processing
     * - Streams the LLM response to a file in real-time
     * - Applies Markdown styling to the final output
     * - Handles multiple prompts by aggregating responses when needed
     *
     * @note The function uses asynchronous callbacks to handle streaming responses from the LLM.
     * @note The status bar is updated throughout the process to show progress.
     * @note If the git log fetch fails or returns no commits, the function exits early with an error message.
     */
    void GenerateReleaseNotes();
    /**
     * @brief Performs an AI-powered code review of the current git repository changes.
     *
     * This function executes a git diff command to retrieve all changes in the repository,
     * then sends the diff output to an LLM (Language Learning Model) for code review analysis.
     * The AI-generated review is streamed to a newly created or opened file in the editor.
     * The process includes status updates in the status bar and handles cancellation tokens
     * to prevent excessive token usage. The final output is styled as markdown.
     *
     * @note This function displays a warning message box if the git diff command fails.
     * @note The function uses callbacks to handle state changes and streaming responses from the LLM.
     * @note The generated review file is styled as markdown upon completion.
     */
    void DoCodeReview();
    /**
     * @brief Finalizes the release notes by merging and formatting the complete response using an LLM.
     *
     * This function takes the aggregated response from previous release note generation steps,
     * sends it to the LLM with a merge prompt, and streams the final formatted release notes
     * into a newly created or opened editor file. The file is styled as Markdown once the
     * streaming is complete.
     *
     * @param complete_reponse The complete aggregated response containing all the release note information to be
     * finalized
     */
    void FinaliseReleaseNotes(const wxString& complete_reponse);
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
    void OnGitAIDropDown(wxAuiToolBarEvent& event);

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
};
