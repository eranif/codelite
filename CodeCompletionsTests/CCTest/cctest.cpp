#include <wx/init.h>	//wxInitializer
#include <wx/string.h>	//wxString
#include <wx/ffile.h>

// CodeLite includes
#include <ctags_manager.h>

#define CHECK_SIZE(actualSize, expcSize) { if(actualSize == expcSize) {\
			printf("%s: Successfull!\n", __FUNCTION__);\
		} else {\
			printf("%s: ERROR: Expcted size: %d, Actual Size:%d\n", __FUNCTION__, expcSize, actualSize);\
		}\
	}

wxString LoadFile(const wxString &filename)
{
	wxFFile f(filename.c_str(), wxT("r"));
	if(f.IsOpened()) {
		wxString content;
		f.ReadAll(&content);
		return content;
	}
	return wxEmptyString;
}

void testWxArrayString_OperatorMeruba()
{
	std::vector<TagEntryPtr> tags;
	TagsManagerST::Get()->AutoCompleteCandidates(wxFileName(wxT("../tests/test_wxarraystr.h")), 7, wxT("myArr[]."), LoadFile(wxT("../tests/test_wxarraystr.h")), tags);
	CHECK_SIZE(tags.size(), 314);
}

void testVectorOfStdString()
{
	std::vector<TagEntryPtr> tags;
	TagsManagerST::Get()->AutoCompleteCandidates(wxFileName(wxT("../tests/test_vector_std_string.h")), 6, wxT("v.at(0)->"), LoadFile(wxT("../tests/test_vector_std_string.h")), tags);
	CHECK_SIZE(tags.size(), 197);
}

void testVectorOfStdString_OperatorMeruba()
{
	std::vector<TagEntryPtr> tags;
	TagsManagerST::Get()->AutoCompleteCandidates(wxFileName(wxT("../tests/test_vector_std_string.h")), 6, wxT("v[0]->"), LoadFile(wxT("../tests/test_vector_std_string.h")), tags);
	CHECK_SIZE(tags.size(), 197);
}

int main(int argc, char **argv)
{
	//Initialize the wxWidgets library
	wxInitializer initializer;
	
	// Load the tags database that is used during the test.
	
	wxFileName fn(wxT("../../SampleWorkspace/SampleWorkspace.tags"));
	TagsManagerST::Get()->OpenDatabase( fn );
	
	// Execute the tests
	testVectorOfStdString();
	testVectorOfStdString_OperatorMeruba();
	testWxArrayString_OperatorMeruba();
	return 0;
}
