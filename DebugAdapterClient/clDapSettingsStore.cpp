#include "clDapSettingsStore.hpp"

#include "fileutils.h"

JSONItem DapEntry::To() const
{
    auto json = JSONItem::createObject();
    json.addProperty("name", m_name);
    json.addProperty("command", m_command);
    json.addProperty("connection_string", m_connection_string);
    json.addProperty("environment", m_environment);
    json.addProperty("flags", m_flags);
    json.addProperty("env_format", (int)m_envFormat);
    json.addProperty("launch_type", (int)m_launch_type);
    return json;
}

void DapEntry::From(const JSONItem& json)
{
    m_name = json["name"].toString();
    m_command = json["command"].toString();
    m_connection_string = json["connection_string"].toString();
    m_environment = json["environment"].toString();
    m_flags = json["flags"].toSize_t(m_flags);
    m_envFormat = (dap::EnvFormat)json["env_format"].toInt((int)dap::EnvFormat::LIST);
    m_launch_type = (DapLaunchType)json["launch_type"].toInt((int)m_launch_type);
}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

clDapSettingsStore::clDapSettingsStore() {}

clDapSettingsStore::~clDapSettingsStore() {}

void clDapSettingsStore::Clear() { m_entries.clear(); }

void clDapSettingsStore::Load(const wxFileName& file)
{
    Clear();

    // ensure that we have something to load
    if(!file.FileExists()) {
        wxFileName::Mkdir(file.GetPath(), wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
        FileUtils::WriteFileContent(file, "[]");
    }

    JSON root(file);
    if(!root.isOk()) {
        return;
    }

    JSONItem arr = root.toElement();
    int count = arr.arraySize();
    for(int i = 0; i < count; ++i) {
        DapEntry d;
        d.From(arr[i]);
        m_entries.insert({ d.GetName(), d });
    }
}

void clDapSettingsStore::Save(const wxFileName& path)
{
    if(!path.IsOk()) {
        return;
    }
    JSON root(cJSON_Array);
    for(const auto& vt : m_entries) {
        root.toElement().arrayAppend(vt.second.To());
    }

    root.save(path);
}

bool clDapSettingsStore::Get(const wxString& name, DapEntry* entry) const
{
    if(!entry || m_entries.count(name) == 0) {
        return false;
    }

    *entry = m_entries.find(name)->second;
    return true;
}

bool clDapSettingsStore::Contains(const wxString& name) const
{
    DapEntry entry;
    return Get(name, &entry);
}

bool clDapSettingsStore::Set(const DapEntry& entry)
{
    m_entries.erase(entry.GetName());
    return m_entries.insert({ entry.GetName(), entry }).second;
}

bool clDapSettingsStore::Delete(const wxString& name)
{
    if(m_entries.count(name) == 0) {
        return false;
    }
    m_entries.erase(name);
    return true;
}

void clDapSettingsStore::Set(const std::vector<DapEntry>& entries)
{
    m_entries.clear();
    Update(entries);
}

void clDapSettingsStore::Update(const std::vector<DapEntry>& entries)
{
    for(const auto& d : entries) {
        m_entries.erase(d.GetName());
        m_entries.insert({ d.GetName(), d });
    }
}
