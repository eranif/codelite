#ifndef NODEDBGREMOTEOBJECT_H
#define NODEDBGREMOTEOBJECT_H

#include "NodeJSONSerialisable.h"
#include <wx/string.h>

class RemoteObject : public clJSONObject
{
    wxString m_type;
    wxString m_subtype;
    wxString m_className;
    wxString m_value;

public:
    RemoteObject();
    virtual ~RemoteObject();

    void FromJSON(const JSONElement& json);
    JSONElement ToJSON(const wxString& name) const;

    void SetClassName(const wxString& className) { this->m_className = className; }
    void SetSubtype(const wxString& subtype) { this->m_subtype = subtype; }
    void SetType(const wxString& type) { this->m_type = type; }
    void SetValue(const wxString& value) { this->m_value = value; }
    const wxString& GetClassName() const { return m_className; }
    const wxString& GetSubtype() const { return m_subtype; }
    const wxString& GetType() const { return m_type; }
    const wxString& GetValue() const { return m_value; }
};

#endif // NODEDBGREMOTEOBJECT_H
