#include <doctest.h>

// clang-format off
#include "MarkdownStyler.hpp"
#include "neseted_diff.h"
// clang-format on

class MockAccessor final : public AccessorBase
{
public:
    explicit MockAccessor(wxString text)
        : m_text(std::move(text))
    {
    }

    wxString GetSubstr(size_t count) const override { return m_text.Mid(m_pos, count); }

    bool StartsWith(std::vector<int> chars) const override
    {
        if (m_pos + chars.size() > m_text.length()) {
            return false;
        }
        for (size_t i = 0; i < chars.size(); ++i) {
            if (static_cast<unsigned char>(m_text[m_pos + i]) != static_cast<unsigned char>(chars[i])) {
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
                if (static_cast<unsigned char>(m_text[i + j]) != static_cast<unsigned char>(chars[j])) {
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
    int GetCurrentCharAsInt() const override { return CanNext() ? static_cast<unsigned char>(m_text[m_pos]) : 0; }
    int GetCharAtAsInt(size_t at) const override
    {
        return (m_pos + at < m_text.length()) ? static_cast<unsigned char>(m_text[m_pos + at]) : 0;
    }

private:
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
    wxString text = kNestedDiffSample;
    MockAccessor accessor{text};
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
