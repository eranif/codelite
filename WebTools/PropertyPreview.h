#ifndef PROPERTYPREVIEW_H
#define PROPERTYPREVIEW_H

#include "nSerializableObject.h"
class ObjectPreview;
class PropertyPreview : public nSerializableObject
{
    wxString m_name;
    wxString m_type;
    wxString m_value;
    ObjectPreview* m_valuePreview = nullptr;
    wxString m_subtype; // "object" types only

public:
    virtual void FromJSON(const JSONItem& json);
    virtual JSONItem ToJSON(const wxString& name) const;
    PropertyPreview();
    virtual ~PropertyPreview();
    void SetName(const wxString& name) { this->m_name = name; }
    void SetSubtype(const wxString& subtype) { this->m_subtype = subtype; }
    void SetType(const wxString& type) { this->m_type = type; }
    void SetValue(const wxString& value) { this->m_value = value; }
    void SetValuePreview(ObjectPreview* valuePreview) { this->m_valuePreview = valuePreview; }
    const wxString& GetName() const { return m_name; }
    const wxString& GetSubtype() const { return m_subtype; }
    const wxString& GetType() const { return m_type; }
    const wxString& GetValue() const { return m_value; }
    ObjectPreview* GetValuePreview() const { return m_valuePreview; }
    bool IsObject() const { return GetType() == "object"; }
    bool IsString() const { return GetType() == "string"; }
    bool IsUndefined() const { return GetType() == "undefined"; }
    bool IsEmpty() const { return m_type.IsEmpty(); }
    wxString ToString() const;
};

#endif // PROPERTYPREVIEW_H
