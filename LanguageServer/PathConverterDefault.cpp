#include "PathConverterDefault.hpp"
#include "SFTPClientData.hpp"
#include "file_logger.h"
#include "globals.h"
#include "ieditor.h"
#include <imanager.h>
#include <wx/filesys.h>

wxString PathConverterDefault::ConvertFrom(const wxString& path) const
{
    // convert network path to local path
    wxFileName fn = wxFileSystem::URLToFileName(path);
    // default path conversion
    return fn.GetFullPath();
}

wxString PathConverterDefault::ConvertTo(const wxString& path) const
{
    clDEBUG() << "Converting" << path;
    wxFileName fn(path);
    // Locate an editor with this file path and check to see if it is a remote file
    IEditor* editor = clGetManager()->FindEditor(fn.GetFullPath());
    if(editor && editor->GetClientData("sftp")) {
        SFTPClientData* pcd = dynamic_cast<SFTPClientData*>(editor->GetClientData("sftp"));
        if(pcd) {
            wxString url = pcd->GetRemotePath();
            url.Prepend("file://");
            clDEBUG() << path << "->" << url;
            return url;
        }
    }
    // default behavior
    wxString url = wxFileSystem::FileNameToURL(fn);
    clDEBUG() << path << "->" << url;
    return url;
}
