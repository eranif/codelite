#include "CxxTokenizer.h"
#include "CxxVariableScanner.h"
#include "ctags_manager.h"
#include "fileutils.h"
#include "tester.hpp"
#include <iostream>
#include <stdio.h>
#include <wx/init.h>
#include <wx/log.h>
#include "CompletionHelper.hpp"

TEST_FUNC(test_expression_1)
{
    CompletionHelper helper;
    {
        wxString expr = helper.get_expression_from_location("m_string.", 0, 0, nullptr);
        CHECK_STRING(expr, "m_string.");
    }

    {
        wxString expr = helper.get_expression_from_location("string name=m_string.", 0, 0, nullptr);
        CHECK_STRING(expr, "m_string.");
    }

    {
        wxString expr = helper.get_expression_from_location("string name=m_string->", 0, 0, nullptr);
        CHECK_STRING(expr, "m_string->");
    }

    {
        wxString expr = helper.get_expression_from_location("get_details().get_name().", 0, 0, nullptr);
        CHECK_STRING(expr, "get_details().get_name().");
    }

    {
        wxString expr = helper.get_expression_from_location("foo(){ std::vector<std::string>::", 0, 0, nullptr);
        CHECK_STRING(expr, "std::vector<std::string>::");
    }

    {
        wxString expr = helper.get_expression_from_location("auto foo = [=](std::ve", 0, 0, nullptr);
        CHECK_STRING(expr, "std::ve");
    }

    return true;
}

int main(int argc, char** argv)
{
    wxInitializer initializer(argc, argv);
    wxLogNull NOLOG;
    Tester::Instance()->RunTests();
    return 0;
}
