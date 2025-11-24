#include "SourceFormatterBase.hpp"

#include "json_utils.h"
#include "macros.h"
#include "wxStringHash.h"

wxDEFINE_EVENT(wxEVT_FORMAT_COMPELTED, clSourceFormatEvent);
wxDEFINE_EVENT(wxEVT_FORMAT_INPLACE_COMPELTED, clSourceFormatEvent);

void SourceFormatterBase::FromJSON(const nlohmann::json& json)
{
    if (!json.is_object()) {
        return;
    }
    m_languages = JsonUtils::ToArrayString(json["languages"]);
    m_flags = json.value("flags", (size_t)FormatterFlags::ENABLED);
    m_configFile = JsonUtils::ToString(json["config_file"]);
    m_name = JsonUtils::ToString(json["name"]);
    m_description = JsonUtils::ToString(json["description"]);
    m_shortDescription = JsonUtils::ToString(json["short_description"]);
}

nlohmann::json SourceFormatterBase::ToJSON() const
{
    return {{"languages", m_languages},
            {"flags", m_flags},
            {"config_file", m_configFile},
            {"name", m_name},
            {"description", m_description},
            {"short_description", m_shortDescription}};
}

bool SourceFormatterBase::CanHandle(FileExtManager::FileType file_type) const
{
    wxString lang = FileExtManager::GetLanguageFromType(file_type);
    return !lang.empty() && m_languages.Index(lang) != wxNOT_FOUND;
}

void SourceFormatterBase::SetFileTypes(const std::vector<FileExtManager::FileType>& types)
{
    wxStringSet_t langs;
    for (auto type : types) {
        wxString lang = FileExtManager::GetLanguageFromType(type);
        if (lang.empty()) {
            continue;
        }
        langs.insert(lang);
    }

    m_languages.clear();
    m_languages.reserve(langs.size());
    for (const auto& lang : langs) {
        m_languages.Add(lang);
    }
}

void SourceFormatterBase::SetConfigFilepath(const wxString& filepath)
{
    m_configFile = filepath;
    SetFlag(FormatterFlags::HAS_FILE_CONFIG, !m_configFile.empty());
}
