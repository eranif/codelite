#include "Diff/clPatchApplier.hpp"
#include "clTempFile.hpp"
#include "cl_standard_paths.h"
#include "fileutils.h"
#include "test_patch_applier_assets.hpp"

#include <doctest.h>
#include <wx/arrstr.h>
#include <wx/string.h>
#include <wx/tokenzr.h>

// Enhanced MockTextArea that supports actual modifications
struct EditableMockTextArea : public ITextArea {
    EditableMockTextArea(const wxString& content) { m_lines = wxStringTokenize(content, "\n", wxTOKEN_RET_EMPTY_ALL); }

    int GetLineCount() override { return m_lines.size(); }

    wxString GetLine(int line) override
    {
        if (line >= 0 && line < static_cast<int>(m_lines.size())) {
            wxString result = m_lines[line];
            if (!result.EndsWith("\n")) {
                result += "\n";
            }
            return result;
        }
        return "";
    }

    int PositionFromLine(int line) override
    {
        int pos = 0;
        for (int i = 0; i < line && i < static_cast<int>(m_lines.size()); ++i) {
            pos += m_lines[i].Length() + 1; // +1 for newline
        }
        return pos;
    }

    void DeleteRange(int start, int lengthDelete) override
    {
        // Find which line contains the start position
        int currentPos = 0;
        for (int i = 0; i < static_cast<int>(m_lines.size()); ++i) {
            int lineLength = m_lines[i].Length() + 1; // +1 for newline
            if (currentPos == start && lengthDelete == lineLength) {
                // Delete entire line
                m_lines.RemoveAt(i);
                return;
            }
            currentPos += lineLength;
        }
    }

    void InsertText(int pos, const wxString& text) override
    {
        // Find which line contains the position
        int currentPos = 0;
        for (int i = 0; i < static_cast<int>(m_lines.size()); ++i) {
            int lineLength = m_lines[i].Length() + 1;
            if (currentPos == pos) {
                // Insert before this line
                wxString textToInsert = text;
                textToInsert.Trim(true); // Remove trailing whitespace including newline
                m_lines.Insert(textToInsert, i);
                return;
            }
            currentPos += lineLength;
        }
        // If position is at the end, append
        if (currentPos == pos) {
            wxString textToInsert = text;
            textToInsert.Trim(true);
            m_lines.Add(textToInsert);
        }
    }

    wxArrayString m_lines;
};

struct MockTextArea : public ITextArea {
    MockTextArea(const wxString& content) { m_lines = wxStringTokenize(content, "\n", wxTOKEN_RET_EMPTY_ALL); }

    int GetLineCount() override { return m_lines.size(); }
    wxString GetLine(int line) override { return m_lines[line]; }
    int PositionFromLine([[maybe_unused]] int line) override { return line * 10; }
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
    int deletions = 0, additions = 0, context = 0;
    for (const auto& line : hunk.lines) {
        if (line.StartsWith("-")) {
            deletions++;
        } else if (line.StartsWith("+")) {
            additions++;
        } else if (line.StartsWith(" ")) {
            context++;
        }
    }
    CHECK_EQ(deletions, 3);
    CHECK_EQ(additions, 10);
    CHECK_EQ(context, 0);
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

// ============================================================================
// ApplyHunkLoose Tests
// ============================================================================

TEST_CASE("ApplyHunkLoose - Simple Match at Beginning")
{
    wxString source = R"(#include <iostream>
int main() {
    printf("Hello");
    return 0;
})";

    EditableMockTextArea ctrl{source};

    wxArrayString hunkLines;
    hunkLines.Add(" #include <iostream>");
    hunkLines.Add("-int main() {");
    hunkLines.Add("+int main(int argc, char** argv) {");

    auto result = PatchApplier::ApplyHunkLoose(&ctrl, hunkLines, 0);

    REQUIRE(result.ok());
    CHECK_EQ(result.value(), 2); // Should return line after modifications

    // Verify the change was applied
    wxString line1 = ctrl.GetLine(1);
    line1.Trim();
    CHECK(line1.Contains("int main(int argc, char** argv) {"));
}

TEST_CASE("ApplyHunkLoose - Match Found in Middle of File")
{
    wxString source = R"(#include <stdio.h>
#include <stdlib.h>

void helper() {
    printf("Helper");
}

int main() {
    printf("Old");
    return 0;
})";

    EditableMockTextArea ctrl{source};

    wxArrayString hunkLines;
    hunkLines.Add(" int main() {");
    hunkLines.Add("-    printf(\"Old\");");
    hunkLines.Add("+    printf(\"New\");");

    auto result = PatchApplier::ApplyHunkLoose(&ctrl, hunkLines, 0);

    REQUIRE(result.ok());

    // Verify the change was applied - should find it at line 7
    wxString line8 = ctrl.GetLine(8);
    line8.Trim();
    CHECK(line8.Contains("printf(\"New\");"));
}

TEST_CASE("ApplyHunkLoose - Multiple Context Lines")
{
    wxString source = R"(void test() {
    line1();
    line2();
    line3();
    line4();
})";

    EditableMockTextArea ctrl{source};

    wxArrayString hunkLines;
    hunkLines.Add(" void test() {");
    hunkLines.Add("     line1();");
    hunkLines.Add("-    line2();");
    hunkLines.Add("+    line2_modified();");
    hunkLines.Add("     line3();");

    auto result = PatchApplier::ApplyHunkLoose(&ctrl, hunkLines, 0);

    REQUIRE(result.ok());

    // Verify the change
    wxString line2 = ctrl.GetLine(2);
    line2.Trim();
    CHECK(line2.Contains("line2_modified();"));
}

TEST_CASE("ApplyHunkLoose - Addition Only at Found Location")
{
    wxString source = R"(#include <stdio.h>

int main() {
    return 0;
})";

    EditableMockTextArea ctrl{source};

    wxArrayString hunkLines;
    hunkLines.Add(" #include <stdio.h>");
    hunkLines.Add("+#include <stdlib.h>");

    auto result = PatchApplier::ApplyHunkLoose(&ctrl, hunkLines, 0);

    REQUIRE(result.ok());

    // Should have inserted the new line
    CHECK_EQ(ctrl.GetLineCount(), 6);
    wxString line1 = ctrl.GetLine(1);
    line1.Trim();
    CHECK(line1.Contains("#include <stdlib.h>"));
}

TEST_CASE("ApplyHunkLoose - Deletion at Found Location")
{
    wxString source = R"(#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
})";

    EditableMockTextArea ctrl{source};

    wxArrayString hunkLines;
    hunkLines.Add(" #include <stdio.h>");
    hunkLines.Add("-#include <stdlib.h>");
    hunkLines.Add(" #include <string.h>");

    auto result = PatchApplier::ApplyHunkLoose(&ctrl, hunkLines, 0);

    REQUIRE(result.ok());

    // Should have deleted the middle line
    CHECK_EQ(ctrl.GetLineCount(), 5);
}

TEST_CASE("ApplyHunkLoose - No Match Found")
{
    wxString source = R"(#include <stdio.h>
int main() {
    return 0;
})";

    EditableMockTextArea ctrl{source};

    wxArrayString hunkLines;
    hunkLines.Add(" void nonexistent() {");
    hunkLines.Add("-    oldcode();");
    hunkLines.Add("+    newcode();");

    auto result = PatchApplier::ApplyHunkLoose(&ctrl, hunkLines, 0);

    REQUIRE_FALSE(result.ok());
    CHECK(result.error_message().Contains("Could not find matching lines"));
}

TEST_CASE("ApplyHunkLoose - Partial Match Should Continue Searching")
{
    wxString source = R"(void test1() {
    line1();
    line2();
}

void test2() {
    line1();
    line3();
})";

    EditableMockTextArea ctrl{source};

    wxArrayString hunkLines;
    hunkLines.Add("     line1();");
    hunkLines.Add("-    line3();");
    hunkLines.Add("+    line3_modified();");

    auto result = PatchApplier::ApplyHunkLoose(&ctrl, hunkLines, 0);

    REQUIRE(result.ok());

    // Should skip test1 and match in test2
    wxString line7 = ctrl.GetLine(7);
    line7.Trim();
    CHECK(line7.Contains("line3_modified();"));
}

TEST_CASE("ApplyHunkLoose - Start From Specific Line")
{
    wxString source = R"(void test1() {
    printf("test");
}

void test2() {
    printf("test");
})";

    EditableMockTextArea ctrl{source};

    wxArrayString hunkLines;
    hunkLines.Add("-    printf(\"test\");");
    hunkLines.Add("+    printf(\"modified\");");

    // Start search from line 4, should find second occurrence
    auto result = PatchApplier::ApplyHunkLoose(&ctrl, hunkLines, 4);

    REQUIRE(result.ok());

    // First occurrence should be unchanged
    wxString line1 = ctrl.GetLine(1);
    line1.Trim();
    CHECK(line1.Contains("printf(\"test\");"));

    // Second occurrence should be modified
    wxString line5 = ctrl.GetLine(5);
    line5.Trim();
    CHECK(line5.Contains("printf(\"modified\");"));
}

TEST_CASE("ApplyHunkLoose - Invalid Arguments - Empty Hunk")
{
    wxString source = R"(int main() {
    return 0;
})";

    EditableMockTextArea ctrl{source};
    wxArrayString emptyLines;

    auto result = PatchApplier::ApplyHunkLoose(&ctrl, emptyLines, 0);

    REQUIRE_FALSE(result.ok());
    CHECK(result.error_message().Contains("Empty hunk"));
}

TEST_CASE("ApplyHunkLoose - Invalid Arguments - Null Control")
{
    wxArrayString hunkLines;
    hunkLines.Add(" test");

    auto result = PatchApplier::ApplyHunkLoose(nullptr, hunkLines, 0);

    REQUIRE_FALSE(result.ok());
    CHECK(result.error_message().Contains("Empty hunk"));
}

TEST_CASE("ApplyHunkLoose - Invalid Arguments - Out of Range Start Line")
{
    wxString source = R"(int main() {
    return 0;
})";

    EditableMockTextArea ctrl{source};

    wxArrayString hunkLines;
    hunkLines.Add(" test");

    auto result = PatchApplier::ApplyHunkLoose(&ctrl, hunkLines, 100);

    REQUIRE_FALSE(result.ok());
    CHECK(result.error_message().Contains("Not enough lines"));
}

TEST_CASE("ApplyHunkLoose - Only Additions - Should Fail")
{
    wxString source = R"(int main() {
    return 0;
})";

    EditableMockTextArea ctrl{source};

    wxArrayString hunkLines;
    hunkLines.Add("+new line 1");
    hunkLines.Add("+new line 2");

    auto result = PatchApplier::ApplyHunkLoose(&ctrl, hunkLines, 0);

    REQUIRE_FALSE(result.ok());
    CHECK(result.error_message().Contains("no context or deletion lines"));
}

TEST_CASE("ApplyHunkLoose - Invalid Line Prefix")
{
    wxString source = R"(int main() {
    return 0;
})";

    EditableMockTextArea ctrl{source};

    wxArrayString hunkLines;
    hunkLines.Add(" int main() {");
    hunkLines.Add("*invalid prefix");

    auto result = PatchApplier::ApplyHunkLoose(&ctrl, hunkLines, 0);

    REQUIRE_FALSE(result.ok());
    CHECK(result.error_message().Contains("must start with"));
}

TEST_CASE("ApplyHunkLoose - Match at End of File")
{
    wxString source = R"(void test1() {
}

void test2() {
    return;
})";

    EditableMockTextArea ctrl{source};

    wxArrayString hunkLines;
    hunkLines.Add(" void test2() {");
    hunkLines.Add("-    return;");
    hunkLines.Add("+    return 0;");

    auto result = PatchApplier::ApplyHunkLoose(&ctrl, hunkLines, 0);

    REQUIRE(result.ok());

    wxString line4 = ctrl.GetLine(4);
    line4.Trim();
    CHECK(line4.Contains("return 0;"));
}

TEST_CASE("ApplyHunkLoose - Complex Real World Scenario")
{
    wxString source = R"(#include <iostream>
#include <string>

class MyClass {
public:
    void method1() {
        std::cout << "Method 1" << std::endl;
    }

    void method2() {
        std::cout << "Method 2" << std::endl;
    }

    void method3() {
        std::cout << "Method 3" << std::endl;
    }
};
)";

    EditableMockTextArea ctrl{source};

    // First hunk: modify method1
    wxArrayString hunk1;
    hunk1.Add("     void method1() {");
    hunk1.Add("-        std::cout << \"Method 1\" << std::endl;");
    hunk1.Add("+        std::cout << \"Updated Method 1\" << std::endl;");

    auto result1 = PatchApplier::ApplyHunkLoose(&ctrl, hunk1, 0);
    REQUIRE(result1.ok());

    // Second hunk: modify method3, starting from where first hunk ended
    wxArrayString hunk2;
    hunk2.Add("     void method3() {");
    hunk2.Add("-        std::cout << \"Method 3\" << std::endl;");
    hunk2.Add("+        std::cout << \"Updated Method 3\" << std::endl;");

    auto result2 = PatchApplier::ApplyHunkLoose(&ctrl, hunk2, result1.value());
    REQUIRE(result2.ok());

    // Verify both changes were applied
    wxString method1Line = ctrl.GetLine(6);
    method1Line.Trim();
    CHECK(method1Line.Contains("Updated Method 1"));

    wxString method3Line = ctrl.GetLine(14);
    method3Line.Trim();
    CHECK(method3Line.Contains("Updated Method 3"));
}

TEST_CASE("ApplyHunkLoose - Empty Lines in Source")
{
    wxString source = R"(void test() {

    printf("hello");

})";

    EditableMockTextArea ctrl{source};

    wxArrayString hunkLines;
    hunkLines.Add("-    printf(\"hello\");");
    hunkLines.Add("+    printf(\"goodbye\");");

    auto result = PatchApplier::ApplyHunkLoose(&ctrl, hunkLines, 0);

    REQUIRE(result.ok());

    wxString line2 = ctrl.GetLine(2);
    line2.Trim();
    CHECK(line2.Contains("printf(\"goodbye\");"));
}
