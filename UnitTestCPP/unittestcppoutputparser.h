//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : unittestcppoutputparser.h
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

#ifndef __unittestcppoutputparser__
#define __unittestcppoutputparser__

#include <vector>
#include <wx/arrstr.h>
#include <wx/dynarray.h>

struct ErrorLineInfo {
    wxString line;
    wxString file;
    wxString description;
};

WX_DECLARE_OBJARRAY(ErrorLineInfo, ErrorLineInfoArray);

class TestSummary
{
public:
    int errorCount;
    int totalTests;
    ErrorLineInfoArray errorLines;

    TestSummary();
    virtual ~TestSummary();

    void PrintSelf();
};

class UnitTestCppOutputParser
{
    wxArrayString m_output;

public:
    UnitTestCppOutputParser(const wxArrayString& output);
    ~UnitTestCppOutputParser();

    void Parse(TestSummary* summary);
};
#endif // __unittestcppoutputparser__
