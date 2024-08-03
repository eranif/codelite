//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : comment.h
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
#ifndef CODELITE_COMMENT_H
#define CODELITE_COMMENT_H

#include <memory>
#include <wx/string.h>

class Comment
{
	wxString m_comment;
	wxString m_file;
	int      m_line;

public:
	/**
	 * construct a Comment object
	 * \param comment comment string
	 * \param file comment file name
	 * \param line comment line number
	 */
	Comment(const wxString &comment, const wxString & file, const int line);

	/**
	 * Copy constructor
	 */
	Comment(const Comment& rhs);

	/**
	 * Destructor
	 */
	virtual ~Comment() {};

	/// assignment operator
	Comment& operator=(const Comment& rhs);

	/// accessors
	/**
	 * Get the file name
	 * \return file name
	 */
	const wxString & GetFile() const {
		return m_file;
	}

	/**
	 * Get string comment
	 * \return comment
	 */
	const wxString & GetComment() const {
		return m_comment;
	}

	/**
	 * Get the line number
	 * \return line number
	 */
	const int & GetLine() const {
		return m_line;
	}
};

using CommentPtr = std::unique_ptr<Comment>;

#endif // CODELITE_COMMENT_H
