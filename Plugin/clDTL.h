//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : clDTL.h
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

#ifndef CLDTL_H
#define CLDTL_H

#include "codelite_exports.h"

#include <vector>
#include <wx/filename.h>
#include <wx/string.h>

enum class PatchAction {
    INVALID,
    ADD_LINE,
    DELETE_LINE,
};

struct WXDLLIMPEXP_SDK PatchStep {
    int line_number = 0;
    PatchAction action = PatchAction::INVALID;
    wxString content;
    PatchStep(int l, PatchAction a, const wxString& s)
        : line_number(l)
        , action(a)
        , content(s)
    {
    }

    wxString to_string() const
    {
        wxString s;
        switch(action) {
        case PatchAction::DELETE_LINE:
            s << "DEL line: " << line_number;
            break;
        case PatchAction::ADD_LINE:
            s << "ADD line: " << line_number << " " << content;
            break;
        default:
            break;
        }
        return s;
    }
};

class WXDLLIMPEXP_SDK clDTL
{
public:
    static const int LINE_PLACEHOLDER = -2;
    static const int LINE_REMOVED = -1;
    static const int LINE_COMMON = 0;
    static const int LINE_ADDED = 1;

    struct WXDLLIMPEXP_SDK LineInfo {
        int m_type;
        wxString m_line;
        LineInfo(const wxString& line, int type)
            : m_type(type)
            , m_line(line)
        {
        }
        LineInfo()
            : m_type(LINE_PLACEHOLDER)
            , m_line("\n")
        {
        }
    };
    typedef std::vector<LineInfo> LineInfoVec_t;
    typedef std::vector<std::pair<int, int>> SeqLinePair_t;

    enum DiffMode { kTwoPanes = 0x01, kOnePane = 0x02 };

private:
    LineInfoVec_t m_resultLeft;
    LineInfoVec_t m_resultRight;
    SeqLinePair_t m_sequences;

public:
    clDTL();
    virtual ~clDTL();

    /**
     * @brief "diff" two files and store the result in the m_result member
     * When 2 files are identical, the result is empty
     */
    void Diff(const wxFileName& fnLeft, const wxFileName& fnRight, DiffMode mode);
    void DiffStrings(const wxString& before, const wxString& after, DiffMode mode);

    const LineInfoVec_t& GetResultLeft() const { return m_resultLeft; }
    const LineInfoVec_t& GetResultRight() const { return m_resultRight; }
    const SeqLinePair_t& GetSequences() const { return m_sequences; }

    /**
     * @brief create step actions to transform `before` -> `after`
     */
    std::vector<PatchStep> CreatePatch(const wxString& before, const wxString& after) const;
};

#endif // CLDTL_H
