//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : unittestcppoutputparser.cpp
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

#include "unittestcppoutputparser.h"
#include "wx/log.h"
#include <wx/crt.h>
#include <wx/regex.h>

#include "compiler.h"
#include <wx/arrimpl.cpp> // this is a magic incantation which must be done!
WX_DEFINE_OBJARRAY(ErrorLineInfoArray);

UnitTestCppOutputParser::~UnitTestCppOutputParser() {}

UnitTestCppOutputParser::UnitTestCppOutputParser(const wxArrayString& output)
    : m_output(output)
{
}

void UnitTestCppOutputParser::Parse(TestSummary* summary)
{
    CompilerPtr compilerVC(new Compiler(NULL, Compiler::kRegexVC));
    CompilerPtr compilerGcc(new Compiler(NULL, Compiler::kRegexGNU));

    const Compiler::CmpListInfoPattern& gnuErrors = compilerGcc->GetErrPatterns();
    const Compiler::CmpListInfoPattern& vcErrors = compilerVC->GetErrPatterns();

    // define the regexes
    // group1: total number of tests
    static wxRegEx reSuccess(wxT("Success: ([0-9]*) tests passed"));

    // failure summary line:
    // FAILURE: 1 out of 6 tests failed (1 failures).
    // group1: number of failures
    // group2: total number of tests
    static wxRegEx reErrorSummary(wxT("FAILURE: ([0-9]*) out of ([0-9]*) tests failed"));

    for(size_t i = 0; i < m_output.GetCount(); i++) {
        wxString line = m_output.Item(i);

        if(reSuccess.IsValid()) {
            // if the Summary line is in the format of: Success! ,,,
            // nothing is left to be checked
            if(reSuccess.Matches(m_output.Item(i))) {
                size_t len(0);
                size_t start(0);
                wxString match;

                reSuccess.GetMatch(&start, &len, 1);
                match = m_output.Item(i).Mid(start, len);
                match.ToLong((long*)&summary->totalTests);
                summary->errorCount = 0;
                summary->errorLines.Clear();
                return;
            }
        }

        // Try the GNU pattern first
        bool isAndErrorLine = false;
        Compiler::CmpListInfoPattern::const_iterator iter = gnuErrors.begin();
        for(; iter != gnuErrors.end(); ++iter) {
            wxRegEx re(iter->pattern, wxRE_ADVANCED | wxRE_ICASE);
            long nFileIndex = wxNOT_FOUND;
            long nLineIndex = wxNOT_FOUND;
            iter->fileNameIndex.ToCLong(&nFileIndex);
            iter->lineNumberIndex.ToCLong(&nLineIndex);
            ErrorLineInfo info;
            wxString lineNumber, filename;
            if(re.Matches(line)) {
                // found an error
                info.file = nFileIndex == wxNOT_FOUND ? "" : re.GetMatch(line, nFileIndex);
                info.line = nLineIndex == wxNOT_FOUND ? "" : re.GetMatch(line, nLineIndex);
                info.description = line;
                summary->errorLines.Add(info);
                summary->errorCount++;
                isAndErrorLine = true;
                break;
            }
        }

        if(!isAndErrorLine) {
            iter = vcErrors.begin();
            for(; iter != vcErrors.end(); ++iter) {
                wxRegEx re(iter->pattern, wxRE_ADVANCED | wxRE_ICASE);
                long nFileIndex = wxNOT_FOUND;
                long nLineIndex = wxNOT_FOUND;
                iter->fileNameIndex.ToCLong(&nFileIndex);
                iter->lineNumberIndex.ToCLong(&nLineIndex);
                ErrorLineInfo info;
                wxString lineNumber, filename;
                if(re.Matches(line)) {
                    // found an error
                    info.file = nFileIndex == wxNOT_FOUND ? "" : re.GetMatch(line, nFileIndex);
                    info.line = nLineIndex == wxNOT_FOUND ? "" : re.GetMatch(line, nLineIndex);
                    info.description = line;
                    summary->errorLines.Add(info);
                    summary->errorCount++;
                    isAndErrorLine = true;
                    break;
                }
            }
        }

        // if this line is an error line, continue
        if(isAndErrorLine) {
            continue;
        }

        // test for error summary line
        if(reErrorSummary.IsValid()) {
            if(reErrorSummary.Matches(m_output.Item(i))) {
                // increase the error count
                size_t len(0);
                size_t start(0);
                wxString match;

                reErrorSummary.GetMatch(&start, &len, 1);
                match = m_output.Item(i).Mid(start, len);
                match.ToLong((long*)&summary->errorCount);

                reErrorSummary.GetMatch(&start, &len, 2);
                match = m_output.Item(i).Mid(start, len);
                match.ToLong((long*)&summary->totalTests);
            }
        }
    }
}

//-----------------------------------------------------------------------------------

TestSummary::TestSummary()
    : errorCount(0)
    , totalTests(0)
{
    errorLines.Clear();
}

TestSummary::~TestSummary() { errorLines.Clear(); }

void TestSummary::PrintSelf()
{
    wxPrintf(wxT("Total tests            : %d\n"), totalTests);
    wxPrintf(wxT("Total errors           : %d\n"), errorCount);
    wxPrintf(wxT("Total error lines found: %u\n"), (unsigned int)errorLines.size());
}
