#include "SmartCompletionsConfig.h"

SmartCompletionsConfig::SmartCompletionsConfig()
    : clConfigItem("settings")
    , m_flags(kEnabled)
{
}

SmartCompletionsConfig::~SmartCompletionsConfig() {}

SmartCompletionsConfig& SmartCompletionsConfig::Load()
{
    clConfig conf("SmartCompletions.conf");
    conf.ReadItem(this);
    return *this;
}

SmartCompletionsConfig& SmartCompletionsConfig::Save()
{
    clConfig conf("SmartCompletions.conf");
    conf.WriteItem(this);
    return *this;
}
void SmartCompletionsConfig::FromJSON(const JSONElement& json)
{
    JSONElement e = json.namedObject(GetName());
    m_flags = e.namedObject("m_flags").toSize_t(m_flags);
    // Read the serialized array
    wxArrayString serializedMap = e.namedObject("m_weight").toArrayString();
    for(size_t i = 0; i < serializedMap.size(); i += 2) {
        wxString k = serializedMap.Item(i);
        long weight = 0;
        serializedMap.Item(i + 1).ToCLong(&weight);
        m_weight[k] = (int)weight;
    }
}

JSONElement SmartCompletionsConfig::ToJSON() const
{
    JSONElement json = JSONElement::createObject(GetName());
    json.addProperty("m_flags", m_flags);

    // Serialize the
    wxArrayString serialized;
    std::for_each(m_weight.begin(), m_weight.end(), [&](const std::unordered_map<wxString, int>::value_type& vt) {
        serialized.Add(vt.first);
        serialized.Add(wxString() << vt.second);
    });
    json.addProperty("m_weight", serialized);
    return json;
}
