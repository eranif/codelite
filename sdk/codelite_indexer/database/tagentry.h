#ifndef __tagentry__
#define __tagentry__

#include <string>

class TagEntry {

	std::string m_line;

protected:
	void initialize();
public:
	TagEntry(const char *line);
	~TagEntry();

};
#endif // __tagentry__
