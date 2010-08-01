#include <stdio.h>
#include <vector>
#include "pptable.h"
#include "pp_include.h"
#include <wx/ffile.h>

void testMacros();
void searchAndReplace();

int main(int argc, char **argv)
{
    //testMacros();
    searchAndReplace();
	return 0;
}

void searchAndReplace()
{
    std::string  inStr       = "if(wx_dynamic_cast(wxApp*, ptr)->OnInit()){";
    std::string  pattern     = "wx_dynamic_cast(%0, %1)";
    std::string  replaceWith = "dynmaic_cast<%0>(%1)";
    std::string  output = CLReplacePatternA(inStr, pattern, replaceWith);
    printf("%s\n", output.c_str());
}

void testMacros()
{
    wxFFile fp(wxT("output.txt"), wxT("a+"));

    PPTable::Instance()->Clear();
    PPScan(wxT("C:/MinGW-4.4.1/lib/gcc/mingw32/4.4.1/include/c++/mingw32/bits/c++config.h"));
    PPScan(wxT("C:/wxWidgets-2.8.10/include/wx/defs.h"));
    PPScan(wxT("C:/MinGW-4.4.1/include/_mingw.h"));

    // Export it into codelite's format
    wxString table = PPTable::Instance()->Export();

    fp.Write(table);
    fp.Close();
}
