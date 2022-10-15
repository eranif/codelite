#include "SourceFormatterBase.hpp"

#include "macros.h"
#include "wxStringHash.h"

wxDEFINE_EVENT(wxEVT_FORMAT_COMPELTED, clSourceFormatEvent);
wxDEFINE_EVENT(wxEVT_FORMAT_INPLACE_COMPELTED, clSourceFormatEvent);

SourceFormatterBase::SourceFormatterBase() {}

SourceFormatterBase::~SourceFormatterBase() {}

void SourceFormatterBase::FromJSON(const JSONItem& json)
{
    m_languages = json["languages"].toArrayString();
    m_flags = json["flags"].toSize_t((size_t)FormatterFlags::ENABLED);
    m_configFile = json["config_file"].toString();
    m_name = json["name"].toString();
    m_description = json["description"].toString();
    m_shortDescription = json["short_description"].toString();
}

JSONItem SourceFormatterBase::ToJSON() const
{
    JSONItem ele = JSONItem::createObject(GetName());
    ele.addProperty("languages", m_languages);
    ele.addProperty("flags", m_flags);
    ele.addProperty("config_file", m_configFile);
    ele.addProperty("name", m_name);
    ele.addProperty("description", m_description);
    ele.addProperty("short_description", m_shortDescription);
    return ele;
}

bool SourceFormatterBase::CanHandle(FileExtManager::FileType file_type) const
{
    wxString lang = FileExtManager::GetLanguageFromType(file_type);
    return !lang.empty() && m_languages.Index(lang) != wxNOT_FOUND;
}

void SourceFormatterBase::SetFileTypes(const std::vector<FileExtManager::FileType>& types)
{
    wxStringSet_t langs;
    for(auto type : types) {
        wxString lang = FileExtManager::GetLanguageFromType(type);
        if(lang.empty()) {
            continue;
        }
        langs.insert(lang);
    }

    m_languages.clear();
    m_languages.reserve(langs.size());
    for(const auto& lang : langs) {
        m_languages.Add(lang);
    }
}

void SourceFormatterBase::SetConfigFilepath(const wxString& filepath)
{
    m_configFile = filepath;
    SetFlag(FormatterFlags::HAS_FILE_CONFIG, !m_configFile.empty());
}
