#include <wx/init.h>	//wxInitializer
#include <wx/string.h>	//wxString
#include "tester.h"
#include <wx/ffile.h>
#include <memory>

// CodeLite includes
#include <ctags_manager.h>

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

TEST_FUNC(testWxArrayString_OperatorMeruba)
{
	std::vector<TagEntryPtr> tags;
	TagsManagerST::Get()->AutoCompleteCandidates(wxFileName(wxT("../tests/test_wxarraystr.h")), 7, wxT("myArr[]."), LoadFile(wxT("../tests/test_wxarraystr.h")), tags);
	CHECK_SIZE(tags.size(), 314);
}

TEST_FUNC(testVectorOfStdString)
{
	std::vector<TagEntryPtr> tags;
	TagsManagerST::Get()->AutoCompleteCandidates(wxFileName(wxT("../tests/test_vector_std_string.h")), 6, wxT("v.at(0)->"), LoadFile(wxT("../tests/test_vector_std_string.h")), tags);
	CHECK_SIZE(tags.size(), 197);
}

TEST_FUNC(testVectorOfStdString_OperatorMeruba)
{
	std::vector<TagEntryPtr> tags;
	TagsManagerST::Get()->AutoCompleteCandidates(wxFileName(wxT("../tests/test_vector_std_string.h")), 6, wxT("v[0]->"), LoadFile(wxT("../tests/test_vector_std_string.h")), tags);
	CHECK_SIZE(tags.size(), 197);
}

TEST_FUNC(testStdSharedPtr)
{
	std::vector<TagEntryPtr> tags;
	TagsManagerST::Get()->AutoCompleteCandidates(wxFileName(wxT("../tests/test_shared_ptr.h")), 10, wxT("p->"), LoadFile(wxT("../tests/test_shared_ptr.h")), tags);
	CHECK_SIZE(tags.size(), 314);
}

TEST_FUNC(testStdAutoPtr)
{
	std::vector<TagEntryPtr> tags;
	TagsManagerST::Get()->AutoCompleteCandidates(wxFileName(wxT("../tests/test_auto_ptr.h")), 4, wxT("p->"), LoadFile(wxT("../tests/test_auto_ptr.h")), tags);
	CHECK_SIZE(tags.size(), 314);
}

TEST_FUNC(testClFrame)
{
	std::vector<TagEntryPtr> tags;
	TagsManagerST::Get()->AutoCompleteCandidates(wxFileName(wxT("../tests/test_cl_frame.h")), 4, wxT("Frame::Get()->"), LoadFile(wxT("../tests/test_cl_frame.h")), tags);
	CHECK_SIZE(tags.size(), 950);
}

int main(int argc, char **argv)
{
	//Initialize the wxWidgets library
	wxInitializer initializer;
	
	// Load the tags database that is used during the test.
	wxFileName fn(wxT("../../SampleWorkspace/SampleWorkspace.tags"));
	TagsManagerST::Get()->OpenDatabase( fn );
	
	// Execute the tests
	Tester::Instance()->RunTests();
	return 0;
}
