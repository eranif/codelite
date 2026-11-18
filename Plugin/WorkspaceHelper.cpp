#include "WorkspaceHelper.hpp"

#include "cl_standard_paths.h"
#include "file_logger.h"
#include "fileutils.h"
#include "macros.h"

#include <wx/filename.h>

#if USE_SFTP
#include "clSFTPManager.hpp"
#endif

thread_local wxStringMap_t REMOTE_FILES_CACHE;

wxString WorkspaceHelper::GetPrivateFilePath(const IWorkspace* workspace, const wxString& filename) const
{
    wxFileName filepath = workspace->GetFileName();
    filepath.AppendDir(".codelite");
    filepath.SetFullName(filename);

    if (!workspace->IsRemote()) {
        return filepath.GetFullPath();
    }

    wxString remote_file_path = filepath.GetFullPath(wxPATH_UNIX);
    remote_file_path.Replace("\\", "/");
    return remote_file_path;
}

bool WorkspaceHelper::ReadPrivateFile(IWorkspace* workspace, const wxString& filename, wxString* content) const
{
    if (workspace) {
        if (workspace->IsRemote()) {
            // Check the cache first
            const wxString remote_file_path = GetPrivateFilePath(workspace, filename);
            wxString file_content;
            if (REMOTE_FILES_CACHE.count(remote_file_path) == 0) {
                // a lengthy operation
                wxBusyCursor bc;
                wxMemoryBuffer memory_buffer;
#if USE_SFTP
                if (!clSFTPManager::Get().AwaitReadFile(remote_file_path, workspace->GetSshAccount(), &memory_buffer)) {
                    clDEBUG() << "Failed to read file:" << remote_file_path << endl;
                    return false;
                }
#endif
                wxString text((const unsigned char*)memory_buffer.GetData(), wxConvUTF8, memory_buffer.GetDataLen());
                REMOTE_FILES_CACHE.insert({remote_file_path, text});
            }

            // the file should exist in the cache
            if (REMOTE_FILES_CACHE.count(remote_file_path) == 0) {
                return false;
            }
            *content = REMOTE_FILES_CACHE.find(remote_file_path)->second;
            return true;
        } else {
            // local workspace
            const wxFileName filepath{GetPrivateFilePath(workspace, filename)};
            return FileUtils::ReadFileContent(filepath, *content);
        }
    } else {
        // no workspace is opened, use the global path
        wxFileName filepath{clStandardPaths::Get().GetUserDataDir(), filename};
        filepath.AppendDir("config");
        filepath.SetFullName(filename);
        return FileUtils::ReadFileContent(filepath, *content);
    }
}

bool WorkspaceHelper::WritePrivateFile(IWorkspace* workspace, const wxString& filename, const wxString& content) const
{
    if (workspace) {
        if (workspace->IsRemote()) {
            // Check the cache first
            const wxString remote_file_path = GetPrivateFilePath(workspace, filename);
            wxString file_content;

            clDEBUG() << "Writing file:" << remote_file_path << endl;
            // write the file to the file system
            // a lengthy operation
            wxBusyCursor bc;
#if USE_SFTP
            if (!clSFTPManager::Get().AwaitWriteFile(content, remote_file_path, workspace->GetSshAccount())) {
                // the `.codelite` folder might be missing, try to create it and write again
                auto account = SSHAccountInfo::LoadAccount(workspace->GetSshAccount());
                clSFTPManager::Get().NewFolder(remote_file_path.BeforeLast('/'), account);
                if (!clSFTPManager::Get().AwaitWriteFile(content, remote_file_path, workspace->GetSshAccount())) {
                    clDEBUG() << "Failed to write file:" << remote_file_path << endl;
                    return false;
                }
            }
#endif
            // update the cache
            REMOTE_FILES_CACHE.erase(remote_file_path);
            REMOTE_FILES_CACHE.insert({remote_file_path, content});

            return true;
        } else {
            // local workspace
            const wxFileName filepath{GetPrivateFilePath(workspace, filename)};
            if (FileUtils::WriteFileContent(filepath, content)) {
                return true;
            }
            // the `.codelite` folder might be missing, try to create it and write again
            wxFileName::Mkdir(filepath.GetPath(), wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
            return FileUtils::WriteFileContent(filepath, content);
        }
    } else {
        // no workspace is opened, use the global path
        wxFileName filepath{clStandardPaths::Get().GetUserDataDir(), filename};
        filepath.AppendDir("config");
        return FileUtils::WriteFileContent(filepath, content);
    }
}
