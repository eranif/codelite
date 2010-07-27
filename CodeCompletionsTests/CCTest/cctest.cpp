#include <wx/init.h>	//wxInitializer
#include <wx/string.h>	//wxString
#include "tester.h"
#include <wx/ffile.h>
#include <memory>
#include <vector>

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

/////////////////////////////////////////////////////////////////////////////
// Code Completion test cases
/////////////////////////////////////////////////////////////////////////////

TEST_FUNC(testUsingNamespace)
{
	std::vector<TagEntryPtr> tags;
	TagsManagerST::Get()->AutoCompleteCandidates(wxFileName(wxT("../tests/using_namespace.h")), 3, wxT("v.at(0)."), LoadFile(wxT("../tests/using_namespace.h")), tags);
	CHECK_SIZE(tags.size(), 197);
	return true;
}

TEST_FUNC(testInnerClassInheritance)
{
	std::vector<TagEntryPtr> tags;
	TagsManagerST::Get()->AutoCompleteCandidates(wxFileName(wxT("../tests/inner_class_inheritance.h")), 3, wxT("td->"), LoadFile(wxT("../tests/inner_class_inheritance.h")), tags);
	CHECK_SIZE(tags.size(), 87);
	return true;
}

TEST_FUNC(testTypedefs)
{
	std::vector<TagEntryPtr> tags;
	TagsManagerST::Get()->AutoCompleteCandidates(wxFileName(wxT("../tests/typedef_tests.h")), 1, wxT("myStack.at(0)."), LoadFile(wxT("../tests/typedef_tests.h")), tags);
	CHECK_SIZE(tags.size(), 5);
	return true;
}

TEST_FUNC(testStlMapLeditor)
{
	// test map template with basic types
	// std::map<wxString, int> mm;
	std::vector<TagEntryPtr> tags;
	TagsManagerST::Get()->AutoCompleteCandidates(wxFileName(wxT("../tests/stl_map_static.h")), 2, wxT("ms_bookmarkShapes.find(0)->first."), LoadFile(wxT("../tests/stl_map_static.h")), tags);
	CHECK_SIZE(tags.size(), 314);
	
	TagsManagerST::Get()->AutoCompleteCandidates(wxFileName(wxT("../tests/stl_map_static.h")), 2, wxT("ms_bookmarkShapes.find(wxT(\"value\"))->first."), LoadFile(wxT("../tests/stl_map_static.h")), tags);
	CHECK_SIZE(tags.size(), 314);

	TagsManagerST::Get()->AutoCompleteCandidates(wxFileName(wxT("../tests/stl_map_static.h")), 2, wxT("ms_bookmarkShapes.find(wxT(\"value\"))->second."), LoadFile(wxT("../tests/stl_map_static.h")), tags);
	CHECK_SIZE(tags.size(), 0);

	return true;
}

TEST_FUNC(testMapWithBasicTypes)
{
	// test map template with basic types
	// std::map<wxString, int> mm;
	std::vector<TagEntryPtr> tags;
	TagsManagerST::Get()->AutoCompleteCandidates(wxFileName(wxT("../tests/simple_tests.h")), 2, wxT("mm.find()->first."), LoadFile(wxT("../tests/simple_tests.h")), tags);
	CHECK_SIZE(tags.size(), 314);
	
	TagsManagerST::Get()->AutoCompleteCandidates(wxFileName(wxT("../tests/simple_tests.h")), 2, wxT("mm.find()->second."), LoadFile(wxT("../tests/simple_tests.h")), tags);
	CHECK_SIZE(tags.size(), 0);
	return true;
}


TEST_FUNC(testTtp)
{
	std::vector<TagEntryPtr> tags;
	TagsManagerST::Get()->AutoCompleteCandidates(wxFileName(wxT("../tests/smart_ptr_of_template.h")), 3, wxT("ttp->GetRoot()->GetData()."), LoadFile(wxT("../tests/smart_ptr_of_template.h")), tags);
	CHECK_SIZE(tags.size(), 76);
	
	TagsManagerST::Get()->AutoCompleteCandidates(wxFileName(wxT("../tests/smart_ptr_of_template.h")), 3, wxT("ttp->GetRoot()->GetKey()."), LoadFile(wxT("../tests/smart_ptr_of_template.h")), tags);
	CHECK_SIZE(tags.size(), 314);
	return true;
}

TEST_FUNC(testTempalteInheritance)
{
	std::vector<TagEntryPtr> tags;
	TagsManagerST::Get()->AutoCompleteCandidates(wxFileName(wxT("../tests/simple_tests.h")), 2, wxT("ContextManager::Get()->"), LoadFile(wxT("../tests/simple_tests.h")), tags);
	CHECK_SIZE(tags.size(), 14);
	return true;
}

TEST_FUNC(testThis)
{
	std::vector<TagEntryPtr> tags;
	TagsManagerST::Get()->AutoCompleteCandidates(wxFileName(wxT("../tests/this_ptr.h")), 4, wxT("this->"), LoadFile(wxT("../tests/this_ptr.h")), tags);
	
#ifdef __WXMSW__
	CHECK_SIZE(tags.size(), 1140);
#else
	CHECK_SIZE(tags.size(), 952);
#endif
	return true;
}

TEST_FUNC(testStdVectorOfTagEntryPtr)
{
	std::vector<TagEntryPtr> tags;
	TagsManagerST::Get()->AutoCompleteCandidates(wxFileName(wxT("../tests/std_vec_tag_entry_ptr.h")), 3, wxT("tags.at(0)->"), LoadFile(wxT("../tests/std_vec_tag_entry_ptr.h")), tags);
	CHECK_SIZE(tags.size(), 76);
	
	TagsManagerST::Get()->AutoCompleteCandidates(wxFileName(wxT("../tests/std_vec_tag_entry_ptr.h")), 3, wxT("tags.at(0)."), LoadFile(wxT("../tests/std_vec_tag_entry_ptr.h")), tags);
	CHECK_SIZE(tags.size(), 14);
	return true;
}

TEST_FUNC(testIterators)
{
	std::vector<TagEntryPtr> tags;
	TagsManagerST::Get()->AutoCompleteCandidates(wxFileName(wxT("../tests/iterators.h")), 3, wxT("mapIter->first."), LoadFile(wxT("../tests/iterators.h")), tags);
	CHECK_SIZE(tags.size(), 314);
	
	tags.clear();
	TagsManagerST::Get()->AutoCompleteCandidates(wxFileName(wxT("../tests/iterators.h")), 3, wxT("vecIter->"), LoadFile(wxT("../tests/iterators.h")), tags);
	CHECK_SIZE(tags.size(), 314);
	return true;
}

TEST_FUNC(testTemplateTypedef)
{
	std::vector<TagEntryPtr> tags;
	TagsManagerST::Get()->AutoCompleteCandidates(wxFileName(wxT("../tests/test_template_typedef.h")), 3, wxT("ManagerST::Get()->"), LoadFile(wxT("../tests/test_template_typedef.h")), tags);
	CHECK_SIZE(tags.size(), 202);
	return true;
}

TEST_FUNC(testNamespace)
{
	std::vector<TagEntryPtr> tags;
	TagsManagerST::Get()->AutoCompleteCandidates(wxFileName(wxT("../tests/test_namespace.h")), 3, wxT("flex::"), LoadFile(wxT("../tests/test_namespace.h")), tags);
	CHECK_SIZE(tags.size(), 36);
	return true;
}

TEST_FUNC(testLocalArgument)
{
	std::vector<TagEntryPtr> tags;
	TagsManagerST::Get()->AutoCompleteCandidates(wxFileName(wxT("../tests/test_local_var.h")), 2, wxT("path."), LoadFile(wxT("../tests/test_local_var.h")), tags);
	CHECK_SIZE(tags.size(), 314);
	return true;
}

TEST_FUNC(testStdAutoPtr)
{
	std::vector<TagEntryPtr> tags;
	TagsManagerST::Get()->AutoCompleteCandidates(wxFileName(wxT("../tests/test_auto_ptr.h")), 4, wxT("p->"), LoadFile(wxT("../tests/test_auto_ptr.h")), tags);
	CHECK_SIZE(tags.size(), 314);
	return true;
}

TEST_FUNC(testVectorOfStdString_OperatorMeruba)
{
	std::vector<TagEntryPtr> tags;
	TagsManagerST::Get()->AutoCompleteCandidates(wxFileName(wxT("../tests/test_vector_std_string.h")), 6, wxT("v[0]->"), LoadFile(wxT("../tests/test_vector_std_string.h")), tags);
	CHECK_SIZE(tags.size(), 197);
	return true;
}

TEST_FUNC(testWxArrayString_OperatorMeruba)
{
	std::vector<TagEntryPtr> tags;
	TagsManagerST::Get()->AutoCompleteCandidates(wxFileName(wxT("../tests/test_wxarraystr.h")), 7, wxT("myArr[]."), LoadFile(wxT("../tests/test_wxarraystr.h")), tags);
	CHECK_SIZE(tags.size(), 314);
	return true;
}


TEST_FUNC(testStdSharedPtr)
{
	std::vector<TagEntryPtr> tags;
	TagsManagerST::Get()->AutoCompleteCandidates(wxFileName(wxT("../tests/test_shared_ptr.h")), 10, wxT("p->"), LoadFile(wxT("../tests/test_shared_ptr.h")), tags);
	CHECK_SIZE(tags.size(), 314);
	return true;
}

TEST_FUNC(testClFrame)
{
	std::vector<TagEntryPtr> tags;
	TagsManagerST::Get()->AutoCompleteCandidates(wxFileName(wxT("../tests/test_cl_frame.h")), 4, wxT("Frame::Get()->"), LoadFile(wxT("../tests/test_cl_frame.h")), tags);
#ifdef __WXMSW__
	CHECK_SIZE(tags.size(), 1140);
#else
	CHECK_SIZE(tags.size(), 952);
#endif
	
	return true;
}


TEST_FUNC(testVectorOfStdString)
{
	std::vector<TagEntryPtr> tags;
	TagsManagerST::Get()->AutoCompleteCandidates(wxFileName(wxT("../tests/test_vector_std_string.h")), 6, wxT("v.at(0)->"), LoadFile(wxT("../tests/test_vector_std_string.h")), tags);
	CHECK_SIZE(tags.size(), 197);
	return true;
}

/**
 * @brief call the test framework
 */
int main(int argc, char **argv)
{
	//Initialize the wxWidgets library
	wxInitializer initializer;
	
	// Load the tags database that is used during the test.
	wxFileName fn(wxT("../../SampleWorkspace/SampleWorkspace.tags"));
	TagsManagerST::Get()->OpenDatabase( fn );
	
	// Execute the tests
	Tester::Instance()->RunTests();
	
	Tester::Release();
	TagsManagerST::Free();
	LanguageST::Free();
	return 0;
}
