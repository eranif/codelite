#ifndef __stringaccessor__
#define __stringaccessor__

#include "string"

class StringAccessor {
	std::string m_str;
	
public:
	StringAccessor(const std::string &str);
	virtual ~StringAccessor();
	
	size_t length() { return m_str.size(); }
	char safeAt(size_t pos);
	bool isWordChar(char ch);
	
	void setStr(const std::string& str) {this->m_str = str;}
	const std::string& getStr() const {return m_str;}
	bool match(const char *str, size_t from);
};
#endif // __stringaccessor__
