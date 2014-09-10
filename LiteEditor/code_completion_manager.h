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
#include "CompileCommandsCreateor.h"
#include "CxxPreProcessorThread.h"

class CodeCompletionManager : public wxEvtHandler
{
protected:
    size_t m_options;
    bool m_wordCompletionRefreshNeeded;
    bool m_buildInProgress;
    CxxPreProcessorThread m_preProcessorThread;
protected:
    /// ctags implementions
    bool DoCtagsWordCompletion(LEditor* editor, const wxString& expr, const wxString& word);
    bool DoCtagsCalltip(LEditor* editor, int line, const wxString& expr, const wxString& text, const wxString& word);
    bool DoCtagsCodeComplete(LEditor* editor, int line, const wxString& expr, const wxString& text);
    bool DoCtagsGotoImpl(LEditor* editor);
    bool DoCtagsGotoDecl(LEditor* editor);

    /// clang implementations
    void DoClangWordCompletion(LEditor* editor);
    void DoClangCalltip(LEditor* editor);
    void DoClangCodeComplete(LEditor* editor);
    void DoClangGotoImpl(LEditor* editor);
    void DoClangGotoDecl(LEditor* editor);

    void DoUpdateOptions();
    void DoUpdateCompilationDatabase();

protected:
    // Event handlers
    void OnBuildEnded(clBuildEvent& e);
    void OnBuildStarted(clBuildEvent& e);
    void OnAppActivated(wxActivateEvent& e);
    void OnCompileCommandsFileGenerated(clCommandEvent& event);
    void OnFileSaved(clCommandEvent &event);
    void OnFileLoaded(clCommandEvent &event);
    void OnWorkspaceConfig(wxCommandEvent &event);
    
public:
    CodeCompletionManager();
    virtual ~CodeCompletionManager();
    
    /**
     * @brief force a refresh based on the current settings
     */
    void RefreshPreProcessorColouring();
    
    // Will be called by the parser thread when done
    void OnParseThreadCollectedMacros(const wxArrayString& definitions, const wxString &filename);
    
    void SetWordCompletionRefreshNeeded(bool wordCompletionRefreshNeeded)
    {
        this->m_wordCompletionRefreshNeeded = wordCompletionRefreshNeeded;
    }
    bool GetWordCompletionRefreshNeeded() const
    {
        return m_wordCompletionRefreshNeeded;
    }
    void SetOptions(size_t options)
    {
        this->m_options = options;
    }
    size_t GetOptions() const
    {
        return m_options;
    }

    static CodeCompletionManager& Get();
    static void Release();

    void WordCompletion(LEditor* editor, const wxString& expr, const wxString& word);
    void Calltip(LEditor* editor, int line, const wxString& expr, const wxString& text, const wxString& word);
    void CodeComplete(LEditor* editor, int line, const wxString& expr, const wxString& text);
    void ProcessMacros(LEditor* editor);
    void GotoImpl(LEditor* editor);
    void GotoDecl(LEditor* editor);
};

#endif // CODECOMPLETIONMANAGER_H
