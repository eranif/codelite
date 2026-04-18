#include "fileutils.h"

#include <doctest.h>

TEST_CASE("FileUtils::ParseGrepLine - basic relative path")
{
    auto result = FileUtils::ParseGrepLine("src/file.cpp:42:some matched text");
    REQUIRE(result.has_value());
    CHECK(result->filename == "src/file.cpp");
    CHECK(result->line_number == 42);
    CHECK(result->matched_text == "some matched text");
}

TEST_CASE("FileUtils::ParseGrepLine - absolute Unix path")
{
    auto result = FileUtils::ParseGrepLine("/home/user/project/file.cpp:100:text with content");
    REQUIRE(result.has_value());
    CHECK(result->filename == "/home/user/project/file.cpp");
    CHECK(result->line_number == 100);
    CHECK(result->matched_text == "text with content");
}

TEST_CASE("FileUtils::ParseGrepLine - Windows absolute path with drive letter")
{
    auto result = FileUtils::ParseGrepLine("C:\\path\\to\\file.txt:50:matched text here");
    REQUIRE(result.has_value());
    CHECK(result->filename == "C:\\path\\to\\file.txt");
    CHECK(result->line_number == 50);
    CHECK(result->matched_text == "matched text here");
}

TEST_CASE("FileUtils::ParseGrepLine - Windows path with forward slashes")
{
    auto result = FileUtils::ParseGrepLine("C:/path/to/file.cpp:25:some code");
    REQUIRE(result.has_value());
    CHECK(result->filename == "C:/path/to/file.cpp");
    CHECK(result->line_number == 25);
    CHECK(result->matched_text == "some code");
}

TEST_CASE("FileUtils::ParseGrepLine - matched text contains colons")
{
    auto result = FileUtils::ParseGrepLine("/home/user/file.cpp:10:error: invalid syntax: check this");
    REQUIRE(result.has_value());
    CHECK(result->filename == "/home/user/file.cpp");
    CHECK(result->line_number == 10);
    CHECK(result->matched_text == "error: invalid syntax: check this");
}

TEST_CASE("FileUtils::ParseGrepLine - Windows path with colons in matched text")
{
    auto result = FileUtils::ParseGrepLine("C:\\project\\file.h:5:namespace std::vector");
    REQUIRE(result.has_value());
    CHECK(result->filename == "C:\\project\\file.h");
    CHECK(result->line_number == 5);
    CHECK(result->matched_text == "namespace std::vector");
}

TEST_CASE("FileUtils::ParseGrepLine - relative path with parent directory")
{
    auto result = FileUtils::ParseGrepLine("../../relative/path.cpp:99:function call");
    REQUIRE(result.has_value());
    CHECK(result->filename == "../../relative/path.cpp");
    CHECK(result->line_number == 99);
    CHECK(result->matched_text == "function call");
}

TEST_CASE("FileUtils::ParseGrepLine - empty matched text")
{
    auto result = FileUtils::ParseGrepLine("file.txt:1:");
    REQUIRE(result.has_value());
    CHECK(result->filename == "file.txt");
    CHECK(result->line_number == 1);
    CHECK(result->matched_text == "");
}

TEST_CASE("FileUtils::ParseGrepLine - line number zero")
{
    auto result = FileUtils::ParseGrepLine("file.txt:0:text");
    REQUIRE(result.has_value());
    CHECK(result->filename == "file.txt");
    CHECK(result->line_number == 0);
    CHECK(result->matched_text == "text");
}

TEST_CASE("FileUtils::ParseGrepLine - large line number")
{
    auto result = FileUtils::ParseGrepLine("bigfile.cpp:123456:code here");
    REQUIRE(result.has_value());
    CHECK(result->filename == "bigfile.cpp");
    CHECK(result->line_number == 123456);
    CHECK(result->matched_text == "code here");
}

TEST_CASE("FileUtils::ParseGrepLine - filename with spaces")
{
    auto result = FileUtils::ParseGrepLine("my file with spaces.txt:10:content");
    REQUIRE(result.has_value());
    CHECK(result->filename == "my file with spaces.txt");
    CHECK(result->line_number == 10);
    CHECK(result->matched_text == "content");
}

TEST_CASE("FileUtils::ParseGrepLine - Windows path with spaces")
{
    auto result = FileUtils::ParseGrepLine("C:\\Program Files\\app\\file.cpp:42:code");
    REQUIRE(result.has_value());
    CHECK(result->filename == "C:\\Program Files\\app\\file.cpp");
    CHECK(result->line_number == 42);
    CHECK(result->matched_text == "code");
}

TEST_CASE("FileUtils::ParseGrepLine - empty input")
{
    auto result = FileUtils::ParseGrepLine("");
    CHECK_FALSE(result.has_value());
}

TEST_CASE("FileUtils::ParseGrepLine - whitespace only")
{
    auto result = FileUtils::ParseGrepLine("   \t  ");
    CHECK_FALSE(result.has_value());
}

TEST_CASE("FileUtils::ParseGrepLine - no colons")
{
    auto result = FileUtils::ParseGrepLine("just a filename.txt");
    CHECK_FALSE(result.has_value());
}

TEST_CASE("FileUtils::ParseGrepLine - only one colon")
{
    auto result = FileUtils::ParseGrepLine("file.txt:42");
    CHECK_FALSE(result.has_value());
}

TEST_CASE("FileUtils::ParseGrepLine - invalid line number")
{
    auto result = FileUtils::ParseGrepLine("file.txt:abc:text");
    CHECK_FALSE(result.has_value());
}

TEST_CASE("FileUtils::ParseGrepLine - negative line number")
{
    auto result = FileUtils::ParseGrepLine("file.txt:-5:text");
    CHECK_FALSE(result.has_value());
}

TEST_CASE("FileUtils::ParseGrepLine - empty filename")
{
    auto result = FileUtils::ParseGrepLine(":42:text");
    CHECK_FALSE(result.has_value());
}

TEST_CASE("FileUtils::ParseGrepLine - trimming whitespace")
{
    auto result = FileUtils::ParseGrepLine("  file.cpp:10:text  ");
    REQUIRE(result.has_value());
    CHECK(result->filename == "file.cpp");
    CHECK(result->line_number == 10);
    CHECK(result->matched_text == "text  ");
}

TEST_CASE("FileUtils::ParseGrepLine - complex path with multiple dots")
{
    auto result = FileUtils::ParseGrepLine("../../../src/core/file.impl.cpp:200:implementation");
    REQUIRE(result.has_value());
    CHECK(result->filename == "../../../src/core/file.impl.cpp");
    CHECK(result->line_number == 200);
    CHECK(result->matched_text == "implementation");
}

TEST_CASE("FileUtils::ParseGrepLine - Windows network path (UNC)")
{
    // Note: UNC paths like \\server\share don't have a drive letter,
    // so they should work as-is
    auto result = FileUtils::ParseGrepLine("\\\\server\\share\\file.txt:15:content");
    REQUIRE(result.has_value());
    CHECK(result->filename == "\\\\server\\share\\file.txt");
    CHECK(result->line_number == 15);
    CHECK(result->matched_text == "content");
}

TEST_CASE("FileUtils::ParseGrepLine - file with no extension")
{
    auto result = FileUtils::ParseGrepLine("Makefile:30:target: dependencies");
    REQUIRE(result.has_value());
    CHECK(result->filename == "Makefile");
    CHECK(result->line_number == 30);
    CHECK(result->matched_text == "target: dependencies");
}

TEST_CASE("FileUtils::ParseGrepLine - filename with dots")
{
    auto result = FileUtils::ParseGrepLine("file.name.with.dots.cpp:5:code");
    REQUIRE(result.has_value());
    CHECK(result->filename == "file.name.with.dots.cpp");
    CHECK(result->line_number == 5);
    CHECK(result->matched_text == "code");
}

TEST_CASE("FileUtils::ParseGrepLine - matched text with special characters")
{
    auto result = FileUtils::ParseGrepLine("file.cpp:8:if (x > 0 && y < 100) { return true; }");
    REQUIRE(result.has_value());
    CHECK(result->filename == "file.cpp");
    CHECK(result->line_number == 8);
    CHECK(result->matched_text == "if (x > 0 && y < 100) { return true; }");
}

TEST_CASE("FileUtils::ParseGrepLine - D drive Windows path")
{
    auto result = FileUtils::ParseGrepLine("D:\\data\\logs\\app.log:1000:error message");
    REQUIRE(result.has_value());
    CHECK(result->filename == "D:\\data\\logs\\app.log");
    CHECK(result->line_number == 1000);
    CHECK(result->matched_text == "error message");
}

TEST_CASE("FileUtils::ParseGrepLine - single character filename")
{
    auto result = FileUtils::ParseGrepLine("a:1:x");
    REQUIRE(result.has_value());
    CHECK(result->filename == "a");
    CHECK(result->line_number == 1);
    CHECK(result->matched_text == "x");
}

TEST_CASE("FileUtils::ParseGrepLine - filename starting with dot (hidden file)")
{
    auto result = FileUtils::ParseGrepLine(".gitignore:5:*.o");
    REQUIRE(result.has_value());
    CHECK(result->filename == ".gitignore");
    CHECK(result->line_number == 5);
    CHECK(result->matched_text == "*.o");
}

TEST_CASE("FileUtils::ParseGrepLine - path with mixed slashes")
{
    auto result = FileUtils::ParseGrepLine("src\\utils/file.cpp:33:mixed slashes");
    REQUIRE(result.has_value());
    CHECK(result->filename == "src\\utils/file.cpp");
    CHECK(result->line_number == 33);
    CHECK(result->matched_text == "mixed slashes");
}

TEST_CASE("FileUtils::ParseGrepLine - real world example 1")
{
    // Typical grep output from a C++ project
    auto result = FileUtils::ParseGrepLine(
        "./src/editor/EditorFrame.cpp:456:    if (event.GetEventType() == wxEVT_CLOSE_WINDOW) {");
    REQUIRE(result.has_value());
    CHECK(result->filename == "./src/editor/EditorFrame.cpp");
    CHECK(result->line_number == 456);
    CHECK(result->matched_text == "    if (event.GetEventType() == wxEVT_CLOSE_WINDOW) {");
}

TEST_CASE("FileUtils::ParseGrepLine - real world example 2")
{
    // Grep output with context showing include statement
    auto result = FileUtils::ParseGrepLine("/usr/include/wx-3.0/wx/string.h:1234:#include <string>");
    REQUIRE(result.has_value());
    CHECK(result->filename == "/usr/include/wx-3.0/wx/string.h");
    CHECK(result->line_number == 1234);
    CHECK(result->matched_text == "#include <string>");
}

TEST_CASE("FileUtils::ParseGrepLine - real world example 3 Windows")
{
    // Windows grep output with namespace declaration
    auto result =
        FileUtils::ParseGrepLine("C:\\msys64\\home\\user\\project\\include\\utils.h:89:namespace FileUtils {");
    REQUIRE(result.has_value());
    CHECK(result->filename == "C:\\msys64\\home\\user\\project\\include\\utils.h");
    CHECK(result->line_number == 89);
    CHECK(result->matched_text == "namespace FileUtils {");
}

TEST_CASE("FileUtils::ParseGrepLine - colon at start of drive should not be filename")
{
    // Edge case: Something like ":C:" shouldn't be parsed as a drive
    auto result = FileUtils::ParseGrepLine(":C:something");
    CHECK_FALSE(result.has_value());
}

TEST_CASE("FileUtils::ParseGrepLine - just Windows drive letter with colons")
{
    auto result = FileUtils::ParseGrepLine("C::5:text");
    CHECK_FALSE(result.has_value());
}
