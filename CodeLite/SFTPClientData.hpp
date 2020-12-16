#ifndef SFTPCLIENTDATA_HPP
#define SFTPCLIENTDATA_HPP

#include <codelite_exports.h>
#include <wx/clntdata.h>
#include <wx/string.h>

class WXDLLIMPEXP_CL SFTPClientData : public wxClientData
{
    wxString localPath;
    wxString remotePath;
    size_t permissions = 0;
    int lineNumber = wxNOT_FOUND;
    wxString accountName;

public:
    SFTPClientData();
    virtual ~SFTPClientData();

    void SetLocalPath(const wxString& localPath) { this->localPath = localPath; }
    void SetRemotePath(const wxString& remotePath) { this->remotePath = remotePath; }
    const wxString& GetLocalPath() const { return localPath; }
    const wxString& GetRemotePath() const { return remotePath; }
    void SetPermissions(size_t permissions) { this->permissions = permissions; }
    size_t GetPermissions() const { return permissions; }
    void SetLineNumber(int lineNumber) { this->lineNumber = lineNumber; }
    int GetLineNumber() const { return lineNumber; }
    void SetAccountName(const wxString& accountName) { this->accountName = accountName; }
    const wxString& GetAccountName() const { return accountName; }
};

#endif // SFTPCLIENTDATA_HPP
