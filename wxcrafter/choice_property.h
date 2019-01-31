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
    virtual ~ChoiceProperty();

    void SetOptions(const wxArrayString& options) { this->m_options = options; }
    void SetSelection(int selection);

    virtual wxArrayString GetOptions() const { return m_options; }
    int GetSelection() const { return m_selection; }
    bool IsEmpty() const { return m_options.IsEmpty(); }
    void Add(const wxString& value);

    virtual wxString GetValue() const;
    virtual void SetValue(const wxString& value);

    // Serialization methods
    virtual JSONElement Serialize() const;
    virtual void UnSerialize(const JSONElement& json);

    virtual PropertyeType GetType() { return PT_CHOICE; }
};

#endif // CHOICEPROPERTY_H
