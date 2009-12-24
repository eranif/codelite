#ifndef CPPCHECKTESTRESULTS_H
#define CPPCHECKTESTRESULTS_H

#include <wx/clntdata.h>
#include <map>
#include <vector>

//------------------------------------------------------------

class CppCheckResult : public wxClientData
{
public:
	wxString id;
	wxString filename;
	long     lineno;
	wxString severity;
	wxString msg;

public:
	CppCheckResult() {}

	CppCheckResult(const CppCheckResult& rhs) {
		*this = rhs;
	}

	virtual ~CppCheckResult();

	CppCheckResult& operator=(const CppCheckResult& rhs) {
		id       = rhs.id;
		filename = rhs.filename.c_str();
		lineno   = rhs.lineno;
		severity = rhs.severity.c_str();
		msg      = rhs.msg.c_str();
		return *this;
	}
};

//------------------------------------------------------------

class CppCheckTestResults
{
	std::map<wxString, std::vector<CppCheckResult>* > m_results;
	size_t                                           m_errorCount;
public:
	CppCheckTestResults();
	virtual ~CppCheckTestResults();

	/**
	 * @brief parse XML for a given file
	 * @param xmlOutput
	 */
	void AddResultsForFile(const wxString &xmlOutput);

	/**
	 * @brief return array with results belong to a given file
	 * @param filename
	 * @return
	 */
	std::vector<CppCheckResult>* GetResultsForFile(const wxString &filename);

	/**
	 * @brief clear the results
	 */
	void ClearAll();

	/**
	 * @brief return number of errors found
	 * @return
	 */
	size_t GetErrorCount() const {
		return m_errorCount;
	}

	/**
	 * @brief get array of files which has error found in them
	 * @return
	 */
	wxArrayString GetFiles();
};

#endif // CPPCHECKTESTRESULTS_H
