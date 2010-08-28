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

struct ByteState {
	short state;  // Holds the current byte state (one of CppWordScanner::STATE_*)
	short depth;  // The current char depth
};

class TextStates
{
public:
	wxString               text;
	std::vector<ByteState> states;
	int                    pos;

public:
	TextStates() : pos(wxNOT_FOUND) {}

	void   SetPosition(int pos);
	wxChar Previous();
	wxChar Next();

	/**
	 * @brief return true if the current TextState is valid. The test is simple:
	 * if the vector size and the text size are equal
	 */
	bool   IsOk() const {
		return states.size() == text.Len();
	}

	/**
	 * @brief return the current scope based on depth
	 * this function searches for two points:
	 * from pos upward until we find depth 0 and from pos downward until we find depth 0 (or EOF). It returns the text between those
	 * two points
	 */
	wxString CurrentScope(int position, int& upperPt, int& lowerPt);
};

class CppWordScanner
{
	wxSortedArrayString m_arr;
	wxString            m_filename;
	wxString            m_text;
	int                 m_offset;

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
	void doFind(const wxString &filter, CppTokensMap &l, int from, int to);
	void doInit();

public:
	CppWordScanner(const wxString &file_name);
	CppWordScanner(const wxString &file_name, const wxString &text, int offset);
	~CppWordScanner();

	void FindAll(CppTokensMap &l);
	void Match(const wxString &word, CppTokensMap &l);
	/**
	 * @brief same as Match(const wxString &word, CppTokensMap &l) however, search for matches only in a given range
	 */
	void Match(const wxString &word, CppTokensMap &l, int from, int to);
	// we use std::vector<char> and NOT std::vector<char> since the specialization of vector<bool>
	// is broken
	TextStates states();
};


#endif // __cppwordscanner__
