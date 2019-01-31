#ifndef WXCNETWORKREPLY_H
#define WXCNETWORKREPLY_H

#include "wxcLib/wxcEnums.h"
#include "wxcLib/json_node.h"
#include <wx/filename.h>
#include <vector>

class wxcNetworkReply
{
    wxCrafter::eReplyType   m_replyType;
    std::vector<wxFileName> m_files;
    wxString                m_wxcpFile;

public:
    wxcNetworkReply();
    wxcNetworkReply(const wxString &json);
    virtual ~wxcNetworkReply();

    // Serialization API
    void FromJSON(const JSONElement& json);
    JSONElement ToJSON() const;

    void SetFiles(const std::vector<wxFileName>& files) {
        this->m_files = files;
    }
    void SetReplyType(const wxCrafter::eReplyType& replyType) {
        this->m_replyType = replyType;
    }
    const std::vector<wxFileName>& GetFiles() const {
        return m_files;
    }
    const wxCrafter::eReplyType& GetReplyType() const {
        return m_replyType;
    }
    void SetWxcpFile(const wxString& wxcpFile) {
        this->m_wxcpFile = wxcpFile;
    }
    const wxString& GetWxcpFile() const {
        return m_wxcpFile;
    }
};

#endif // WXCNETWORKREPLY_H
