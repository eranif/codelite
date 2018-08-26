//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : clang_driver.h
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

#ifndef CLANGDRIVER_H
#define CLANGDRIVER_H

#if HAS_LIBCLANG

#include "asyncprocess.h"
#include "clang_cleaner_thread.h"
#include "clang_pch_maker_thread.h"
#include "clangpch_cache.h"
#include <clang-c/Index.h>
#include <map>
#include <wx/event.h>
#include "macros.h"

class IEditor;
class ClangDriverCleaner;

struct ClangRequest {
    wxString filename;
    WorkingContext context;
};

class ClangDriver : public wxEvtHandler
{
protected:
    bool m_isBusy;
    ClangWorkerThread m_pchMakerThread;
    WorkingContext m_context;
    CXIndex m_index;
    IEditor* m_activeEditor;
    int m_position;
    ClangCleanerThread m_clangCleanerThread;
    wxStringMap_t m_filesTable;

protected:
    void DoCleanup();
    FileTypeCmpArgs_t DoPrepareCompilationArgs(const wxString& projectName, const wxString& sourceFile,
                                               wxString& projectPath);
    void DoParseCompletionString(CXCompletionString str, int depth, wxString& entryName, wxString& signature,
                                 wxString& completeString, wxString& returnValue);
    void DoGotoDefinition(ClangThreadReply* reply);
    ClangThreadRequest* DoMakeClangThreadRequest(IEditor* editor, WorkingContext context);
    ClangThreadRequest::List_t DoCreateListOfModifiedBuffers(IEditor* excludeEditor);

    // Event handlers
    void DoDeleteTempFile(const wxString& fileName);

public:
    ClangDriver();
    virtual ~ClangDriver();

    void QueueRequest(IEditor* editor, WorkingContext context);
    void ReparseFile(const wxString& filename);
    void CodeCompletion(IEditor* editor);
    void Abort();

    bool IsBusy() const { return m_isBusy; }

    void SetContext(WorkingContext context) { this->m_context = context; }
    WorkingContext GetContext() const { return m_context; }

    void ClearCache();
    bool IsCacheEmpty();

    // Event Handlers
    void OnPrepareTUEnded(wxCommandEvent& e);
    void OnCacheCleared(wxCommandEvent& e);
    void OnTUCreateError(wxCommandEvent& e);
    void OnWorkspaceLoaded(wxCommandEvent& event);
};

#endif // HAS_LIBCLANG

#endif // CLANGDRIVER_H
