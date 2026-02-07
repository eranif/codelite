#ifndef COMBOXPROPERTY_H
#define COMBOXPROPERTY_H

#include "macros.h"
#include "property_base.h"

#include <wx/arrstr.h>

extern const wxEventType wxEVT_WINID_UPDATED;

class WinIdProperty : public PropertyBase
{
    wxString m_winid;
    static wxArrayString s_options;

public:
    static wxStringSet_t m_winIdSet;

public:
    WinIdProperty();
    ~WinIdProperty() override = default;

    wxString GetValue() const override { return m_winid; }
    void SetValue(const wxString& value) override { m_winid = value; }

    // Serialization methods
    JSONItem Serialize() const override;
    void UnSerialize(const JSONItem& json) override;

    PropertyeType GetType() override { return PT_COMBOX; }

    wxArrayString GetOptions() const override { return s_options; }
};

#endif // COMBOXPROPERTY_H
