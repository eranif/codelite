
#include "unittestcppoutputparser.h"
#include <wx/regex.h>

UnitTestCppOutputParser::~UnitTestCppOutputParser()
{
}

UnitTestCppOutputParser::UnitTestCppOutputParser(const wxArrayString& output)
		: m_output(output)
{

}

void UnitTestCppOutputParser::Parse(TestSummary &summary)
{
// define the regexes
// group1: total number of tests
	static wxRegEx reSuccess(wxT("Success: ([0-9]*) tests passed"));

// main.cpp(82): error: Failure in Test4: false
// group1: file name
// group2: line number
	static wxRegEx reError(wxT("(^[a-zA-Z:]{0,2}[a-zA-Z\\.0-9_/\\+\\-]+ *)\\([0-9]*)\\): error:"));

// failure summary line:
// FAILURE: 1 out of 6 tests failed (1 failures).
// group1: number of failures
// group2: total number of tests
	static wxRegEx reErrorSummary(wxT("FAILURE: ([0-9]*) out of ([0-9]*) tests failed"));

	TestSummary ts;
	for (size_t i=0; i<m_output.GetCount(); i++) {
		if (reSuccess.IsValid()) {
			// if the Summary line is in the format of: Success! ,,,
			// nothing is left to be checked
			if (reSuccess.Matches(m_output.Item(i))) {
				size_t len(0);
				size_t start(0);
				wxString match;
				
				reSuccess.GetMatch(&start, &len, 0);
				match = m_output.Item(i).Mid(start, len);
				match.ToLong((long*)&ts.totalTests);
				ts.errorCount = 0;
				ts.errorLines.Clear();
				return;
			}
		}
		
		// test for error line
		if (reError.IsValid()) {
			if(reError.Matches(m_output.Item(i))) {
				
			}
		}
	}
}
