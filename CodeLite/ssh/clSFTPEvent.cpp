#include "clSFTPEvent.h"

wxDEFINE_EVENT(wxEVT_SFTP_SAVE_FILE, clSFTPEvent);
wxDEFINE_EVENT(wxEVT_SFTP_RENAME_FILE, clSFTPEvent);
wxDEFINE_EVENT(wxEVT_SFTP_DELETE_FILE, clSFTPEvent);
wxDEFINE_EVENT(wxEVT_SFTP_OPEN_FILE, clSFTPEvent);
wxDEFINE_EVENT(wxEVT_SFTP_SESSION_OPENED, clSFTPEvent);
wxDEFINE_EVENT(wxEVT_SFTP_SESSION_CLOSED, clSFTPEvent);
wxDEFINE_EVENT(wxEVT_SFTP_FILE_READ, clSFTPEvent);

clSFTPEvent::clSFTPEvent(wxEventType commandType, int winid)
    : clCommandEvent(commandType, winid)
{
}

