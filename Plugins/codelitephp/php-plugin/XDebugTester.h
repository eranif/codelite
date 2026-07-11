//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : XDebugTester.h
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

#ifndef XDEBUGTESTER_H
#define XDEBUGTESTER_H

#include <map>
#include <wx/string.h>

class XDebugTester
{
public:
    using StringPair_t = std::pair<wxString, wxString>;
    using ResultMap_t = std::map<wxString, XDebugTester::StringPair_t>;

protected:
    XDebugTester::ResultMap_t m_results;

public:
    XDebugTester() = default;
    virtual ~XDebugTester() = default;

    /**
     * @brief run the xdebug test
     */
    bool RunTest();
    const XDebugTester::ResultMap_t& GetResults() const { return m_results; }
};

#endif // XDEBUGTESTER_H
