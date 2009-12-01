#include <stdio.h>
#include <vector>

#include "include_finder.h"

int main(int argc, char **argv)
{
	std::vector<IncludeStatement> includes;
	for(int i=0; i<10; i++)
		IncludeFinder("../test.h", includes);

	for(size_t i=0; i<includes.size(); i++) {
		IncludeStatement is = includes.at(i);
		printf("Include       :%s\n", is.file.c_str());
		printf("Included From :%s\n", is.includedFrom.c_str());
		printf("Line          :%d\n", is.line);
		printf("Pattern       :%s\n", is.pattern.c_str());
	}
	return 0;
}
