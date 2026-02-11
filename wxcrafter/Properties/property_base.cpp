#include "property_base.h"

const wxEventType wxEVT_WXC_PROPERTY_CHANGED = ::wxNewEventType();
PropertyBase::PropertyBase(const wxString& tooltip)
    : m_tooltip(tooltip)
{
}

void PropertyBase::DoBaseSerialize(JSONItem& json) const { json.addProperty(wxT("m_label"), m_label); }

void PropertyBase::DoBaseUnSerialize(const JSONItem& json) { m_label = json.namedObject(wxT("m_label")).toString(); }

void PropertyBase::NotifyChanged()
{
    wxCommandEvent evt(wxEVT_WXC_PROPERTY_CHANGED);
    evt.SetEventObject(this);
    ProcessEvent(evt);
}
