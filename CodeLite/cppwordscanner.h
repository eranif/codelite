//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : cppwordscanner.h
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
#ifndef __cppwordscanner__
#define __cppwordscanner__

#include <wx/arrstr.h>
#include <vector>
#include "cpptoken.h"
#include "smart_ptr.h"
#include "codelite_exports.h"
#include <set>
#include "macros.h"
#include "wxStringHash.h"

struct ByteState {
    short state;   // Holds the current byte state (one of CppWordScanner::STATE_*)
    short depth;   // The current char depth
    short depthId; // The depth ID (there can be multiple blocks of the same depth in a given scope)
    int lineNo;    // the line number which holds this byte
};

class WXDLLIMPEXP_CL TextStates
{
public:
    wxString text;
    std::vector<ByteState> states;
    std::vector<int> lineToPos;
    int pos;

public:
    TextStates()
        : pos(wxNOT_FOUND)
    {
    }

    virtual ~TextStates() {}

    void SetPosition(int pos);
    wxChar Previous();
    wxChar Next();

    /**
     * @brief return true if the current TextState is valid. The test is simple:
     * if the vector size and the text size are equal
     */
    bool IsOk() const { return states.size() == text.length(); }

    void SetState(size_t where, int state, int depth, int lineNo);

    /**
     * @brief return the end of a given function
     * @param position function start position. This function searches for the first opening brace from position '{' and
     * returns the position of the matching
     * closing brace '}'
     */
    int FunctionEndPos(int position);

    /**
     * @brief convert line number to position
     */
    int LineToPos(int lineNo);
};

typedef SmartPtr<TextStates> TextStatesPtr;

class WXDLLIMPEXP_CL CppWordScanner
{
    wxStringSet_t m_keywords;
    wxString m_filename;
    wxString m_text;
    int m_offset;

public:
    enum {
        STATE_NORMAL = 0,
        STATE_C_COMMENT,
        STATE_CPP_COMMENT,
        STATE_DQ_STRING,
        STATE_SINGLE_STRING,
        STATE_PRE_PROCESSING
    };

protected:
    void doFind(const wxString& filter, CppTokensMap& l, int from, int to);
    void doInit();

public:
    CppWordScanner() {}
    CppWordScanner(const wxString& file_name);
    CppWordScanner(const wxString& file_name, const wxString& text, int offset);

    /**
     * @brief tokenize the file and return list of tokens
     * @return
     */
    CppToken::Vec_t tokenize();

    ~CppWordScanner();

    void FindAll(CppTokensMap& l);
    void Match(const wxString& word, CppTokensMap& l);
    /**
     * @brief same as Match(const wxString &word, CppTokensMap &l) however, search for matches only in a given range
     */
    void Match(const wxString& word, CppTokensMap& l, int from, int to);
    // we use std::vector<char> and NOT std::vector<char> since the specialization of vector<bool>
    // is broken
    TextStatesPtr states();
};

#endif // __cppwordscanner__
