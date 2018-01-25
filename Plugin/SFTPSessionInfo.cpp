#include "SFTPSessionInfo.h"

#if USE_SFTP
#include <algorithm>

SFTPSessionInfo::SFTPSessionInfo() {}

SFTPSessionInfo::~SFTPSessionInfo() {}

void SFTPSessionInfo::FromJSON(const JSONElement& json)
{
    m_files.clear();
    m_account = json.namedObject("name").toString();
    m_rootFolder = json.namedObject("rootFolder").toString();
    wxArrayString files = json.namedObject("files").toArrayString();
    std::for_each(files.begin(), files.end(), [&](const wxString& file) { m_files.push_back(file); });
}

JSONElement SFTPSessionInfo::ToJSON() const
{
    JSONElement json = JSONElement::createObject();
    wxArrayString files;
    std::for_each(m_files.begin(), m_files.end(), [&](const wxString& file) { files.Add(file); });
    json.addProperty("name", m_account);
    json.addProperty("rootFolder", m_rootFolder);
    json.addProperty("files", files);
    return json;
}

//-------------------------------------------------------------
// SFTPSessionInfoList
//-------------------------------------------------------------
SFTPSessionInfoList::SFTPSessionInfoList()
    : clConfigItem("sessions")
{
}
SFTPSessionInfoList::~SFTPSessionInfoList() {}

void SFTPSessionInfoList::FromJSON(const JSONElement& json)
{
    m_sessions.clear();
    int arrSize = json.arraySize();
    for(int i = 0; i < arrSize; ++i) {
        JSONElement o = json.arrayItem(i);
        SFTPSessionInfo sess;
        sess.FromJSON(o);
        m_sessions[sess.GetAccount()] = sess;
    }
}

JSONElement SFTPSessionInfoList::ToJSON() const
{
    JSONElement json = JSONElement::createArray(GetName());
    std::for_each(m_sessions.begin(), m_sessions.end(), [&](const SFTPSessionInfo::Map_t::value_type& vt) {
        const SFTPSessionInfo& sess = vt.second;
        json.arrayAppend(sess.ToJSON());
    });
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

bool SFTPSessionInfoList::GetSession(const wxString& accountName, SFTPSessionInfo& sess) const
{
    SFTPSessionInfo::Map_t::const_iterator iter = m_sessions.find(accountName);
    if(iter == m_sessions.end()) {
        return false;
    }
    sess = iter->second;
    return true;
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
