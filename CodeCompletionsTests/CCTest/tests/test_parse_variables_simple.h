void testCC()
{
    // Load the tags database that is used during the test.
    wxFileName fn(wxT("../../SampleWorkspace/.codelite/SampleWorkspace.tags"));
    TagsManagerST::Get()->OpenDatabase(fn);

    TagsOptionsData tod;
    TagsManagerST::Get()->SetCtagsOptions(tod);
    TagsManagerST::Get()->GetDatabase()->SetSingleSearchLimit(
        5000); // For the testing to pass we need to set a huge number of matches
    // Execute the tests
    Tester::Instance()->RunTests();

    Tester::Release();
    TagsManagerST::Free();
    LanguageST::Free();
}


int a,b;
int c = 1, d = 2;
int e(3), f(4);
int g{1};
std::string str;
std::map<std::string, std::string, _Allocator()> stringMap;