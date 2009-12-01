#ifndef INCLUDE_FINDER_H
#define INCLUDE_FINDER_H

#include <vector>
#include <string>

class IncludeStatement {
public:
	std::string file;
	int         line;
	std::string includedFrom;
	std::string pattern;
};

extern int IncludeFinder( const char* filePath, std::vector<IncludeStatement> &includes );

#endif
