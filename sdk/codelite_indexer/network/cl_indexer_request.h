#ifndef __clindexercommand__
#define __clindexercommand__

#include <string>
#include <vector>

class clIndexerRequest
{
	std::vector<std::string> m_files;
	std::string m_ctagOptions;
	size_t m_cmd;

public:
	enum {
		CLI_PARSE
	};

public:
	clIndexerRequest();
	~clIndexerRequest();

	void fromBinary(char *data);
	char *toBinary(size_t &buffer_size);

	void setCmd(const size_t& cmd) {
		this->m_cmd = cmd;
	}
	void setFiles(const std::vector<std::string>& files) {
		this->m_files = files;
	}
	const size_t& getCmd() const {
		return m_cmd;
	}
	const std::vector<std::string>& getFiles() const {
		return m_files;
	}
	void setCtagOptions(const std::string& ctagOptions) {
		this->m_ctagOptions = ctagOptions;
	}
	const std::string& getCtagOptions() const {
		return m_ctagOptions;
	}
};
#endif // __clindexercommand__
