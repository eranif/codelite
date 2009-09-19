#ifndef __clindexerreply__
#define __clindexerreply__
#include <string>

class CPPCheckerReply
{
	size_t      m_completionCode;
	std::string m_fileName;
	std::string m_report;
public:

	// completion code possible values:
	enum {
		CheckingInProgress = 0    , // m_report contains the report (part of it)
		CheckingCompleted         , //
		StatusMessage
	};

public:
	CPPCheckerReply();
	~CPPCheckerReply();

	void fromBinary(char *data);
	char *toBinary(size_t &buffer_size);

	void setCompletionCode(const size_t& completionCode) {
		this->m_completionCode = completionCode;
	}
	void setFileName(const std::string& fileName) {
		this->m_fileName = fileName;
	}
	const size_t& getCompletionCode() const {
		return m_completionCode;
	}
	const std::string& getFileName() const {
		return m_fileName;
	}
	void setReport(const std::string& report) {
		this->m_report = report;
	}
	const std::string& getReport() const {
		return m_report;
	}
};
#endif // __clindexerreply__
