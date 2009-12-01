#ifndef CRAWLER_INCLUDE_H
#define CRAWLER_INCLUDE_H

#include <vector>
#include "fc_fileopener.h"

class IncludeStatement {
public:
	std::string file;
	int         line;
	std::string includedFrom;
	std::string pattern;
};

extern int crawlerScan        ( const char* filePath );
extern int crawlerFindIncludes( const char* filePath, std::vector<IncludeStatement> &includes );

#endif
