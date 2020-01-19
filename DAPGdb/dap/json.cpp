#include "json.hpp"
#include <string>

namespace dap
{
JSON::JSON(const string& text)
    : m_json(NULL)
{
    m_json = cJSON_Parse(text.c_str());
}

JSON::JSON(cJSON* json)
    : m_json(json)
{
}

JSON::JSON(JSONItem item)
    : m_json(item.release())
{
}

JSON::JSON(int type)
    : m_json(NULL)
{
    if(type == cJSON_Array)
        m_json = cJSON_CreateArray();
    else if(type == cJSON_NULL)
        m_json = cJSON_CreateNull();
    else
        m_json = cJSON_CreateObject();
}

JSON::~JSON()
{
    if(m_json) {
        cJSON_Delete(m_json);
        m_json = NULL;
    }
}

JSONItem JSON::toElement() const
{
    if(!m_json) { return JSONItem(NULL); }
    return JSONItem(m_json);
}

string JSON::errorString() const { return _errorString; }

JSONItem JSONItem::property(const string& name) const
{
    if(!m_json) { return JSONItem(NULL); }

    cJSON* obj = cJSON_GetObjectItem(m_json, name.c_str());
    if(!obj) { return JSONItem(NULL); }
    return JSONItem(obj);
}

void JSON::clear()
{
    int type = cJSON_Object;
    if(m_json) {
        type = m_json->type;
        cJSON_Delete(m_json);
        m_json = NULL;
    }
    if(type == cJSON_Array)
        m_json = cJSON_CreateArray();
    else
        m_json = cJSON_CreateObject();
}

cJSON* JSON::release()
{
    cJSON* p = m_json;
    m_json = NULL;
    return p;
}

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
JSONItem::JSONItem(cJSON* json)
    : m_json(json)
{
    if(m_json) {
        if(m_json->string) { m_name = m_json->string; }
        m_type = m_json->type;
    }
}

JSONItem::JSONItem(const string& name, double val)
    : m_name(name)
    , m_type(cJSON_Number)
    , m_valueNumer(val)
{
}

JSONItem::JSONItem(const string& name, const std::string& val)
    : m_name(name)
    , m_type(cJSON_String)
{
    m_valueString.reserve(val.length() + 1);
    m_valueString.append(val);
}

JSONItem::JSONItem(const string& name, const char* pval, size_t len)
    : m_name(name)
    , m_type(cJSON_String)
{
    m_valueString.reserve(len + 1);
    m_valueString.append(pval);
}

JSONItem::JSONItem(const string& name, bool val)
    : m_name(name)
    , m_type(val ? cJSON_True : cJSON_False)
{
}

JSONItem JSONItem::arrayItem(int pos) const
{
    if(!m_json) { return JSONItem(NULL); }

    if(m_json->type != cJSON_Array) return JSONItem(NULL);

    int size = cJSON_GetArraySize(m_json);
    if(pos >= size) return JSONItem(NULL);

    return JSONItem(cJSON_GetArrayItem(m_json, pos));
}

bool JSONItem::isNull() const
{
    if(!m_json) { return false; }
    return m_json->type == cJSON_NULL;
}

bool JSONItem::toBool(bool defaultValue) const
{
    if(!m_json) { return defaultValue; }

    if(!isBool()) { return defaultValue; }

    return m_json->type == cJSON_True;
}

vector<string> JSONItem::toStringArray(const vector<string>& defaultValue) const
{
    if(!m_json) { return defaultValue; }
    if(m_json->type != cJSON_Array) { return defaultValue; }
    vector<string> v;
    int count = cJSON_GetArraySize(m_json);
    for(int i = 0; i < count; ++i) {
        cJSON* s = cJSON_GetArrayItem(m_json, i);
        if(s->type == cJSON_String && s->valuestring) { v.push_back(s->valuestring); }
    }
    return v;
}

string JSONItem::toString(const string& defaultValue) const
{
    if(!m_json) { return defaultValue; }
    if(m_json->type != cJSON_String) { return defaultValue; }
    return string(m_json->valuestring);
}

bool JSONItem::isBool() const
{
    if(!m_json) { return false; }

    return m_json->type == cJSON_True || m_json->type == cJSON_False;
}

bool JSONItem::isString() const
{
    if(!m_json) { return false; }

    return m_json->type == cJSON_String;
}

void JSONItem::append(const JSONItem& element)
{
    if(!m_json) { return; }

    switch(element.getType()) {
    case cJSON_False:
        cJSON_AddFalseToObject(m_json, element.getName().c_str());
        break;

    case cJSON_True:
        cJSON_AddTrueToObject(m_json, element.getName().c_str());
        break;

    case cJSON_NULL:
        cJSON_AddNullToObject(m_json, element.getName().c_str());
        break;

    case cJSON_Number:
        cJSON_AddNumberToObject(m_json, element.m_name.c_str(), element.m_valueNumer);
        break;

    case cJSON_String:
        cJSON_AddStringToObject(m_json, element.m_name.c_str(), element.m_valueString.c_str());
        break;

    case cJSON_Array:
    case cJSON_Object:
        cJSON_AddItemToObject(m_json, element.m_name.c_str(), element.m_json);
        break;
    }
}

void JSONItem::arrayAppend(const JSONItem& element)
{
    if(!m_json) { return; }

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
        p = cJSON_CreateNumber(element.m_valueNumer);
        break;

    case cJSON_String:
        p = cJSON_CreateString(element.m_valueString.c_str());
        break;
    case cJSON_Array:
    case cJSON_Object:
        p = element.m_json;
        break;
    }
    if(p) { cJSON_AddItemToArray(m_json, p); }
}

JSONItem JSONItem::createArray(const string& name)
{
    JSONItem arr(cJSON_CreateArray());
    arr.setName(name);
    arr.setType(cJSON_Array);
    return arr;
}

JSONItem JSONItem::createObject(const string& name)
{
    JSONItem obj(cJSON_CreateObject());
    obj.setName(name);
    obj.setType(cJSON_Object);
    return obj;
}

char* JSONItem::FormatRawString(bool formatted) const
{
    if(!m_json) { return NULL; }

    if(formatted) {
        return cJSON_Print(m_json);

    } else {
        return cJSON_PrintUnformatted(m_json);
    }
}

string JSONItem::Format(bool formatted) const
{
    if(!m_json) { return ""; }

    char* p = formatted ? cJSON_Print(m_json) : cJSON_PrintUnformatted(m_json);
    string s = p;
    free(p);
    return s;
}

int JSONItem::toInt(int defaultVal) const
{
    if(!m_json) { return defaultVal; }

    if(m_json->type != cJSON_Number) { return defaultVal; }

    return m_json->valueint;
}

size_t JSONItem::toSize_t(size_t defaultVal) const
{
    if(!m_json) { return defaultVal; }

    if(m_json->type != cJSON_Number) { return defaultVal; }

    return (size_t)m_json->valueint;
}

double JSONItem::toDouble(double defaultVal) const
{
    if(!m_json) { return defaultVal; }

    if(m_json->type != cJSON_Number) { return defaultVal; }

    return m_json->valuedouble;
}

int JSONItem::arraySize() const
{
    if(!m_json) { return 0; }

    if(m_json->type != cJSON_Array) return 0;

    return cJSON_GetArraySize(m_json);
}

JSONItem& JSONItem::add(const string& name, bool value)
{
    append(JSONItem(name, value));
    return *this;
}

JSONItem& JSONItem::add(const string& name, const std::string& value)
{
    append(JSONItem(name, value));
    return *this;
}

JSONItem& JSONItem::add(const string& name, long value)
{
    append(JSONItem(name, (double)value));
    return *this;
}

void JSONItem::arrayAppend(const string& value) { arrayAppend(JSONItem("", value)); }

bool JSONItem::hasProperty(const string& name) const
{
    if(!m_json) { return false; }
    cJSON* obj = cJSON_GetObjectItem(m_json, name.c_str());
    return obj != NULL;
}

JSONItem& JSONItem::add(const string& name, const JSONItem& element)
{
    if(!m_json) { return *this; }
    cJSON_AddItemToObject(m_json, name.c_str(), element.m_json);
    return *this;
}

void JSONItem::removeProperty(const string& name)
{
    // delete child property
    if(!m_json) { return; }
    cJSON_DeleteItemFromObject(m_json, name.c_str());
}

JSONItem& JSONItem::add(const string& name, size_t value) { return add(name, (int)value); }

JSONItem JSONItem::firstChild()
{
    m_walker = NULL;
    if(!m_json) { return JSONItem(NULL); }

    if(!m_json->child) { return JSONItem(NULL); }

    m_walker = m_json->child;
    return JSONItem(m_walker);
}

JSONItem JSONItem::nextChild()
{
    if(!m_walker) { return JSONItem(NULL); }

    JSONItem element(m_walker->next);
    m_walker = m_walker->next;
    return element;
}

JSONItem& JSONItem::add(const string& name, const char* value) { return add(name, string(value)); }

bool JSONItem::isArray() const
{
    if(!m_json) { return false; }
    return m_json->type == cJSON_Array;
}

bool JSONItem::isObject() const
{
    if(!m_json) { return false; }
    return m_json->type == cJSON_Object;
}

bool JSONItem::isNumber() const
{
    if(!m_json) { return false; }
    return m_json->type == cJSON_Number;
}

JSONItem JSONItem::detachProperty(const string& name)
{
    if(!m_json) { return JSONItem(NULL); }
    cJSON* j = cJSON_DetachItemFromObject(m_json, name.c_str());
    return JSONItem(j);
}

JSONItem& JSONItem::add(const string& name, const vector<string>& arr)
{
    auto a = createArray(name);
    add(a);
    for(const auto& s : arr) {
        a.arrayAppend(s);
    }
    return *this;
}

}; // namespace dap
