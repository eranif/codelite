#include <stdio.h>
#include <wx/tokenzr.h>
#include <vector>
#include "pptable.h"
#include "pp_include.h"
#include <wx/ffile.h>

void testMacros();
void searchAndReplaceA();
void searchAndReplaceW();

int main(int argc, char **argv) {
	//testMacros();
	for(size_t i=0; i<100; i++) {
		//printf("searchAndReplaceA\n");
		searchAndReplaceA();
	}
	return 0;
}

void searchAndReplaceW() {
#ifdef __WXMSW__
	wxFFile input(wxT("C:\\Users\\eran\\src\\Development\\C++\\codelite\\CL_trunk\\CodeLite\\tags_options_data.cpp"), wxT("r"));
#else
	wxFFile input(wxT("/home/eran/devl/codelite/CodeLite/tags_options_data.cpp"), wxT("r"));
#endif

	if(input.IsOpened()) {

		wxString content;
		input.ReadAll(&content);
		wxArrayString lines = wxStringTokenize(content, wxT("\n"), wxTOKEN_STRTOK);
		for(size_t i=0; i<lines.GetCount(); i++) {
			wxString inStr       = lines.Item(i);
			wxString pattern     = wxT("WXUNUSED(%0)");
			wxString replaceWith = wxT("%0");
			wxString output;

			output = inStr;
			while(CLReplacePattern(output, pattern, replaceWith, output)) {}

		}
	}
}

void searchAndReplaceA() {
#ifdef __WXMSW__
	wxFFile input(wxT("C:\\Users\\eran\\src\\Development\\C++\\codelite\\CL_trunk\\CodeLite\\tags_options_data.cpp"), wxT("r"));
#else
	wxFFile input(wxT("/home/eran/devl/codelite/CodeLite/tags_options_data.cpp"), wxT("r"));
#endif

	if(input.IsOpened()) {
		wxString content;
		input.ReadAll(&content);
		wxArrayString lines = wxStringTokenize(content, wxT("\n"), wxTOKEN_STRTOK);
		for(size_t i=0; i<lines.GetCount(); i++) {
			std::string  inStr       = lines.Item(i).mb_str(wxConvUTF8).data();
			std::string  pattern     = "WXUNUSED(%0)";
			std::string  replaceWith = "%0";
			std::string  output;

			output = inStr;
			while(CLReplacePatternA(output, pattern, replaceWith, output)) {}
		}
	}
}

void testMacros() {
	wxFFile fp(wxT("output.txt"), wxT("w+"));

	PPTable::Instance()->Clear();
	//PPScan(wxT("C:/Users/eran/src/MinGW-4.4.1/lib/gcc/mingw32/4.4.1/include/c++/mingw32/bits/c++config.h"));
	//PPScan(wxT("C:/Users/eran/src/wxWidgets-2.8.10/include/wx/defs.h"));
	//PPScan(wxT("C:/Users/eran/src/MinGW-4.4.1/include/_mingw.h"));
	PPScan(wxT("C:/Users/eran/src/wxWidgets-2.8.10/include/wx/dlimpexp.h"));

	// Export it into codelite's format
	wxString table = PPTable::Instance()->Export();

	fp.Write(table);
	fp.Close();
}
