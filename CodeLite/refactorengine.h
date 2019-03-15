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
#include "cl_command_event.h"

class SearchThread;
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

// --------------------------------------------------------------
// Refactoring event
// --------------------------------------------------------------
class WXDLLIMPEXP_CL clRefactoringEvent : public clCommandEvent
{
    CppToken::Vec_t m_matches;
    CppToken::Vec_t m_possibleMatches;

public:
    clRefactoringEvent(wxEventType commandType = wxEVT_NULL, int winid = 0)
        : clCommandEvent(commandType, winid)
    {
    }
    clRefactoringEvent(const clRefactoringEvent& event) { *this = event; }
    clRefactoringEvent& operator=(const clRefactoringEvent& src)
    {
        clCommandEvent::operator=(src);
        m_matches = src.m_matches;
        m_possibleMatches = src.m_possibleMatches;
        return *this;
    }

    virtual ~clRefactoringEvent() {}
    void SetMatches(const CppToken::Vec_t& matches) { this->m_matches = matches; }
    void SetPossibleMatches(const CppToken::Vec_t& possibleMatches) { this->m_possibleMatches = possibleMatches; }
    const CppToken::Vec_t& GetMatches() const { return m_matches; }
    const CppToken::Vec_t& GetPossibleMatches() const { return m_possibleMatches; }
    virtual wxEvent* Clone() const { return new clRefactoringEvent(*this); }
};

typedef void (wxEvtHandler::*clRefactoringEventFunction)(clRefactoringEvent&);
#define clRefactoringEventHandler(func) wxEVENT_HANDLER_CAST(clRefactoringEventFunction, func)

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_REFACTOR_ENGINE_REFERENCES, clRefactoringEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_REFACTOR_ENGINE_RENAME_SYMBOL, clRefactoringEvent);

class WXDLLIMPEXP_CL RefactoringEngine : public wxEvtHandler
{
    CppToken::Vec_t m_candidates;
    CppToken::Vec_t m_possibleCandidates;
    wxEvtHandler* m_evtHandler;

    RefactorSource m_refactorSource;
    enum eActionType {
        kNone,
        kFindReferences,
        kRenameSymbol,
    };
    eActionType m_currentAction = kNone;
    CppToken::Vec_t m_tokens;
    wxString m_symbolName;
    bool m_onlyDefiniteMatches = false;
    friend class ScopeCleaner;
    SearchThread* m_seartchThread = nullptr;
    
    class ScopeCleaner
    {
    public:
        ScopeCleaner() {}
        ~ScopeCleaner() { RefactoringEngine::Instance()->DoCleanup(); }
    };

public:
    static RefactoringEngine* Instance();
    static void Shutdown();
    
protected:
#if wxUSE_GUI
    clProgressDlg* CreateProgressDialog(const wxString& title, int maxValue);
#endif
    void DoFindReferences(const wxString& symname, const wxFileName& fn, int line, int pos, const wxFileList_t& files,
                          bool onlyDefiniteMatches, eActionType type);
    void DoCompleteFindReferences();
    void DoCleanup();

private:
    RefactoringEngine();
    ~RefactoringEngine();
    bool DoResolveWord(TextStatesPtr states, const wxFileName& fn, int pos, int line, const wxString& word,
                       RefactorSource* rs);

    void OnSearchStarted(wxCommandEvent& event);
    void OnSearchEnded(wxCommandEvent& event);
    void OnSearchMatch(wxCommandEvent& event);

public:
    bool IsBusy() const { return m_currentAction != kNone; }
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
