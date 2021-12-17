//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : code_completion_manager.h
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

#ifndef CODECOMPLETIONMANAGER_H
#define CODECOMPLETIONMANAGER_H

#include "CompileCommandsGenerator.h"
#include "CxxPreProcessorCache.h"
#include "ServiceProvider.h"
#include "cl_command_event.h"
#include "cl_editor.h"
#include <thread>
#include <wx/event.h>
#include <wx/filename.h>
#include <wx/string.h>

class CodeCompletionManager : public ServiceProvider
{
protected:
    size_t m_options = 0;
    bool m_wordCompletionRefreshNeeded = false;
    bool m_buildInProgress = false;
    std::thread* m_compileCommandsThread = nullptr;
    wxFileName m_compileCommands;
    time_t m_compileCommandsLastModified = 0;
    CompileCommandsGenerator::Ptr_t m_compileCommandsGenerator;

protected:
    void DoProcessCompileCommands();
    static void ThreadProcessCompileCommandsEntry(CodeCompletionManager* owner, const wxString& rootFolder);
    void CompileCommandsFileProcessed(const wxArrayString& includePaths);
    size_t CreateBlockCommentKeywordsList(wxCodeCompletionBoxEntry::Vec_t& entries) const;

protected:
    // Event handlers
    void OnBuildEnded(clBuildEvent& e);
    void OnFilesAdded(clCommandEvent& e);
    void OnWorkspaceLoaded(clWorkspaceEvent& e);
    void OnWorkspaceClosed(clWorkspaceEvent& event);

    void OnBuildStarted(clBuildEvent& e);
    void OnAppActivated(wxActivateEvent& e);
    void OnCompileCommandsFileGenerated(clCommandEvent& event);
    void OnFileSaved(clCommandEvent& event);
    void OnFileLoaded(clCommandEvent& event);
    void OnWorkspaceConfig(wxCommandEvent& event);
    void OnEnvironmentVariablesModified(clCommandEvent& event);
    void OnBlockCommentCodeComplete(clCodeCompletionEvent& event);
    void OnBlockCommentWordComplete(clCodeCompletionEvent& event);

public:
    CodeCompletionManager();
    virtual ~CodeCompletionManager();

    void SetWordCompletionRefreshNeeded(bool wordCompletionRefreshNeeded)
    {
        this->m_wordCompletionRefreshNeeded = wordCompletionRefreshNeeded;
    }

    bool GetWordCompletionRefreshNeeded() const { return m_wordCompletionRefreshNeeded; }
    void SetOptions(size_t options) { this->m_options = options; }
    size_t GetOptions() const { return m_options; }

    static CodeCompletionManager& Get();
    static void Release();
};

#endif // CODECOMPLETIONMANAGER_H
