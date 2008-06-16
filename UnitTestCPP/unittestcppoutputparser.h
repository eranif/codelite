#ifndef __unittestcppoutputparser__
#define __unittestcppoutputparser__

#include <wx/arrstr.h>
#include <vector>

struct ErrorLineInfo {
	wxString line;
	wxString file;
	wxString description;
};

class TestSummary
{
public:
	int errorCount;
	int totalTests;
	std::vector<ErrorLineInfo> errorLines;
	
	TestSummary() : errorCount(0), totalTests(0) {}
};

class UnitTestCppOutputParser
{
	wxArrayString m_output;
public:
	UnitTestCppOutputParser(const wxArrayString &output);
	~UnitTestCppOutputParser();

	void Parse(TestSummary &summary);
};
#endif // __unittestcppoutputparser__
