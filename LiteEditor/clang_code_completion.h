//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : clang_code_completion.h
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

#ifndef CLANGCODECOMPLETION_H
#define CLANGCODECOMPLETION_H

#if HAS_LIBCLANG

#include <wx/event.h>
#include "asyncprocess.h"
#include <map>
#include "entry.h"
#include "clang_driver.h"
#include "clang_output_parser_api.h"
#include <set>
#include "cl_command_event.h"

class IEditor;
class IManager;

/**
 * @class ClangCodeCompletion
 * @author eran
 * @date 07/18/10
 * @file clang_code_completion.h
 * @brief codelite's interface to clang's code completion
 */
class ClangCodeCompletion : public wxEvtHandler
{
public:
    typedef std::set<wxString> Set_t;

protected:
    static ClangCodeCompletion* ms_instance;
    ClangDriver                 m_clang;
    bool                        m_allEditorsAreClosing;

    friend class ClangDriver;
public:
    static ClangCodeCompletion* Instance();
    static void Release();

    /**
     * @brief perform codecompletion in the editor
     */
    void CodeComplete(IEditor *editor);
    /**
     * @brief perform word-completin in the editor
     */
    void WordComplete(IEditor *editor);
    /**
     * @brief provide list of macros and pass them to the editor (this is useful for disabling block of text which is not visible due to #if #endif conditions)
     */
    void ListMacros(IEditor *editor);
    /**
     * @brief go to the definition under the caret
     */
    void GotoDeclaration(IEditor *editor);
    /**
     * @brief go to the definition under the caret
     */
    void GotoImplementation(IEditor *editor);

    /**
     * @brief display calltip for a function
     */
    void Calltip(IEditor *editor);
    void CancelCodeComplete();
    void ClearCache();
    bool IsCacheEmpty();

protected:
    void DoCleanUp();

    // Event handling
    void OnFileLoaded(wxCommandEvent &e);
    void OnFileSaved(clCommandEvent &e);
    void OnAllEditorsClosing(wxCommandEvent &e);
    void OnAllEditorsClosed(wxCommandEvent &e);

    void OnBuildStarting(clBuildEvent &e);
    void OnBuildEnded(clBuildEvent &e);
    void OnWorkspaceClosed(wxCommandEvent &e);

private:
    ClangCodeCompletion();
    ~ClangCodeCompletion();

};

#endif // HAS_LIBCLANG
#endif // CLANGCODECOMPLETION_H
