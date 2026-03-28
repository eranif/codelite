#include "JSON.h"

#include <doctest.h>

TEST_CASE("json parsing")
{
    JSON invalidJson("invalid");
    CHECK(!invalidJson.isOk());
    JSON validJson("42");
    CHECK(validJson.isOk());
}

TEST_CASE("empty json")
{
    JSONItem item{};
    CHECK(!item.isOk());
    CHECK(!item.isNull());
    CHECK(!item.isBool());
    CHECK(!item.isString());
    CHECK(!item.isNumber());
    CHECK(!item.isArray());
    CHECK(!item.isObject());

    CHECK(item.format().IsEmpty());

    CHECK(item.toBool(false) == false);
    CHECK(item.toBool(true) == true);

    CHECK(item.toString("Not a string") == "Not a string");

    CHECK(item.toInt(42) == 42);
    CHECK(item.toDouble(4.2) == 4.2);

    CHECK(item.GetAsVector().empty());
    CHECK(item.arraySize() == 0);
    CHECK(!item[0].isOk());

    CHECK(!item.hasNamedObject("a"));
    CHECK(!item.namedObject("a").isOk());
    CHECK(!item["a"].isOk());
    CHECK(item.GetAsMap().empty());
}

TEST_CASE("null json")
{
    JSON json("null");
    JSONItem item{json.toElement()};
    CHECK(item.isOk());
    CHECK(item.isNull());
    CHECK(!item.isBool());
    CHECK(!item.isString());
    CHECK(!item.isNumber());
    CHECK(!item.isArray());
    CHECK(!item.isObject());
    CHECK(item.format() == "null");

    CHECK(item.toBool(false) == false);
    CHECK(item.toBool(true) == true);

    CHECK(item.toString("Not a string") == "Not a string");

    CHECK(item.toInt(42) == 42);
    CHECK(item.toDouble(4.2) == 4.2);

    CHECK(item.GetAsVector().empty());
    CHECK(item.arraySize() == 0);
    CHECK(!item[0].isOk());

    CHECK(!item.hasNamedObject("a"));
    CHECK(!item.namedObject("a").isOk());
    CHECK(!item["a"].isOk());
    CHECK(item.GetAsMap().empty());
}

TEST_CASE("true json")
{
    JSON json("true");
    JSONItem item{json.toElement()};
    CHECK(item.isOk());
    CHECK(!item.isNull());
    CHECK(item.isBool());
    CHECK(!item.isString());
    CHECK(!item.isNumber());
    CHECK(!item.isArray());
    CHECK(!item.isObject());
    CHECK(item.format() == "true");

    CHECK(item.toBool(false) == true);
    CHECK(item.toBool(true) == true);

    CHECK(item.toString("Not a string") == "Not a string");

    CHECK(item.toInt(42) == 42);
    CHECK(item.toDouble(4.2) == 4.2);

    CHECK(item.GetAsVector().empty());
    CHECK(item.arraySize() == 0);
    CHECK(!item[0].isOk());

    CHECK(!item.hasNamedObject("a"));
    CHECK(!item.namedObject("a").isOk());
    CHECK(!item["a"].isOk());
    CHECK(item.GetAsMap().empty());
}
TEST_CASE("false json")
{
    JSON json("false");
    JSONItem item{json.toElement()};
    CHECK(item.isOk());
    CHECK(!item.isNull());
    CHECK(item.isBool());
    CHECK(!item.isString());
    CHECK(!item.isNumber());
    CHECK(!item.isArray());
    CHECK(!item.isObject());
    CHECK(item.format() == "false");

    CHECK(item.toBool(false) == false);
    CHECK(item.toBool(true) == false);

    CHECK(item.toString("Not a string") == "Not a string");

    CHECK(item.toInt(42) == 42);
    CHECK(item.toDouble(4.2) == 4.2);

    CHECK(item.GetAsVector().empty());
    CHECK(item.arraySize() == 0);
    CHECK(!item[0].isOk());

    CHECK(!item.hasNamedObject("a"));
    CHECK(!item.namedObject("a").isOk());
    CHECK(!item["a"].isOk());
    CHECK(item.GetAsMap().empty());
}

TEST_CASE("string json")
{
    JSON json(R"("Hello world")");
    JSONItem item{json.toElement()};
    CHECK(item.isOk());
    CHECK(!item.isNull());
    CHECK(!item.isBool());
    CHECK(item.isString());
    CHECK(!item.isNumber());
    CHECK(!item.isArray());
    CHECK(!item.isObject());
    CHECK(item.format() == R"("Hello world")");

    CHECK(item.toBool(false) == false);
    CHECK(item.toBool(true) == true);

    CHECK(item.toString("Not a string") == "Hello world");

    CHECK(item.toInt(42) == 42);
    CHECK(item.toDouble(4.2) == 4.2);

    CHECK(item.GetAsVector().empty());
    CHECK(item.arraySize() == 0);
    CHECK(!item[0].isOk());

    CHECK(!item.hasNamedObject("a"));
    CHECK(!item.namedObject("a").isOk());
    CHECK(!item["a"].isOk());
    CHECK(item.GetAsMap().empty());
}

TEST_CASE("number json")
{
    JSON json("12.34");
    JSONItem item{json.toElement()};
    CHECK(item.isOk());
    CHECK(!item.isNull());
    CHECK(!item.isBool());
    CHECK(!item.isString());
    CHECK(item.isNumber());
    CHECK(!item.isArray());
    CHECK(!item.isObject());
    CHECK(item.format() == "12.34");

    CHECK(item.toBool(false) == false);
    CHECK(item.toBool(true) == true);

    CHECK(item.toString("Not a string") == "Not a string");

    CHECK(item.toInt(42) == 12);
    CHECK(item.toDouble(4.2) == 12.34);

    CHECK(item.GetAsVector().empty());
    CHECK(item.arraySize() == 0);
    CHECK(!item[0].isOk());

    CHECK(!item.hasNamedObject("a"));
    CHECK(!item.namedObject("a").isOk());
    CHECK(!item["a"].isOk());
    CHECK(item.GetAsMap().empty());
}

TEST_CASE("array json")
{
    JSON json("[1, true, null]");
    JSONItem item{json.toElement()};
    CHECK(item.isOk());
    CHECK(!item.isNull());
    CHECK(!item.isBool());
    CHECK(!item.isString());
    CHECK(!item.isNumber());
    CHECK(item.isArray());
    CHECK(!item.isObject());
    CHECK(item.format(false) == "[1,true,null]");

    CHECK(item.toBool(false) == false);
    CHECK(item.toBool(true) == true);

    CHECK(item.toString("Not a string") == "Not a string");

    CHECK(item.toInt(42) == 42);
    CHECK(item.toDouble(4.2) == 4.2);

    CHECK(item.GetAsVector().size() == 3);
    CHECK(item.arraySize() == 3);
    CHECK(item[0].isNumber());
    CHECK(item[1].isBool());
    CHECK(item[2].isNull());

    CHECK(!item.hasNamedObject("a"));
    CHECK(!item.namedObject("a").isOk());
    CHECK(!item["a"].isOk());
    CHECK(item.GetAsMap().empty());
}

TEST_CASE("object json")
{
    JSON json(R"({"key1":"value", "key2":51})");
    JSONItem item{json.toElement()};
    CHECK(item.isOk());
    CHECK(!item.isNull());
    CHECK(!item.isBool());
    CHECK(!item.isString());
    CHECK(!item.isNumber());
    CHECK(!item.isArray());
    CHECK(item.isObject());
    CHECK(item.format(false) == R"({"key1":"value","key2":51})");

    CHECK(item.toBool(false) == false);
    CHECK(item.toBool(true) == true);

    CHECK(item.toString("Not a string") == "Not a string");

    CHECK(item.toInt(42) == 42);
    CHECK(item.toDouble(4.2) == 4.2);

    CHECK(item.GetAsVector().empty());
    CHECK(item.arraySize() == 0);
    CHECK(!item[0].isOk());

    CHECK(item.contains("key1"));
    CHECK(item.contains("key2"));
    CHECK(!item.contains("key"));
    CHECK(item.hasNamedObject("key1"));
    CHECK(item.hasNamedObject("key2"));
    CHECK(!item.hasNamedObject("key"));
    CHECK(item.namedObject("key1").isString());
    CHECK(item.namedObject("key2").isNumber());
    CHECK(!item.namedObject("key not present").isOk());
    CHECK(item["key1"].isString());
    CHECK(item["key2"].isNumber());
    CHECK(!item["key not present"].isOk());
    CHECK(item.GetAsMap().size() == 2);
}

TEST_CASE("array append")
{
    JSON json(JsonType::Array);
    auto item = json.toElement();
    item.arrayAppend(4.2);
    item.arrayAppend(51);
    item.arrayAppend(std::string("Hello"));
    item.arrayAppend("world");
    item.arrayAppend(wxString(L"..."));
    item.arrayAppend(JSONItem::createObject());
    CHECK(item.format(false) == R"([4.2,51.0,"Hello","world","...",{}])");
    CHECK(item.format(false) == json.toElement().format(false));
}

TEST_CASE("object append/remove")
{
    JSON json(JsonType::Object);
    auto item = json.toElement();
    item.addProperty("key1", true);
    item.addProperty("key2", 51);
    item.addProperty("key3", std::string("Hello"));
    item.addProperty("key4", "world");
    item.addProperty("key5", wxString(L"..."));
    item.AddArray("key6");
    item.addProperty("key7", std::vector{4, 8, 15, 16, 23, 42});
    CHECK(item.format(false) ==
          R"({"key1":true,"key2":51,"key3":"Hello","key4":"world","key5":"...","key6":[],"key7":[4,8,15,16,23,42]})");
    item.removeProperty("Not present");
    CHECK(item.format(false) ==
          R"({"key1":true,"key2":51,"key3":"Hello","key4":"world","key5":"...","key6":[],"key7":[4,8,15,16,23,42]})");
    item.removeProperty("key5");
    CHECK(item.format(false) ==
          R"({"key1":true,"key2":51,"key3":"Hello","key4":"world","key6":[],"key7":[4,8,15,16,23,42]})");
    CHECK(item.format(false) == json.toElement().format(false));
}

TEST_CASE("object addXxx")
{
    JSON json(JsonType::Object);
    auto item = json.toElement();
    auto array = item.AddArray("array");
    array.arrayAppend(42);
    auto& self = item.addNull("Null");
    CHECK(&self == &item);
    auto object = item.AddObject("object");
    auto object2 = JSONItem::createObject();
    auto& object3 = object.addProperty("inner", object2);
    CHECK(&object == &object3);
    JSON moved(JsonType::Object);
    moved.toElement().addProperty("a", 1);
    item.addProperty("moved", std::move(moved));
    CHECK(item.format(false) == R"({"array":[42.0],"Null":null,"object":{"inner":{}},"moved":{"a":1}})");
}

TEST_CASE("toArrayString")
{
    wxArrayString strings{"hello", "world"};

    JSON root(JsonType::Object);
    root.toElement().addProperty("strings", strings);

    CHECK(strings == root.toElement()["strings"].toArrayString());
}

TEST_CASE("toIntArray")
{
    const std::vector<int> v{4, 8, 15, 16, 23, 42};

    JSON root(JsonType::Object);
    root.toElement().addProperty("v", v);

    CHECK(v == root.toElement()["v"].toIntArray());
}

TEST_CASE("toDoubleArray")
{
    const std::vector<double> v{1.1, 2.2, 3.3};

    JSON root(JsonType::Array);
    for (auto d : v) {
        root.toElement().arrayAppend(d);
    }

    CHECK(v == root.toElement().toDoubleArray());
}

TEST_CASE("toStringMap") {
    const wxStringMap_t map{{"1", "one"}, {"2", "two"}};

    JSON root(JsonType::Object);
    root.toElement().addProperty("map", map);

    CHECK(map == root.toElement()["map"].toStringMap());
}

TEST_CASE("GetAsVector") {
    JSON root("[1,2,3]");
    const auto v = root.toElement().GetAsVector();
    REQUIRE(v.size() == 3);
    CHECK(1 == v[0].toInt());
    CHECK(2 == v[1].toInt());
    CHECK(3 == v[2].toInt());
}

TEST_CASE("GetAsMap") {
    JSON root(R"({"age":42,"name":"Doe"})");
    const auto m = root.toElement().GetAsMap();
    REQUIRE(m.size() == 2);
    CHECK(42 == m.at("age").toInt());
    CHECK("Doe" == m.at("name").toString());
}
