#ifndef __clindexercommand__
#define __clindexercommand__

#include <string>
#include <vector>

class CPPCheckerRequest
{
	int         m_cmd;
	std::string m_file;

public:
	CPPCheckerRequest();
	~CPPCheckerRequest();

	void fromBinary(char *data);
	char *toBinary(size_t &buffer_size);

	void setFile(const std::string& file) {
		this->m_file = file;
	}
	const std::string& getFile() const {
		return m_file;
	}
	void setCmd(const int& cmd) {
		this->m_cmd = cmd;
	}
	const int& getCmd() const {
		return m_cmd;
	}
};
#endif
