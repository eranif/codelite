#include "MarkdownStyler.hpp"

#include <doctest.h>

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
