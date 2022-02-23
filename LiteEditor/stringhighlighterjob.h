//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : stringhighlighterjob.h
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
#ifndef __stringhighlighterjob__
#define __stringhighlighterjob__

#include <vector>

struct StringHighlightOutput {
    wxString filename;
    std::vector<std::pair<int, int>> matches;

    StringHighlightOutput()
        : filename(wxT(""))
    {
    }

    ~StringHighlightOutput() {}
};

class StringHighlighterJob
{
    wxString m_str;
    wxString m_word;
    int m_offset;
    StringHighlightOutput m_output;

public:
    StringHighlighterJob() {}
    StringHighlighterJob(const wxString& str, const wxString& word, int offset);
    void Set(const wxString& str, const wxString& word, int offset);
    virtual ~StringHighlighterJob();

    void SetOffset(int offset) { this->m_offset = offset; }
    void SetStr(const wxString& str) { this->m_str = str; }
    void SetWord(const wxString& word) { this->m_word = word; }
    int GetOffset() const { return m_offset; }
    const StringHighlightOutput& GetOutput() const { return m_output; }
    const wxString& GetStr() const { return m_str; }
    const wxString& GetWord() const { return m_word; }

public:
    virtual void Process();
};
#endif // __stringhighlighterjob__
