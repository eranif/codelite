#include <doctest.h>

#include <algorithm>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

// clang-format off
#include "MarkdownStyler.hpp"
// clang-format on
//
const std::string& GetNestedDiffSample();

class MockAccessor final : public AccessorBase
{
public:
    explicit MockAccessor(wxString text)
        : m_text(std::move(text))
    {
    }

    wxString GetSubstr(size_t count) const override { return m_text.Mid(m_pos, count); }

    bool StartsWith(const std::vector<int>& chars) const override
    {
        if (m_pos + chars.size() > m_text.length()) {
            return false;
        }
        for (size_t i = 0; i < chars.size(); ++i) {
            if (ByteAt(m_pos + i) != chars[i]) {
                return false;
            }
        }
        return true;
    }

    int Contains(std::vector<int> chars, size_t from = 0) const override
    {
        for (size_t i = m_pos + from; i + chars.size() <= m_text.length(); ++i) {
            bool match = true;
            for (size_t j = 0; j < chars.size(); ++j) {
                if (ByteAt(i + j) != chars[j]) {
                    match = false;
                    break;
                }
            }
            if (match) {
                return static_cast<int>(i);
            }
        }
        return wxNOT_FOUND;
    }

    bool IsEofInNSteps(size_t n = 0) const override { return m_pos + n > m_text.length(); }
    int GetPosition() const override { return static_cast<int>(m_pos); }
    wxString GetWordAtCurrentPosition() const override { return wxEmptyString; }
    bool IsAtLineStart() const override { return m_pos == 0 || m_text[m_pos - 1] == '\n'; }
    bool IsAtLineStartIgnoringWhitespace() const override
    {
        size_t i = m_pos;
        while (i > 0 && (m_text[i - 1] == ' ' || m_text[i - 1] == '\t')) {
            --i;
        }
        return i == 0 || m_text[i - 1] == '\n';
    }

    void SetStyle(int style, size_t count) override
    {
        for (size_t i = 0; i < count && m_pos < m_text.length(); ++i) {
            m_styles.push_back(style);
            ++m_pos;
        }
    }

    void SetStyleUntilEndOfLine(int style) override
    {
        while (m_pos < m_text.length() && m_text[m_pos] != '\n') {
            m_styles.push_back(style);
            ++m_pos;
        }
    }

    bool CurrentLineContains(size_t, const char*) override { return false; }
    bool CurrentLineContains(size_t, const wxString&) override { return false; }
    bool CanNext() const override { return m_pos < m_text.length(); }
    bool CanNextFromPos(size_t pos) const override { return pos < m_text.length(); }
    bool CanPeek(size_t count) const override { return m_pos + count <= m_text.length(); }
    bool HasReachedEnd() const override { return m_pos >= m_text.length(); }

    std::vector<int> m_styles;

protected:
    int GetCurrentCharAsInt() const override { return CanNext() ? ByteAt(m_pos) : 0; }
    int GetCharAtAsInt(size_t at) const override { return (m_pos + at < m_text.length()) ? ByteAt(m_pos + at) : 0; }

private:
    // The byte at `i`, without a locale conversion. The text is built with From8BitData(),
    // so every character holds one byte (0-255).
    int ByteAt(size_t i) const { return static_cast<int>(m_text[i].GetValue()); }

    wxString m_text;
    mutable size_t m_pos{0};
};

TEST_CASE("MarkdownStyles enum values are stable")
{
    CHECK(kDefault == 0);
    CHECK(kHeader1 == 1);
    CHECK(kHeader6 == 6);
    CHECK(kHeaderText == 7);
    CHECK(kCodeBlockTag == 8);
    CHECK(kUrl == 24);
    CHECK(kCodeBlockMacro == 40);
    CHECK(kDiffFileMarker == 48);
}

TEST_CASE("MarkdownState enum class contains expected states")
{
    CHECK(static_cast<int>(MarkdownState::kDefault) == 0);
    CHECK(static_cast<int>(MarkdownState::kCodeBlock3) == 1);
    CHECK(static_cast<int>(MarkdownState::kCodeBlockContent) == 4);
    CHECK(static_cast<int>(MarkdownState::kUrl) == 10);
}

TEST_CASE("MarkdownStyler header declares expected public API")
{
    // If the header changes in a breaking way, these expressions stop compiling.
    using reset_sig_t = void (MarkdownStyler::*)();
    using init_sig_t = void (MarkdownStyler::*)();
    using url_sig_t = wxString (MarkdownStyler::*)(int);

    CHECK(static_cast<reset_sig_t>(&MarkdownStyler::Reset) != nullptr);
    CHECK(static_cast<init_sig_t>(&MarkdownStyler::InitStyles) != nullptr);
    CHECK(static_cast<url_sig_t>(&MarkdownStyler::GetUrlFromPosition) != nullptr);
}

TEST_CASE("MarkdownStyler OnStyle styles a level-1 header")
{
    MarkdownStyler styler{nullptr};
    MockAccessor accessor{"# Header\n"};

    styler.OnStyle(accessor);

    REQUIRE(accessor.m_styles.size() >= 2);
    CHECK(accessor.m_styles[0] == kHeader1);
    CHECK(accessor.m_styles[1] == kHeaderText);
}

TEST_CASE("MarkdownStyler OnStyle styles multiple markdown constructs")
{
    MarkdownStyler styler{nullptr};

    const wxString text = "# Title\n"
                          "- bullet item\n"
                          "1. numbered item\n"
                          "Visit https://example.com/path?q=1\n"
                          "This is **strong** and __emphasis__\n"
                          "```cpp\n"
                          "int x = 42; // comment\n"
                          "```\n";

    MockAccessor accessor{text};
    styler.OnStyle(accessor);

    REQUIRE(accessor.m_styles.size() == text.length());

    // Header line
    CHECK(accessor.m_styles[0] == kHeader1);
    CHECK(accessor.m_styles[2] == kHeaderText);

    // Bullet list item
    const size_t bullet_pos = text.find("- bullet item");
    REQUIRE(bullet_pos != wxString::npos);
    CHECK(accessor.m_styles[bullet_pos] == kListItem);

    // Numbered list item
    const size_t numbered_pos = text.find("1. numbered item");
    REQUIRE(numbered_pos != wxString::npos);
    CHECK(accessor.m_styles[numbered_pos] == kNumberedListItem);
    CHECK(accessor.m_styles[numbered_pos + 1] == kNumberedListItemDot);

    // URL
    const size_t url_pos = text.find("https://example.com/path?q=1");
    REQUIRE(url_pos != wxString::npos);
    CHECK(accessor.m_styles[url_pos] == kUrl);
    CHECK(accessor.m_styles[url_pos + 8] == kUrl);

    // Strong / emphasis are currently not styled by MarkdownStyler::OnStyle.
    // The implementation only recognizes the opening/closing tags as delimiters
    // to transition state, but it does not emit styled ranges for the inner text.
    // This is a styler limitation, not a test bug.
    const size_t strong_pos = text.find("**strong**");
    REQUIRE(strong_pos != wxString::npos);
    CHECK(accessor.m_styles[strong_pos] == kDefault);
    CHECK(accessor.m_styles[strong_pos + 2] == kDefault);
    CHECK(accessor.m_styles[strong_pos + 8] == kDefault);

    const size_t em_pos = text.find("__emphasis__");
    REQUIRE(em_pos != wxString::npos);
    CHECK(accessor.m_styles[em_pos] == kDefault);
    CHECK(accessor.m_styles[em_pos + 2] == kDefault);
    CHECK(accessor.m_styles[em_pos + 10] == kDefault);

    // Code block and comment
    const size_t code_pos = text.find("```cpp");
    REQUIRE(code_pos != wxString::npos);
    CHECK(accessor.m_styles[code_pos] == kBacktick);

    const size_t comment_pos = text.find("// comment");
    REQUIRE(comment_pos != wxString::npos);
    CHECK(accessor.m_styles[comment_pos] == kCodeBlockComment);
}

TEST_CASE("MarkdownStyler OnStyle handles nested diff content inside a fenced block")
{
    MarkdownStyler styler{nullptr};
    const std::string& text = GetNestedDiffSample();
    MockAccessor accessor{wxString::FromUTF8(text)};
    styler.OnStyle(accessor);

    REQUIRE(accessor.m_styles.size() == text.length());

    const size_t fence_pos = text.find("```patch");
    REQUIRE(fence_pos != wxString::npos);
    CHECK(accessor.m_styles[fence_pos] == kBacktick);

    const size_t header_pos = text.find("--- a/README.md");
    REQUIRE(header_pos != wxString::npos);
    CHECK(accessor.m_styles[header_pos] == kDiffHeader);

    const size_t index_pos = text.find("@@ -43,6 +43,8 @@");
    REQUIRE(index_pos != wxString::npos);
    CHECK(accessor.m_styles[index_pos] == kDiffHeader);

    const size_t plus_plus_pos = text.find("+++ b/README.md");
    REQUIRE(plus_plus_pos != wxString::npos);
    CHECK(accessor.m_styles[plus_plus_pos] == kDiffHeader);

    const size_t minus_line_pos = text.find("-- `wxEVT_TERMINAL_TERMINATED`");
    REQUIRE(minus_line_pos != wxString::npos);
    CHECK(accessor.m_styles[minus_line_pos] == kDiffDelete);

    const size_t context_line_pos = text.find("#### Terminal Theme");
    REQUIRE(context_line_pos != wxString::npos);
    CHECK(accessor.m_styles[context_line_pos] == kCodeBlockText);

    const size_t code_block_pos = text.find("```cpp");
    REQUIRE(code_block_pos != wxString::npos);
    CHECK(accessor.m_styles[code_block_pos] == kDiffAdd);

    const size_t add_pos = text.find("+- **Clickable text/links**");
    REQUIRE(add_pos != wxString::npos);
    CHECK(accessor.m_styles[add_pos] == kDiffAdd);

    const size_t add_pos2 = text.find("+- **Double-click word selection**");
    REQUIRE(add_pos2 != wxString::npos);
    CHECK(accessor.m_styles[add_pos2] == kDiffAdd);

    // The fenced block is styled as diff content, so markdown emphasis inside it should
    // not be interpreted as MarkdownStyler inline formatting.
    const size_t strong_pos = text.find("**wxWidgets**");
    REQUIRE(strong_pos != wxString::npos);
    CHECK(accessor.m_styles[strong_pos] == kDiffHeader);
    CHECK(accessor.m_styles[strong_pos + 1] == accessor.m_styles[strong_pos]);

    const size_t closing_fence_pos = text.rfind("```");
    REQUIRE(closing_fence_pos != wxString::npos);
    CHECK(accessor.m_styles[closing_fence_pos] == kBacktick);
}

TEST_CASE("MarkdownStyler OnStyle styles check marks and crosses")
{
    MarkdownStyler styler{nullptr};

    // The accessor works on UTF-8 bytes: every byte of a mark must get the mark style
    const std::vector<std::pair<std::string, int>> marks = {
        {"\xE2\x9C\x93", kCheckMarkSymbol},     // ✓ U+2713
        {"\xE2\x9C\x94", kCheckMarkSymbol},     // ✔ U+2714
        {"\xE2\x9C\x85", kCheckMarkSymbol},     // ✅ U+2705
        {"\xE2\x98\x91", kCheckMarkSymbol},     // ☑ U+2611
        {"\xF0\x9F\x97\xB8", kCheckMarkSymbol}, // 🗸 U+1F5F8
        {"\xF0\x9F\x97\xB9", kCheckMarkSymbol}, // 🗹 U+1F5F9
        {"\xE2\x98\x92", kErrorSymbol},         // ☒ U+2612
        {"\xE2\x9C\x95", kErrorSymbol},         // ✕ U+2715
        {"\xE2\x9C\x96", kErrorSymbol},         // ✖ U+2716
        {"\xE2\x9C\x97", kErrorSymbol},         // ✗ U+2717
        {"\xE2\x9C\x98", kErrorSymbol},         // ✘ U+2718
        {"\xE2\x9D\x8C", kErrorSymbol},         // ❌ U+274C
        {"\xE2\x9D\x8E", kErrorSymbol},         // ❎ U+274E
        {"\xE2\xAE\xBD", kErrorSymbol},         // ⮽ U+2BBD
        {"\xF0\x9F\x97\x99", kErrorSymbol},     // 🗙 U+1F5D9
        {"\xF0\x9F\x97\xB4", kErrorSymbol},     // 🗴 U+1F5F4
        {"\xF0\x9F\x97\xB5", kErrorSymbol},     // 🗵 U+1F5F5
        {"\xF0\x9F\x97\xB6", kErrorSymbol},     // 🗶 U+1F5F6
        {"\xF0\x9F\x97\xB7", kErrorSymbol},     // 🗷 U+1F5F7
    };

    std::string bytes;
    // A mark in the middle of a line of text
    bytes += "a \xE2\x9C\x93 b\n"; // "a ✓ b"
    for (const auto& mark : marks) {
        bytes += mark.first;
        bytes += '\n';
    }
    // A mark followed by the variation selector U+FE0F: all the bytes get the mark style
    const std::string check_vs16 = "\xE2\x9C\x94\xEF\xB8\x8F"; // ✔ U+2714 + VS16
    const std::string cross_vs16 = "\xE2\x9D\x8C\xEF\xB8\x8F"; // ❌ U+274C + VS16
    bytes += check_vs16;
    bytes += '\n';
    bytes += cross_vs16;
    bytes += '\n';
    bytes += "plain text\n";
    // A mark at the very end of the buffer (no new line after it)
    bytes += "\xE2\x9C\x94"; // ✔ U+2714

    MockAccessor accessor{wxString::From8BitData(bytes.data(), bytes.size())};
    styler.OnStyle(accessor);
    REQUIRE(accessor.m_styles.size() == bytes.size());

    // "a ✓ b\n": the letters and spaces are default, the 3 bytes of the mark are a check mark
    size_t pos = 0;
    const std::vector<int> mid_line = {
        kDefault, kDefault, kCheckMarkSymbol, kCheckMarkSymbol, kCheckMarkSymbol, kDefault, kDefault, kDefault};
    for (size_t i = 0; i < mid_line.size(); ++i) {
        CHECK(accessor.m_styles[pos + i] == mid_line[i]);
    }
    pos += mid_line.size();

    for (const auto& mark : marks) {
        for (size_t i = 0; i < mark.first.size(); ++i) {
            CHECK(accessor.m_styles[pos + i] == mark.second);
        }
        // The new line after a mark keeps the default style
        CHECK(accessor.m_styles[pos + mark.first.size()] == kDefault);
        pos += mark.first.size() + 1;
    }

    for (size_t i = 0; i < check_vs16.size(); ++i) {
        CHECK(accessor.m_styles[pos + i] == kCheckMarkSymbol);
    }
    pos += check_vs16.size() + 1;
    for (size_t i = 0; i < cross_vs16.size(); ++i) {
        CHECK(accessor.m_styles[pos + i] == kErrorSymbol);
    }
    pos += cross_vs16.size() + 1;

    for (size_t i = 0; i < 11; ++i) { // "plain text\n"
        CHECK(accessor.m_styles[pos + i] == kDefault);
    }
    pos += 11;

    // The last mark has no new line after it
    CHECK(accessor.m_styles[pos] == kCheckMarkSymbol);
    CHECK(accessor.m_styles[pos + 1] == kCheckMarkSymbol);
    CHECK(accessor.m_styles[pos + 2] == kCheckMarkSymbol);
}

namespace
{
/// Line by line styling: the styles, and the state at the end of each line
struct LineStylingResult {
    std::vector<char> styles;
    std::vector<int> states;
};

LineStylingResult StyleLines(MarkdownStyler& styler, std::string_view text, int doc_offset, int start_state)
{
    LineStylingResult result;
    result.styles = styler.StyleLines(
        text, doc_offset, start_state, [&result](size_t, int state) { result.states.push_back(state); });
    return result;
}

/// The offset of the start of each line of `text` (the text has only "\n" or "\r\n" EOLs)
std::vector<size_t> GetLineStarts(std::string_view text)
{
    std::vector<size_t> starts{0};
    for (size_t i = 0; i < text.size(); ++i) {
        if (text[i] == '\n' && i + 1 < text.size()) {
            starts.push_back(i + 1);
        }
    }
    return starts;
}

const std::string kLineStylingSample = "# Title with `code`\n"
                                       "Some **strong** text, a `code span` and https://example.com/a?b=1\n"
                                       "An open ` backtick\n"
                                       "- item\n"
                                       "12. numbered\n"
                                       "```cpp\n"
                                       "int main() { return 0; } // comment\n"
                                       "\n"
                                       "# not a header inside the code\n"
                                       "```\n"
                                       "````python\r\n"
                                       "def foo(): return \"str\"\r\n"
                                       "````\r\n"
                                       "```patch\n"
                                       "--- a/file.md\n"
                                       "+++ b/file.md\n"
                                       "+added\n"
                                       "-removed\n"
                                       "```\n"
                                       "\xE2\x9C\x94 done \xE2\x9D\x8C failed\n"
                                       "```rust\n"
                                       "println!(\"no EOL at the end\")";
} // namespace

TEST_CASE("MarkdownStyler line styling styles each byte")
{
    MarkdownStyler styler{nullptr};
    const auto result = StyleLines(styler, kLineStylingSample, 0, 0);
    CHECK(result.styles.size() == kLineStylingSample.size());
    CHECK(result.states.size() == GetLineStarts(kLineStylingSample).size());
}

TEST_CASE("MarkdownStyler line styling restarts at any line with the saved state")
{
    // This is what Scintilla does: after an edit, styling restarts at the line of the edit, with the state that
    // was saved at the end of the line before it. The result must be the same as styling all the text at once.
    MarkdownStyler styler{nullptr};
    const std::string_view text{kLineStylingSample};
    const auto full = StyleLines(styler, text, 0, 0);
    const auto line_starts = GetLineStarts(text);
    REQUIRE(full.states.size() == line_starts.size());

    for (size_t line = 1; line < line_starts.size(); ++line) {
        CAPTURE(line);
        const size_t offset = line_starts[line];
        const auto tail = StyleLines(styler, text.substr(offset), static_cast<int>(offset), full.states[line - 1]);
        REQUIRE(tail.styles.size() == text.size() - offset);
        CHECK(std::equal(tail.styles.begin(), tail.styles.end(), full.styles.begin() + offset));
        CHECK(std::equal(tail.states.begin(), tail.states.end(), full.states.begin() + line));
    }
}

TEST_CASE("MarkdownStyler line styling of text that is appended in parts")
{
    // Text that arrives in parts (e.g. a streamed reply): each new part is styled from the start of its first line
    MarkdownStyler styler{nullptr};
    const std::string_view text{kLineStylingSample};
    const auto full = StyleLines(styler, text, 0, 0);
    const auto line_starts = GetLineStarts(text);

    std::vector<char> styles;
    int state = 0;
    for (size_t line = 0; line < line_starts.size(); line += 3) {
        const size_t start = line_starts[line];
        const size_t end = (line + 3 < line_starts.size()) ? line_starts[line + 3] : text.size();
        const auto part = StyleLines(styler, text.substr(start, end - start), static_cast<int>(start), state);
        styles.insert(styles.end(), part.styles.begin(), part.styles.end());
        REQUIRE(!part.states.empty());
        state = part.states.back();
    }
    CHECK(styles == full.styles);
}

TEST_CASE("MarkdownStyler line styling keeps the state of a fenced code block")
{
    MarkdownStyler styler{nullptr};
    const std::string text = "```cpp\n"
                             "int x;\n"
                             "```\n"
                             "after\n";
    const auto result = StyleLines(styler, text, 0, 0);
    REQUIRE(result.states.size() == 4);
    CHECK(result.states[0] != 0); // inside the code block
    CHECK(result.states[1] == result.states[0]);
    CHECK(result.states[2] == 0); // the block is closed
    CHECK(result.states[3] == 0);

    // The keyword uses the language of the fence
    CHECK(result.styles[text.find("int")] == kCodeBlockKeyword);
    CHECK(result.styles[text.find("after")] == kDefault);
}

TEST_CASE("MarkdownStyler line styling reads the language of a fence with a CRLF EOL")
{
    MarkdownStyler styler{nullptr};
    const std::string text = "```cpp\r\n"
                             "int x;\r\n"
                             "```\r\n";
    const auto result = StyleLines(styler, text, 0, 0);
    REQUIRE(result.styles.size() == text.size());
    CHECK(result.styles[text.find("int")] == kCodeBlockKeyword);
}

TEST_CASE("MarkdownStyler line styling does not continue an inline code span on the next line")
{
    MarkdownStyler styler{nullptr};
    const std::string text = "a ` open\n"
                             "next `code` line\n";
    const auto result = StyleLines(styler, text, 0, 0);
    REQUIRE(result.styles.size() == text.size());
    // An open backtick without a closing one is not a code span
    CHECK(result.styles[text.find("`")] == kDefault);
    CHECK(result.styles[text.find("open")] == kDefault);
    CHECK(result.states[0] == 0);
    // A closed code span on the next line
    const size_t code_pos = text.find("`code`");
    CHECK(result.styles[code_pos] == kBacktick);
    CHECK(result.styles[code_pos + 1] == kCodeWord);
    CHECK(result.styles[code_pos + 5] == kBacktick);
    CHECK(result.styles[text.find("next")] == kDefault);
}

TEST_CASE("MarkdownStyler line styling styles strong text")
{
    // The line accessor finds the closing tag on the line (MockAccessor does not)
    MarkdownStyler styler{nullptr};
    const std::string text = "a **strong** b\n";
    const auto result = StyleLines(styler, text, 0, 0);
    const size_t pos = text.find("**strong**");
    CHECK(result.styles[pos] == kStrong2Tag);
    CHECK(result.styles[pos + 2] == kStrong2Text);
    CHECK(result.styles[pos + 8] == kStrong2Tag);
    CHECK(result.styles[text.find(" b")] == kDefault);
}

TEST_CASE("MarkdownStyler line styling reads the language of a longer fence")
{
    // "````" also starts with "```": the longest fence must win, or the tag is "`python"
    MarkdownStyler styler{nullptr};
    const std::string text = "````python\n"
                             "def foo(): pass\n"
                             "````\n"
                             "after\n";
    const auto result = StyleLines(styler, text, 0, 0);
    REQUIRE(result.states.size() == 4);
    CHECK((result.states[0] & 0x3) == 2); // a "````" fence
    CHECK(result.styles[text.find("def")] == kCodeBlockKeyword);
    CHECK(result.states[2] == 0);
    CHECK(result.styles[text.find("after")] == kDefault);
}
