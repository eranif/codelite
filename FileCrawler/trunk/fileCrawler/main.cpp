#include <stdio.h>
#include <vector>

#include "crawler_include.h"

//#include <windows.h>

std::string extract_path(const std::string &filePath);

int main(int argc, char **argv)
{
/*//	fcFileOpener::Instance()->AddSearchPath("C:/MinGW-3.4.5/include");
//	fcFileOpener::Instance()->AddSearchPath("C:/wxWidgets-2.8.10/include");
	fcFileOpener::Instance()->AddSearchPath("C:\\MinGW-4.4.0\\include");
	fcFileOpener::Instance()->AddSearchPath("C:\\MinGW-4.4.0\\lib\\gcc\\mingw32\\4.4.0\\include\\c++");
	fcFileOpener::Instance()->AddExcludePath("C:\\MinGW-4.4.0\\lib\\gcc\\mingw32\\4.4.0\\include\\c++\\debug");
//	fcFileOpener::Instance()->AddExcludePath("C:/wxWidgets-2.8.10/include/wx/unix");
//	fcFileOpener::Instance()->AddExcludePath("C:/wxWidgets-2.8.10/include/wx/univ");

//	fcFileOpener::Instance()->AddSearchPath("/usr/include/");
//	fcFileOpener::Instance()->AddSearchPath("/usr/include/c++/4.2.4/");
//	fcFileOpener::Instance()->AddSearchPath("/usr/include/wx-2.8/");

	fcFileOpener::Instance()->ClearResults();
	for (size_t i=0; i<1; i++) {
	//	double start = GetTickCount();
		if(crawlerScan("../test.h") < 0) {
			printf("failed to scan files\n");
		}
		printf("Found %d files \n", fcFileOpener::Instance()->GetResults().size());

	//	double end   = GetTickCount();
	//	printf("Found %d files in %f seconds\n", fcFileOpener::Instance()->GetResults().size(), (end-start)/1000);
//		std::set<std::string>::const_iterator iter = fcFileOpener::Instance()->GetResults().begin();
//
//		printf("---->\n");
//		for (; iter != fcFileOpener::Instance()->GetResults().end(); iter ++) {
//			printf("%s\n", (*iter).c_str());
//		}
	}*/

	std::vector<IncludeStatement> includes;
	crawlerFindIncludes("../test.h", includes);
	std::vector<IncludeStatement>::iterator iter = includes.begin();
	for( ; iter != includes.end(); iter++ ) {
		printf("%s at %d\n", (*iter).file.c_str(), (*iter).line);
	}
	return 0;
}
