#ifndef __clindexercommand__
#define __clindexercommand__

#include <string>
#include <vector>

class clIndexerRequest
{
	std::vector<std::string> m_files;
	std::string m_ctagOptions;
	size_t m_cmd;
	std::string m_databaseFileName;
public:
	enum {
		CLI_PARSE,
		CLI_PARSE_AND_SAVE
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
	void setDatabaseFileName(const std::string& databaseFileName) {
		this->m_databaseFileName = databaseFileName;
	}
	const std::string& getDatabaseFileName() const {
		return m_databaseFileName;
	}
};
#endif // __clindexercommand__
