#ifndef OBJECTPREVIEW_H
#define OBJECTPREVIEW_H

#include "nSerializableObject.h"
#include <vector>

class PropertyPreview;
class ObjectPreview : public nSerializableObject
{
    wxString m_type;
    wxString m_subtype;
    wxString m_description;
    bool m_overflow = false;
    std::vector<PropertyPreview*> m_properties;

protected:
    void DeleteProperties();

public:
    virtual void FromJSON(const JSONItem& json);
    virtual JSONItem ToJSON(const wxString& name) const;

    ObjectPreview();
    virtual ~ObjectPreview();
    
    bool IsEmpty() const { return m_type.IsEmpty(); }
    
    void SetDescription(const wxString& description) { this->m_description = description; }
    void SetOverflow(bool overflow) { this->m_overflow = overflow; }
    void SetProperties(const std::vector<PropertyPreview*>& properties) { this->m_properties = properties; }
    void SetSubtype(const wxString& subtype) { this->m_subtype = subtype; }
    void SetType(const wxString& type) { this->m_type = type; }
    const wxString& GetDescription() const { return m_description; }
    bool IsOverflow() const { return m_overflow; }
    const std::vector<PropertyPreview*>& GetProperties() const { return m_properties; }
    const wxString& GetSubtype() const { return m_subtype; }
    const wxString& GetType() const { return m_type; }
    wxString ToString() const;
};

#endif // OBJECTPREVIEW_H
