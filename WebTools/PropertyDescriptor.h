#ifndef PROPERTYDESCRIPTOR_H
#define PROPERTYDESCRIPTOR_H

#include "RemoteObject.h"
#include "nSerializableObject.h"

class PropertyDescriptor : public nSerializableObject
{
    wxString m_name;
    RemoteObject m_value;

public:
    virtual void FromJSON(const JSONItem& json);
    virtual JSONItem ToJSON(const wxString& name) const;
    PropertyDescriptor();
    void SetName(const wxString& name) { this->m_name = name; }
    void SetValue(const RemoteObject& value) { this->m_value = value; }
    const wxString& GetName() const { return m_name; }
    const RemoteObject& GetValue() const { return m_value; }
    virtual ~PropertyDescriptor();
    wxString ToString() const;
    bool IsEmpty() const;
    wxString GetTextPreview() const;
    bool HasChildren() const;
};

#endif // PROPERTYDESCRIPTOR_H
