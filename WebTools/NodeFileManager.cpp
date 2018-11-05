#include "NodeFileManager.h"

NodeFileManager::NodeFileManager() {}

NodeFileManager::~NodeFileManager() {}

NodeFileManager& NodeFileManager::Get()
{
    static NodeFileManager mgr;
    return mgr;
}

void NodeFileManager::Clear() { m_files.clear(); }

void NodeFileManager::AddFile(const wxString& id, const wxString& url) { m_files.insert({ id, url }); }

wxString NodeFileManager::GetFilePath(const wxString& id) const
{
    std::unordered_map<wxString, wxString>::const_iterator iter = m_files.find(id);
    if(iter == m_files.end()) { return ""; }
    return iter->second;
}
