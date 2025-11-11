#include "col_header_flags_property.h"

#include "StdToWX.h"
#include "json_utils.h"
#include "wxgui_helpers.h"

#include <wx/dataview.h>
#include <wx/headercol.h>

ColHeaderFlagsProperty::ColHeaderFlagsProperty(const wxString& label, int initialValue, const wxString& tip,
                                               eColumnKind kind)
    : PropertyBase(tip)
    , m_initialValue(initialValue)
{
    SetLabel(label);
    if(kind == eColumnKind::kListCtrl) {
        m_names = StdToWX::ToArrayString({
            "wxCOL_RESIZABLE",
            "wxCOL_SORTABLE",
            "wxCOL_REORDERABLE",
            "wxCOL_HIDDEN",
        });

        m_values.Add(wxCOL_RESIZABLE);
        m_values.Add(wxCOL_SORTABLE);
        m_values.Add(wxCOL_REORDERABLE);
        m_values.Add(wxCOL_HIDDEN);

    } else {
        m_names = StdToWX::ToArrayString({ "wxDATAVIEW_COL_RESIZABLE", "wxDATAVIEW_COL_SORTABLE",
                                           "wxDATAVIEW_COL_REORDERABLE", "wxDATAVIEW_COL_HIDDEN" });
        m_values.Add(wxDATAVIEW_COL_RESIZABLE);
        m_values.Add(wxDATAVIEW_COL_SORTABLE);
        m_values.Add(wxDATAVIEW_COL_REORDERABLE);
        m_values.Add(wxDATAVIEW_COL_HIDDEN);
    }
}

ColHeaderFlagsProperty::ColHeaderFlagsProperty()
    : PropertyBase("")
{
}

PropertyeType ColHeaderFlagsProperty::GetType() { return PT_COL_HEADER_FLAGS; }

wxString ColHeaderFlagsProperty::GetValue() const
{
    wxString str_value;
    for(size_t i = 0; i < m_values.GetCount(); ++i) {
        if(m_initialValue & m_values.Item(i)) { str_value << m_names.Item(i) << "|"; }
    }
    if(!str_value.IsEmpty()) { str_value.RemoveLast(); }
    return str_value;
}

void ColHeaderFlagsProperty::SetValue(const wxString& value)
{
    m_initialValue = 0;
    wxArrayString selections = wxCrafter::Split(value, "|,;");
    for(size_t i = 0; i < selections.GetCount(); ++i) {
        int index = m_names.Index(selections.Item(i));
        if(index != wxNOT_FOUND) { m_initialValue |= m_values.Item(index); }
    }
}

nlohmann::json ColHeaderFlagsProperty::Serialize() const
{
    nlohmann::json json = {{"type", "colHeaderFlags"}};
    DoBaseSerialize(json);
    json["stringValue"] = GetValue();
    return json;
}

void ColHeaderFlagsProperty::UnSerialize(const nlohmann::json& json)
{
    if (!json.is_object()) {
        return;
    }
    DoBaseUnSerialize(json);
    SetValue(JsonUtils::ToString(json["stringValue"]));
}

wxArrayString ColHeaderFlagsProperty::GetOptions() const { return m_names; }

wxArrayInt ColHeaderFlagsProperty::GetOptionsValues() const { return m_values; }

long ColHeaderFlagsProperty::GetValueLong() const { return m_initialValue; }
