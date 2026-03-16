#include "Diff/clPatchApplier.hpp"
#include "clTempFile.hpp"
#include "cl_standard_paths.h"
#include "fileutils.h"
#include "test_patch_applier_assets.hpp"

#include <doctest.h>
#include <wx/arrstr.h>
#include <wx/string.h>
#include <wx/tokenzr.h>

struct MockTextArea : public ITextArea {
    MockTextArea(const wxString& content) { m_lines = wxStringTokenize(content, "\n", wxTOKEN_RET_EMPTY_ALL); }

    int GetLineCount() override { return m_lines.size(); }
    wxString GetLine(int line) override { return m_lines[line]; }
    int PositionFromLine([[maybe_unused]] int line) override { return 0; }
    void DeleteRange([[maybe_unused]] int start, [[maybe_unused]] int lengthDelete) override {}
    void InsertText([[maybe_unused]] int pos, [[maybe_unused]] const wxString& text) override {}

    wxArrayString m_lines;
};

TEST_CASE("PatchApplier - Multiple Hunks")
{
    auto patch = PatchApplier::ParseUnifiedPatch(patch_content_multiple_hunks1);
    CHECK_EQ(patch.hunks.size(), 4);

    MockTextArea ctrl{multiple_hunks1_source_file};
    int start_line(0);
    for (const auto& hunk : patch.hunks) {
        auto res = PatchApplier::ApplyHunk(&ctrl, hunk.lines, start_line);
        CHECK(res.ok());
        CHECK(res.value() > 0);
        start_line = res.value();
    }
}

TEST_CASE("ParseUnifiedPatchLoose - Custom Format Without Line Numbers")
{
    auto patch = PatchApplier::ParseUnifiedPatchLoose(loose_patch_custom_format);

    // Should parse one hunk
    REQUIRE_EQ(patch.hunks.size(), 1);

    const auto& hunk = patch.hunks[0];

    // Line numbers should be 0 (not parsed from @@)
    CHECK_EQ(hunk.originalStart, 0);
    CHECK_EQ(hunk.originalCount, 0);
    CHECK_EQ(hunk.newStart, 0);
    CHECK_EQ(hunk.newCount, 0);

    // Should have 3 deletions and 10 additions = 13 lines
    CHECK_EQ(hunk.lines.size(), 13);

    // Check that lines are properly categorized
    int deletions = 0, additions = 0;
    for (const auto& line : hunk.lines) {
        if (line.StartsWith("-")) {
            deletions++;
        } else if (line.StartsWith("+")) {
            additions++;
        }
    }
    CHECK_EQ(deletions, 3);
    CHECK_EQ(additions, 10);
}

TEST_CASE("ParseUnifiedPatchLoose - Standard Format With Line Numbers")
{
    auto patch = PatchApplier::ParseUnifiedPatchLoose(loose_patch_standard_format);

    REQUIRE_EQ(patch.hunks.size(), 1);

    const auto& hunk = patch.hunks[0];

    // Line numbers should be parsed
    CHECK_EQ(hunk.originalStart, 10);
    CHECK_EQ(hunk.originalCount, 3);
    CHECK_EQ(hunk.newStart, 10);
    CHECK_EQ(hunk.newCount, 7);

    // Should have 1 context, 1 deletion, 2 additions, 1 context = 5 lines
    CHECK_EQ(hunk.lines.size(), 5);

    // Verify first and last lines
    CHECK(hunk.lines[0].StartsWith(" ")); // Context line
    CHECK(hunk.lines[1].StartsWith("-")); // Deletion
    CHECK(hunk.lines[2].StartsWith("+")); // Addition
}

TEST_CASE("ParseUnifiedPatchLoose - Multiple Hunks")
{
    auto patch = PatchApplier::ParseUnifiedPatchLoose(loose_patch_multiple_hunks);

    // Should parse two hunks
    REQUIRE_EQ(patch.hunks.size(), 2);

    // First hunk: include statement change
    const auto& hunk1 = patch.hunks[0];
    CHECK_EQ(hunk1.lines.size(), 2);
    CHECK(hunk1.lines[0].StartsWith("-"));
    CHECK(hunk1.lines[1].StartsWith("+"));

    // Second hunk: function call change
    const auto& hunk2 = patch.hunks[1];
    CHECK_EQ(hunk2.lines.size(), 4);
    CHECK(hunk2.lines[0].StartsWith(" ")); // Context
    CHECK(hunk2.lines[1].StartsWith("-")); // Deletion
    CHECK(hunk2.lines[2].StartsWith("+")); // Addition
}

TEST_CASE("ParseUnifiedPatchLoose - Only Additions")
{
    auto patch = PatchApplier::ParseUnifiedPatchLoose(loose_patch_additions_only);

    REQUIRE_EQ(patch.hunks.size(), 1);

    const auto& hunk = patch.hunks[0];

    // All lines should be additions
    for (const auto& line : hunk.lines) {
        CHECK(line.StartsWith("+"));
    }

    CHECK_EQ(hunk.lines.size(), 6);
}

TEST_CASE("ParseUnifiedPatchLoose - Only Deletions")
{
    auto patch = PatchApplier::ParseUnifiedPatchLoose(loose_patch_deletions_only);

    REQUIRE_EQ(patch.hunks.size(), 1);

    const auto& hunk = patch.hunks[0];

    // All lines should be deletions
    for (const auto& line : hunk.lines) {
        CHECK(line.StartsWith("-"));
    }

    CHECK_EQ(hunk.lines.size(), 3);
}

TEST_CASE("ParseUnifiedPatchLoose - With Context Lines")
{
    auto patch = PatchApplier::ParseUnifiedPatchLoose(loose_patch_with_context);

    REQUIRE_EQ(patch.hunks.size(), 1);

    const auto& hunk = patch.hunks[0];

    // Check line numbers are parsed
    CHECK_EQ(hunk.originalStart, 1);
    CHECK_EQ(hunk.originalCount, 5);
    CHECK_EQ(hunk.newStart, 1);
    CHECK_EQ(hunk.newCount, 5);

    // Count context, deletions, and additions
    int context = 0, deletions = 0, additions = 0;
    for (const auto& line : hunk.lines) {
        if (line.StartsWith(" ")) {
            context++;
        } else if (line.StartsWith("-")) {
            deletions++;
        } else if (line.StartsWith("+")) {
            additions++;
        }
    }

    CHECK_EQ(context, 4); // #include, blank, int main, return 0
    CHECK_EQ(deletions, 1);
    CHECK_EQ(additions, 1);
}

TEST_CASE("ParseUnifiedPatchLoose - With Ignored Lines")
{
    auto patch = PatchApplier::ParseUnifiedPatchLoose(loose_patch_with_ignored_lines);

    // Should parse two hunks (ignored text between them)
    REQUIRE_EQ(patch.hunks.size(), 2);

    // First hunk
    const auto& hunk1 = patch.hunks[0];
    CHECK_EQ(hunk1.lines.size(), 2);
    CHECK(hunk1.lines[0].StartsWith("-"));
    CHECK(hunk1.lines[1].StartsWith("+"));

    // Second hunk
    const auto& hunk2 = patch.hunks[1];
    CHECK_EQ(hunk2.lines.size(), 3);
    CHECK(hunk2.lines[0].StartsWith(" "));
    CHECK(hunk2.lines[1].StartsWith("-"));
    CHECK(hunk2.lines[2].StartsWith("+"));
}

TEST_CASE("ParseUnifiedPatchLoose - No Hunks")
{
    auto patch = PatchApplier::ParseUnifiedPatchLoose(loose_patch_no_hunks);

    // Should have no hunks because there's no @@ marker
    CHECK_EQ(patch.hunks.size(), 0);
}

TEST_CASE("ParseUnifiedPatchLoose - With Empty Lines in Hunk")
{
    auto patch = PatchApplier::ParseUnifiedPatchLoose(loose_patch_with_empty_lines);

    REQUIRE_EQ(patch.hunks.size(), 1);

    const auto& hunk = patch.hunks[0];

    // Empty lines within hunks should be ignored (not added to lines array)
    // Should have: context, deletion, addition, context = 4 lines
    CHECK_EQ(hunk.lines.size(), 4);

    // Verify the lines
    CHECK(hunk.lines[0].StartsWith(" ")); // void test() {
    CHECK(hunk.lines[1].StartsWith("-")); // line1();
    CHECK(hunk.lines[2].StartsWith("+")); // line2();
    CHECK(hunk.lines[3].StartsWith(" ")); // }
}

TEST_CASE("ParseUnifiedPatchLoose - Minimal Patch")
{
    auto patch = PatchApplier::ParseUnifiedPatchLoose(loose_patch_minimal);

    REQUIRE_EQ(patch.hunks.size(), 1);

    const auto& hunk = patch.hunks[0];

    // No line numbers parsed
    CHECK_EQ(hunk.originalStart, 0);
    CHECK_EQ(hunk.originalCount, 0);

    // Should have 1 deletion and 1 addition
    CHECK_EQ(hunk.lines.size(), 2);
    CHECK(hunk.lines[0].StartsWith("-"));
    CHECK(hunk.lines[1].StartsWith("+"));
}

TEST_CASE("ParseUnifiedPatchLoose - Empty Patch")
{
    auto patch = PatchApplier::ParseUnifiedPatchLoose("");

    // Empty patch should result in no hunks
    CHECK_EQ(patch.hunks.size(), 0);
}

TEST_CASE("ParseUnifiedPatchLoose - Whitespace Only Patch")
{
    auto patch = PatchApplier::ParseUnifiedPatchLoose("   \n\n   \n");

    // Whitespace-only patch should result in no hunks
    CHECK_EQ(patch.hunks.size(), 0);
}
