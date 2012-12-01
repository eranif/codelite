#include "json_node.h"
#include <stdlib.h>
#include <wx/filename.h>
#include <wx/ffile.h>

JSONRoot::JSONRoot(const wxString& text)
    : _json(NULL)
{
    _json = cJSON_Parse(text.mb_str(wxConvUTF8).data());
}

JSONRoot::JSONRoot(int type)
    : _json(NULL)
{
    if(type == cJSON_Array)
        _json = cJSON_CreateArray();
    else
        _json = cJSON_CreateObject();
}


JSONRoot::JSONRoot(const wxFileName& filename)
    : _json(NULL)
{
    wxString content;
    wxFFile fp(filename.GetFullPath(), wxT("r+b"));
    if( fp.IsOpened() ) {
        if( fp.ReadAll( &content, wxConvUTF8 ) ) {
            _json = cJSON_Parse( content.mb_str(wxConvUTF8).data() );
        }
    }
}

JSONRoot::~JSONRoot()
{
    if(_json) {
        cJSON_Delete(_json);
        _json = NULL;
    }
}

void JSONRoot::save(const wxFileName& fn)
{
    wxFFile fp( fn.GetFullPath(), wxT("w+b") );
    if( fp.IsOpened() ) {
        fp.Write( toElement().format(), wxConvUTF8 );
        fp.Close();
    }
}

JSONElement JSONRoot::toElement() const 
{
    if(!_json) {
        return JSONElement(NULL);
    }
    return JSONElement(_json);
}

wxString JSONRoot::errorString() const
{
    return _errorString;
}

JSONElement JSONElement::namedObject(const wxString& name) const 
{
    if(!_json) {
        return JSONElement(NULL);
    }

    cJSON *obj = cJSON_GetObjectItem(_json, name.mb_str(wxConvUTF8).data());
    if(!obj) {
        return JSONElement(NULL);
    }
    return JSONElement(obj);
}

void JSONRoot::clear()
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

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
JSONElement::JSONElement(cJSON* json)
    : _json(json)
    , _type(-1)
{
    if(_json) {
        _name = wxString(_json->string, wxConvUTF8);
        _type = _json->type;
    }
}

JSONElement::JSONElement(const wxString &name, const wxVariant& val, int type)
    : _json(NULL)
    , _type(type)
{
    _value = val;
    _name = name;
}

JSONElement JSONElement::arrayItem(int pos) const 
{
    if(!_json) {
        return JSONElement(NULL);
    }

    if(_json->type != cJSON_Array)
        return JSONElement(NULL);

    int size = cJSON_GetArraySize(_json);
    if(pos >= size)
        return JSONElement(NULL);

    return JSONElement(cJSON_GetArrayItem(_json, pos));
}

bool JSONElement::isNull() const 
{
    if(!_json) {
       return false;
    }
    return _json->type == cJSON_NULL;
}

bool JSONElement::toBool() const 
{
    if(!_json) {
        return false;
    }

    if(!isBool()) {
        return false;
    }

    return _json->type == cJSON_True;
}

wxString JSONElement::toString() const 
{
    if(!_json) {
        return wxT("");
    }

    if(_json->type != cJSON_String) {
        return wxT("");
    }

    return wxString(_json->valuestring, wxConvUTF8);
}

bool JSONElement::isBool() const 
{
    if(!_json) {
        return false;
    }

    return _json->type == cJSON_True || _json->type == cJSON_False;
}

void JSONElement::append(const JSONElement& element) 
{
    if(!_json) {
        return;
    }

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
        cJSON_AddStringToObject(_json, element.getName().mb_str(wxConvUTF8).data(), element.getValue().GetString().mb_str(wxConvUTF8).data());
        break;

    case cJSON_Array:
    case cJSON_Object:
        cJSON_AddItemToObject(_json, element.getName().mb_str(wxConvUTF8).data(), element._json);
        break;
    }
}

void JSONElement::arrayAppend(const JSONElement& element) 
{
    if(!_json) {
       return;
    }

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
    if(p) {
        cJSON_AddItemToArray(_json, p);

    }
}

JSONElement JSONElement::createArray(const wxString &name) 
{
    JSONElement arr(cJSON_CreateArray());
    arr.setName(name);
    arr.setType(cJSON_Array);
    return arr;
}

JSONElement JSONElement::createObject(const wxString &name) 
{
    JSONElement obj(cJSON_CreateObject());
    obj.setName(name);
    obj.setType(cJSON_Object);
    return obj;
}

wxString JSONElement::format() const 
{
    if(!_json) {
        return wxT("");
    }

    char *p = cJSON_Print(_json);
    wxString output(p, wxConvUTF8);
    free(p);
    return output;
}

int JSONElement::toInt(int defaultVal) const
{
    if(!_json) {
        return defaultVal;
    }

    if(_json->type != cJSON_Number) {
        return defaultVal;
    }

    return _json->valueint;
}

double JSONElement::toDouble(double defaultVal) const
{
    if(!_json) {
        return defaultVal;
    }

    if(_json->type != cJSON_Number) {
        return defaultVal;
    }

    return _json->valuedouble;
}

int JSONElement::arraySize() const
{
    if(!_json) {
        return 0;
    }

    if(_json->type != cJSON_Array)
        return 0;

    return cJSON_GetArraySize(_json);
}

JSONElement& JSONElement::addProperty(const wxString& name, bool value)
{
    if(value) {
        append(JSONElement(name, value, cJSON_True));
        
    } else {
        append(JSONElement(name, value, cJSON_False));
    }
    return *this;
}

JSONElement& JSONElement::addProperty(const wxString& name, const wxString& value)
{
    append(JSONElement(name, value, cJSON_String));
    return *this;
}

JSONElement& JSONElement::addProperty(const wxString& name, const wxChar* value)
{
    append(JSONElement(name, wxString(value), cJSON_String));
    return *this;
}

JSONElement& JSONElement::addProperty(const wxString& name, int value)
{
    append(JSONElement(name, value, cJSON_Number));
    return *this;
}

JSONElement& JSONElement::addProperty(const wxString& name, const wxArrayString& arr)
{
    JSONElement arrEle = JSONElement::createArray(name);
    for(size_t i=0; i<arr.GetCount(); i++) {
        arrEle.arrayAppend(arr.Item(i));
    }
    append(arrEle);
    return *this;
}

void JSONElement::arrayAppend(const wxString& value)
{
    arrayAppend(JSONElement(wxT(""), value, cJSON_String));
}

wxArrayString JSONElement::toArrayString() const
{
    wxArrayString arr;
    if(!_json) {
        return arr;
    }

    if(_json->type != cJSON_Array) {
        return arr;
    }
    
    for(int i=0; i<arraySize(); i++) {
        arr.Add(arrayItem(i).toString());
    }
    return arr;
}

bool JSONElement::hasNamedObject(const wxString& name) const
{
    if(!_json) {
        return false;
    }

    cJSON *obj = cJSON_GetObjectItem(_json, name.mb_str(wxConvUTF8).data());
    return obj != NULL;
}

JSONElement& JSONElement::addProperty(const wxString& name, const wxPoint& pt)
{
    wxString szStr;
    szStr << pt.x << "," << pt.y;
    return addProperty(name, szStr);
}

JSONElement& JSONElement::addProperty(const wxString& name, const wxSize& sz)
{
    wxString szStr;
    szStr << sz.x << "," << sz.y;
    return addProperty(name, szStr);

}

wxPoint JSONElement::toPoint() const
{
    if(!_json) {
        return wxDefaultPosition;
    }

    if(_json->type != cJSON_String) {
        return wxDefaultPosition;
    }

    wxString str = _json->valuestring;
    wxString x = str.BeforeFirst(',');
    wxString y = str.AfterFirst(',');
    
    long nX(-1), nY(-1);
    if ( !x.ToLong(&nX) || !y.ToLong(&nY) )
        return wxDefaultPosition;
    
    return wxPoint(nX, nY);
}

wxSize JSONElement::toSize() const
{
    wxPoint pt = toPoint();
    return wxSize(pt.x, pt.y);
}

