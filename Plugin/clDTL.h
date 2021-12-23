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

/**
 * @class clDTL
 * @brief Diff 2 files and return the result
 * @code

    // An example of using the clDTL class:
    clDTL d;
    d.Diff(filePath1, filePath2);
    const clDTL::LineInfoVec_t &result  = d.GetResult();

    // Create 2 strings "left" and "right"
    wxString leftContent, rightContent;
    for(size_t i=0; i<result.size(); ++i) {
        // format the lines
        switch(result.at(i).m_type) {
        case clDTL::LINE_ADDED:
            leftContent  << "- \n";
            rightContent << "+ " << result.at(i).m_line;
            break;
        case clDTL::LINE_REMOVED:
            leftContent  << "+ " << result.at(i).m_line;
            rightContent << "- \n";
            break;
        case clDTL::LINE_COMMON:
            leftContent  << " " << result.at(i).m_line;
            rightContent << " " << result.at(i).m_line;
            break;
        }
    }
 * @endcode
 */
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

    const LineInfoVec_t& GetResultLeft() const { return m_resultLeft; }
    const LineInfoVec_t& GetResultRight() const { return m_resultRight; }
    const SeqLinePair_t& GetSequences() const { return m_sequences; }
};

#endif // CLDTL_H
