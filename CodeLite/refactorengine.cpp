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
#include <wx/progdlg.h>
#include <wx/sizer.h>
#include "progress_dialog.h"
#include "refactoring_storage.h"

const wxEventType wxEVT_REFACTORING_ENGINE_CACHE_INITIALIZING = wxNewEventType();

RefactoringEngine::RefactoringEngine() {}

RefactoringEngine::~RefactoringEngine() {}

RefactoringEngine* RefactoringEngine::Instance()
{
    static RefactoringEngine ms_instance;
    return &ms_instance;
}

void RefactoringEngine::Clear()
{
    m_possibleCandidates.clear();
    m_candidates.clear();
}

void RefactoringEngine::RenameGlobalSymbol(const wxString& symname, const wxFileName& fn, int line, int pos,
                                           const wxFileList_t& files)
{
    DoFindReferences(symname, fn, line, pos, files, false);
}

void RefactoringEngine::RenameLocalSymbol(const wxString& symname, const wxFileName& fn, int line, int pos)
{
    // Clear previous results
    Clear();

    // Load the file and get a state map + the text from the scanner
    CppWordScanner scanner(fn.GetFullPath());

    // get the current file states
    TextStatesPtr states = scanner.states();
    if(!states) {
        return;
    }

    // get the local by scanning from the current function's
    TagEntryPtr tag = TagsManagerST::Get()->FunctionFromFileLine(fn, line + 1);
    if(!tag) {
        return;
    }

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
        if(ch == 0) {
            break;
        }

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
                if(depth <= 0) {
                    cont = false;
                }
            }
            break;
        case wxT(' '):
        case wxT('\n'):
        case wxT('\v'):
        case wxT('\t'):
        case wxT('\r'):
            prevGt = false;
            if(depth <= 0) {
                cont = false;
            }
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

        if(cont) {
            expression.Prepend(ch);
        }
    }
    return expression;
}

clProgressDlg* RefactoringEngine::CreateProgressDialog(const wxString& title, int maxValue)
{
    clProgressDlg* prgDlg = NULL;
    // Create a progress dialog
    prgDlg = new clProgressDlg(NULL, title, wxT(""), maxValue);
    return prgDlg;
}

void RefactoringEngine::FindReferences(const wxString& symname, const wxFileName& fn, int line, int pos,
                                       const wxFileList_t& files)
{
    DoFindReferences(symname, fn, line, pos, files, true);
}

void RefactoringEngine::DoFindReferences(const wxString& symname, const wxFileName& fn, int line, int pos,
                                         const wxFileList_t& files, bool onlyDefiniteMatches)
{
    // Clear previous results
    Clear();

    if(!m_storage.IsCacheReady()) {
        m_storage.InitializeCache(files);
        return;
    }

    // Container for the results found in the 'files'
    CppTokensMap tokensMap;

    // Load the file and get a state map + the text from the scanner
    CppWordScanner scanner(fn.GetFullPath());

    // get the current file states
    TextStatesPtr states = scanner.states();
    if(!states) return;

    // Attempt to understand the expression that the caret is currently located at (using line:pos:file)
    RefactorSource rs;
    if(!DoResolveWord(states, fn, pos + symname.Len(), line, symname, &rs)) return;

    wxFileList_t modifiedFilesList = m_storage.FilterUpToDateFiles(files);
    clProgressDlg* prgDlg = NULL;
    if(!modifiedFilesList.empty()) {
        prgDlg = CreateProgressDialog(_("Updating cache..."), files.size());
        // Search the provided input files for the symbol to rename and prepare
        // a CppTokensMap
        for(size_t i = 0; i < modifiedFilesList.size(); i++) {
            wxFileName curfile = modifiedFilesList.at(i);

            wxString msg;
            msg << _("Caching file: ") << curfile.GetFullName();
            // update the progress bar
            if(!prgDlg->Update(i, msg)) {
                prgDlg->Destroy();
                Clear();
                return;
            }

            // Scan only valid C / C++ files
            switch(FileExtManager::GetType(curfile.GetFullName())) {
            case FileExtManager::TypeHeader:
            case FileExtManager::TypeSourceC:
            case FileExtManager::TypeSourceCpp: {
                // load matches for a give symbol
                m_storage.Match(symname, curfile.GetFullPath(), tokensMap);
            } break;
            default:
                break;
            }
        }
        prgDlg->Destroy();
    }
    // load all tokens, first we need to parse the workspace files...
    CppToken::Vec_t tokens = m_storage.GetTokens(symname, files);
    if(tokens.empty()) return;

    // sort the tokens
    std::sort(tokens.begin(), tokens.end());

    RefactorSource target;
    CppToken::Vec_t::iterator iter = tokens.begin();
    int counter(0);

    TextStatesPtr statesPtr(NULL);
    wxString statesPtrFileName;
    prgDlg = CreateProgressDialog(_("Stage 2/2: Parsing matches..."), (int)tokens.size());

    for(; iter != tokens.end(); ++iter) {

        // TODO :: send an event here to report our progress
        wxFileName f(iter->getFilename());
        wxString msg;
        msg << _("Parsing expression ") << counter << wxT("/") << tokens.size() << _(" in file: ") << f.GetFullName();
        if(!prgDlg->Update(counter, msg)) {
            // user clicked 'Cancel'
            Clear();
            prgDlg->Destroy();
            return;
        }

        counter++;
        // reset the result
        target.Reset();

        if(!statesPtr || statesPtrFileName != iter->getFilename()) {
            // Create new statesPtr
            CppWordScanner sc(iter->getFilename());
            statesPtr = sc.states();
            statesPtrFileName = iter->getFilename();
        }

        if(!statesPtr) continue;

        if(DoResolveWord(statesPtr, wxFileName(iter->getFilename()), iter->getOffset(), iter->getLineNumber(), symname,
                         &target)) {

            // set the line number
            if(statesPtr->states.size() > iter->getOffset())
                iter->setLineNumber(statesPtr->states[iter->getOffset()].lineNo);

            if(target.name == rs.name && target.scope == rs.scope) {
                // full match
                m_candidates.push_back(*iter);

            } else if(target.name == rs.scope && !rs.isClass) {
                // source is function, and target is class
                m_candidates.push_back(*iter);

            } else if(target.name == rs.name && rs.isClass) {
                // source is class, and target is ctor
                m_candidates.push_back(*iter);

            } else if(!onlyDefiniteMatches) {
                // add it to the possible match list
                m_possibleCandidates.push_back(*iter);
            }
        } else if(!onlyDefiniteMatches) {
            // resolved word failed, add it to the possible list
            m_possibleCandidates.push_back(*iter);
        }
    }

    prgDlg->Destroy();
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

bool RefactoringEngine::IsCacheInitialized() const { return m_storage.IsCacheReady(); }
