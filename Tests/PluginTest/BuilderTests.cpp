#include "builder/builder.h"

#include <doctest.h>

TEST_CASE("Builder::ToMakefileVariableRefs converts references to the makefile syntax")
{
    SUBCASE("$NAME, ${NAME} and %NAME% become $(NAME)")
    {
        CHECK(Builder::ToMakefileVariableRefs("$PATH;C:/bin") == "$(PATH);C:/bin");
        CHECK(Builder::ToMakefileVariableRefs("${HOME}/lib") == "$(HOME)/lib");
        CHECK(Builder::ToMakefileVariableRefs("%PATH%;C:\\tools") == "$(PATH);C:\\tools");
        CHECK(Builder::ToMakefileVariableRefs("$A$B") == "$(A)$(B)");
        CHECK(Builder::ToMakefileVariableRefs("$MY_VAR1/x") == "$(MY_VAR1)/x");
    }

    SUBCASE("$(NAME) and $$ are not changed")
    {
        CHECK(Builder::ToMakefileVariableRefs("$(WXWIN)/include") == "$(WXWIN)/include");
        CHECK(Builder::ToMakefileVariableRefs("$$HOME") == "$$HOME");
        CHECK(Builder::ToMakefileVariableRefs("a$$") == "a$$");
    }

    SUBCASE("Text that is not a reference is not changed")
    {
        CHECK(Builder::ToMakefileVariableRefs("") == "");
        CHECK(Builder::ToMakefileVariableRefs("C:/msys64/usr/bin") == "C:/msys64/usr/bin");
        CHECK(Builder::ToMakefileVariableRefs("cost$") == "cost$");
        CHECK(Builder::ToMakefileVariableRefs("${}") == "${}");
        CHECK(Builder::ToMakefileVariableRefs("${A-B}") == "${A-B}");
        CHECK(Builder::ToMakefileVariableRefs("${NAME") == "${NAME");
        CHECK(Builder::ToMakefileVariableRefs("$@ $<") == "$@ $<");
        CHECK(Builder::ToMakefileVariableRefs("100%") == "100%");
        CHECK(Builder::ToMakefileVariableRefs("%%") == "%%");
        CHECK(Builder::ToMakefileVariableRefs("%.o") == "%.o");
        CHECK(Builder::ToMakefileVariableRefs("%A B%") == "%A B%");
        // Percent-encoded text: a %NAME% name does not start with a digit
        CHECK(Builder::ToMakefileVariableRefs("a%20b%20c") == "a%20b%20c");
        CHECK(Builder::ToMakefileVariableRefs("http://user:p%40ss%21@host") == "http://user:p%40ss%21@host");
        CHECK(Builder::ToMakefileVariableRefs("%_X1%") == "$(_X1)");
    }
}

TEST_CASE("Builder::CreateMakefileEnvVarsSection writes only the variables that the makefile uses")
{
    const Builder::EnvVarEntries variables = {
        {"PATH", "$PATH;C:/msys64/usr/bin"},
        {"API_KEY", "secret"},
        {"WXWIN", "C:/wx"},
        {"WXCFG", "clang_x64_dll/mswu"},
        {"LIBDIR", "$(BASE)/lib"},
        {"BASE", "C:/base"},
        {"UNUSED", "x"},
        {"SRCS", "a.c"},
        {"FEATURE", "1"},
    };

    const wxString makefile = "IncludePath := -I$(WXWIN)/include\n"
                              "LibPath := -L$(LIBDIR)\n"
                              "Objects := $(SRCS:.c=.o)\n"
                              "ifdef FEATURE\n"
                              "endif\n"
                              "Api := $(API_KEY_FILE)\n";

    const wxString text = Builder::CreateMakefileEnvVarsSection(makefile, variables, ":=");

    // PATH is always written, with the references converted
    CHECK(text.Contains("PATH:=$(PATH);C:/msys64/usr/bin\n"));
    // Used directly, as a substitution reference, or in "ifdef"
    CHECK(text.Contains("WXWIN:=C:/wx\n"));
    CHECK(text.Contains("SRCS:=a.c\n"));
    CHECK(text.Contains("FEATURE:=1\n"));
    // Used by a written variable
    CHECK(text.Contains("LIBDIR:=$(BASE)/lib\n"));
    CHECK(text.Contains("BASE:=C:/base\n"));
    // Not used: "API_KEY" is only a part of "API_KEY_FILE"
    CHECK_FALSE(text.Contains("API_KEY:="));
    CHECK_FALSE(text.Contains("secret"));
    CHECK_FALSE(text.Contains("WXCFG"));
    CHECK_FALSE(text.Contains("UNUSED"));

    // The order of the variables is kept
    CHECK(text.Find("PATH:=") < text.Find("WXWIN:="));
    CHECK(text.Find("LIBDIR:=") < text.Find("BASE:="));
}

TEST_CASE("Builder::CreateMakefileEnvVarsSection keeps duplicates and uses the assignment operator")
{
    const Builder::EnvVarEntries variables = {
        {"Path", "%PATH%;C:/tools"},
        {"DIR", "a"},
        {"DIR", "b"},
    };

    const wxString text = Builder::CreateMakefileEnvVarsSection("X = $(DIR)\n", variables, "=");
    CHECK(text.Contains("Path=$(PATH);C:/tools\n"));
    CHECK(text.Contains("DIR=a\nDIR=b\n"));
}
