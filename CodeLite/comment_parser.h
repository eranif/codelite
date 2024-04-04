//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2009 by Eran Ifrah
// file name            : comment_parser.h
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

#ifndef COMMENT_PARSER_H
#define COMMENT_PARSER_H

#include "codelite_exports.h"

#include <map>
#include <string>

class WXDLLIMPEXP_CL CommentParseResult
{
private:
    std::map<size_t, std::string> m_comments;
    std::string m_filename;

public:
    void addComment(const std::string& comment, size_t line, bool cppComment)
    {
        // try to group this comment with the one above it
        std::string cmt = comment;
        if(cppComment && line) {
            size_t prevLine = line - 1;
            std::map<size_t, std::string>::iterator iter = m_comments.find(prevLine);
            if(iter != m_comments.end()) {
                cmt = iter->second;
                cmt += "\n";
                cmt += comment;

                // remove the previous comment from the map
                m_comments.erase(iter);
            }
        }
        m_comments[line] = cmt;
    }

    std::string getCommentForLine(size_t line) const
    {
        std::map<size_t, std::string>::const_iterator iter = m_comments.find(line);
        if(iter == m_comments.end()) return "";
        return iter->second;
    }

    void print()
    {
        std::map<size_t, std::string>::const_iterator iter = m_comments.begin();
        for(; iter != m_comments.end(); iter++) {
            printf("Line   : %u\n", (unsigned int)iter->first);
            printf("Comment:\n%s\n", iter->second.c_str());
        }
    }

    void setFilename(const std::string& filename) { m_filename = filename; }

    const std::string& getFilename() const { return m_filename; }

    void clear()
    {
        m_comments.clear();
        m_filename.clear();
    }
};

extern WXDLLIMPEXP_CL int ParseComments(const char* filePath, CommentParseResult& comments);

#endif // COMMENT_PARSER_H
