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

#include "clFontHelper.h"
#include "fileutils.h"
#include "JSON.h"
#include <stdlib.h>
#include <wx/ffile.h>
#include <wx/filename.h>

JSON::JSON(const wxString& text)
    : _json(NULL)
{
    _json = cJSON_Parse(text.mb_str(wxConvUTF8).data());
}

JSON::JSON(cJSON* json)
    : _json(json)
{
}

JSON::JSON(JSONItem item)
    : _json(item.release())
{
}

JSON::JSON(int type)
    : _json(NULL)
{
    if(type == cJSON_Array)
        _json = cJSON_CreateArray();
    else if(type == cJSON_NULL)
        _json = cJSON_CreateNull();
    else
        _json = cJSON_CreateObject();
}

JSON::JSON(const wxFileName& filename)
    : _json(NULL)
{
    wxString content;
    if(!FileUtils::ReadFileContent(filename, content)) { return; }
    _json = cJSON_Parse(content.mb_str(wxConvUTF8).data());
}

JSON::~JSON()
{
    if(_json) {
        cJSON_Delete(_json);
        _json = NULL;
    }
}

void JSON::save(const wxFileName& fn) const
{
    if(!isOk()) {
        FileUtils::WriteFileContent(fn, "[]");
    } else {
        FileUtils::WriteFileContent(fn, toElement().format(), wxConvUTF8);
    }
}

JSONItem JSON::toElement() const
{
    if(!_json) { return JSONItem(NULL); }
    return JSONItem(_json);
}

wxString JSON::errorString() const { return _errorString; }

JSONItem JSONItem::namedObject(const wxString& name) const
{
    if(!_json) { return JSONItem(NULL); }

    cJSON* obj = cJSON_GetObjectItem(_json, name.mb_str(wxConvUTF8).data());
    if(!obj) { return JSONItem(NULL); }
    return JSONItem(obj);
}

void JSON::clear()
{
    int type = cJSON_Object;
    if(_json) {
        type = _json->type;
        cJSON_Delete(_json);
        _json = NULL;
    }
    if(type == cJSON_Array)
        _json = cJSON_CreateArray();
    else
        _json = cJSON_CreateObject();
}

cJSON* JSON::release()
{
    cJSON* p = _json;
    _json = NULL;
    return p;
}

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
JSONItem::JSONItem(cJSON* json)
    : _json(json)
    , _type(-1)
    , _walker(NULL)
{
    if(_json) {
        _name = wxString(_json->string, wxConvUTF8);
        _type = _json->type;
    }
}

JSONItem::JSONItem(const wxString& name, const wxVariant& val, int type)
    : _json(NULL)
    , _type(type)
    , _walker(NULL)
{
    _value = val;
    _name = name;
}

JSONItem JSONItem::arrayItem(int pos) const
{
    if(!_json) { return JSONItem(NULL); }

    if(_json->type != cJSON_Array) return JSONItem(NULL);

    int size = cJSON_GetArraySize(_json);
    if(pos >= size) return JSONItem(NULL);

    return JSONItem(cJSON_GetArrayItem(_json, pos));
}

bool JSONItem::isNull() const
{
    if(!_json) { return false; }
    return _json->type == cJSON_NULL;
}

bool JSONItem::toBool(bool defaultValue) const
{
    if(!_json) { return defaultValue; }

    if(!isBool()) { return defaultValue; }

    return _json->type == cJSON_True;
}

wxString JSONItem::toString(const wxString& defaultValue) const
{
    if(!_json) { return defaultValue; }

    if(_json->type != cJSON_String) { return defaultValue; }

    return wxString(_json->valuestring, wxConvUTF8);
}

bool JSONItem::isBool() const
{
    if(!_json) { return false; }

    return _json->type == cJSON_True || _json->type == cJSON_False;
}

bool JSONItem::isString() const
{
    if(!_json) { return false; }

    return _json->type == cJSON_String;
}

void JSONItem::append(const JSONItem& element)
{
    if(!_json) { return; }

    switch(element.getType()) {
    case cJSON_False:
        cJSON_AddFalseToObject(_json, element.getName().mb_str(wxConvUTF8).data());
        break;

    case cJSON_True:
        cJSON_AddTrueToObject(_json, element.getName().mb_str(wxConvUTF8).data());
        break;

    case cJSON_NULL:
        cJSON_AddNullToObject(_json, element.getName().mb_str(wxConvUTF8).data());
        break;

    case cJSON_Number:
        cJSON_AddNumberToObject(_json, element.getName().mb_str(wxConvUTF8).data(), element.getValue().GetLong());
        break;

    case cJSON_String:
        cJSON_AddStringToObject(_json, element.getName().mb_str(wxConvUTF8).data(),
                                element.getValue().GetString().mb_str(wxConvUTF8).data());
        break;

    case cJSON_Array:
    case cJSON_Object:
        cJSON_AddItemToObject(_json, element.getName().mb_str(wxConvUTF8).data(), element._json);
        break;
    }
}

void JSONItem::arrayAppend(const JSONItem& element)
{
    if(!_json) { return; }

    cJSON* p = NULL;
    switch(element.getType()) {
    case cJSON_False:
        p = cJSON_CreateFalse();
        break;

    case cJSON_True:
        p = cJSON_CreateTrue();
        break;

    case cJSON_NULL:
        p = cJSON_CreateNull();
        break;

    case cJSON_Number:
        p = cJSON_CreateNumber(element.getValue().GetDouble());
        break;

    case cJSON_String:
        p = cJSON_CreateString(element.getValue().GetString().mb_str(wxConvUTF8).data());
        break;
    case cJSON_Array:
    case cJSON_Object:
        p = element._json;
        break;
    }
    if(p) { cJSON_AddItemToArray(_json, p); }
}

JSONItem JSONItem::createArray(const wxString& name)
{
    JSONItem arr(cJSON_CreateArray());
    arr.setName(name);
    arr.setType(cJSON_Array);
    return arr;
}

JSONItem JSONItem::createObject(const wxString& name)
{
    JSONItem obj(cJSON_CreateObject());
    obj.setName(name);
    obj.setType(cJSON_Object);
    return obj;
}

char* JSONItem::FormatRawString(bool formatted) const
{
    if(!_json) { return NULL; }

    if(formatted) {
        return cJSON_Print(_json);

    } else {
        return cJSON_PrintUnformatted(_json);
    }
}

wxString JSONItem::format(bool formatted) const
{
    if(!_json) { return wxT(""); }

    char* p = formatted ? cJSON_Print(_json) : cJSON_PrintUnformatted(_json);
    wxString output(p, wxConvUTF8);
    free(p);
    return output;
}

int JSONItem::toInt(int defaultVal) const
{
    if(!_json) { return defaultVal; }

    if(_json->type != cJSON_Number) { return defaultVal; }

    return _json->valueint;
}

size_t JSONItem::toSize_t(size_t defaultVal) const
{
    if(!_json) { return defaultVal; }

    if(_json->type != cJSON_Number) { return defaultVal; }

    return (size_t)_json->valueint;
}

double JSONItem::toDouble(double defaultVal) const
{
    if(!_json) { return defaultVal; }

    if(_json->type != cJSON_Number) { return defaultVal; }

    return _json->valuedouble;
}

int JSONItem::arraySize() const
{
    if(!_json) { return 0; }

    if(_json->type != cJSON_Array) return 0;

    return cJSON_GetArraySize(_json);
}

JSONItem& JSONItem::addProperty(const wxString& name, bool value)
{
    if(value) {
        append(JSONItem(name, value, cJSON_True));

    } else {
        append(JSONItem(name, value, cJSON_False));
    }
    return *this;
}

JSONItem& JSONItem::addProperty(const wxString& name, const wxString& value)
{
    append(JSONItem(name, value, cJSON_String));
    return *this;
}

JSONItem& JSONItem::addProperty(const wxString& name, const wxChar* value)
{
    append(JSONItem(name, wxString(value), cJSON_String));
    return *this;
}

JSONItem& JSONItem::addProperty(const wxString& name, long value)
{
    append(JSONItem(name, value, cJSON_Number));
    return *this;
}

JSONItem& JSONItem::addProperty(const wxString& name, const wxArrayString& arr)
{
    JSONItem arrEle = JSONItem::createArray(name);
    for(size_t i = 0; i < arr.GetCount(); i++) {
        arrEle.arrayAppend(arr.Item(i));
    }
    append(arrEle);
    return *this;
}

void JSONItem::arrayAppend(const wxString& value) { arrayAppend(JSONItem(wxT(""), value, cJSON_String)); }

wxArrayString JSONItem::toArrayString(const wxArrayString& defaultValue) const
{
    if(!_json) { return defaultValue; }

    if(_json->type != cJSON_Array) { return defaultValue; }

    wxArrayString arr;
    for(int i = 0; i < arraySize(); i++) {
        arr.Add(arrayItem(i).toString());
    }
    return arr;
}

bool JSONItem::hasNamedObject(const wxString& name) const
{
    if(!_json) { return false; }

    cJSON* obj = cJSON_GetObjectItem(_json, name.mb_str(wxConvUTF8).data());
    return obj != NULL;
}
#if wxUSE_GUI
JSONItem& JSONItem::addProperty(const wxString& name, const wxPoint& pt)
{
    wxString szStr;
    szStr << pt.x << "," << pt.y;
    return addProperty(name, szStr);
}

JSONItem& JSONItem::addProperty(const wxString& name, const wxSize& sz)
{
    wxString szStr;
    szStr << sz.x << "," << sz.y;
    return addProperty(name, szStr);
}

wxPoint JSONItem::toPoint() const
{
    if(!_json) { return wxDefaultPosition; }

    if(_json->type != cJSON_String) { return wxDefaultPosition; }

    wxString str = _json->valuestring;
    wxString x = str.BeforeFirst(',');
    wxString y = str.AfterFirst(',');

    long nX(-1), nY(-1);
    if(!x.ToLong(&nX) || !y.ToLong(&nY)) return wxDefaultPosition;

    return wxPoint(nX, nY);
}

wxColour JSONItem::toColour(const wxColour& defaultColour) const
{
    if(!_json) { return defaultColour; }

    if(_json->type != cJSON_String) { return defaultColour; }
    return wxColour(_json->valuestring);
}

wxSize JSONItem::toSize() const
{
    wxPoint pt = toPoint();
    return wxSize(pt.x, pt.y);
}
#endif

JSONItem& JSONItem::addProperty(const wxString& name, const JSONItem& element)
{
    if(!_json) { return *this; }
    cJSON_AddItemToObject(_json, name.mb_str(wxConvUTF8).data(), element._json);
    return *this;
}

void JSONItem::removeProperty(const wxString& name)
{
    // delete child property
    if(!_json) { return; }
    cJSON_DeleteItemFromObject(_json, name.mb_str(wxConvUTF8).data());
}
#if wxUSE_GUI
JSONItem& JSONItem::addProperty(const wxString& name, const wxStringMap_t& stringMap)
{
    if(!_json) return *this;

    JSONItem arr = JSONItem::createArray(name);
    wxStringMap_t::const_iterator iter = stringMap.begin();
    for(; iter != stringMap.end(); ++iter) {
        JSONItem obj = JSONItem::createObject();
        obj.addProperty("key", iter->first);
        obj.addProperty("value", iter->second);
        arr.arrayAppend(obj);
    }
    append(arr);
    return *this;
}
#endif
wxStringMap_t JSONItem::toStringMap() const
{
    wxStringMap_t res;
    if(!_json) { return res; }

    if(_json->type != cJSON_Array) { return res; }

    for(int i = 0; i < arraySize(); ++i) {
        wxString key = arrayItem(i).namedObject("key").toString();
        wxString val = arrayItem(i).namedObject("value").toString();
        res.insert(std::make_pair(key, val));
    }
    return res;
}
JSONItem& JSONItem::addProperty(const wxString& name, size_t value) { return addProperty(name, (int)value); }

#if wxUSE_GUI
JSONItem& JSONItem::addProperty(const wxString& name, const wxColour& colour)
{
    wxString colourValue;
    if(colour.IsOk()) { colourValue = colour.GetAsString(wxC2S_HTML_SYNTAX); }
    return addProperty(name, colourValue);
}
#endif
JSONItem JSONItem::firstChild()
{
    _walker = NULL;
    if(!_json) { return JSONItem(NULL); }

    if(!_json->child) { return JSONItem(NULL); }

    _walker = _json->child;
    return JSONItem(_walker);
}

JSONItem JSONItem::nextChild()
{
    if(!_walker) { return JSONItem(NULL); }

    JSONItem element(_walker->next);
    _walker = _walker->next;
    return element;
}
JSONItem& JSONItem::addProperty(const wxString& name, const char* value, const wxMBConv& conv)
{
    return addProperty(name, wxString(value, conv));
}

#if wxUSE_GUI
JSONItem& JSONItem::addProperty(const wxString& name, const wxFont& font)
{
    return addProperty(name, clFontHelper::ToString(font));
}

wxFont JSONItem::toFont(const wxFont& defaultFont) const
{
    wxString str = toString();
    if(str.IsEmpty()) return defaultFont;
    wxFont f = clFontHelper::FromString(str);
    return f;
}
#endif

bool JSONItem::isArray() const
{
    if(!_json) { return false; }
    return _json->type == cJSON_Array;
}

bool JSONItem::isNumber() const
{
    if(!_json) { return false; }
    return _json->type == cJSON_Number;
}

JSONItem JSONItem::detachProperty(const wxString& name)
{
    if(!_json) { return JSONItem(NULL); }
    cJSON* j = cJSON_DetachItemFromObject(_json, name.c_str());
    return JSONItem(j);
}
