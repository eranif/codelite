#ifndef LLDBPIVOT_H
#define LLDBPIVOT_H

#include <wx/string.h>
class LLDBPivot
{
    wxString m_localFolder;
    wxString m_remoteFolder;

public:
    LLDBPivot();
    virtual ~LLDBPivot();

    void Clear() {
        m_localFolder.Clear();
        m_remoteFolder.Clear();
    }
    
    /**
     * @brief sanity
     * @return 
     */
    bool IsValid() const;
    
    void SetLocalFolder(const wxString& localFolder) {
        this->m_localFolder = localFolder;
    }
    void SetRemoteFolder(const wxString& remoteFolder) {
        this->m_remoteFolder = remoteFolder;
    }
    const wxString& GetLocalFolder() const {
        return m_localFolder;
    }
    const wxString& GetRemoteFolder() const {
        return m_remoteFolder;
    }
    
    /**
     * @brief convert local path to remote path. ToRemote() will always use UNIX style format ("/")
     */
    wxString ToRemote(const wxString &localPath) const;
    /**
     * @brief convert remote path to local path. ToLocal() will use the native path separator ("/" or "\")
     */
    wxString ToLocal(const wxString &remotePath) const;
};

#endif // LLDBPIVOT_H
