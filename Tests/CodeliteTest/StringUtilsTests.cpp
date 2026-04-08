#include "../CodeLite/StringUtils.h"

#include <doctest.h>
#include <vector>

namespace
{
using SplitResult = std::vector<std::vector<wxString>>;

void RequireSplitEq(const SplitResult& actual, const SplitResult& expected)
{
    REQUIRE(actual.size() == expected.size());
    for (size_t i = 0; i < expected.size(); ++i) {
        REQUIRE(actual[i].size() == expected[i].size());
        for (size_t j = 0; j < expected[i].size(); ++j) {
            CHECK(actual[i][j] == expected[i][j]);
        }
    }
}

SplitResult ToSplitResult(std::initializer_list<std::initializer_list<const char*>> init)
{
    SplitResult out;
    out.reserve(init.size());
    for (const auto& group : init) {
        out.emplace_back();
        out.back().reserve(group.size());
        for (const char* s : group) {
            out.back().emplace_back(s);
        }
    }
    return out;
}
} // namespace

TEST_CASE("StringUtils::SplitShellCommand - empty input")
{
    const auto result = StringUtils::SplitShellCommand("");
    CHECK(result.empty());
}

TEST_CASE("StringUtils::SplitShellCommand - whitespace only input")
{
    const auto result = StringUtils::SplitShellCommand("   \t  ");
    CHECK(result.empty());
}

TEST_CASE("StringUtils::SplitShellCommand - single command")
{
    const auto result = StringUtils::SplitShellCommand("git diff");
    RequireSplitEq(result, ToSplitResult({{"git", "diff"}}));
}

TEST_CASE("StringUtils::SplitShellCommand - happy path with &&")
{
    const auto result = StringUtils::SplitShellCommand(R"(git diff && ls -l && echo "hello && world")");
    RequireSplitEq(result, ToSplitResult({{"git", "diff"}, {"ls", "-l"}, {"echo", "hello && world"}}));
}

TEST_CASE("StringUtils::SplitShellCommand - mixed separators")
{
    const auto result = StringUtils::SplitShellCommand(R"(cmd1 arg1 ; cmd2 && cmd3 || cmd4)");
    RequireSplitEq(result, ToSplitResult({{"cmd1", "arg1"}, {"cmd2"}, {"cmd3"}, {"cmd4"}}));
}

TEST_CASE("StringUtils::SplitShellCommand - quoted separators are preserved")
{
    const auto result = StringUtils::SplitShellCommand(R"(echo "a && b" ; echo 'c || d' ; echo `e ; f`)");
    // BuildArgv preserves quoting characters for single quotes and backticks.
    RequireSplitEq(result, ToSplitResult({{"echo", "a && b"}, {"echo", "'c || d'"}, {"echo", "`e ; f`"}}));
}

TEST_CASE("StringUtils::SplitShellCommand - escaped separator outside quotes")
{
    const wxString command = R"RAW(echo foo\;bar && echo done)RAW";
    const auto result = StringUtils::SplitShellCommand(command);
    RequireSplitEq(result, ToSplitResult({{"echo", R"RAW(foo\;bar)RAW"}, {"echo", "done"}}));
}

TEST_CASE("StringUtils::SplitShellCommand - trailing separator")
{
    const auto result = StringUtils::SplitShellCommand("echo one && ");
    RequireSplitEq(result, ToSplitResult({{"echo", "one"}}));
}

TEST_CASE("StringUtils::SplitShellCommand - leading separators are ignored")
{
    const auto result = StringUtils::SplitShellCommand(";; && echo hello");
    RequireSplitEq(result, ToSplitResult({{"echo", "hello"}}));
}

TEST_CASE("StringUtils::SplitShellCommand - unmatched quote keeps remainder together")
{
    const wxString command = R"RAW(echo "hello && world && ls -l)RAW";
    const auto result = StringUtils::SplitShellCommand(command);
    // BuildArgv keeps the leading quote when the quote is unmatched.
    RequireSplitEq(result, ToSplitResult({{"echo", R"RAW("hello && world && ls -l)RAW"}}));
}

TEST_CASE("StringUtils::SplitShellCommand - complex command with nesting style text")
{
    const wxString command = R"RAW(python -c "print('a && b')" && echo ok)RAW";
    const auto result = StringUtils::SplitShellCommand(command);
    RequireSplitEq(result, ToSplitResult({{"python", "-c", "print('a && b')"}, {"echo", "ok"}}));
}

TEST_CASE("StringUtils::SplitShellCommand - consecutive separators do not create empty commands")
{
    const auto result = StringUtils::SplitShellCommand("cmd1 &&&& cmd2;;;cmd3||||cmd4");
    RequireSplitEq(result, ToSplitResult({{"cmd1"}, {"cmd2"}, {"cmd3"}, {"cmd4"}}));
}

TEST_CASE("StringUtils::SplitShellCommand - separators inside escaped quotes are preserved")
{
    const wxString command = R"RAW(echo \"hello && world\" && echo done)RAW";
    const auto result = StringUtils::SplitShellCommand(command);
    RequireSplitEq(result, ToSplitResult({{"echo", R"RAW(\"hello)RAW"}, {R"RAW(world\")RAW"}, {"echo", "done"}}));
}

TEST_CASE("StringUtils::SplitShellCommand - separator at start and end only returns real commands")
{
    const auto result = StringUtils::SplitShellCommand("&& echo one ;; echo two ||");
    RequireSplitEq(result, ToSplitResult({{"echo", "one"}, {"echo", "two"}}));
}

TEST_CASE("StringUtils::SplitShellCommand - pipes and ampersands split commands")
{
    const auto result = StringUtils::SplitShellCommand("echo a|b & c | d && echo e");
    RequireSplitEq(result, ToSplitResult({{"echo", "a"}, {"b"}, {"c"}, {"d"}, {"echo", "e"}}));
}

TEST_CASE("StringUtils::SplitShellCommand - single ampersand splits commands")
{
    const auto result = StringUtils::SplitShellCommand("cmd1 & cmd2");
    RequireSplitEq(result, ToSplitResult({{"cmd1"}, {"cmd2"}}));
}

TEST_CASE("StringUtils::SplitShellCommand - mixed ampersands and logical operators")
{
    const wxString command = R"RAW(cmd1 && cmd2 & cmd3 || cmd4)RAW";
    const auto result = StringUtils::SplitShellCommand(command);
    RequireSplitEq(result, ToSplitResult({{"cmd1"}, {"cmd2"}, {"cmd3"}, {"cmd4"}}));
}

TEST_CASE("StringUtils::SplitShellCommand - leading ampersand is ignored")
{
    const auto result = StringUtils::SplitShellCommand("& cmd1");
    RequireSplitEq(result, ToSplitResult({{"cmd1"}}));
}

TEST_CASE("StringUtils::SplitShellCommand - trailing ampersand is ignored")
{
    const auto result = StringUtils::SplitShellCommand("cmd1 &");
    RequireSplitEq(result, ToSplitResult({{"cmd1"}}));
}

TEST_CASE("StringUtils::SplitShellCommand - empty command between separators is skipped")
{
    const auto result = StringUtils::SplitShellCommand("cmd1 ; ; ; cmd2");
    RequireSplitEq(result, ToSplitResult({{"cmd1"}, {"cmd2"}}));
}

TEST_CASE("StringUtils::SplitShellCommand - command with trailing backslash remains intact")
{
    const wxString command = R"RAW(echo hello\\)RAW";
    const auto result = StringUtils::SplitShellCommand(command);
    RequireSplitEq(result, ToSplitResult({{"echo", R"RAW(hello\\)RAW"}}));
}

TEST_CASE("StringUtils::SplitShellCommand - unmatched single quote does not split on separators")
{
    const auto result = StringUtils::SplitShellCommand(R"(echo 'hello && world ; still one command)");
    RequireSplitEq(result, ToSplitResult({{"echo", "'hello && world ; still one command"}}));
}

TEST_CASE("StringUtils::SplitShellCommand - unmatched double quote does not split on separators")
{
    const auto result = StringUtils::SplitShellCommand(R"(echo "hello || world ; still one command)");
    RequireSplitEq(result, ToSplitResult({{"echo", "\"hello || world ; still one command"}}));
}
