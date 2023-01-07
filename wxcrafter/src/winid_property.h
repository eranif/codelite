#ifndef COMBOXPROPERTY_H
#define COMBOXPROPERTY_H

#include "property_base.h"
#include <macros.h>
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
    virtual ~WinIdProperty();

    virtual wxString GetValue() const { return m_winid; }
    virtual void SetValue(const wxString& value) { m_winid = value; }

    // Serialization methods
    virtual JSONElement Serialize() const;
    virtual void UnSerialize(const JSONElement& json);

    virtual PropertyeType GetType() { return PT_COMBOX; }

    virtual wxArrayString GetOptions() const { return s_options; }
};

#endif // COMBOXPROPERTY_H
