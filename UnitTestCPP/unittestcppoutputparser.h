#ifndef __unittestcppoutputparser__
#define __unittestcppoutputparser__
#include <wx/arrstr.h>

class TestSummary
{
public:
	int errorCount;
	int totalTests;
	wxArrayString errorLines;
	
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
