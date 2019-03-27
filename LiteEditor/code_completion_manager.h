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

#include <wx/string.h>
#include <wx/filename.h>
#include "cl_editor.h"
#include "cl_command_event.h"
#include <wx/event.h>
#include "CxxPreProcessorThread.h"
#include "CxxPreProcessorCache.h"
#include "CxxUsingNamespaceCollectorThread.h"
#include <thread>
#include "CompileCommandsGenerator.h"
#include "ServiceProvider.h"

class CodeCompletionManager : public ServiceProvider
{
protected:
    size_t m_options;
    bool m_wordCompletionRefreshNeeded;
    bool m_buildInProgress;
    CxxPreProcessorThread m_preProcessorThread;
    CxxUsingNamespaceCollectorThread m_usingNamespaceThread;
    std::thread* m_compileCommandsThread = nullptr;
    wxFileName m_compileCommands;
    time_t m_compileCommandsLastModified = 0;
    CompileCommandsGenerator::Ptr_t m_compileCommandsGenerator;

protected:
    /// ctags implementions
    bool DoCtagsWordCompletion(clEditor* editor, const wxString& expr, const wxString& word);
    bool DoCtagsCalltip(clEditor* editor, int line, const wxString& expr, const wxString& text, const wxString& word);
    bool DoCtagsCodeComplete(clEditor* editor, int line, const wxString& expr, const wxString& text);
    bool DoCtagsGotoImpl(clEditor* editor);
    bool DoCtagsGotoDecl(clEditor* editor);

    void DoUpdateOptions();
    void DoProcessCompileCommands();
    static void ThreadProcessCompileCommandsEntry(CodeCompletionManager* owner, const wxString& rootFolder);
    void CompileCommandsFileProcessed(const wxArrayString& includePaths);
    size_t CreateBlockCommentKeywordsList(wxCodeCompletionBoxEntry::Vec_t& entries) const;

protected:
    // Event handlers
    void OnBuildEnded(clBuildEvent& e);
    void OnFilesAdded(clCommandEvent& e);
    void OnWorkspaceLoaded(wxCommandEvent& e);

    void OnBuildStarted(clBuildEvent& e);
    void OnAppActivated(wxActivateEvent& e);
    void OnCompileCommandsFileGenerated(clCommandEvent& event);
    void OnFileSaved(clCommandEvent& event);
    void OnFileLoaded(clCommandEvent& event);
    void OnWorkspaceConfig(wxCommandEvent& event);
    void OnWorkspaceClosed(wxCommandEvent& event);
    void OnEnvironmentVariablesModified(clCommandEvent& event);
    void OnBlockCommentCodeComplete(clCodeCompletionEvent& event);
    void OnBlockCommentWordComplete(clCodeCompletionEvent& event);

protected:
    // Code completion handlers
    void OnCodeCompletion(clCodeCompletionEvent& event);
    void OnWordCompletion(clCodeCompletionEvent& event);
    void OnFindSymbol(clCodeCompletionEvent& event);
    void OnFindDecl(clCodeCompletionEvent& event);
    void OnFindImpl(clCodeCompletionEvent& event);
    void OnFunctionCalltip(clCodeCompletionEvent& event);
    void OnTypeInfoToolTip(clCodeCompletionEvent& event);
    
public:
    CodeCompletionManager();
    virtual ~CodeCompletionManager();
    
    /**
     * @brief force a refresh based on the current settings
     */
    void RefreshPreProcessorColouring();

    // Callback for collecting macros completed
    void OnParseThreadCollectedMacros(const wxArrayString& definitions, const wxString& filename);

    // Callback for collecting 'using namespaces' completed
    void OnFindUsingNamespaceDone(const wxArrayString& usingNamespace, const wxString& filename);

    void SetWordCompletionRefreshNeeded(bool wordCompletionRefreshNeeded)
    {
        this->m_wordCompletionRefreshNeeded = wordCompletionRefreshNeeded;
    }
    bool GetWordCompletionRefreshNeeded() const { return m_wordCompletionRefreshNeeded; }
    void SetOptions(size_t options) { this->m_options = options; }
    size_t GetOptions() const { return m_options; }

    static CodeCompletionManager& Get();
    static void Release();

    bool WordCompletion(clEditor* editor, const wxString& expr, const wxString& word);
    bool Calltip(clEditor* editor, int line, const wxString& expr, const wxString& text, const wxString& word);
    bool CodeComplete(clEditor* editor, int line, const wxString& expr, const wxString& text);
    void ProcessMacros(clEditor* editor);
    void ProcessUsingNamespace(clEditor* editor);
    void GotoImpl(clEditor* editor);
    void GotoDecl(clEditor* editor);
    bool GetDefinitionsAndSearchPaths(clEditor* editor, wxArrayString& searchPaths, wxArrayString& definitions);
    void UpdateParserPaths();
};

#endif // CODECOMPLETIONMANAGER_H
