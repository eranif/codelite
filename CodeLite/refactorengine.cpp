//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : refactorengine.cpp
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

#include "refactorengine.h"
#include "cppwordscanner.h"
#include "entry.h"
#include "ctags_manager.h"
#include "fileextmanager.h"
#include "event_notifier.h"
#include "search_thread.h"
#if wxUSE_GUI
#include <wx/progdlg.h>
#include <wx/sizer.h>
#include "progress_dialog.h"
#endif

wxDEFINE_EVENT(wxEVT_REFACTOR_ENGINE_REFERENCES, clRefactoringEvent);
wxDEFINE_EVENT(wxEVT_REFACTOR_ENGINE_RENAME_SYMBOL, clRefactoringEvent);

RefactoringEngine::RefactoringEngine()
{
    Bind(wxEVT_SEARCH_THREAD_MATCHFOUND, &RefactoringEngine::OnSearchMatch, this);
    Bind(wxEVT_SEARCH_THREAD_SEARCHEND, &RefactoringEngine::OnSearchEnded, this);
    Bind(wxEVT_SEARCH_THREAD_SEARCHCANCELED, &RefactoringEngine::OnSearchEnded, this);
    Bind(wxEVT_SEARCH_THREAD_SEARCHSTARTED, &RefactoringEngine::OnSearchStarted, this);
    m_seartchThread = new SearchThread();
    m_seartchThread->Start();
}

RefactoringEngine::~RefactoringEngine()
{
    wxDELETE(m_seartchThread);
    Unbind(wxEVT_SEARCH_THREAD_MATCHFOUND, &RefactoringEngine::OnSearchMatch, this);
    Unbind(wxEVT_SEARCH_THREAD_SEARCHEND, &RefactoringEngine::OnSearchEnded, this);
    Unbind(wxEVT_SEARCH_THREAD_SEARCHCANCELED, &RefactoringEngine::OnSearchEnded, this);
    Unbind(wxEVT_SEARCH_THREAD_SEARCHSTARTED, &RefactoringEngine::OnSearchStarted, this);
}

static RefactoringEngine* ms_instance = nullptr;
RefactoringEngine* RefactoringEngine::Instance()
{
    if(!ms_instance) { ms_instance = new RefactoringEngine(); }
    return ms_instance;
}

void RefactoringEngine::Shutdown()
{
    if(ms_instance) {
        delete ms_instance;
        ms_instance = nullptr;
    }
}

void RefactoringEngine::Clear() { DoCleanup(); }

void RefactoringEngine::RenameGlobalSymbol(const wxString& symname, const wxFileName& fn, int line, int pos,
                                           const wxFileList_t& files)
{
    DoFindReferences(symname, fn, line, pos, files, false, kRenameSymbol);
}

void RefactoringEngine::RenameLocalSymbol(const wxString& symname, const wxFileName& fn, int line, int pos)
{
    // Clear previous results
    Clear();

    // Load the file and get a state map + the text from the scanner
    CppWordScanner scanner(fn.GetFullPath());

    // get the current file states
    TextStatesPtr states = scanner.states();
    if(!states) { return; }

    // get the local by scanning from the current function's
    TagEntryPtr tag = TagsManagerST::Get()->FunctionFromFileLine(fn, line + 1);
    if(!tag) { return; }

    // Get the line number of the function
    int funcLine = tag->GetLine() - 1;

    // Convert the line number to offset
    int from = states->LineToPos(funcLine);
    int to = states->FunctionEndPos(from);

    if(to == wxNOT_FOUND) return;

    // search for matches in the given range
    CppTokensMap l;
    scanner.Match(symname, l, from, to);

    CppToken::Vec_t tokens;
    l.findTokens(symname, tokens);
    if(tokens.empty()) return;

    // Loop over the matches
    // Incase we did manage to resolve the word, it means that it is NOT a local variable (DoResolveWord only wors for
    // globals NOT for locals)
    RefactorSource target;
    std::vector<CppToken>::iterator iter = tokens.begin();
    for(; iter != tokens.end(); iter++) {
        wxFileName f(iter->getFilename());
        if(!DoResolveWord(states, wxFileName(iter->getFilename()), iter->getOffset(), line, symname, &target)) {
            m_candidates.push_back(*iter);
        }
    }
}

bool RefactoringEngine::DoResolveWord(TextStatesPtr states, const wxFileName& fn, int pos, int line,
                                      const wxString& word, RefactorSource* rs)
{
    std::vector<TagEntryPtr> tags;

    // try to process the current expression
    wxString expr = GetExpression(pos, states);

    // sanity
    if(states->text.length() < (size_t)pos + 1) return false;

    // Hack:
    // disable sqlite3.c
    if(fn.GetFullName() == "sqlite3.c") return false;

    // get the scope
    // Optimize the text for large files
    wxString text(states->text.substr(0, pos + 1));

    // we simply collect declarations & implementations

    // try implemetation first
    bool found(false);
    TagsManagerST::Get()->FindImplDecl(fn, line, expr, word, text, tags, true, true);
    if(tags.empty() == false) {
        // try to see if we got a function and not class/struct

        for(size_t i = 0; i < tags.size(); i++) {
            TagEntryPtr tag = tags.at(i);
            // find first non class/struct tag
            if(tag->GetKind() != wxT("class") && tag->GetKind() != wxT("struct")) {

                // if there is no match, add it anyways
                if(!found) {
                    rs->isClass = (tag->GetKind() == wxT("class") || tag->GetKind() == wxT("struct"));
                    rs->name = tag->GetName();
                    rs->scope = tag->GetScope();
                    found = true;
                } else if(rs->scope == wxT("<global>") && rs->isClass == false) {
                    // give predecense to <global> variables
                    rs->isClass = (tag->GetKind() == wxT("class") || tag->GetKind() == wxT("struct"));
                    rs->name = tag->GetName();
                    rs->scope = tag->GetScope();
                    found = true;
                }
                found = true;
            }
        }

        // if no match was found, keep the first result but keep searching
        if(!found) {

            TagEntryPtr tag = tags.at(0);
            rs->scope = tag->GetScope();
            rs->name = tag->GetName();
            rs->isClass = tag->IsClass() || tag->IsStruct();
            found = true;

        } else {
            return true;
        }
    }

    // Ok, the "implementation" search did not yield definite results, try declaration
    tags.clear();
    TagsManagerST::Get()->FindImplDecl(fn, line, expr, word, text, tags, false, true);
    if(tags.empty() == false) {
        // try to see if we got a function and not class/struct
        for(size_t i = 0; i < tags.size(); i++) {
            TagEntryPtr tag = tags.at(i);
            // find first non class/struct tag
            if(tag->GetKind() != wxT("class") && tag->GetKind() != wxT("struct")) {
                rs->name = tag->GetName();
                rs->scope = tag->GetScope();
                return true;
            }
        }

        // if no match was found, keep the first result but keep searching
        if(!found) {
            TagEntryPtr tag = tags.at(0);
            rs->scope = tag->GetScope();
            rs->name = tag->GetName();
            rs->isClass = tag->IsClass() || tag->IsStruct();
        }
        return true;
    }

    // if we got so far, CC failed to parse the expression
    return false;
}

wxString RefactoringEngine::GetExpression(int pos, TextStatesPtr states)
{
    bool cont(true);
    int depth(0);
    bool prevGt(false);
    wxString expression;

    states->SetPosition(pos);
    while(cont && depth >= 0) {
        wxChar ch = states->Previous();

        // eof?
        if(ch == 0) { break; }

        switch(ch) {
        case wxT(';'):
            cont = false;
            break;
        case wxT('-'):
            if(prevGt) {
                prevGt = false;
                // if previous char was '>', we found an arrow so reduce the depth
                // which was increased
                depth--;
            } else {
                if(depth <= 0) { cont = false; }
            }
            break;
        case wxT(' '):
        case wxT('\n'):
        case wxT('\v'):
        case wxT('\t'):
        case wxT('\r'):
            prevGt = false;
            if(depth <= 0) { cont = false; }
            break;
        case wxT('{'):
        case wxT('='):
            prevGt = false;
            cont = false;
            break;
        case wxT('('):
        case wxT('['):
            depth--;
            prevGt = false;
            if(depth < 0) {
                // dont include this token
                cont = false;
            }
            break;
        case wxT(','):
        case wxT('*'):
        case wxT('&'):
        case wxT('!'):
        case wxT('~'):
        case wxT('+'):
        case wxT('^'):
        case wxT('|'):
        case wxT('%'):
        case wxT('?'):
            prevGt = false;
            if(depth <= 0) {
                // dont include this token
                cont = false;
            }
            break;
        case wxT('>'):
            prevGt = true;
            depth++;
            break;
        case wxT('<'):
            prevGt = false;
            depth--;
            if(depth < 0) {
                // dont include this token
                cont = false;
            }
            break;
        case wxT(')'):
        case wxT(']'):
            prevGt = false;
            depth++;
            break;
        default:
            prevGt = false;
            break;
        }

        if(cont) { expression.Prepend(ch); }
    }
    return expression;
}

#if wxUSE_GUI
clProgressDlg* RefactoringEngine::CreateProgressDialog(const wxString& title, int maxValue)
{
    clProgressDlg* prgDlg = NULL;
    // Create a progress dialog
    prgDlg = new clProgressDlg(EventNotifier::Get()->TopFrame(), title, wxT(""), maxValue);
    return prgDlg;
}
#endif

void RefactoringEngine::FindReferences(const wxString& symname, const wxFileName& fn, int line, int pos,
                                       const wxFileList_t& files)
{
    DoFindReferences(symname, fn, line, pos, files, true, kFindReferences);
}

void RefactoringEngine::DoFindReferences(const wxString& symname, const wxFileName& fn, int line, int pos,
                                         const wxFileList_t& files, bool onlyDefiniteMatches, eActionType type)
{
    if(IsBusy()) { return; }
    // Clear previous results
    Clear();

    // Container for the results found in the 'files'
    CppTokensMap tokensMap;

    // Load the file and get a state map + the text from the scanner
    CppWordScanner scanner(fn.GetFullPath());

    // get the current file states
    TextStatesPtr states = scanner.states();
    if(!states) return;

    // Attempt to understand the expression that the caret is currently located at (using line:pos:file)
    m_refactorSource.Reset();
    if(!DoResolveWord(states, fn, pos + symname.Len(), line, symname, &m_refactorSource)) return;

    // Now that we got here, ask the search thread to search for this symbol in the list of input files
    SearchData sd;
    sd.SetFindString(symname);
    sd.SetEnablePipeSupport(false);
    sd.SetMatchCase(true);
    sd.SetMatchWholeWord(true);
    sd.SetRegularExpression(false);
    wxArrayString filesArray;
    filesArray.reserve(files.size());
    for(const wxFileName& fn : files) {
        if(fn.GetFullPath() == "sqlite3.c") { continue; }
        filesArray.Add(fn.GetFullPath());
    }

    sd.SetFiles(filesArray);
    sd.SetOwner(this); // send back the events here

    // based on the input file, set the file extensions
    wxString extensions;
    FileExtManager::FileType fileType = FileExtManager::GetType(fn.GetFullName(), FileExtManager::TypeText);
    switch(fileType) {
    case FileExtManager::TypeSourceC:
    case FileExtManager::TypeSourceCpp:
    case FileExtManager::TypeHeader:
        extensions = "*.cpp;*.c;*.h;*.hpp;*.cxx;*.cc;";
        break;
    default:
        extensions = "*";
        break;
    }
    sd.SetExtensions(extensions); // All the files in the list should be scanned
    sd.SetEncoding("ISO-8859-1");
    m_seartchThread->PerformSearch(sd);
    m_currentAction = type;
    m_symbolName = symname;
    m_onlyDefiniteMatches = onlyDefiniteMatches;

    // Find references will complete when the 'Find in files' action is completed
}

TagEntryPtr RefactoringEngine::SyncSignature(const wxFileName& fn, int line, int pos, const wxString& word,
                                             const wxString& text, const wxString& expr)
{
    TagEntryPtr func = TagsManagerST::Get()->FunctionFromFileLine(fn, line);
    if(!func) return NULL;

    bool bIsImpl = (func->GetKind() == wxT("function"));

    // Found the counterpart
    std::vector<TagEntryPtr> tags;
    TagsManagerST::Get()->FindImplDecl(fn, line, expr, word, text, tags, !bIsImpl);
    if(tags.size() != 1) return NULL;

    TagEntryPtr tag = tags.at(0);
    if(tag->IsMethod() == false) return NULL;

    wxString signature;
    if(bIsImpl) {
        // The "source" is an implementaion, which means that we need to prepare declaration signature
        // this could be tricky since we might lose the "default" values
        signature = TagsManagerST::Get()->NormalizeFunctionSig(
            func->GetSignature(), Normalize_Func_Default_value | Normalize_Func_Name | Normalize_Func_Reverse_Macro);
    } else {
        // Prepare an "implementation" signature
        signature = TagsManagerST::Get()->NormalizeFunctionSig(func->GetSignature(),
                                                               Normalize_Func_Name | Normalize_Func_Reverse_Macro);
    }

    tag->SetSignature(signature);
    return tag;
}

void RefactoringEngine::OnSearchStarted(wxCommandEvent& event) { wxUnusedVar(event); }

void RefactoringEngine::OnSearchEnded(wxCommandEvent& event) { DoCompleteFindReferences(); }

void RefactoringEngine::OnSearchMatch(wxCommandEvent& event)
{
    SearchResultList* res = (SearchResultList*)event.GetClientData();
    if(!res) { return; }

    for(const SearchResult& result : *res) {
        CppToken tok;
        tok.setFilename(result.GetFileName());
        tok.setLineNumber(result.GetLineNumber());
        tok.setOffset(result.GetPosition());
        tok.setName(result.GetFindWhat());
        m_tokens.push_back(std::move(tok));
    }
    wxDELETE(res);
}

void RefactoringEngine::DoCompleteFindReferences()
{
    ScopeCleaner cleaner; // ensure that DoCleanup is called when leave this scope

    // Load all tokens, first we need to parse the workspace files...
    CppToken::Vec_t tokens = std::move(m_tokens);

    RefactorSource target;
    int counter(0);

    TextStatesPtr statesPtr(NULL);
    wxString statesPtrFileName;
#if wxUSE_GUI
    clProgressDlg* prgDlg = CreateProgressDialog(_("Parsing matches..."), (int)tokens.size());
#endif
    for(CppToken& token : tokens) {
        wxFileName f(token.getFilename());
#if wxUSE_GUI
        wxString msg;
        msg << _("Parsing expression ") << counter << wxT("/") << tokens.size() << _(" in file: ") << f.GetFullName();
        if(!prgDlg->Update(counter, msg)) {
            // user clicked 'Cancel'
            Clear();
            prgDlg->Destroy();
            return;
        }
#endif
        counter++;
        // reset the result
        target.Reset();

        if(!statesPtr || statesPtrFileName != token.getFilename()) {
            // Create new statesPtr
            CppWordScanner sc(token.getFilename());
            statesPtr = sc.states();
            statesPtrFileName = token.getFilename();
        }

        if(!statesPtr) continue;

        if(DoResolveWord(statesPtr, wxFileName(token.getFilename()), token.getOffset(), token.getLineNumber(),
                         m_symbolName, &target)) {

            // set the line number
            if(statesPtr->states.size() > token.getOffset())
                token.setLineNumber(statesPtr->states[token.getOffset()].lineNo);

            if(target.name == m_refactorSource.name && target.scope == m_refactorSource.scope) {
                // full match
                m_candidates.push_back(token);

            } else if(target.name == m_refactorSource.scope && !m_refactorSource.isClass) {
                // source is function, and target is class
                m_candidates.push_back(token);

            } else if(target.name == m_refactorSource.name && m_refactorSource.isClass) {
                // source is class, and target is ctor
                m_candidates.push_back(token);

            } else if(!m_onlyDefiniteMatches) {
                // add it to the possible match list
                m_possibleCandidates.push_back(token);
            }
        } else if(!m_onlyDefiniteMatches) {
            // resolved word failed, add it to the possible list
            m_possibleCandidates.push_back(token);
        }
    }
#if wxUSE_GUI
    prgDlg->Destroy();
#endif

    clRefactoringEvent event(m_currentAction == kRenameSymbol ? wxEVT_REFACTOR_ENGINE_RENAME_SYMBOL
                                                              : wxEVT_REFACTOR_ENGINE_REFERENCES);
    event.SetPossibleMatches(m_possibleCandidates);
    event.SetMatches(m_candidates);
    event.SetString(m_symbolName);
    EventNotifier::Get()->AddPendingEvent(event);
}

void RefactoringEngine::DoCleanup()
{
    m_possibleCandidates.clear();
    m_candidates.clear();
    m_currentAction = kNone;
    m_refactorSource.Reset();
    m_onlyDefiniteMatches = false;
    m_symbolName.Clear();
    m_tokens.clear();
}
