#ifndef __cpptoken__
#define __cpptoken__

#include <string>
#include <list>

class CppToken
{
	std::string name;	//the name of the token
	size_t offset;		//file offset

public:
	CppToken();
	~CppToken();
	
	void reset();
	
	void append(const char ch);
	
	void setName(const std::string& name) {
		this->name = name;
	}
	void setOffset(const size_t& offset) {
		this->offset = offset;
	}

	const std::string& getName() const {
		return name;
	}
	const size_t& getOffset() const {
		return offset;
	}
	
	void print();
};
typedef std::list<CppToken> CppTokenList;
#endif // __cpptoken__
