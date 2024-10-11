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

void CppWordScanner::doInit() { TagsManager::GetCXXKeywords(m_keywords); }
