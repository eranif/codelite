#ifndef WXCREPLYEVENTDATA_H
#define WXCREPLYEVENTDATA_H

#include <vector>
#include <wx/string.h>
#include <wx/filename.h>
#include <wx/clntdata.h>

/// a wxCommandEvent. GetClientObject() contains an instance of wxcReplyEventData
#define wxcEVT_NET_REPLY_FILES_GENERATED 21000
#define wxcEVT_NET_REPLY_CONN_TERMINATED 21001

class wxcReplyEventData : public wxClientData
{
public:
    typedef std::vector<wxString> StringVec_t;

protected:
    wxcReplyEventData::StringVec_t m_files;
    wxString m_wxcpFile;

public:
    wxcReplyEventData();
    virtual ~wxcReplyEventData();

    void SetFiles( const std::vector<wxString>& files );
    void SetFiles( const std::vector<wxFileName>& files );

    const wxcReplyEventData::StringVec_t& GetFiles() const {
        return m_files;
    }
    void SetWxcpFile(const wxString& wxcpFile) {
        this->m_wxcpFile = wxcpFile.c_str();
    }
    const wxString& GetWxcpFile() const {
        return m_wxcpFile;
    }
};

#endif // WXCREPLYEVENTDATA_H
