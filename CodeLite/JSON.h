//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : json_node.h
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

#ifndef ZJSONNODE_H
#define ZJSONNODE_H

#include "codelite_exports.h"
#include "json_utils.h"
#include "macros.h"

#include <assistant/common/json.hpp>
#include <memory>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <vector>
#include <wx/arrstr.h>
#include <wx/filename.h>
#include <wx/gdicmn.h>
#include <wx/string.h>
#include <wx/vector.h>

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

class JSON;

class WXDLLIMPEXP_CL JSONItem
{
    friend JSON;

public:
    explicit JSONItem(std::nullptr_t) {}
    JSONItem(const nlohmann::json&);
    JSONItem(const nlohmann::ordered_json&);

    JSONItem() = default;
    virtual ~JSONItem() = default;

    // Readers
    ////////////////////////////////////////////////
    JSONItem namedObject(const wxString& name) const;
    /**
     * @brief Checks whether this JSON object contains a member with the specified name.
     *
     * @param name wxString The name of the member to search for in the JSON object.
     * @return bool true if the object contains a member with the given name; otherwise false.
     */
    bool contains(const wxString& name) const;

    /**
     * @brief An alias to the `contains` method above.
     */
    bool hasNamedObject(const wxString& name) const { return contains(name); }

    JSONItem operator[](int index) const;
    JSONItem operator[](const wxString& name) const;

    std::vector<JSONItem> GetAsVector() const;
    std::unordered_map<std::string_view, JSONItem> GetAsMap() const;

    bool toBool(bool defaultValue = false) const;
    wxString toString(const wxString& defaultValue = wxEmptyString) const;
    wxArrayString toArrayString(const wxArrayString& defaultValue = wxArrayString()) const;
    std::vector<double> toDoubleArray(const std::vector<double>& defaultValue = {}) const;
    std::vector<int> toIntArray(const std::vector<int>& defaultValue = {}) const;
    JSONItem arrayItem(int pos) const;

    // Return the object type
    bool isNull() const;
    bool isBool() const;
    bool isString() const;
    bool isNumber() const;
    bool isArray() const;
    bool isObject() const;

    wxString format(bool formatted = true) const;
    /**
     * @brief format the JSON into a raw c string
     * The caller should free the pointer (using free())
     */
    char* FormatRawString(bool formatted = true) const;
    int arraySize() const;
    int toInt(int defaultValue = -1) const;

    /// Convert the value into `E` from
    template <typename E>
        requires(std::is_enum_v<E>)
    E fromNumber(E defaultValue) const
    {
        return static_cast<E>(toInt(static_cast<int>(defaultValue)));
    }

    template <typename T>
    inline T GetValue() const
    {
        if constexpr (std::is_same_v<T, bool>) {
            return toBool();
        } else if constexpr (std::is_same_v<T, int>) {
            return toInt();
        } else if constexpr (std::is_same_v<T, size_t>) {
            return toSize_t();
        } else if constexpr (std::is_same_v<T, double>) {
            return toDouble();
        } else if constexpr (std::is_same_v<T, std::string>) {
            return toString().ToStdString(wxConvUTF8);
        } else if constexpr (std::is_same_v<T, wxString>) {
            return toString();
        } else if constexpr (std::is_same_v<T, wxArrayString>) {
            return toArrayString();
        } else {
            static_assert(!std::is_same_v<T, T>, "GetValue called with unsupported type.");
        }
    }
    size_t toSize_t(size_t defaultValue = 0) const;
    double toDouble(double defaultValue = -1.0) const;

    wxSize toSize() const;

    wxStringMap_t toStringMap(const wxStringMap_t& default_map = {}) const;

    // Writers
    ////////////////////////////////////////////////
    /**
     * @brief create new object
     * @return the newly created object
     */
    static JSONItem createObject();
    /**
     * @brief create new array
     * @return the newly created array
     */
    static JSONItem createArray();

    /**
     * @brief add array to this json and return a reference to the newly added array
     */
    JSONItem AddArray(const wxString& name);

    /**
     * @brief add object to this json and return a reference to the newly added object
     */
    JSONItem AddObject(const wxString& name);

    JSONItem& addNull(const wxString& name);

    JSONItem& addProperty(const wxString& name, const wxString& value);
    JSONItem& addProperty(const wxString& name, const std::string& value);
    JSONItem& addProperty(const wxString& name, const wxChar* value);
    JSONItem& addProperty(const wxString& name, int value) { return addProperty(name, (long)value); }
    JSONItem& addProperty(const wxString& name, long value);
    JSONItem& addProperty(const wxString& name, size_t value);
    JSONItem& addProperty(const wxString& name, bool value);
    JSONItem& addProperty(const wxString& name, JSON&& json);
    JSONItem& addProperty(const wxString& name, const wxFileName& filename);
    JSONItem& addProperty(const wxString& name, const std::vector<int>& arr_int);
    template <class T = int>
    typename std::enable_if<!std::is_same<wxVector<T>, std::vector<T>>::value, JSONItem&>::type
    addProperty(const wxString& name, const wxVector<T>& arr_int)
    {
        return addProperty(name, std::vector<T>(arr_int.begin(), arr_int.end()));
    }

    JSONItem& addProperty(const wxString& name, const wxSize& sz);

    JSONItem& addProperty(const wxString& name, const wxArrayString& arr);
    JSONItem& addProperty(const wxString& name, const wxStringMap_t& stringMap);
    JSONItem& addProperty(const wxString& name, const JSONItem& element);
    JSONItem& addProperty(const wxString& name, const char* value, const wxMBConv& conv = wxConvUTF8);

    /**
     * @brief delete property by name
     */
    void removeProperty(const wxString& name);

    //////////////////////////////////////////////////
    // Array operations
    //////////////////////////////////////////////////

    /**
     * @brief append new number
     * @return the newly added property
     */
    void arrayAppend(JSONItem&& element);
    void arrayAppend(const wxString& value);
    void arrayAppend(const char* value);
    void arrayAppend(const std::string& value);
    void arrayAppend(int number);
    void arrayAppend(double number);

    bool isOk() const { return m_json != nullptr; }

private:
    JSONItem(std::shared_ptr<nlohmann::ordered_json> root, nlohmann::ordered_json* current);

private:
    std::shared_ptr<nlohmann::ordered_json> m_root;
    nlohmann::ordered_json* m_json = nullptr;
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
enum class JsonType {
    Array,
    Null,
    Object,
};

class WXDLLIMPEXP_CL JSON
{
    friend JSONItem;

public:
    explicit JSON(JsonType type);
    explicit JSON(const wxString& text);
    explicit JSON(const wxFileName& filename);

    // Make this class not copyable
    JSON(const JSON&) = delete;
    JSON& operator=(const JSON&) = delete;
    ~JSON() = default;

    void save(const wxFileName& fn) const;
    bool isOk() const { return m_json != nullptr; }

    JSONItem toElement() const;

private:
    std::shared_ptr<nlohmann::ordered_json> m_json;
};

#endif // ZJSONNODE_H
