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

CppWordScanner::CppWordScanner(const wxString &file_name)
	: m_filename(file_name)
	, m_offset(0)
{
	// disable log
	wxLogNull nolog;

	wxFFile thefile(file_name, wxT("rb"));
	if(thefile.IsOpened()) {
		wxFileOffset size = thefile.Length();
		wxString fileData;
		fileData.Alloc(size);

		wxCSConv fontEncConv(wxFONTENCODING_ISO8859_1);
		thefile.ReadAll( &m_text, fontEncConv );
	}
	doInit();
}

CppWordScanner::CppWordScanner(const wxString& file_name, const wxString& text, int offset)
	: m_filename(file_name)
	, m_text    (text.c_str())
	, m_offset  (offset)
{
	doInit();
}

CppWordScanner::~CppWordScanner()
{
}

void CppWordScanner::FindAll(CppTokensMap &l)
{
	doFind(wxEmptyString, l, wxNOT_FOUND, wxNOT_FOUND);
}

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

	// set the scan range
	size_t f = (from == wxNOT_FOUND) ? 0             : from;
	size_t t = (to   == wxNOT_FOUND) ? m_text.size() : to;

	// sanity
	if(f >= m_text.size() || t >= m_text.size())
		return;

	for (size_t i=f; i<t; i++) {
		char ch = accessor.safeAt(i);

		switch (state) {

		case STATE_NORMAL:
			if (accessor.match("#", i)) {

				if (	i == 0 || 						// satrt of document
				        accessor.match("\n", i-1)) { 	// we are at start of line
					state = STATE_PRE_PROCESSING;
				}
			} else if (accessor.match("//", i)) {

				// C++ comment, advance i
				state = STATE_CPP_COMMENT;
				i++;

			} else if (accessor.match("/*", i)) {

				// C comment
				state = STATE_C_COMMENT;
				i++;

			} else if (accessor.match("'", i)) {

				// single quoted string
				state = STATE_SINGLE_STRING;

			} else if (accessor.match("\"", i)) {

				// dbouble quoted string
				state = STATE_DQ_STRING;

			} else if ( accessor.isWordChar(ch) ) {

				// is valid C++ word?
				token.append( ch );
				if (token.getOffset() == wxString::npos) {
					token.setOffset( i + m_offset );
				}
			} else {

				if (token.getName().empty() == false) {

					if ((int)token.getName().GetChar(0) >= 48 && (int)token.getName().GetChar(0) <= 57) {
						token.reset();
					} else {
						//dont add C++ key words
						if (m_arr.Index(token.getName()) == wxNOT_FOUND) {

							// Ok, we are not a number!
							// filter out non matching words
							if (filter.empty() || filter == token.getName()) {
								token.setFilename(m_filename);
								l.addToken(token);
							}
						}
						token.reset();
					}
				}
			}

			break;
		case STATE_PRE_PROCESSING:
			//skip pre processor lines
			if ( accessor.match("\n", i) && !accessor.match("\\", i-1) ) {
				// no wrap
				state = STATE_NORMAL;
			}
			break;
		case STATE_C_COMMENT:
			if ( accessor.match("*/", i)) {
				state = STATE_NORMAL;
				i++;
			}
			break;
		case STATE_CPP_COMMENT:
			if ( accessor.match("\n", i)) {
				state = STATE_NORMAL;
			}
			break;
		case STATE_DQ_STRING:
			if (accessor.match("\\\"", i)) {
				//escaped string
				i++;
			} else if(accessor.match("\\", i)) {
				i++;
			} else if (accessor.match("\"", i)) {
				state = STATE_NORMAL;
			}
			break;
		case STATE_SINGLE_STRING:
			if (accessor.match("\\'", i)) {
				//escaped single string
				i++;

			} else if(accessor.match("\\", i)) {
				i++;

			} else if (accessor.match("'", i)) {
				state = STATE_NORMAL;
			}
			break;
		}
	}
}

void CppWordScanner::doInit()
{
	wxString key_words =
	    wxT("auto break case char const continue default define defined do double elif else endif enum error extern float"
	        "for  goto if ifdef ifndef include int line long pragma register return short signed sizeof static struct switch"
	        "typedef undef union unsigned void volatile while class namespace delete friend inline new operator overload"
	        "protected private public this virtual template typename dynamic_cast static_cast const_cast reinterpret_cast"
	        "using throw catch size_t");

	//add this items into map
	m_arr = wxStringTokenize(key_words, wxT(" "));
	m_arr.Sort();
}

TextStates CppWordScanner::states()
{
	TextStates bitmap;
	bitmap.states.resize(m_text.size());

	if(bitmap.states.size() == 0)
		return bitmap;

	bitmap.text = m_text;

	int state(STATE_NORMAL);
	int depth(0);
	StringAccessor accessor(m_text);

	for (size_t i=0; i<m_text.size(); i++) {
		switch (state) {

		case STATE_NORMAL:
			if (accessor.match("#", i)) {

				if ( i == 0 ||                    // satrt of document
				     accessor.match("\n", i-1)) { // we are at start of line
					state = STATE_PRE_PROCESSING;
				}
			} else if (accessor.match("//", i)) {

				// C++ comment, advance i
				state = STATE_CPP_COMMENT;
				bitmap.states[i].state = STATE_CPP_COMMENT;
				bitmap.states[i].depth = depth;
				i++;

			} else if (accessor.match("/*", i)) {

				// C comment
				state = STATE_C_COMMENT;
				bitmap.states[i].state = STATE_C_COMMENT;
				bitmap.states[i].depth = depth;
				i++;

			} else if (accessor.match("'", i)) {

				// single quoted string
				state = STATE_SINGLE_STRING;

			} else if (accessor.match("\"", i)) {

				// dbouble quoted string
				state = STATE_DQ_STRING;

			} else if (accessor.match("{", i)) {
				depth++;

			} else if (accessor.match("}", i)) {
				depth--;
			}

			break;
		case STATE_PRE_PROCESSING:
			//skip pre processor lines
			if ( accessor.match("\n", i) && !accessor.match("\\", i-1) ) {
				// no wrap
				state = STATE_NORMAL;
			}
			break;
		case STATE_C_COMMENT:
			if ( accessor.match("*/", i)) {
				bitmap.states[i].state = state;
				bitmap.states[i].depth = depth;
				state = STATE_NORMAL;
				i++;
			}
			break;
		case STATE_CPP_COMMENT:
			if ( accessor.match("\n", i)) {
				state = STATE_NORMAL;
			}
			break;
		case STATE_DQ_STRING:
			if (accessor.match("\\\"", i)) {
				//escaped string
				bitmap.states[i].state = STATE_DQ_STRING;
				bitmap.states[i].depth = depth;
				i++;
			} else if(accessor.match("\\", i)) {
				bitmap.states[i].state = STATE_DQ_STRING;
				bitmap.states[i].depth = depth;
				i++;
			} else if (accessor.match("\"", i)) {
				state = STATE_NORMAL;
			}
			break;
		case STATE_SINGLE_STRING:
			if (accessor.match("\\'", i)) {
				//escaped single string
				bitmap.states[i].state = STATE_SINGLE_STRING;
				bitmap.states[i].depth = depth;
				i++;
			} else if(accessor.match("\\", i)) {
				bitmap.states[i].state = STATE_SINGLE_STRING;
				bitmap.states[i].depth = depth;
				i++;

			} else if (accessor.match("'", i)) {
				state = STATE_NORMAL;
			}
			break;
		}
		bitmap.states[i].state = state;
		bitmap.states[i].depth = depth;
	}
	return bitmap;
}

wxString TextStates::CurrentScope(int position, int& upperPt, int& lowerPt)
{
	// Sanity
	if(text.Len() != states.size())
		return wxT("");

	if(position < 0)
		return wxT("");

	if(position >= (int)text.Len())
		return wxT("");

	if(states[position].depth <= 0) {
		// we are already at depth 0 (which means global scope)
		return wxT("");
	}

	// Fact:
	// The current depth is greater than 0

	// Note that each call to 'Next' / 'Prev' updates the 'pos' member

	// Step 1: search from this point upward until we find depth 0
	SetPosition(position);

	upperPt = position;
	lowerPt = position;

	wxChar ch = Previous();
	while ( ch ) {
		if(states[pos].depth == 0)
			break;
		ch = Previous();
	}

	upperPt = pos;

	// Step 2: search from position downward
	SetPosition(position);
	ch = Next();
	while ( ch ) {
		if(states[pos].depth == 0)
			break;
		ch = Next();
	}
	lowerPt = pos;

	if(lowerPt > upperPt) {
		return text.Mid(upperPt, lowerPt - upperPt);
	}

	return wxT("");
}

wxChar TextStates::Next()
{
	if(text.Len() != states.size())
		return 0;

	if(pos == wxNOT_FOUND)
		return 0;

	// reached end of text
	pos++;
	while( pos < (int)text.Len() ) {
		int st = states[pos].state;
		if(st == CppWordScanner::STATE_NORMAL) {
			return text[pos];
		}
		pos++;
	}
	return 0;
}

wxChar TextStates::Previous()
{
	if(text.Len() != states.size())
		return 0;

	if(pos == wxNOT_FOUND)
		return 0;

	// reached start of text
	if(pos == 0)
		return 0;

	pos--;
	while( pos ) {
		int st = states[pos].state;
		if(st == CppWordScanner::STATE_NORMAL) {
			return text[pos];
		}
		pos--;
	}
	return 0;
}

void TextStates::SetPosition(int pos)
{
	this->pos = pos;
}

