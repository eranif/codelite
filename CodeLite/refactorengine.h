//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2009 by Eran Ifrah
// file name            : refactorengine.h
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

#ifndef REFACTORENGINE_H
#define REFACTORENGINE_H

#include <wx/event.h>
#include <wx/filename.h>
#include <vector>
#include <list>
#include "entry.h"
#include "cppwordscanner.h"
#include "cpptoken.h"
#include "codelite_exports.h"
#include "refactoring_storage.h"

class clProgressDlg;

//----------------------------------------------------------------------------------

struct RefactorSource {
    wxString name;
    wxString scope;
    bool isClass;

    RefactorSource()
        : name(wxEmptyString)
        , scope(wxEmptyString)
        , isClass(false)
    {
    }

    void Reset()
    {
        name.clear();
        scope.clear();
        isClass = false;
    }
};

//-----------------------------------------------------------------------------------
extern WXDLLIMPEXP_CL const wxEventType wxEVT_REFACTORING_ENGINE_CACHE_INITIALIZING;

class WXDLLIMPEXP_CL RefactoringEngine
{
    CppToken::Vec_t m_candidates;
    CppToken::Vec_t m_possibleCandidates;
    wxEvtHandler* m_evtHandler;
    RefactoringStorage m_storage;

public:
    static RefactoringEngine* Instance();

protected:
    clProgressDlg* CreateProgressDialog(const wxString& title, int maxValue);
    void DoFindReferences(const wxString& symname, const wxFileName& fn, int line, int pos, const wxFileList_t& files,
                          bool onlyDefiniteMatches);

private:
    RefactoringEngine();
    ~RefactoringEngine();
    bool DoResolveWord(TextStatesPtr states, const wxFileName& fn, int pos, int line, const wxString& word,
                       RefactorSource* rs);

public:
    void InitializeCache(const wxFileList_t& files) { m_storage.InitializeCache(files); }
    bool IsCacheInitialized() const;
    void SetCandidates(const CppToken::Vec_t& candidates) { this->m_candidates = candidates; }
    void SetPossibleCandidates(const CppToken::Vec_t& possibleCandidates)
    {
        this->m_possibleCandidates = possibleCandidates;
    }
    const CppToken::Vec_t& GetCandidates() const { return m_candidates; }
    const CppToken::Vec_t& GetPossibleCandidates() const { return m_possibleCandidates; }
    wxString GetExpression(int pos, TextStatesPtr states);

    void Clear();
    /**
     * @brief rename global symbol. Global Symbol can be one of:
     * <li>member
     * <li>global variable
     * <li>class
     * <li>struct
     * <li>union
     * <li>namespace
     * <li>function
     * <li>prototype
     * <li>typedef
     * @param symname the name of the symbol
     * @param fn current file
     * @param line the symbol is loacted in this file
     * @param pos at that position
     * @param files perform the refactoring on these files
     */
    void RenameGlobalSymbol(const wxString& symname, const wxFileName& fn, int line, int pos,
                            const wxFileList_t& files);

    /**
     * @param rename local variable
     * @param symname symbol we want to rename
     * @param fn the current file name (full path)
     * @param line the line where our symbol appears
     * @param pos the position of the symbol (this should be pointing to the *start* of the symbol)
     */
    void RenameLocalSymbol(const wxString& symname, const wxFileName& fn, int line, int pos);

    /**
     * @brief find usages of given symbol in a list of files
     * @param symname symbol to search for
     * @param fn active file open which holds the context of the symbol
     * @param line the line where the symbol exists
     * @param pos the position of the symbol (this should be pointing to the *start* of the symbol)
     * @param files list of files to search in
     */
    void FindReferences(const wxString& symname, const wxFileName& fn, int line, int pos, const wxFileList_t& files);

    /**
     * @brief given a location (file:line:pos) use the current location function signature
     * and return the propsed counter-part tag
     */
    TagEntryPtr SyncSignature(const wxFileName& fn, int line, int pos, const wxString& word, const wxString& text,
                              const wxString& expr);
};

#endif // REFACTORENGINE_H
