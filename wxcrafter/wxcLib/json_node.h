//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
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

#ifndef WXC_JSONNODE_H
#define WXC_JSONNODE_H

#include <wx/string.h>
#include <wx/variant.h>
#include <wx/filename.h>
#include <wx/arrstr.h>
#include <wx/gdicmn.h>
#include <map>
#include "cJSON.h"
#include <wx/colour.h>
#include "../CodeLite/macros.h"

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

class JSONElement
{
protected:
    cJSON* _json;
    int _type;
    wxString _name;

    // Values
    wxVariant _value;
    cJSON* _walker;

public:
    JSONElement(cJSON* json);
    JSONElement(const wxString& name, const wxVariant& val, int type);

    virtual ~JSONElement() {}

    // Walkers
    JSONElement firstChild();
    JSONElement nextChild();

    // Setters
    ////////////////////////////////////////////////
    void setName(const wxString& _name) { this->_name = _name; }
    void setType(int _type) { this->_type = _type; }
    int getType() const { return _type; }
    const wxString& getName() const { return _name; }
    const wxVariant& getValue() const { return _value; }
    void setValue(const wxVariant& _value) { this->_value = _value; }
    // Readers
    ////////////////////////////////////////////////
    JSONElement namedObject(const wxString& name) const;
    bool hasNamedObject(const wxString& name) const;

    bool toBool(bool defaultValue = false) const;
    wxString toString(const wxString& defaultValue = wxEmptyString) const;
    wxArrayString toArrayString() const;
    JSONElement arrayItem(int pos) const;
    bool isNull() const;
    bool isBool() const;
    bool isString() const;
    wxString format() const;
    /**
     * @brief format the JSON into a raw c string
     * The caller should free the pointer (using free())
     */
    char* FormatRawString(bool formatted = true) const;
    int arraySize() const;
    int toInt(int defaultVal = -1) const;
    size_t toSize_t(size_t defaultVal = 0) const;
    double toDouble(double defaultVal = -1.0) const;
    wxSize toSize() const;
    wxPoint toPoint() const;
    wxColour toColour(const wxColour& defaultColour = wxNullColour) const;
    wxStringMap_t toStringMap() const;

    // Writers
    ////////////////////////////////////////////////
    /**
     * @brief create new named object and append it to this json element
     * @return the newly created object
     */
    static JSONElement createObject(const wxString& name = wxT(""));
    /**
     * @brief create new named array and append it to this json element
     * @return the newly created array
     */
    static JSONElement createArray(const wxString& name = wxT(""));

    /**
     * @brief append new element to this json element
     */
    void append(const JSONElement& element);

    JSONElement& addProperty(const wxString& name, const wxString& value);
    JSONElement& addProperty(const wxString& name, const wxChar* value);
    JSONElement& addProperty(const wxString& name, int value) { return addProperty(name, (long)value); }
    JSONElement& addProperty(const wxString& name, long value);
    JSONElement& addProperty(const wxString& name, size_t value);
    JSONElement& addProperty(const wxString& name, bool value);
    JSONElement& addProperty(const wxString& name, const wxSize& sz);
    JSONElement& addProperty(const wxString& name, const wxPoint& pt);
    JSONElement& addProperty(const wxString& name, const wxColour& colour);
    JSONElement& addProperty(const wxString& name, const wxArrayString& arr);
    JSONElement& addProperty(const wxString& name, const wxStringMap_t& stringMap);
    JSONElement& addProperty(const wxString& name, const JSONElement& element);
    JSONElement& addProperty(const wxString& name, const char* value, const wxMBConv& conv = wxConvUTF8);

    /**
     * @brief delete property by name
     */
    void removeProperty(const wxString& name);

    /**
     * @brief detach element from json. Return the detached element
     */
    JSONElement detachProperty(const wxString& name);

    //////////////////////////////////////////////////
    // Array operations
    //////////////////////////////////////////////////

    /**
     * @brief append new number
     * @return the newly added property
     */
    void arrayAppend(const JSONElement& element);
    void arrayAppend(const wxString& value);

    bool isOk() const { return _json != NULL; }
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

class JSONRoot
{
protected:
    cJSON* _json;
    wxString _errorString;

public:
    JSONRoot(int type);
    JSONRoot(const wxString& text);
    JSONRoot(const wxFileName& filename);
    virtual ~JSONRoot();

    void save(const wxFileName& fn) const;
    wxString errorString() const;
    bool isOk() const { return _json != NULL; }

    JSONElement toElement() const;

    void clear();
    /**
     * @brief release the internal pointer and return it wrapped by JSONElement
     */
    JSONElement release();

private:
    // Make this class not copyable
    JSONRoot(const JSONRoot& src);
    JSONRoot& operator=(const JSONRoot& src);
};

#endif // WXC_JSONNODE_H
