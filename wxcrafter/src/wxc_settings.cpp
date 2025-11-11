#include "wxc_settings.h"

#include "StringUtils.h"
#include "fileutils.h"
#include "json_utils.h"
#include "wxgui_helpers.h"

#include <wx/filename.h>

wxcSettings::wxcSettings()
    : m_flags(0)
    , m_initCompleted(false)
{
    Load();
}

wxcSettings& wxcSettings::Get()
{
    static wxcSettings _settings;
    return _settings;
}

void wxcSettings::Load()
{
    wxFileName fn(wxCrafter::GetConfigFile());
    wxString content;
    FileUtils::ReadFileContent(fn, content);

    const nlohmann::json root = nlohmann::json::parse(StringUtils::ToStdString(content), nullptr, false);
    if (root.is_discarded() || !root.is_object()) {
        return;
    }

    m_flags = root.value("m_annoyDialogs", m_flags);
    m_flags &= ~USE_TABBED_MODE;
    m_sashPosition = root.value("m_sashPosition", 150);
    m_secondarySashPos = root.value("m_secondarySashPos", -1);
    m_treeviewSashPos = root.value("m_treeviewSashPos", -1);
    m_history = JsonUtils::ToArrayString(root["recentFiles"]);

    m_templateClasses.clear();
    for (const auto& item : root.value("m_templateClasses", nlohmann::json::array())) {
        CustomControlTemplate templateControl;
        templateControl.FromJSON(item);
        templateControl.SetControlId(::wxNewEventType());
        m_templateClasses.insert(std::make_pair(templateControl.GetClassName(), templateControl));
    }
}

void wxcSettings::Save()
{
    wxFileName fn(wxCrafter::GetConfigFile());

    m_flags &= ~USE_TABBED_MODE;
    nlohmann::json root = {{"m_annoyDialogs", (int)m_flags},
                           {"m_sashPosition", m_sashPosition},
                           {"m_secondarySashPos", m_secondarySashPos},
                           {"m_treeviewSashPos", m_treeviewSashPos},
                           {"recentFiles", StringUtils::ToStdStrings(m_history)}
    };

    auto& arr = root["m_templateClasses"];

    for (const auto& [_, customControlTemplate] : m_templateClasses) {
        arr.push_back(customControlTemplate.ToJSON());
    }
    FileUtils::WriteFileContentRaw(fn, root.dump());
}

void wxcSettings::RegisterCustomControl(CustomControlTemplate& cct)
{
    CustomControlTemplateMap_t::iterator iter = m_templateClasses.find(cct.GetClassName());
    if(iter != m_templateClasses.end()) {
        m_templateClasses.erase(iter);
    }
    cct.SetControlId(::wxNewEventType());
    m_templateClasses.insert(std::make_pair(cct.GetClassName(), cct));
}

CustomControlTemplate wxcSettings::FindByControlId(int controlId) const
{
    for (const auto& p : m_templateClasses) {
        if (p.second.GetControlId() == controlId) {
            return p.second;
        }
    }
    return CustomControlTemplate();
}

CustomControlTemplate wxcSettings::FindByControlName(const wxString& name) const
{
    CustomControlTemplateMap_t::const_iterator iter = m_templateClasses.find(name);
    if(iter == m_templateClasses.end()) {
        return CustomControlTemplate();
    }
    return iter->second;
}

void wxcSettings::DeleteCustomControl(const wxString& name)
{
    CustomControlTemplateMap_t::iterator iter = m_templateClasses.find(name);
    if(iter == m_templateClasses.end()) {
        return;
    }

    m_templateClasses.erase(iter);
}

void wxcSettings::MergeCustomControl(const nlohmann::json& arr)
{
    if (!arr.is_array()) {
        return;
    }
    for (auto& item : arr) {
        CustomControlTemplate templateControl;
        templateControl.FromJSON(item);
        templateControl.SetControlId(::wxNewEventType());

        if(m_templateClasses.count(templateControl.GetClassName())) {
            // an instance with this name already exists, replace it
            m_templateClasses.erase(templateControl.GetClassName());
        }
        m_templateClasses.insert(std::make_pair(templateControl.GetClassName(), templateControl));
    }

    // Apply the changes
    Save();
}

nlohmann::json wxcSettings::GetCustomControlsAsJSON(const wxArrayString& controls) const
{
    nlohmann::json arr;
    for (const auto& [name, customControlTemplate] : m_templateClasses) {
        if (controls.Index(name) != wxNOT_FOUND) {
            arr.push_back(customControlTemplate.ToJSON());
        }
    }
    return arr;
}

// ----------------------------------------------------------------------
// CustomControlTemplate
// ----------------------------------------------------------------------

CustomControlTemplate::CustomControlTemplate()
    : m_controlId(wxNOT_FOUND)
{
}

void CustomControlTemplate::FromJSON(const nlohmann::json& json)
{
    if (!json.is_object()) {
        return;
    }
    m_includeFile = JsonUtils::ToString(json["m_includeFile"]);
    m_allocationLine = JsonUtils::ToString(json["m_allocationLine"]);
    m_className = JsonUtils::ToString(json["m_className"]);
    m_xrcPreviewClass = JsonUtils::ToString(json["m_xrcPreviewClass"]);
    m_events = JsonUtils::ToStringMap(json["m_events"]);
}

nlohmann::json CustomControlTemplate::ToJSON() const
{
    return {{"m_includeFile", m_includeFile},
            {"m_allocationLine", m_allocationLine},
            {"m_className", m_className},
            {"m_xrcPreviewClass", m_xrcPreviewClass},
            {"m_events", JsonUtils::ToJson(m_events)}
    };
}
