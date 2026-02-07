#include "json_utils.h"

#include <doctest.h>

TEST_CASE("wxSize")
{
    const wxSize size = {1, 2};
    CHECK(size == JsonUtils::ToSize(JsonUtils::ToJsonValue(size)));
}

TEST_CASE("wxStringMap_t")
{
    const wxStringMap_t map = {{"hello", "world"}, {"key", "value"}};
    CHECK(map == JsonUtils::ToStringMap(JsonUtils::ToJson(map)));
}
