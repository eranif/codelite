#ifndef __clindexerreply__
#define __clindexerreply__
#include <string>
class clIndexerReply
{
	size_t m_completionCode;
	std::string m_fileName;
	std::string m_tags;

public:
	clIndexerReply();
	~clIndexerReply();

	void fromBinary(char *data);
	char *toBinary(size_t &buffer_size);

	void setCompletionCode(const size_t& completionCode) {
		this->m_completionCode = completionCode;
	}
	void setFileName(const std::string& fileName) {
		this->m_fileName = fileName;
	}
	void setTags(const std::string& tags) {
		this->m_tags = tags;
	}
	const size_t& getCompletionCode() const {
		return m_completionCode;
	}
	const std::string& getFileName() const {
		return m_fileName;
	}
	const std::string& getTags() const {
		return m_tags;
	}

};
#endif // __clindexerreply__
