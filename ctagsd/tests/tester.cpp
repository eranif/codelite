#include "tester.hpp"

#include "clAnsiEscapeCodeColourBuilder.hpp"
#ifdef _WIN32
#include <Windows.h>
#endif
#include <stdio.h>

Tester* Tester::ms_instance = 0;

Tester::Tester() {}

Tester::~Tester() {}

Tester* Tester::Instance()
{
    if(ms_instance == 0) {
        ms_instance = new Tester();
    }
    return ms_instance;
}

void Tester::Release()
{
    if(ms_instance) {
        delete ms_instance;
    }
    ms_instance = 0;
}

void Tester::AddTest(ITest* t) { m_tests.push_back(t); }

void Tester::RunTests()
{
#ifdef _WIN32
    SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), ENABLE_VIRTUAL_TERMINAL_PROCESSING | ENABLE_PROCESSED_OUTPUT);
#endif

    clAnsiEscapeCodeColourBuilder builder;
    builder.SetTheme(eAsciiTheme::DARK);

    vector<wxString> failures;
    for(size_t i = 0; i < m_tests.size(); i++) {
        ITest* test = m_tests[i];
        if(test->test()) {
            builder.Add(wxString() << test->name() << "....", eAsciiColours::NORMAL_TEXT);
            builder.Add("OK", eAsciiColours::GREEN);
            builder.Add(wxString() << " (" << test->get_check_counter() << " checks performed)", eAsciiColours::GRAY);
            wxPrintf(wxT("%s\n"), builder.GetString());
        } else {
            builder.Add(wxString() << test->name() << "....", eAsciiColours::NORMAL_TEXT);
            builder.Add("FAILED", eAsciiColours::RED);
            builder.Add(wxString() << " (" << test->file() << ":" << test->line() << ")", eAsciiColours::GRAY);
            wxPrintf(wxT("%s\n"), builder.GetString());
            failures.push_back(builder.GetString());
        }
        builder.Clear();
    }

    printf("\n====> Summary: <====\n\n");

    if(failures.empty()) {
        printf("All tests passed successfully!\n");
    } else {
        printf("Some tests failed. See summary below\n");
        for(const wxString& message : failures) {
            wxPrintf("%s\n", message);
        }
    }
}
