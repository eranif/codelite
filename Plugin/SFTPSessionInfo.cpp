#include "SFTPSessionInfo.h"

#if USE_SFTP
#include "file_logger.h"

#include <algorithm>

void SFTPSessionInfo::FromJSON(const JSONItem& json)
{
    m_files.clear();
    m_account = json.namedObject("name").toString();
    m_rootFolder = json.namedObject("rootFolder").toString();
    wxArrayString files = json.namedObject("files").toArrayString();
    for (const wxString& file : files) {
        m_files.push_back(file);
    }
}

JSONItem SFTPSessionInfo::ToJSON() const
{
    JSONItem json = JSONItem::createObject();
    wxArrayString files;
    for (const wxString& file : m_files) {
        files.Add(file);
    }
    json.addProperty("name", m_account);
    json.addProperty("rootFolder", m_rootFolder);
    json.addProperty("files", files);
    return json;
}

void SFTPSessionInfo::Clear()
{
    m_files.clear();
    m_account.Clear();
    m_rootFolder.Clear();
}

bool SFTPSessionInfo::IsOk() const { return !m_account.IsEmpty(); }

void SFTPSessionInfo::RemoveFile(const wxString& path)
{
    std::vector<wxString>::iterator iter =
        std::find_if(m_files.begin(), m_files.end(), [&](const wxString& file) { return path == file; });
    if(iter != m_files.end()) {
        clDEBUG() << "SFTP session: removing file:" << path << "from session:" << GetAccount();
        m_files.erase(iter);
    }
}

void SFTPSessionInfo::AddFile(const wxString& path)
{
    std::vector<wxString>::iterator iter =
        std::find_if(m_files.begin(), m_files.end(), [&](const wxString& file) { return path == file; });
    if(iter == m_files.end()) {
        clDEBUG() << "SFTP session: adding file:" << path << "to session:" << GetAccount();
        m_files.push_back(path);
    }
}

//-------------------------------------------------------------
// SFTPSessionInfoList
//-------------------------------------------------------------
SFTPSessionInfoList::SFTPSessionInfoList()
    : clConfigItem("sessions")
{
}

void SFTPSessionInfoList::FromJSON(const JSONItem& json)
{
    m_sessions.clear();
    int arrSize = json.arraySize();
    for(int i = 0; i < arrSize; ++i) {
        JSONItem o = json.arrayItem(i);
        SFTPSessionInfo sess;
        sess.FromJSON(o);
        m_sessions[sess.GetAccount()] = sess;
    }
}

JSONItem SFTPSessionInfoList::ToJSON() const
{
    JSONItem json = JSONItem::createArray(GetName());
    for (const auto& [_, sess] : m_sessions) {
        json.arrayAppend(sess.ToJSON());
    }
    return json;
}

SFTPSessionInfoList& SFTPSessionInfoList::Load()
{
    clConfig conf("sftp-sessions.conf");
    conf.ReadItem(this);
    return *this;
}

SFTPSessionInfoList& SFTPSessionInfoList::Save()
{
    clConfig conf("sftp-sessions.conf");
    conf.WriteItem(this);
    return *this;
}

SFTPSessionInfo& SFTPSessionInfoList::GetSession(const wxString& accountName)
{
    SFTPSessionInfo::Map_t::iterator iter = m_sessions.find(accountName);
    if(iter == m_sessions.end()) {
        static SFTPSessionInfo NullSession;
        return NullSession;
    }
    return iter->second;
}

SFTPSessionInfoList& SFTPSessionInfoList::SetSession(const SFTPSessionInfo& sess)
{
    if(sess.GetAccount().IsEmpty()) {
        return *this;
    }
    if(m_sessions.count(sess.GetAccount())) {
        m_sessions.erase(sess.GetAccount());
    }
    m_sessions[sess.GetAccount()] = sess;
    return *this;
}

#endif
