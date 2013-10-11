#include "remote_file_info.h"
#include <wx/stdpaths.h>

RemoteFileInfo::RemoteFileInfo()
{
}

RemoteFileInfo::~RemoteFileInfo()
{
}

void RemoteFileInfo::SetRemoteFile(const wxString& remoteFile)
{
    this->m_remoteFile = remoteFile;
    
    // Generate a temporary file location
    wxFileName fnRemoteFile(m_remoteFile);
    wxFileName localFile (wxStandardPaths::Get().GetUserDataDir(), fnRemoteFile.GetFullName());
    localFile.AppendDir("sftp");
    localFile.AppendDir("tmp");
    // Ensure that the path to the sftp temp folder exists
    localFile.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
    m_localFile = localFile.GetFullPath();
}
