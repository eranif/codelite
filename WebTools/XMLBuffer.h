//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : XMLBuffer.h
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

#ifndef XMLBUFFER_H
#define XMLBUFFER_H

#include "XML/XMLLexerAPI.h"
#include "macros.h"

#include <vector>
#include <wx/string.h>

class XMLBuffer
{
public:
    struct Scope {
        wxString tag;
        int line;
        bool isEmptyTag;
        
        Scope()
            : line(wxNOT_FOUND)
            , isEmptyTag(false)
        {
        }
        bool IsOk() const { return line != wxNOT_FOUND && !tag.IsEmpty(); }
    };

    enum eState {
        kNonXmlSection = -1,
        kNormal,
        kCdata,
        kComment,
    };

protected:
    wxString m_buffer;
    XMLScanner_t m_scanner;
    std::vector<XMLBuffer::Scope> m_elements;
    eState m_state;
    bool m_htmlMode;

    /// HTML empty tags
    static wxStringSet_t m_emptyTags;

protected:
    void OnOpenTag();
    void OnCloseTag();
    void OnTagClosePrefix();
    bool ConsumeUntil(int until);

public:
    XMLBuffer(const wxString& buffer, bool htmlMode = false);
    virtual ~XMLBuffer();
    void Parse();

    XMLBuffer::Scope GetCurrentScope() const;
    bool InComment() const { return m_state == kComment; }
    bool InCData() const { return m_state == kCdata; }
    /**
     * @brief return if 'tag' is an empty html tag
     */
    static bool IsEmptyHtmlTag(const wxString& tag);
};

#endif // XMLBUFFER_H
