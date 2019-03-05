#ifndef NODEDBGREMOTEOBJECT_H
#define NODEDBGREMOTEOBJECT_H

#include "ObjectPreview.h"
#include "nSerializableObject.h"
#include <wx/string.h>

class RemoteObject : public nSerializableObject
{
    wxString m_type;
    wxString m_subtype;
    wxString m_className;
    wxString m_value;
    ObjectPreview m_preview;
    wxString m_expression; // The expression originated this object
    wxString m_objectId;

public:
    RemoteObject();
    virtual ~RemoteObject();

    void FromJSON(const JSONItem& json);
    JSONItem ToJSON(const wxString& name) const;
    bool IsObject() const { return GetType() == "object"; }
    bool IsString() const { return GetType() == "string"; }
    bool IsUndefined() const { return GetType() == "undefined"; }
    bool IsFunction() const { return GetType() == "function"; }

    void SetObjectId(const wxString& objectId) { this->m_objectId = objectId; }
    const wxString& GetObjectId() const { return m_objectId; }
    void SetClassName(const wxString& className) { this->m_className = className; }
    void SetSubtype(const wxString& subtype) { this->m_subtype = subtype; }
    void SetType(const wxString& type) { this->m_type = type; }
    void SetValue(const wxString& value) { this->m_value = value; }
    const wxString& GetClassName() const { return m_className; }
    const wxString& GetSubtype() const { return m_subtype; }
    const wxString& GetType() const { return m_type; }
    const wxString& GetValue() const { return m_value; }
    void SetExpression(const wxString& expression) { this->m_expression = expression; }
    const wxString& GetExpression() const { return m_expression; }
    void SetPreview(const ObjectPreview& preview) { this->m_preview = preview; }
    const ObjectPreview& GetPreview() const { return m_preview; }
    /**
     * @brief return a string representing this obejct for UI display (tooltip, console etc)
     */
    wxString ToString() const;
    bool IsEmpty() const { return m_type.IsEmpty(); }
    
    // For display purposes
    wxString GetTextPreview() const;
    bool HasChildren() const;
};

#endif // NODEDBGREMOTEOBJECT_H
