#include "clDapSettingsStore.hpp"

JSONItem DapEntry::To() const
{
    auto json = JSONItem::createObject();
    json.addProperty("name", m_name);
    json.addProperty("command", m_command);
    json.addProperty("connection_string", m_connection_string);
    json.addProperty("environment", m_environment);
    json.addProperty("is_remote", m_is_remote);
    json.addProperty("ssh_account", m_ssh_account);
    return json;
}

void DapEntry::From(const JSONItem& json)
{
    m_name = json["name"].toString();
    m_command = json["command"].toString();
    m_connection_string = json["connection_string"].toString();
    m_environment = json["environment"].toString();
    m_is_remote = json["is_remote"].toBool(m_is_remote);
    m_ssh_account = json["ssh_account"].toString();
}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

clDapSettingsStore::clDapSettingsStore() {}

clDapSettingsStore::~clDapSettingsStore() {}

void clDapSettingsStore::Clear()
{
    m_entries.clear();
    m_filepath.Clear();
}

void clDapSettingsStore::Load(const wxFileName& file)
{
    Clear();
    JSON root(file);
    if(!root.isOk()) {
        return;
    }

    m_filepath = file;
    JSONItem arr = root.toElement();
    int count = arr.arraySize();
    for(int i = 0; i < count; ++i) {
        DapEntry d;
        d.From(arr[i]);
        m_entries.insert({ d.GetName(), d });
    }
}

void clDapSettingsStore::Store()
{
    if(!m_filepath.IsOk()) {
        return;
    }
    JSON root(cJSON_Array);
    for(const auto& vt : m_entries) {
        root.toElement().arrayAppend(vt.second.To());
    }

    root.save(m_filepath);
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
