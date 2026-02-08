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
#include "macros.h"

#include <cJSON.h>
#include <map>
#include <string_view>
#include <type_traits>
#include <vector>
#include <wx/filename.h>
#include <wx/gdicmn.h>
#include <wx/string.h>
#include <wx/variant.h>
#include <wx/vector.h>

#if wxUSE_GUI
#include <wx/arrstr.h>
#include <wx/colour.h>
#include <wx/font.h>
#endif

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

class WXDLLIMPEXP_CL JSONItem
{
public:
    explicit JSONItem(cJSON* json);
    JSONItem() = default;
    virtual ~JSONItem() = default;

    // Readers
    ////////////////////////////////////////////////
    JSONItem namedObject(const wxString& name) const;
    bool hasNamedObject(const wxString& name) const;

    /// If your array is big (hundred of entries) use
    /// `GetAsVector` and iterate it instead
    JSONItem operator[](int index) const;
    JSONItem operator[](const wxString& name) const;

    /// the C implementation for accessing large arrays, is the sum of an arithmetic progression.
    /// Use this method to get an array with `O(1)` access
    /// This call is `O(n)`
    std::vector<JSONItem> GetAsVector() const;

    /// the C implementation for accessing by name is by `O(n)`
    /// Use this method when you have large number of items and
    /// `O(1)` is required
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
    int toInt(int defaultVal = -1) const;

    /// Convert the value into `T` from
    template <typename T>
    T fromNumber(T default_value) const
    {
        return static_cast<T>(toInt((int)default_value));
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
    size_t toSize_t(size_t defaultVal = 0) const;
    double toDouble(double defaultVal = -1.0) const;

    wxColour toColour(const wxColour& defaultColour = wxNullColour) const;
    wxFont toFont(const wxFont& defaultFont = wxNullFont) const;
    wxSize toSize() const;
    wxPoint toPoint() const;

    wxStringMap_t toStringMap(const wxStringMap_t& default_map = {}) const;

    // Writers
    ////////////////////////////////////////////////
    /**
     * @brief create new named object and append it to this json element
     * @return the newly created object
     */
    static JSONItem createObject(const wxString& name = wxT(""));
    /**
     * @brief create new named array and append it to this json element
     * @return the newly created array
     */
    static JSONItem createArray(const wxString& name = wxT(""));

    /**
     * @brief add array to this json and return a reference to the newly added array
     */
    JSONItem AddArray(const wxString& name);

    /**
     * @brief add object to this json and return a reference to the newly added object
     */
    JSONItem AddObject(const wxString& name);

    /**
     * @brief append new element to this json element
     */
    void append(const JSONItem& element);

    JSONItem& addProperty(const wxString& name, const wxString& value);
    JSONItem& addProperty(const wxString& name, const std::string& value);
    JSONItem& addProperty(const wxString& name, const wxChar* value);
    JSONItem& addProperty(const wxString& name, int value) { return addProperty(name, (long)value); }
    JSONItem& addProperty(const wxString& name, long value);
    JSONItem& addProperty(const wxString& name, size_t value);
    JSONItem& addProperty(const wxString& name, bool value);
    JSONItem& addProperty(const wxString& name, cJSON* pjson);
    JSONItem& addProperty(const wxString& name, const wxFileName& filename);
    JSONItem& addProperty(const wxString& name, const std::vector<int>& arr_int);
    template <class T = int>
    typename std::enable_if<!std::is_same<wxVector<T>, std::vector<T>>::value, JSONItem&>::type
    addProperty(const wxString& name, const wxVector<T>& arr_int)
    {
        return addProperty(name, std::vector<T>(arr_int.begin(), arr_int.end()));
    }

    JSONItem& addProperty(const wxString& name, const wxSize& sz);
    JSONItem& addProperty(const wxString& name, const wxPoint& pt);
    JSONItem& addProperty(const wxString& name, const wxColour& colour);
    JSONItem& addProperty(const wxString& name, const wxFont& font);

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
    void arrayAppend(const JSONItem& element);
    void arrayAppend(const wxString& value);
    void arrayAppend(const char* value);
    void arrayAppend(const std::string& value);
    void arrayAppend(int number);
    void arrayAppend(double number);

    bool isOk() const { return m_json != NULL; }

    /**
     * @brief release the internal pointer
     */
    cJSON* release()
    {
        cJSON* temp = m_json;
        m_json = nullptr;
        return temp;
    }

private:
    const wxString& GetPropertyName() const { return m_propertyName; }
    void SetPropertyName(const wxString& name) { m_propertyName = name; }
    int getType() const { return m_type; }
    void setType(int m_type) { this->m_type = m_type; }

private:
    cJSON* m_json = nullptr;
    wxString m_propertyName;
    int m_type = wxNOT_FOUND;

    // Values
    wxString m_valueString;
    double m_valueNumer = 0;
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
enum class JsonType
{
    Array,
    Null,
    Object,
};

class WXDLLIMPEXP_CL JSON
{
public:
    explicit JSON(JsonType type);
    explicit JSON(const wxString& text);
    explicit JSON(const wxFileName& filename);
    explicit JSON(JSONItem item);
    explicit JSON(cJSON* json);

    // Make this class not copyable
    JSON(const JSON&) = delete;
    JSON& operator=(const JSON&) = delete;
    virtual ~JSON();

    void save(const wxFileName& fn) const;
    bool isOk() const { return m_json != NULL; }

    JSONItem toElement() const;

    void clear();
    cJSON* release();

protected:
    cJSON* m_json = nullptr;
};

#endif // ZJSONNODE_H
