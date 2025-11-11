#include "choice_property.h"

#include "StringUtils.h"
#include "json_utils.h"

ChoiceProperty::ChoiceProperty()
    : PropertyBase(wxT(""))
    , m_selection(wxNOT_FOUND)
{
}

ChoiceProperty::ChoiceProperty(const wxString& label, const wxArrayString& options, int selection, const wxString& tip)
    : PropertyBase(tip)
{
    m_options.insert(m_options.end(), options.begin(), options.end());
    m_selection = selection;
    SetLabel(label);
}

void ChoiceProperty::SetSelection(int selection)
{
    if(selection < 0 || selection >= (int)m_options.GetCount()) return;

    this->m_selection = selection;
}

wxString ChoiceProperty::GetValue() const
{
    if(m_selection < 0 || m_selection >= (int)m_options.GetCount()) return wxT("");
    return m_options.Item(m_selection);
}

void ChoiceProperty::SetValue(const wxString& value)
{
    int where = m_options.Index(value);
    if(where == wxNOT_FOUND) {
        m_selection = m_options.Add(value);

    } else {
        m_selection = where;
    }
}

void ChoiceProperty::Add(const wxString& value)
{
    int where = m_options.Index(value);
    if(where == wxNOT_FOUND) m_options.Add(value);
}

nlohmann::json ChoiceProperty::Serialize() const
{
    nlohmann::json json = {{"type", "choice"}};
    DoBaseSerialize(json);
    json["m_selection"] = m_selection;
    json["m_options"] = StringUtils::ToStdStrings(m_options);
    return json;
}

void ChoiceProperty::UnSerialize(const nlohmann::json& json)
{
    if (!json.is_object()) {
        return;
    }
    DoBaseUnSerialize(json);
    m_selection = json.value("m_selection", m_selection);
    m_options = JsonUtils::ToArrayString(json["m_options"]);

    //    // This hack is needed to support migration from the old values
    //    if ( GetLabel() == PROP_SPLIT_MODE ) {
    //        m_options.Clear();
    //        // The order matters here: we must add vertical and then horizontal
    //        m_options.Add("wxSPLIT_VERTICAL");
    //        m_options.Add("wxSPLIT_HORIZONTAL");
    //    }
}
