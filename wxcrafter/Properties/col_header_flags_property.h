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
    wxArrayString GetOptions() const override;
    wxArrayInt GetOptionsValues() const override;
    long GetValueLong() const override;

    ColHeaderFlagsProperty();
    ColHeaderFlagsProperty(const wxString& label, int initialValue, const wxString& tip,
                           eColumnKind kind = eColumnKind::kListCtrl);
    ~ColHeaderFlagsProperty() override = default;

public:
    PropertyeType GetType() override;
    wxString GetValue() const override;
    nlohmann::json Serialize() const override;
    void SetValue(const wxString& value) override;
    void UnSerialize(const nlohmann::json& json) override;
};

#endif // COLHEADERFLAGSPROPERTY_H
