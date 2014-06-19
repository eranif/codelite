#include <stdio.h>
#include <vector>

#include "crawler_include.h"
#include <wx/wxcrtvararg.h>

//#include <windows.h>

wxString extract_path(const wxString &filePath);

int main(int argc, char **argv)
{
//	fcFileOpener::Get()->AddSearchPath("C:/MinGW-3.4.5/include");
//	fcFileOpener::Get()->AddSearchPath("C:/wxWidgets-2.8.10/include");
//	fcFileOpener::Get()->AddSearchPath("C:\\MinGW-4.4.0\\include");
//	fcFileOpener::Get()->AddSearchPath("C:\\MinGW-4.4.0\\lib\\gcc\\mingw32\\4.4.0\\include\\c++");
    fcFileOpener::Get()->AddSearchPath("D:/wxWidgets-3.0.1/include");
    fcFileOpener::Get()->AddExcludePath("D:/wxWidgets-3.0.1/include/wx/android");
    fcFileOpener::Get()->AddExcludePath("D:/wxWidgets-3.0.1/include/wx/univ");
    fcFileOpener::Get()->AddExcludePath("D:/wxWidgets-3.0.1/include/wx/unix");
//	fcFileOpener::Get()->AddExcludePath("C:\\MinGW-4.4.0\\lib\\gcc\\mingw32\\4.4.0\\include\\c++\\debug");
//	fcFileOpener::Get()->AddExcludePath("C:/wxWidgets-2.8.10/include/wx/unix");
//	fcFileOpener::Get()->AddExcludePath("C:/wxWidgets-2.8.10/include/wx/univ");

//	fcFileOpener::Get()->AddSearchPath("/usr/include/");
//	fcFileOpener::Get()->AddSearchPath("/usr/include/c++/4.2.4/");
//	fcFileOpener::Get()->AddSearchPath("/usr/include/wx-2.8/");

    fcFileOpener::Get()->ClearResults();
    for (size_t i=0; i<1; i++) {
        //	double start = GetTickCount();
        if(crawlerScan("../test.h") < 0) {
            printf("failed to scan files\n");
        }
        printf("Found %d files \n", fcFileOpener::Get()->GetResults().size());

        std::set<wxString>::const_iterator iter = fcFileOpener::Get()->GetResults().begin();

        printf("---->\n");
        for (; iter != fcFileOpener::Get()->GetResults().end(); iter ++) {
            wxPrintf("%s\n", (*iter));
        }

        //printf("----> Includes \n");
        //const fcFileOpener::List_t& includeStatements = fcFileOpener::Get()->GetIncludeStatements();
        //fcFileOpener::List_t::const_iterator it = includeStatements.begin();
        //for(; it != includeStatements.end(); ++it) {
        //    printf("%s\n", (*it).c_str());
        //}
    }

    std::set<wxString> ns = fcFileOpener::Get()->GetNamespaces();
    std::set<wxString>::iterator iter = ns.begin();
    for(; iter != ns.end(); iter++) {
        wxPrintf("Using namespace: %s\n", iter->c_str());
    }

    std::set<wxString> nsa = fcFileOpener::Get()->GetNamespaceAliases();
    iter = nsa.begin();
    for(; iter != nsa.end(); iter++) {
        wxPrintf("Namespace alias: %s\n", iter->c_str());
    }
    return 0;
}
