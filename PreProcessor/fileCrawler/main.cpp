#include <stdio.h>
#include <vector>
#include "pptable.h"
#include "pp_include.h"
#include <wx/ffile.h>

void testMacros();
void searchAndReplaceA();
void searchAndReplaceW();

int main(int argc, char **argv)
{
    //testMacros();
	for(size_t i=0; i<500; i++)
		searchAndReplaceW();
	return 0;
}

void searchAndReplaceW()
{
    wxString inStr       = wxT("if(wx_dynamic_cast(wxApp*, ptr)->OnInit()) if(wx_dynamic_cast(wxApp*, ptr)->OnInit())");
    wxString pattern     = wxT("wx_dynamic_cast(%0, %1)");
    wxString replaceWith = wxT("dynmaic_cast<%0>(%1)");
    wxString output;
	
	output = inStr;
	while(CLReplacePattern(output, pattern, replaceWith, output)) {
		
	}
}

void searchAndReplaceA()
{
    std::string  inStr       = "if(wx_dynamic_cast(wxApp*, ptr)->OnInit()) if(wx_dynamic_cast(wxApp*, ptr)->OnInit())";
    std::string  pattern     = "wx_dynamic_cast(%0, %1)";
    std::string  replaceWith = "dynmaic_cast<%0>(%1)";
    std::string  output;
	
	output = inStr;
	while(CLReplacePatternA(output, pattern, replaceWith, output)) {}
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
