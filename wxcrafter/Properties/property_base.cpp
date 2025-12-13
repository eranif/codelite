#include "property_base.h"

#include "json_utils.h"

const wxEventType wxEVT_WXC_PROPERTY_CHANGED = ::wxNewEventType();

PropertyBase::PropertyBase(const wxString& tooltip)
    : m_tooltip(tooltip)
{
}

void PropertyBase::DoBaseSerialize(nlohmann::json& json) const { json["m_label"] = m_label; }

void PropertyBase::DoBaseUnSerialize(const nlohmann::json& json) {
    if (!json.is_object()) {
        return;
    }
    m_label = JsonUtils::ToString(json["m_label"]);
}

void PropertyBase::NotifyChanged()
{
    wxCommandEvent evt(wxEVT_WXC_PROPERTY_CHANGED);
    evt.SetEventObject(this);
    ProcessEvent(evt);
}
