#include "ObjectPreview.h"
#include "PropertyPreview.h"

ObjectPreview::~ObjectPreview() { DeleteProperties(); }

void ObjectPreview::FromJSON(const JSONItem& json)
{
    m_type = json.namedObject("type").toString();
    m_subtype = json.namedObject("subtype").toString();
    m_description = json.namedObject("description").toString();
    m_overflow = json.namedObject("overflow").toBool(m_overflow);

    DeleteProperties();
    if(json.hasNamedObject("properties")) {
        JSONItem props = json.namedObject("properties");
        int size = props.arraySize();
        for(int i = 0; i < size; ++i) {
            JSONItem prop = props.arrayItem(i);
            PropertyPreview* pp = new PropertyPreview();
            pp->FromJSON(prop);
            m_properties.push_back(pp);
        }
    }
}

JSONItem ObjectPreview::ToJSON(const wxString& name) const
{
    JSONItem json = JSONItem::createObject(name);
    json.addProperty("type", m_type);
    json.addProperty("subtype", m_subtype);
    json.addProperty("description", m_description);
    json.addProperty("overflow", m_overflow);
    if(!m_properties.empty()) {
        JSONItem arr = JSONItem::createArray("properties");
        for (const auto* property : m_properties) {
            arr.arrayAppend(property->ToJSON(""));
        }
    }
    return json;
}

void ObjectPreview::DeleteProperties()
{
    for (auto* property : m_properties) {
        wxDELETE(property);
    }
    m_properties.clear();
}

wxString ObjectPreview::ToString() const
{
    wxString str;
    if(IsEmpty()) {
        return "{...}";
    } else {
        str << GetType();
        if(!GetSubtype().IsEmpty()) { str << " (" << GetSubtype() << ")"; }
        if(!GetProperties().empty()) {
            str << ": [";
            for (const auto* property : m_properties) {
                str << property->ToString() << ", ";
            }
        }
        if(IsOverflow()) {
            str << "...";
        } else {
            // remove the last comma we added
            str.RemoveLast(2);
        }
        if(!GetProperties().empty()) { str << "]"; }
        return str;
    }
}
