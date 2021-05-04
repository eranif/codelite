#include "PathConverterDefault.hpp"
#include "SFTPClientData.hpp"
#include "file_logger.h"
#include "fileutils.h"
#include "globals.h"
#include "ieditor.h"
#include <imanager.h>
#include <wx/filesys.h>
#include <wx/regex.h>

LSP::FilePath PathConverterDefault::ConvertFrom(const wxString& path) const
{
    // convert network path to local path
    wxString decodedPath = FileUtils::DecodeURI(path);
    if(decodedPath.StartsWith("file://")) {
        decodedPath.Remove(0, wxStrlen("file://"));
    }

#ifdef __WXMSW__
    // On Windows, path is now set to
    // /C:/src/path/to/file/a.cpp, the following code removes the leading slash
    if(decodedPath.length() >= 3 && decodedPath[0] == '/' && decodedPath[2] == ':') {
        decodedPath.Remove(0, 1); // remove the leading slash
    }
#endif

    LSP::FilePath fp(decodedPath);

#ifdef __WXMSW__
    if(decodedPath.StartsWith("/")) {
        // UNIX path on Windows !?
        // Check for open SFTP files
        IEditor::List_t editors;
        clGetManager()->GetAllEditors(editors);
        for(auto editor : editors) {
            if(editor->GetClientData("sftp")) {
                SFTPClientData* pcd = dynamic_cast<SFTPClientData*>(editor->GetClientData("sftp"));
                if(pcd) {
                    const wxString& remotePath = pcd->GetRemotePath();
                    if(decodedPath == remotePath) {
                        // it's a remote file
                        return pcd->GetLocalPath();
                    }
                }
            }
        }
        fp.SetIsRemoteFile(true);
    }
#else
    // if the path does not exist on our file system, assume a remote file
    if(!wxFileName::FileExists(decodedPath)) {
        fp.SetIsRemoteFile(true);
    }
#endif
    // default path conversion
    return fp;
}

LSP::FilePath PathConverterDefault::ConvertTo(const wxString& path) const
{
    clDEBUG() << "Converting" << path;
    wxFileName fn(path);

    // Locate an editor with this file path and check to see if it is a remote file
    IEditor* editor = clGetManager()->FindEditor(fn.GetFullPath());
    if(editor && editor->IsRemoteFile()) {
        wxString url = editor->GetRemotePath();
        url.Prepend("file://");
        clDEBUG() << path << "->" << url;
        return url;
    } else {
        // default behavior
        wxString url = wxFileSystem::FileNameToURL(fn);
        clDEBUG() << path << "->" << url;
        return LSP::FilePath(url);
    }
}
