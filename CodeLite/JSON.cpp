//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : json_node.cpp
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#include "JSON.h"

#include "fileutils.h"

#include <wx/filename.h>

JSON::JSON(const wxString& text)
{
    m_json = std::make_shared<nlohmann::ordered_json>(nlohmann::ordered_json::parse(text.mb_str(wxConvUTF8).data()));
}

JSON::JSON(JsonType type)
{
    if (type == JsonType::Array) {
        m_json = std::make_shared<nlohmann::ordered_json>(nlohmann::ordered_json::array());
    } else if (type == JsonType::Null) {
        m_json = std::make_shared<nlohmann::ordered_json>(nlohmann::ordered_json(nullptr));
    } else {
        m_json = std::make_shared<nlohmann::ordered_json>(nlohmann::ordered_json::object());
    }
}

JSON::JSON(const wxFileName& filename)
{
    wxString content;
    if (!FileUtils::ReadFileContent(filename, content)) {
        return;
    }
    m_json = std::make_shared<nlohmann::ordered_json>(nlohmann::ordered_json::parse(content.ToStdString(wxConvUTF8)));
}

void JSON::save(const wxFileName& fn) const
{
    if (!isOk()) {
        FileUtils::WriteFileContent(fn, "{}");
    } else {
        FileUtils::WriteFileContent(fn, toElement().format(), wxConvUTF8);
    }
}

JSONItem JSON::toElement() const
{
    if (!m_json) {
        return JSONItem(nullptr);
    }
    return JSONItem(m_json, m_json.get());
}

JSONItem JSONItem::createArray()
{
    auto root = std::make_shared<nlohmann::ordered_json>(nlohmann::ordered_json::array());
    return JSONItem(root, root.get());
}

JSONItem JSONItem::createObject()
{
    auto root = std::make_shared<nlohmann::ordered_json>(nlohmann::ordered_json::object());
    return JSONItem(root, root.get());
}

///////////////////////////////////////////////////////////////////////////////
JSONItem::JSONItem(std::shared_ptr<nlohmann::ordered_json> root, nlohmann::ordered_json* json)
    : m_root(std::move(root))
    , m_json(json)
{
}

char* JSONItem::FormatRawString(bool formatted) const
{
    if (!m_json) {
        return nullptr;
    }
    const auto s = m_json->dump(formatted ? 2 : -1);
    return strdup(s.data());
}

wxString JSONItem::format(bool formatted) const
{
    if (!m_json) {
        return wxT("");
    }
    return wxString::FromUTF8(m_json->dump(formatted ? 2 : -1));
}

bool JSONItem::isNull() const { return m_json && m_json->is_null(); }
bool JSONItem::isBool() const { return m_json && m_json->is_boolean(); }
bool JSONItem::isString() const { return m_json && m_json->is_string(); }
bool JSONItem::isNumber() const { return m_json && m_json->is_number(); }
bool JSONItem::isArray() const { return m_json && m_json->is_array(); }
bool JSONItem::isObject() const { return m_json && m_json->is_object(); }

bool JSONItem::toBool(bool defaultValue) const
{
    if (!m_json || !isBool()) {
        return defaultValue;
    }
    return m_json->get<bool>();
}

wxString JSONItem::toString(const wxString& defaultValue) const
{
    if (!m_json || !isString()) {
        return defaultValue;
    }
    return wxString::FromUTF8(m_json->get<std::string>());
}

int JSONItem::toInt(int defaultValue) const
{
    if (!m_json || !isNumber()) {
        return defaultValue;
    }
    return m_json->get<int>();
}

size_t JSONItem::toSize_t(size_t defaultValue) const
{
    if (!m_json || !isNumber()) {
        return defaultValue;
    }
    return m_json->get<std::size_t>();
}

double JSONItem::toDouble(double defaultValue) const
{
    if (!m_json || !isNumber()) {
        return defaultValue;
    }
    return m_json->get<double>();
}

int JSONItem::arraySize() const
{
    if (!m_json || !isArray()) {
        return 0;
    }
    return static_cast<int>(m_json->size());
}

JSONItem JSONItem::arrayItem(int pos) const
{
    if (!m_json || !isArray() || pos < 0 || arraySize() <= pos) {
        return JSONItem(nullptr);
    }
    return JSONItem(m_root, &m_json->at(pos));
}

JSONItem JSONItem::operator[](int index) const { return arrayItem(index); }

std::vector<JSONItem> JSONItem::GetAsVector() const
{
    if (!m_json || !isArray()) {
        return {};
    }

    std::vector<JSONItem> res;
    res.reserve(arraySize());
    for (auto& child : *m_json) {
        res.emplace_back(JSONItem{m_root, &child});
    }
    return res;
}

wxArrayString JSONItem::toArrayString(const wxArrayString& defaultValue) const
{
    if (!m_json || !isArray()) {
        return defaultValue;
    }

    const int arr_size = arraySize();
    if (arr_size == 0) {
        return defaultValue;
    }

    wxArrayString arr;
    arr.reserve(arr_size);
    for (const auto& child : *m_json) {
        arr.push_back(wxString::FromUTF8(child.get<std::string>()));
    }
    return arr;
}

std::vector<double> JSONItem::toDoubleArray(const std::vector<double>& defaultValue) const
{
    if (!m_json || !isArray()) {
        return defaultValue;
    }

    const int arr_size = arraySize();
    if (arr_size == 0) {
        return defaultValue;
    }

    std::vector<double> arr;
    arr.reserve(arr_size);
    for (const auto& child : *m_json) {
        arr.push_back(child.get<double>());
    }
    return arr;
}

std::vector<int> JSONItem::toIntArray(const std::vector<int>& defaultValue) const
{
    if (!m_json || !isArray()) {
        return defaultValue;
    }

    const int arr_size = arraySize();
    if (arr_size == 0) {
        return defaultValue;
    }

    std::vector<int> arr;
    arr.reserve(arr_size);
    for (const auto& child : *m_json) {
        arr.push_back(child.get<int>());
    }
    return arr;
}

bool JSONItem::contains(const wxString& name) const
{
    if (!m_json || !isObject()) {
        return false;
    }
    const auto keyBuffer = name.mb_str(wxConvUTF8);
    return m_json->contains(keyBuffer.data());
}

JSONItem JSONItem::namedObject(const wxString& name) const
{
    if (!m_json || !isObject()) {
        return JSONItem(nullptr);
    }
    const auto keyBuffer = name.mb_str(wxConvUTF8);
    if (m_json->contains(keyBuffer.data())) {
        return {m_root, &m_json->at(keyBuffer.data())};
    } else {
        return JSONItem(nullptr);
    }
}

JSONItem JSONItem::operator[](const wxString& name) const { return namedObject(name); }

std::unordered_map<std::string_view, JSONItem> JSONItem::GetAsMap() const
{
    if (!m_json || !isObject()) {
        return {};
    }

    std::unordered_map<std::string_view, JSONItem> res;
    for (auto& [key, child] : m_json->items()) {
        res.emplace(key, JSONItem{m_root, &child});
    }
    return res;
}

wxStringMap_t JSONItem::toStringMap(const wxStringMap_t& default_map) const
{
    if (!m_json || !isArray()) {
        return default_map;
    }
    wxStringMap_t res;

    for (int i = 0; i < arraySize(); ++i) {
        wxString key = arrayItem(i).namedObject("key").toString();
        wxString val = arrayItem(i).namedObject("value").toString();
        res.emplace(key, val);
    }
    return res;
}

void JSONItem::arrayAppend(const char* value)
{
    if (!m_json) {
        return;
    }
    m_json->push_back(value);
}

void JSONItem::arrayAppend(const std::string& value) { arrayAppend(value.c_str()); }
void JSONItem::arrayAppend(const wxString& value) { arrayAppend(value.mb_str(wxConvUTF8).data()); }

void JSONItem::arrayAppend(double number)
{
    if (!m_json) {
        return;
    }
    m_json->push_back(number);
}

void JSONItem::arrayAppend(int number) { arrayAppend(static_cast<double>(number)); }

void JSONItem::arrayAppend(JSONItem&& element)
{
    if (!m_json) {
        return;
    }
    if (element.m_json) {
        m_json->push_back(*element.m_json);
        element.m_root = nullptr;
        element.m_json = nullptr;
    }
}

JSONItem& JSONItem::addNull(const wxString& name)
{
    if (m_json) {
        m_json->emplace(name.ToStdString(wxConvUTF8).data(), nullptr);
    }
    return *this;
}
JSONItem JSONItem::AddArray(const wxString& name)
{
    JSONItem json = createArray();
    addProperty(name, json);
    return namedObject(name);
}

JSONItem JSONItem::AddObject(const wxString& name)
{
    JSONItem json = createObject();
    addProperty(name, json);
    return namedObject(name);
}

JSONItem& JSONItem::addProperty(const wxString& name, bool value)
{
    if (m_json) {
        m_json->emplace(name.mb_str(wxConvUTF8).data(), value);
    }
    return *this;
}

JSONItem& JSONItem::addProperty(const wxString& name, const wxString& value)
{
    return addProperty(name, value.ToStdString(wxConvUTF8));
}

JSONItem& JSONItem::addProperty(const wxString& name, const std::string& value)
{
    if (m_json) {
        m_json->emplace(name.mb_str(wxConvUTF8).data(), value);
    }
    return *this;
}

JSONItem& JSONItem::addProperty(const wxString& name, const wxChar* value)
{
    return addProperty(name, wxString(value));
}

JSONItem& JSONItem::addProperty(const wxString& name, long value)
{
    if (m_json) {
        m_json->emplace(name.mb_str(wxConvUTF8).data(), value);
    }
    return *this;
}

JSONItem& JSONItem::addProperty(const wxString& name, const wxArrayString& arr)
{
    JSONItem arrEle = JSONItem::createArray();
    for (const auto& s : arr) {
        arrEle.arrayAppend(s);
    }
    addProperty(name, arrEle);
    return *this;
}

JSONItem& JSONItem::addProperty(const wxString& name, const JSONItem& element)
{
    if (!m_json || !element.m_json) {
        return *this;
    }
    (*m_json)[name.mb_str(wxConvUTF8).data()] = *element.m_json;
    return *this;
}

#if wxUSE_GUI
JSONItem& JSONItem::addProperty(const wxString& name, const wxStringMap_t& stringMap)
{
    if (!m_json)
        return *this;

    JSONItem arr = JSONItem::createArray();
    for (const auto& [key, value] : stringMap) {
        JSONItem obj = JSONItem::createObject();
        obj.addProperty("key", key);
        obj.addProperty("value", value);
        arr.arrayAppend(std::move(obj));
    }
    addProperty(name, arr);
    return *this;
}
#endif

JSONItem& JSONItem::addProperty(const wxString& name, size_t value) { return addProperty(name, (int)value); }

JSONItem& JSONItem::addProperty(const wxString& name, const char* value, const wxMBConv& conv)
{
    return addProperty(name, wxString(value, conv));
}

JSONItem& JSONItem::addProperty(const wxString& name, const wxFileName& filename)
{
    return addProperty(name, filename.GetFullPath());
}

JSONItem& JSONItem::addProperty(const wxString& name, JSON&& json)
{
    if (!m_json || !json.m_json) {
        return *this;
    }
    (*m_json)[name.mb_str(wxConvUTF8).data()] = std::move(*json.m_json);
    json.m_json = nullptr;
    return *this;
}

JSONItem& JSONItem::addProperty(const wxString& name, const std::vector<int>& arr_int)
{
    if (!m_json || !isObject()) {
        return *this;
    }

    // create array
    JSONItem arr = AddArray(name);
    for (size_t n : arr_int) {
        arr.m_json->push_back(n);
    }
    return *this;
}

void JSONItem::removeProperty(const wxString& name)
{
    // delete child property
    if (!m_json) {
        return;
    }
    m_json->erase(name.mb_str(wxConvUTF8).data());
}

JSONItem& JSONItem::addProperty(const wxString& name, const wxSize& sz)
{
    wxString szStr;
    szStr << sz.x << "," << sz.y;
    return addProperty(name, szStr);
}

wxSize JSONItem::toSize() const
{
    if (!m_json || !isString()) {
        return wxDefaultSize;
    }
    wxString str = toString();
    wxString x = str.BeforeFirst(',');
    wxString y = str.AfterFirst(',');

    long nX(-1), nY(-1);
    if (!x.ToLong(&nX) || !y.ToLong(&nY)) {
        return wxDefaultSize;
    }

    return wxSize(nX, nY);
}
