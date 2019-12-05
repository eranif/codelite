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
// clang-format off
#include <wx/string.h>
#include <wx/variant.h>
#include <wx/filename.h>
#include <wx/gdicmn.h>
#include "codelite_exports.h"
#include <map>
#include "cJSON.h"
#if wxUSE_GUI
#include <wx/arrstr.h>
#include <wx/colour.h>
#include <wx/font.h>
#endif
#include "macros.h"
// clang-format on

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

class WXDLLIMPEXP_CL JSONItem
{
protected:
    cJSON* m_json = nullptr;
    cJSON* m_walker = nullptr;
    std::string m_name;
    int m_type = wxNOT_FOUND;

    // Values
    std::string m_valueString;
    double m_valueNumer = 0;

public:
    JSONItem(cJSON* json);
    JSONItem(const wxString& name, double val);
    JSONItem(const wxString& name, const std::string& val);
    JSONItem(const wxString& name, const char* pval, size_t len);
    JSONItem(const wxString& name, bool val);
    virtual ~JSONItem() {}

    // Walkers
    JSONItem firstChild();
    JSONItem nextChild();

    void SetValueNumer(double valueNumer) { this->m_valueNumer = valueNumer; }
    double GetValueNumer() const { return m_valueNumer; }

    void SetValueString(const std::string& valueString) { this->m_valueString = valueString; }
    const std::string& GetValueString() const { return m_valueString; }

    // Setters
    ////////////////////////////////////////////////
    void setName(const wxString& m_name) { this->m_name = m_name; }
    void setType(int m_type) { this->m_type = m_type; }
    int getType() const { return m_type; }
    wxString getName() const { return m_name; }

    // Readers
    ////////////////////////////////////////////////
    JSONItem namedObject(const wxString& name) const;
    bool hasNamedObject(const wxString& name) const;

    bool toBool(bool defaultValue = false) const;
    wxString toString(const wxString& defaultValue = wxEmptyString) const;
    wxArrayString toArrayString(const wxArrayString& defaultValue = wxArrayString()) const;
    JSONItem arrayItem(int pos) const;

    // Retuen the object type
    bool isNull() const;
    bool isBool() const;
    bool isString() const;
    bool isNumber() const;
    bool isArray() const;

    wxString format(bool formatted = true) const;
    /**
     * @brief format the JSON into a raw c string
     * The caller should free the pointer (using free())
     */
    char* FormatRawString(bool formatted = true) const;
    int arraySize() const;
    int toInt(int defaultVal = -1) const;
    size_t toSize_t(size_t defaultVal = 0) const;
    double toDouble(double defaultVal = -1.0) const;
    wxFileName toFileName() const;

#if wxUSE_GUI
    wxColour toColour(const wxColour& defaultColour = wxNullColour) const;
    wxFont toFont(const wxFont& defaultFont = wxNullFont) const;
    wxSize toSize() const;
    wxPoint toPoint() const;
#endif

    wxStringMap_t toStringMap() const;

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
    JSONItem& addProperty(const wxString& name, const wxFileName& filename);

#if wxUSE_GUI
    JSONItem& addProperty(const wxString& name, const wxSize& sz);
    JSONItem& addProperty(const wxString& name, const wxPoint& pt);
    JSONItem& addProperty(const wxString& name, const wxColour& colour);
    JSONItem& addProperty(const wxString& name, const wxFont& font);
#endif
    JSONItem& addProperty(const wxString& name, const wxArrayString& arr);
    JSONItem& addProperty(const wxString& name, const wxStringMap_t& stringMap);
    JSONItem& addProperty(const wxString& name, const JSONItem& element);
    JSONItem& addProperty(const wxString& name, const char* value, const wxMBConv& conv = wxConvUTF8);

    /**
     * @brief delete property by name
     */
    void removeProperty(const wxString& name);

    /**
     * @brief detach element from json. Return the detached element
     */
    JSONItem detachProperty(const wxString& name);

    //////////////////////////////////////////////////
    // Array operations
    //////////////////////////////////////////////////

    /**
     * @brief append new number
     * @return the newly added property
     */
    void arrayAppend(const JSONItem& element);
    void arrayAppend(const wxString& value);

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
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

class WXDLLIMPEXP_CL JSON
{
protected:
    cJSON* m_json;
    wxString _errorString;

public:
    JSON(int type);
    JSON(const wxString& text);
    JSON(const wxFileName& filename);
    JSON(JSONItem item);
    JSON(cJSON* json);
    virtual ~JSON();

    void save(const wxFileName& fn) const;
    wxString errorString() const;
    bool isOk() const { return m_json != NULL; }

    JSONItem toElement() const;

    void clear();
    cJSON* release();

private:
    // Make this class not copyable
    JSON(const JSON& src);
    JSON& operator=(const JSON& src);
};

#endif // ZJSONNODE_H
