#include "clPatchApplier.hpp"

#include <doctest.h>

// ============================================================================
// ParseUnifiedPatch Tests
// ============================================================================

TEST_CASE("ParseUnifiedPatch - empty patch")
{
    wxString patchContent = "";
    UnifiedPatch patch = PatchApplier::ParseUnifiedPatch(patchContent);

    CHECK(patch.originalFile.IsEmpty());
    CHECK(patch.newFile.IsEmpty());
    CHECK(patch.hunks.empty());
}

TEST_CASE("ParseUnifiedPatch - simple patch with one hunk")
{
    wxString patchContent = "--- a/file.txt\n"
                            "+++ b/file.txt\n"
                            "@@ -1,3 +1,4 @@\n"
                            " line1\n"
                            " line2\n"
                            "+new line\n"
                            " line3\n";

    UnifiedPatch patch = PatchApplier::ParseUnifiedPatch(patchContent);

    CHECK(patch.originalFile == "a/file.txt");
    CHECK(patch.newFile == "b/file.txt");
    CHECK(patch.hunks.size() == 1);

    const PatchHunk& hunk = patch.hunks[0];
    CHECK(hunk.originalStart == 1);
    CHECK(hunk.originalCount == 3);
    CHECK(hunk.newStart == 1);
    CHECK(hunk.newCount == 4);
    CHECK(hunk.lines.size() >= 4);
    CHECK(hunk.lines[0] == " line1");
    CHECK(hunk.lines[1] == " line2");
    CHECK(hunk.lines[2] == "+new line");
    CHECK(hunk.lines[3] == " line3");
}

TEST_CASE("ParseUnifiedPatch - patch with deletion")
{
    wxString patchContent = "--- a/file.txt\n"
                            "+++ b/file.txt\n"
                            "@@ -1,4 +1,3 @@\n"
                            " line1\n"
                            "-removed line\n"
                            " line2\n"
                            " line3\n";

    UnifiedPatch patch = PatchApplier::ParseUnifiedPatch(patchContent);

    CHECK(patch.hunks.size() == 1);

    const PatchHunk& hunk = patch.hunks[0];
    CHECK(hunk.originalStart == 1);
    CHECK(hunk.originalCount == 4);
    CHECK(hunk.newStart == 1);
    CHECK(hunk.newCount == 3);
    CHECK(hunk.lines.size() >= 4);
    CHECK(hunk.lines[0] == " line1");
    CHECK(hunk.lines[1] == "-removed line");
    CHECK(hunk.lines[2] == " line2");
    CHECK(hunk.lines[3] == " line3");
}

TEST_CASE("ParseUnifiedPatch - patch with multiple hunks")
{
    wxString patchContent = "--- a/file.txt\n"
                            "+++ b/file.txt\n"
                            "@@ -1,2 +1,3 @@\n"
                            " line1\n"
                            "+added line\n"
                            " line2\n"
                            "@@ -10,2 +11,3 @@\n"
                            " line10\n"
                            "+another added line\n"
                            " line11\n";

    UnifiedPatch patch = PatchApplier::ParseUnifiedPatch(patchContent);

    CHECK(patch.hunks.size() == 2);

    CHECK(patch.hunks[0].originalStart == 1);
    CHECK(patch.hunks[0].originalCount == 2);
    CHECK(patch.hunks[0].newStart == 1);
    CHECK(patch.hunks[0].newCount == 3);

    CHECK(patch.hunks[1].originalStart == 10);
    CHECK(patch.hunks[1].originalCount == 2);
    CHECK(patch.hunks[1].newStart == 11);
    CHECK(patch.hunks[1].newCount == 3);
}

TEST_CASE("ParseUnifiedPatch - hunk header without count (implies 1)")
{
    wxString patchContent = "--- a/file.txt\n"
                            "+++ b/file.txt\n"
                            "@@ -5 +5,2 @@\n"
                            " context\n"
                            "+new line\n";

    UnifiedPatch patch = PatchApplier::ParseUnifiedPatch(patchContent);

    CHECK(patch.hunks.size() == 1);
    CHECK(patch.hunks[0].originalStart == 5);
    CHECK(patch.hunks[0].originalCount == 1);
    CHECK(patch.hunks[0].newStart == 5);
    CHECK(patch.hunks[0].newCount == 2);
}

TEST_CASE("ParseUnifiedPatch - patch with modification (replace)")
{
    wxString patchContent = "--- a/file.txt\n"
                            "+++ b/file.txt\n"
                            "@@ -1,3 +1,3 @@\n"
                            " line1\n"
                            "-old line\n"
                            "+new line\n"
                            " line3\n";

    UnifiedPatch patch = PatchApplier::ParseUnifiedPatch(patchContent);

    CHECK(patch.hunks.size() == 1);
    const PatchHunk& hunk = patch.hunks[0];
    CHECK(hunk.lines.size() >= 4);
    CHECK(hunk.lines[0] == " line1");
    CHECK(hunk.lines[1] == "-old line");
    CHECK(hunk.lines[2] == "+new line");
    CHECK(hunk.lines[3] == " line3");
}

TEST_CASE("ParseUnifiedPatch - patch with no newline marker")
{
    wxString patchContent = "--- a/file.txt\n"
                            "+++ b/file.txt\n"
                            "@@ -1,2 +1,2 @@\n"
                            " line1\n"
                            "-last line\n"
                            "\\ No newline at end of file\n"
                            "+new last line\n"
                            "\\ No newline at end of file\n";

    UnifiedPatch patch = PatchApplier::ParseUnifiedPatch(patchContent);

    CHECK(patch.hunks.size() == 1);
    // The backslash lines should be included but will be skipped during application
    bool hasBackslashLine = false;
    for (size_t i = 0; i < patch.hunks[0].lines.size(); ++i) {
        if (patch.hunks[0].lines[i].StartsWith("\\")) {
            hasBackslashLine = true;
            break;
        }
    }
    CHECK(hasBackslashLine);
}

TEST_CASE("ParseUnifiedPatch - file paths with tabs (git format)")
{
    wxString patchContent = "--- a/path/to/file.txt\t2024-01-01 00:00:00\n"
                            "+++ b/path/to/file.txt\t2024-01-02 00:00:00\n"
                            "@@ -1,1 +1,1 @@\n"
                            "-old\n"
                            "+new\n";

    UnifiedPatch patch = PatchApplier::ParseUnifiedPatch(patchContent);

    CHECK(patch.originalFile == "a/path/to/file.txt");
    CHECK(patch.newFile == "b/path/to/file.txt");
}

// ============================================================================
// PatchResult structure tests
// ============================================================================

TEST_CASE("PatchResult - default initialization")
{
    PatchResult result{};
    CHECK(result.success == false);
    CHECK(result.errorMessage.IsEmpty());
    CHECK(result.hunksApplied == 0);
    CHECK(result.hunksFailed == 0);
    CHECK(result.warnings.IsEmpty());
}

TEST_CASE("PatchResult - successful result")
{
    PatchResult result{true, "", 2, 0, {}};
    CHECK(result.success == true);
    CHECK(result.hunksApplied == 2);
    CHECK(result.hunksFailed == 0);
}

TEST_CASE("PatchResult - partial success")
{
    wxArrayString warnings;
    warnings.Add("Hunk #2 FAILED");
    PatchResult result{true, "", 1, 1, warnings};
    CHECK(result.success == true);
    CHECK(result.hunksApplied == 1);
    CHECK(result.hunksFailed == 1);
    CHECK(result.warnings.size() == 1);
}

// ============================================================================
// PatchOptions structure tests
// ============================================================================

TEST_CASE("PatchOptions - default values")
{
    PatchOptions options{};
    CHECK(options.reverse == false);
    CHECK(options.fuzzFactor == 0);
    CHECK(options.dryRun == false);
    CHECK(options.backup == true);
    CHECK(options.backupSuffix == ".orig");
}

TEST_CASE("PatchOptions - custom values")
{
    PatchOptions options;
    options.reverse = true;
    options.fuzzFactor = 3;
    options.dryRun = true;
    options.backup = false;
    options.backupSuffix = ".bak";

    CHECK(options.reverse == true);
    CHECK(options.fuzzFactor == 3);
    CHECK(options.dryRun == true);
    CHECK(options.backup == false);
    CHECK(options.backupSuffix == ".bak");
}

// ============================================================================
// PatchHunk structure tests
// ============================================================================

TEST_CASE("PatchHunk - default initialization")
{
    PatchHunk hunk{};
    CHECK(hunk.originalStart == 0);
    CHECK(hunk.originalCount == 0);
    CHECK(hunk.newStart == 0);
    CHECK(hunk.newCount == 0);
    CHECK(hunk.lines.IsEmpty());
}

TEST_CASE("PatchHunk - with data")
{
    PatchHunk hunk;
    hunk.originalStart = 10;
    hunk.originalCount = 5;
    hunk.newStart = 10;
    hunk.newCount = 7;
    hunk.lines.Add(" context");
    hunk.lines.Add("+added");
    hunk.lines.Add("-removed");

    CHECK(hunk.originalStart == 10);
    CHECK(hunk.originalCount == 5);
    CHECK(hunk.newStart == 10);
    CHECK(hunk.newCount == 7);
    CHECK(hunk.lines.size() == 3);
}

// ============================================================================
// UnifiedPatch structure tests
// ============================================================================

TEST_CASE("UnifiedPatch - default initialization")
{
    UnifiedPatch patch{};
    CHECK(patch.originalFile.IsEmpty());
    CHECK(patch.newFile.IsEmpty());
    CHECK(patch.hunks.empty());
}

TEST_CASE("UnifiedPatch - with multiple hunks")
{
    UnifiedPatch patch;
    patch.originalFile = "original.cpp";
    patch.newFile = "modified.cpp";

    PatchHunk hunk1;
    hunk1.originalStart = 1;
    hunk1.originalCount = 3;
    patch.hunks.push_back(hunk1);

    PatchHunk hunk2;
    hunk2.originalStart = 100;
    hunk2.originalCount = 5;
    patch.hunks.push_back(hunk2);

    CHECK(patch.originalFile == "original.cpp");
    CHECK(patch.newFile == "modified.cpp");
    CHECK(patch.hunks.size() == 2);
    CHECK(patch.hunks[0].originalStart == 1);
    CHECK(patch.hunks[1].originalStart == 100);
}

// ============================================================================
// Edge cases for ParseUnifiedPatch
// ============================================================================

TEST_CASE("ParseUnifiedPatch - only header no hunks")
{
    wxString patchContent = "--- a/file.txt\n"
                            "+++ b/file.txt\n";

    UnifiedPatch patch = PatchApplier::ParseUnifiedPatch(patchContent);

    CHECK(patch.originalFile == "a/file.txt");
    CHECK(patch.newFile == "b/file.txt");
    CHECK(patch.hunks.empty());
}

TEST_CASE("ParseUnifiedPatch - hunk with only additions (new file)")
{
    wxString patchContent = "--- /dev/null\n"
                            "+++ b/newfile.txt\n"
                            "@@ -0,0 +1,3 @@\n"
                            "+line1\n"
                            "+line2\n"
                            "+line3\n";

    UnifiedPatch patch = PatchApplier::ParseUnifiedPatch(patchContent);

    CHECK(patch.originalFile == "/dev/null");
    CHECK(patch.newFile == "b/newfile.txt");
    CHECK(patch.hunks.size() == 1);
    CHECK(patch.hunks[0].originalStart == 0);
    CHECK(patch.hunks[0].originalCount == 0);
    CHECK(patch.hunks[0].newStart == 1);
    CHECK(patch.hunks[0].newCount == 3);
}

TEST_CASE("ParseUnifiedPatch - hunk with only deletions (file removal)")
{
    wxString patchContent = "--- a/oldfile.txt\n"
                            "+++ /dev/null\n"
                            "@@ -1,3 +0,0 @@\n"
                            "-line1\n"
                            "-line2\n"
                            "-line3\n";

    UnifiedPatch patch = PatchApplier::ParseUnifiedPatch(patchContent);

    CHECK(patch.originalFile == "a/oldfile.txt");
    CHECK(patch.newFile == "/dev/null");
    CHECK(patch.hunks.size() == 1);
    CHECK(patch.hunks[0].originalStart == 1);
    CHECK(patch.hunks[0].originalCount == 3);
    CHECK(patch.hunks[0].newStart == 0);
    CHECK(patch.hunks[0].newCount == 0);
}

TEST_CASE("ParseUnifiedPatch - complex hunk with mixed changes")
{
    wxString patchContent = "--- a/complex.cpp\n"
                            "+++ b/complex.cpp\n"
                            "@@ -5,10 +5,12 @@\n"
                            " #include <string>\n"
                            "+#include <vector>\n"
                            "+#include <map>\n"
                            " \n"
                            " int main() {\n"
                            "-    printf(\"Hello\");\n"
                            "-    return 0;\n"
                            "+    std::cout << \"Hello\" << std::endl;\n"
                            "+    return EXIT_SUCCESS;\n"
                            " }\n"
                            " \n"
                            " // End of file\n";

    UnifiedPatch patch = PatchApplier::ParseUnifiedPatch(patchContent);

    CHECK(patch.hunks.size() == 1);
    const PatchHunk& hunk = patch.hunks[0];
    CHECK(hunk.originalStart == 5);
    CHECK(hunk.originalCount == 10);
    CHECK(hunk.newStart == 5);
    CHECK(hunk.newCount == 12);

    // Count different line types
    int additions = 0, deletions = 0, context = 0;
    for (size_t i = 0; i < hunk.lines.size(); ++i) {
        if (hunk.lines[i].StartsWith("+"))
            additions++;
        else if (hunk.lines[i].StartsWith("-"))
            deletions++;
        else if (hunk.lines[i].StartsWith(" "))
            context++;
    }
    CHECK(additions == 4);
    CHECK(deletions == 2);
    // Context lines: 6 from content + possible trailing empty line treated as context
    CHECK(context >= 6);
}

TEST_CASE("ParseUnifiedPatch - handles empty lines in diff")
{
    wxString patchContent = "--- a/file.txt\n"
                            "+++ b/file.txt\n"
                            "@@ -1,4 +1,4 @@\n"
                            " line1\n"
                            "\n"
                            " line3\n"
                            "+line4\n";

    UnifiedPatch patch = PatchApplier::ParseUnifiedPatch(patchContent);

    CHECK(patch.hunks.size() == 1);
    // Empty lines should be treated as context
    bool hasSpaceLine = false;
    for (size_t i = 0; i < patch.hunks[0].lines.size(); ++i) {
        if (patch.hunks[0].lines[i] == " ") {
            hasSpaceLine = true;
            break;
        }
    }
    CHECK(hasSpaceLine);
}

TEST_CASE("ParseUnifiedPatch - large line numbers")
{
    wxString patchContent = "--- a/large.txt\n"
                            "+++ b/large.txt\n"
                            "@@ -99999,3 +99999,4 @@\n"
                            " context\n"
                            "+added\n"
                            " more context\n"
                            " end\n";

    UnifiedPatch patch = PatchApplier::ParseUnifiedPatch(patchContent);

    CHECK(patch.hunks.size() == 1);
    CHECK(patch.hunks[0].originalStart == 99999);
    CHECK(patch.hunks[0].newStart == 99999);
}

TEST_CASE("ParseUnifiedPatch - hunk header with extra text after @@")
{
    // Git often includes function context after the second @@
    wxString patchContent = "--- a/file.cpp\n"
                            "+++ b/file.cpp\n"
                            "@@ -10,3 +10,4 @@ void MyClass::myFunction()\n"
                            " {\n"
                            "+    // Added comment\n"
                            "     doSomething();\n"
                            " }\n";

    UnifiedPatch patch = PatchApplier::ParseUnifiedPatch(patchContent);

    CHECK(patch.hunks.size() == 1);
    CHECK(patch.hunks[0].originalStart == 10);
    CHECK(patch.hunks[0].originalCount == 3);
    CHECK(patch.hunks[0].newStart == 10);
    CHECK(patch.hunks[0].newCount == 4);
}

// ============================================================================
// ApplyPatch tests (dry run mode to avoid file system side effects)
// ============================================================================

TEST_CASE("ApplyPatch - empty patch returns error")
{
    PatchOptions options;
    options.dryRun = true;

    // Note: This test would require a real file to exist
    // In a real test environment, you'd create a temp file
    PatchResult result = PatchApplier::ApplyPatch("nonexistent_file.txt", "", options);

    CHECK(result.success == false);
    CHECK(result.errorMessage == "No valid hunks found in patch");
}

TEST_CASE("ApplyPatch - patch with no hunks returns error")
{
    wxString patchContent = "--- a/file.txt\n"
                            "+++ b/file.txt\n";

    PatchOptions options;
    options.dryRun = true;

    PatchResult result = PatchApplier::ApplyPatch("nonexistent.txt", patchContent, options);

    CHECK(result.success == false);
    CHECK(result.errorMessage == "No valid hunks found in patch");
}
