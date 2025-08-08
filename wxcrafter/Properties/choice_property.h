#ifndef CHOICEPROPERTY_H
#define CHOICEPROPERTY_H

#include "property_base.h"
#include <wx/arrstr.h>

class ChoiceProperty : public PropertyBase
{
    wxArrayString m_options;
    int m_selection;

public:
    ChoiceProperty(const wxString& label, const wxArrayString& options, int selection, const wxString& tip);
    ChoiceProperty();
    ~ChoiceProperty() override = default;

    void SetOptions(const wxArrayString& options) { this->m_options = options; }
    void SetSelection(int selection);

    wxArrayString GetOptions() const override { return m_options; }
    int GetSelection() const { return m_selection; }
    bool IsEmpty() const { return m_options.IsEmpty(); }
    void Add(const wxString& value);

    wxString GetValue() const override;
    void SetValue(const wxString& value) override;

    // Serialization methods
    JSONElement Serialize() const override;
    void UnSerialize(const JSONElement& json) override;

    PropertyeType GetType() override { return PT_CHOICE; }
};

#endif // CHOICEPROPERTY_H
