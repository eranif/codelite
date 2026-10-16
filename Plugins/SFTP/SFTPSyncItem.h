#pragma once

#include <wx/string.h>

/// A local file to upload and the full path of its target on the remote machine
struct SFTPSyncItem {
    wxString local;
    wxString remote;
};
