#include "SourceFormatterBase.hpp"

SourceFormatterBase::SourceFormatterBase() {}

SourceFormatterBase::~SourceFormatterBase() {}

void SourceFormatterBase::FromJSON(const JSONItem& json)
{
    auto types = json["file_types"].toIntArray();
    for(auto type : types) {
        m_extensions.insert((FileExtManager::FileType)type);
    }
    m_flags = json["flags"].toSize_t((size_t)FormatterFlags::ENABLED);
    m_configFile = json["config_file"].toString();
    m_name = json["name"].toString();
    m_description = json["description"].toString();
}

JSONItem SourceFormatterBase::ToJSON() const
{
    JSONItem ele = JSONItem::createObject(GetName());

    std::vector<int> types;
    types.reserve(m_extensions.size());
    for(auto type : m_extensions) {
        types.push_back((int)type);
    }

    ele.addProperty("file_types", types);
    ele.addProperty("flags", m_flags);
    ele.addProperty("config_file", m_configFile);
    ele.addProperty("name", m_name);
    ele.addProperty("description", m_description);
    return ele;
}
