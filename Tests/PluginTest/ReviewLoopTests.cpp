#include <doctest.h>

// clang-format off
#include "ReviewLoop.hpp"
// clang-format on

namespace
{
const wxString kValidId = "3f2c1d9e-1a2b-4c3d-8e4f-0123456789ab";
const wxString kPrefix = ".agents/reviews/";
} // namespace

TEST_SUITE("ReviewLoop")
{
    TEST_CASE("IsReviewFolder accepts the folder of a loop")
    {
        CHECK(ReviewLoop::IsReviewFolder(kPrefix + kValidId));

        // Whatever NewId() makes must be accepted
        for (int i = 0; i < 100; ++i) {
            const wxString id = ReviewLoop::NewId();
            CHECK_MESSAGE(ReviewLoop::IsReviewFolder(kPrefix + id), id.ToStdString());
            CHECK(ReviewLoop(id).Folder() == kPrefix + id);
        }
    }

    TEST_CASE("IsReviewFolder rejects a wrong id length")
    {
        CHECK_FALSE(ReviewLoop::IsReviewFolder(kPrefix));
        CHECK_FALSE(ReviewLoop::IsReviewFolder(kPrefix + kValidId.Left(35)));
        CHECK_FALSE(ReviewLoop::IsReviewFolder(kPrefix + kValidId + "0"));
        CHECK_FALSE(ReviewLoop::IsReviewFolder(kPrefix + kValidId + "/"));
    }

    TEST_CASE("IsReviewFolder rejects upper-case and other characters")
    {
        CHECK_FALSE(ReviewLoop::IsReviewFolder(kPrefix + kValidId.Upper()));
        CHECK_FALSE(ReviewLoop::IsReviewFolder(kPrefix + "3f2c1d9e-1a2b-4c3d-8e4f-0123456789ag"));
        // A dash in a wrong place, and a hex digit where a dash must be
        CHECK_FALSE(ReviewLoop::IsReviewFolder(kPrefix + "3f2c1d9e1-a2b-4c3d-8e4f-0123456789ab"));
        CHECK_FALSE(ReviewLoop::IsReviewFolder(kPrefix + "3f2c1d9e01a2b-4c3d-8e4f-0123456789ab"));
        // 36 characters that are not an id
        CHECK_FALSE(ReviewLoop::IsReviewFolder(kPrefix + wxString('.', 36)));
    }

    TEST_CASE("IsReviewFolder rejects a path that is not under the reviews folder")
    {
        CHECK_FALSE(ReviewLoop::IsReviewFolder(kValidId));
        CHECK_FALSE(ReviewLoop::IsReviewFolder(".agents/" + kValidId));
        CHECK_FALSE(ReviewLoop::IsReviewFolder("../" + kPrefix + kValidId));
        CHECK_FALSE(ReviewLoop::IsReviewFolder("/" + kPrefix + kValidId));
        CHECK_FALSE(ReviewLoop::IsReviewFolder("x/" + kPrefix + kValidId));
        CHECK_FALSE(ReviewLoop::IsReviewFolder(".agents/reviews" + kValidId));
        CHECK_FALSE(ReviewLoop::IsReviewFolder(wxEmptyString));
    }

    TEST_CASE("ParseVerdict ignores decoration around the status")
    {
        CHECK(ReviewLoop::ParseVerdict("STATUS: CLEAN\n") == ReviewLoop::Verdict::Clean);
        CHECK(ReviewLoop::ParseVerdict("\n\n**STATUS: CLEAN**\n\nNothing found") == ReviewLoop::Verdict::Clean);
        CHECK(ReviewLoop::ParseVerdict("STATUS: CLEAN.") == ReviewLoop::Verdict::Clean);
        CHECK(ReviewLoop::ParseVerdict("`STATUS: FINDINGS`\n1. a bug") == ReviewLoop::Verdict::Findings);
        CHECK(ReviewLoop::ParseVerdict("## status: findings") == ReviewLoop::Verdict::Findings);
        CHECK(ReviewLoop::ParseVerdict("Looks fine to me") == ReviewLoop::Verdict::Unknown);
        CHECK(ReviewLoop::ParseVerdict(wxEmptyString) == ReviewLoop::Verdict::Unknown);
    }
}
