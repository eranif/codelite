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
