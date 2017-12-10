#include "clSFTPEvent.h"

wxDEFINE_EVENT(wxEVT_SFTP_SAVE_FILE, clSFTPEvent);
wxDEFINE_EVENT(wxEVT_SFTP_RENAME_FILE, clSFTPEvent);
wxDEFINE_EVENT(wxEVT_SFTP_DELETE_FILE, clSFTPEvent);

clSFTPEvent::~clSFTPEvent() {}

clSFTPEvent::clSFTPEvent(const clSFTPEvent& event) { *this = event; }

clSFTPEvent::clSFTPEvent(wxEventType commandType, int winid)
    : clCommandEvent(commandType, winid)
{
}

clSFTPEvent& clSFTPEvent::operator=(const clSFTPEvent& src)
{
    clCommandEvent::operator=(src);
    
    m_account = src.m_account;
    m_localFile = src.m_localFile;
    m_remoteFile = src.m_remoteFile;
    m_newRemoteFile = src.m_newRemoteFile;
    return *this;
}
