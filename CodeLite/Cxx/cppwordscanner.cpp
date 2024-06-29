//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : cppwordscanner.cpp
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
#include <wx/ffile.h>
#include <wx/strconv.h>
#include <wx/tokenzr.h>
#include <wx/log.h>
#include "cppwordscanner.h"
#include "stringaccessor.h"
#include "dirsaver.h"
#include "ctags_manager.h"

CppWordScanner::CppWordScanner(const wxString& fileName)
    : m_filename(fileName)
    , m_offset(0)
{
    // disable log
    wxLogNull nolog;
    wxCSConv fontEncConv(wxFONTENCODING_ISO8859_1);

    wxFFile thefile(m_filename, wxT("rb"));
    if(thefile.IsOpened()) {
        m_text.Clear();
        thefile.ReadAll(&m_text, fontEncConv);
        if(m_text.IsEmpty()) {
            // Try another converter
            fontEncConv = wxFONTENCODING_UTF8;
            thefile.ReadAll(&m_text, fontEncConv);
        }
    }
    doInit();
}

CppWordScanner::CppWordScanner(const wxString& fileName, const wxString& text, int offset)
    : m_filename(fileName)
    , m_text(text.c_str())
    , m_offset(offset)
{
    doInit();
}

CppWordScanner::~CppWordScanner() {}

void CppWordScanner::FindAll(CppTokensMap& tokensMap) { doFind("", tokensMap, wxNOT_FOUND, wxNOT_FOUND); }

void CppWordScanner::Match(const wxString& word, CppTokensMap& l)
{
    // scan the entire text
    doFind(word, l, wxNOT_FOUND, wxNOT_FOUND);
}

void CppWordScanner::Match(const wxString& word, CppTokensMap& l, int from, int to)
{
    // scan the entire text
    doFind(word, l, from, to);
}

void CppWordScanner::doFind(const wxString& filter, CppTokensMap& l, int from, int to)
{
    int state(STATE_NORMAL);
    StringAccessor accessor(m_text);
    CppToken token;
    int lineNo(0);

    // set the scan range
    size_t f = (from == wxNOT_FOUND) ? 0 : from;
    size_t t = (to == wxNOT_FOUND) ? m_text.size() : to;

    // sanity
    if(f > m_text.size() || t > m_text.size()) return;

    for(size_t i = f; i < t; i++) {
        char ch = accessor.safeAt(i);

        // Keep track of line numbers
        if(accessor.match("\n", i) && (state == STATE_NORMAL || state == STATE_PRE_PROCESSING ||
                                       state == STATE_CPP_COMMENT || state == STATE_C_COMMENT)) {
            lineNo++;
        }

        switch(state) {

        case STATE_NORMAL:
            if(accessor.match("#", i)) {

                if(i == 0 ||                      // satrt of document
                   accessor.match("\n", i - 1)) { // we are at start of line
                    state = STATE_PRE_PROCESSING;
                }
            } else if(accessor.match("//", i)) {

                // C++ comment, advance i
                state = STATE_CPP_COMMENT;
                i++;

            } else if(accessor.match("/*", i)) {

                // C comment
                state = STATE_C_COMMENT;
                i++;

            } else if(accessor.match("'", i)) {

                // single quoted string
                state = STATE_SINGLE_STRING;

            } else if(accessor.match("\"", i)) {

                // dbouble quoted string
                state = STATE_DQ_STRING;

            } else if(accessor.isWordChar(ch)) {

                // is valid C++ word?
                token.append(ch);
                if(token.getOffset() == wxString::npos) {
                    token.setOffset(i + m_offset);
                }
            } else {

                if(token.getName().empty() == false) {

                    if((int)token.getName().at(0) >= 48 && (int)token.getName().at(0) <= 57) {
                        token.reset();
                    } else {
                        // dont add C++ key words
                        wxString tmpName(token.getName());

                        if(m_keywords.count(tmpName) == 0) {

                            token.setFilename(m_filename);
                            token.setLineNumber(lineNo);

                            // Ok, we are not a number!
                            // filter out non matching words
                            if(filter.empty() || filter == token.getName()) {
                                l.addToken(token);
                            }
                        }
                        token.reset();
                    }
                }
            }

            break;
        case STATE_PRE_PROCESSING:
            // if the char before the \n is \ (backslash) or \r\ (CR followed by backslash) remain in pre-processing
            // state
            if(accessor.match("\n", i) && (!accessor.match("\\", i - 1) && !accessor.match("\\\r", i - 2))) {
                // no wrap
                state = STATE_NORMAL;

            } else if(accessor.match("//", i)) {
                // C++ comment, advance i
                state = STATE_CPP_COMMENT;
                i++;
            }
            break;
        case STATE_C_COMMENT:
            if(accessor.match("*/", i)) {
                state = STATE_NORMAL;
                i++;
            }
            break;
        case STATE_CPP_COMMENT:
            if(accessor.match("\n", i)) {
                state = STATE_NORMAL;
            }
            break;
        case STATE_DQ_STRING:
            if(accessor.match("\\\"", i)) {
                // escaped string
                i++;
            } else if(accessor.match("\\", i)) {
                i++;
            } else if(accessor.match("\"", i)) {
                state = STATE_NORMAL;
            }
            break;
        case STATE_SINGLE_STRING:
            if(accessor.match("\\'", i)) {
                // escaped single string
                i++;

            } else if(accessor.match("\\", i)) {
                i++;

            } else if(accessor.match("'", i)) {
                state = STATE_NORMAL;
            }
            break;
        }
    }
}

CppToken::Vec_t CppWordScanner::tokenize()
{
    int state(STATE_NORMAL);
    StringAccessor accessor(m_text);
    CppToken token;
    int lineNo(0);
    CppToken::Vec_t allTokens;

    // set the scan range
    size_t f = 0;
    size_t t = m_text.size();

    // sanity
    if(f > m_text.size() || t > m_text.size()) return CppToken::Vec_t();

    for(size_t i = f; i < t; i++) {
        char ch = accessor.safeAt(i);

        // Keep track of line numbers
        if(accessor.match("\n", i) && (state == STATE_NORMAL || state == STATE_PRE_PROCESSING ||
                                       state == STATE_CPP_COMMENT || state == STATE_C_COMMENT)) {
            lineNo++;
        }

        switch(state) {

        case STATE_NORMAL:
            if(accessor.match("#", i)) {

                if(i == 0 ||                      // satrt of document
                   accessor.match("\n", i - 1)) { // we are at start of line
                    state = STATE_PRE_PROCESSING;
                }
            } else if(accessor.match("//", i)) {

                // C++ comment, advance i
                state = STATE_CPP_COMMENT;
                i++;

            } else if(accessor.match("/*", i)) {

                // C comment
                state = STATE_C_COMMENT;
                i++;

            } else if(accessor.match("'", i)) {

                // single quoted string
                state = STATE_SINGLE_STRING;

            } else if(accessor.match("\"", i)) {

                // dbouble quoted string
                state = STATE_DQ_STRING;

            } else if(accessor.isWordChar(ch)) {

                // is valid C++ word?
                token.append(ch);
                if(token.getOffset() == wxString::npos) {
                    token.setOffset(i + m_offset);
                }
            } else {

                if(token.getName().empty() == false) {

                    if((int)token.getName().at(0) >= 48 && (int)token.getName().at(0) <= 57) {
                        token.reset();
                    } else {
                        // dont add C++ key words
                        wxString tmpName(token.getName());

                        if(m_keywords.count(tmpName) == 0) {

                            token.setFilename(m_filename);
                            token.setLineNumber(lineNo);
                            allTokens.push_back(token);
                        }
                        token.reset();
                    }
                }
            }

            break;
        case STATE_PRE_PROCESSING:
            // if the char before the \n is \ (backslash) or \r\ (CR followed by backslash) remain in pre-processing
            // state
            if(accessor.match("\n", i) && (!accessor.match("\\", i - 1) && !accessor.match("\\\r", i - 2))) {
                // no wrap
                state = STATE_NORMAL;

            } else if(accessor.match("//", i)) {
                // C++ comment, advance i
                state = STATE_CPP_COMMENT;
                i++;
            }
            break;
        case STATE_C_COMMENT:
            if(accessor.match("*/", i)) {
                state = STATE_NORMAL;
                i++;
            }
            break;
        case STATE_CPP_COMMENT:
            if(accessor.match("\n", i)) {
                state = STATE_NORMAL;
            }
            break;
        case STATE_DQ_STRING:
            if(accessor.match("\\\"", i)) {
                // escaped string
                i++;
            } else if(accessor.match("\\", i)) {
                i++;
            } else if(accessor.match("\"", i)) {
                state = STATE_NORMAL;
            }
            break;
        case STATE_SINGLE_STRING:
            if(accessor.match("\\'", i)) {
                // escaped single string
                i++;

            } else if(accessor.match("\\", i)) {
                i++;

            } else if(accessor.match("'", i)) {
                state = STATE_NORMAL;
            }
            break;
        }
    }
    return allTokens;
}

void CppWordScanner::doInit() { TagsManager::GetCXXKeywords(m_keywords); }

TextStatesPtr CppWordScanner::states()
{
    TextStatesPtr bitmap(new TextStates());
    bitmap->states.resize(m_text.size());

    if(bitmap->states.size() == 0) {
        return NULL;
    }

    bitmap->text = m_text;

    int state(STATE_NORMAL);
    int depth(0);
    int lineNo(0);

    StringAccessor accessor(m_text);
    for(size_t i = 0; i < m_text.size(); i++) {

        // Keep track of line numbers
        if(accessor.match("\n", i) && (state == STATE_NORMAL || state == STATE_PRE_PROCESSING ||
                                       state == STATE_CPP_COMMENT || state == STATE_C_COMMENT)) {
            lineNo++;
        }

        switch(state) {

        case STATE_NORMAL:
            if(accessor.match("#", i)) {

                if(i == 0 ||                      // satrt of document
                   accessor.match("\n", i - 1)) { // we are at start of line
                    state = STATE_PRE_PROCESSING;
                }

            } else if(accessor.match("//", i)) {

                // C++ comment, advance i
                state = STATE_CPP_COMMENT;
                bitmap->SetState(i, STATE_CPP_COMMENT, depth, lineNo);
                i++;

            } else if(accessor.match("/*", i)) {

                // C comment
                state = STATE_C_COMMENT;
                bitmap->SetState(i, STATE_C_COMMENT, depth, lineNo);
                i++;

            } else if(accessor.match("'", i)) {

                // single quoted string
                state = STATE_SINGLE_STRING;

            } else if(accessor.match("\"", i)) {

                // dbouble quoted string
                state = STATE_DQ_STRING;

            } else if(accessor.match("{", i)) {
                depth++;

            } else if(accessor.match("}", i)) {
                depth--;
            }

            break;
        case STATE_PRE_PROCESSING:
            // if the char before the \n is \ (backslash) or \r\ (CR followed by backslash) remain in pre-processing
            // state
            if(accessor.match("\n", i) && (!accessor.match("\\", i - 1) && !accessor.match("\\\r", i - 2))) {
                // no wrap
                state = STATE_NORMAL;

            } else if(accessor.match("//", i)) {
                // C++ comment, advance i
                state = STATE_CPP_COMMENT;
                bitmap->SetState(i, STATE_CPP_COMMENT, depth, lineNo);
                i++;
            }
            break;
        case STATE_C_COMMENT:
            if(accessor.match("*/", i)) {
                bitmap->SetState(i, state, depth, lineNo);
                state = STATE_NORMAL;
                i++;
            }
            break;
        case STATE_CPP_COMMENT:
            if(accessor.match("\n", i)) {
                state = STATE_NORMAL;
            }
            break;
        case STATE_DQ_STRING:
            if(accessor.match("\\\"", i)) {
                // escaped string
                bitmap->SetState(i, STATE_DQ_STRING, depth, lineNo);
                i++;

            } else if(accessor.match("\\", i)) {
                bitmap->SetState(i, STATE_DQ_STRING, depth, lineNo);
                i++;

            } else if(accessor.match("\"", i)) {
                state = STATE_NORMAL;
            }
            break;
        case STATE_SINGLE_STRING:
            if(accessor.match("\\'", i)) {
                // escaped single string
                bitmap->SetState(i, STATE_SINGLE_STRING, depth, lineNo);
                i++;
            } else if(accessor.match("\\", i)) {
                bitmap->SetState(i, STATE_SINGLE_STRING, depth, lineNo);
                i++;

            } else if(accessor.match("'", i)) {
                state = STATE_NORMAL;
            }
            break;
        }
        bitmap->SetState(i, state, depth, lineNo);
    }
    return bitmap;
}

int TextStates::FunctionEndPos(int position)
{
    // Sanity
    if(text.length() != states.size()) return wxNOT_FOUND;

    if(position < 0) return wxNOT_FOUND;

    if(position >= (int)text.length()) return wxNOT_FOUND;

    if(states[position].depth < 0) {
        // we are already at depth 0 (which means global scope)
        return wxNOT_FOUND;
    }

    // Note that each call to 'Next' / 'Prev' updates the 'pos' member
    int curdepth = states[position].depth;

    // Step 1: search from this point downward until we find an opening brace (i.e. depth is equal to curdepth+1)
    SetPosition(position);

    wxChar ch = Next();
    while(ch) {
        if(states[pos].depth == curdepth + 1) break;
        ch = Next();
    }

    // Step 2: Continue from the current position,
    // but this time break when depth is curdepth

    ch = Next();
    while(ch) {
        if(states[pos].depth == curdepth) break;
        ch = Next();
    }

    if(pos > position) {
        return pos;
    }

    return wxNOT_FOUND;
}

wxChar TextStates::Next()
{
    if(text.length() != states.size()) return 0;

    if(pos == wxNOT_FOUND) return 0;

    // reached end of text
    pos++;
    while(pos < (int)text.length()) {
        int st = states[pos].state;
        if(st == CppWordScanner::STATE_NORMAL) {
            if(text.length() > (size_t)pos) return text.at(pos);
            ;
            return 0;
        }
        pos++;
    }
    return 0;
}

wxChar TextStates::Previous()
{
    if(text.length() != states.size()) return 0;

    if(pos == wxNOT_FOUND) return 0;

    // reached start of text
    if(pos == 0) return 0;

    pos--;
    while(pos) {
        int st = states[pos].state;
        if(st == CppWordScanner::STATE_NORMAL) {
            if(text.length() > (size_t)pos) return text.at(pos);
            return 0;
        }
        pos--;
    }
    return 0;
}

void TextStates::SetPosition(int pos) { this->pos = pos; }

void TextStates::SetState(size_t where, int state, int depth, int lineNo)
{
    if(where < states.size()) {
        states[where].depth = depth;
        states[where].state = state;
        states[where].lineNo = lineNo;
    }

    if(lineToPos.empty() || (int)lineToPos.size() - 1 < lineNo) {
        lineToPos.push_back(where);
    }
}

int TextStates::LineToPos(int lineNo)
{
    if(IsOk() == false) return wxNOT_FOUND;

    if(lineToPos.empty() || (int)lineToPos.size() < lineNo || lineNo < 0) return wxNOT_FOUND;

    return lineToPos.at(lineNo);
}
