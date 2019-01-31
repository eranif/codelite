#ifndef COLHEADERFLAGSPROPERTY_H
#define COLHEADERFLAGSPROPERTY_H

#include "property_base.h" // Base class: PropertyBase

enum class eColumnKind {
    kDataView,
    kListCtrl,
};

class ColHeaderFlagsProperty : public PropertyBase
{
    wxArrayString m_names;
    wxArrayInt m_values;
    int m_initialValue;

public:
    virtual wxArrayString GetOptions() const;
    virtual wxArrayInt GetOptionsValues() const;
    virtual long GetValueLong() const;

    ColHeaderFlagsProperty();
    ColHeaderFlagsProperty(const wxString& label, int initialValue, const wxString& tip,
                           eColumnKind kind = eColumnKind::kListCtrl);
    virtual ~ColHeaderFlagsProperty();

public:
    virtual PropertyeType GetType();
    virtual wxString GetValue() const;
    virtual JSONElement Serialize() const;
    virtual void SetValue(const wxString& value);
    virtual void UnSerialize(const JSONElement& json);
};

#endif // COLHEADERFLAGSPROPERTY_H
