#include <stdio.h>
#include <vector>

#include "crawler_include.h"

//#include <windows.h>

std::string extract_path(const std::string &filePath);

int main(int argc, char **argv)
{
//	fcFileOpener::Instance()->AddSearchPath("C:/MinGW-3.4.5/include");
//	fcFileOpener::Instance()->AddSearchPath("C:/wxWidgets-2.8.10/include");
//	fcFileOpener::Instance()->AddSearchPath("C:\\MinGW-4.4.0\\include");
//	fcFileOpener::Instance()->AddSearchPath("C:\\MinGW-4.4.0\\lib\\gcc\\mingw32\\4.4.0\\include\\c++");
    fcFileOpener::Instance()->AddSearchPath("D:\\src\\wx294\\include");
//	fcFileOpener::Instance()->AddExcludePath("C:\\MinGW-4.4.0\\lib\\gcc\\mingw32\\4.4.0\\include\\c++\\debug");
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

		std::set<std::string>::const_iterator iter = fcFileOpener::Instance()->GetResults().begin();

		printf("---->\n");
		for (; iter != fcFileOpener::Instance()->GetResults().end(); iter ++) {
			printf("%s\n", (*iter).c_str());
		}
        
        //printf("----> Includes \n");
        //const fcFileOpener::List_t& includeStatements = fcFileOpener::Instance()->GetIncludeStatements();
        //fcFileOpener::List_t::const_iterator it = includeStatements.begin();
        //for(; it != includeStatements.end(); ++it) {
        //    printf("%s\n", (*it).c_str());
        //}
	}

	std::set<std::string> ns = fcFileOpener::Instance()->GetNamespaces();
	std::set<std::string>::iterator iter = ns.begin();
	for(; iter != ns.end(); iter++) {
		printf("Using namespace: %s\n", iter->c_str());
	}

	std::set<std::string> nsa = fcFileOpener::Instance()->GetNamespaceAliases();
	iter = nsa.begin();
	for(; iter != nsa.end(); iter++) {
		printf("Namespace alias: %s\n", iter->c_str());
	}
	return 0;
}
