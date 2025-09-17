#include "FileManager.hpp"

#if USE_SFTP
#include "clSFTPManager.hpp"
#endif

#include "clWorkspaceManager.h"

wxString FileManager::GetFullPath(const wxString& name, const WriteOptions& options)
{
    auto workspace = clWorkspaceManager::Get().GetWorkspace();
    if (workspace == nullptr || options.force_global) {
        // No workspace is opened, assume local.
        // Local workspace
        wxFileName fn{name};
        if (fn.IsRelative()) {
            fn.MakeAbsolute();
        }
        return fn.GetFullPath();
    }

    // We have a workspace opened.
    if (workspace->IsRemote()) {
        // Remote workspace
        bool is_relative = !name.StartsWith("/");
        if (is_relative) {
            wxString fullpath;
            fullpath << workspace->GetDir() << "/" << name;
            return fullpath;
        }
        return name;
    } else {
        // Local workspace
        wxFileName fn{name};
        if (fn.IsRelative()) {
            fn = wxFileName{workspace->GetDir(), name};
            return fn.GetFullPath();
        }
        return fn.GetFullPath();
    }
}

wxString FileManager::GetSettingFileFullPath(const wxString& name, const WriteOptions& options)
{
    auto workspace = clWorkspaceManager::Get().GetWorkspace();
    if (workspace == nullptr || options.force_global) {
        // No workspace is opened, assume local.
        // Local workspace
        wxFileName fn{clStandardPaths::Get().GetUserDataDir(), name};
        fn.AppendDir("config");
        return fn.GetFullPath();
    }

    wxString fullpath = workspace->GetDir();
    fullpath << "/.codelite/" << name;
    if (workspace->IsRemote()) {
        return fullpath;
    }
    return wxFileName{fullpath}.GetFullPath();
}

bool FileManager::Create(const wxString& filepath, const WriteOptions& options)
{
    wxString fullpath = GetFullPath(filepath, options);

#if USE_SFTP
    auto workspace = clWorkspaceManager::Get().GetWorkspace();
    if (workspace && workspace->IsRemote()) {
        // requires clSFTPManager
        if (clSFTPManager::Get().IsFileExists(fullpath, workspace->GetSshAccount())) {
            // already exists.
            return true;
        }

        // Create it.
        return clSFTPManager::Get().AwaitWriteFile(wxEmptyString, fullpath, workspace->GetSshAccount());
    }
#endif

    // Local file.
    if (wxFileName{fullpath}.FileExists()) {
        return true;
    }
    return FileUtils::WriteFileContent(fullpath, wxEmptyString);
}

bool FileManager::CreateSettingsFile(const wxString& name, const WriteOptions& options)
{
    wxString fullpath = GetSettingFileFullPath(name, options);
    return Create(fullpath, options);
}

bool FileManager::FileExists(const wxString& filepath, const WriteOptions& options)
{
    wxString fullpath = GetFullPath(filepath, options);
#if USE_SFTP
    auto workspace = clWorkspaceManager::Get().GetWorkspace();
    if (workspace && workspace->IsRemote()) {
        return clSFTPManager::Get().IsFileExists(fullpath, workspace->GetSshAccount());
    }
#endif
    return wxFileName{fullpath}.FileExists();
}

std::optional<wxString> FileManager::ReadContent(const wxString& filepath, const WriteOptions& options)
{
    wxString fullpath = GetFullPath(filepath,options);
#if USE_SFTP
    auto workspace = clWorkspaceManager::Get().GetWorkspace();
    if (workspace && workspace->IsRemote()) {
        wxMemoryBuffer membuf;
        if (!clSFTPManager::Get().AwaitReadFile(fullpath, workspace->GetSshAccount(), &membuf)) {
            return std::nullopt;
        }

        wxString content{(const char*)membuf.GetData(), options.GetConv(), membuf.GetDataLen()};
        return content;
    }
#endif

    // Local file
    wxString content;
    if (!FileUtils::ReadFileContent(fullpath, content, options.GetConv())) {
        return std::nullopt;
    }
    return content;
}

std::optional<wxString> FileManager::ReadSettingsFileContent(const wxString& name, const WriteOptions& options)
{
    wxString fullpath = GetSettingFileFullPath(name, options);
    return ReadContent(fullpath, options);
}

bool FileManager::WriteContent(const wxString& filepath,
                               const wxString& content,
                               bool overwrite,
                               const WriteOptions& options)
{
    wxString fullpath = GetFullPath(filepath, options);

    if (!overwrite && FileExists(filepath, options)) {
        return false;
    }

#if USE_SFTP
    auto workspace = clWorkspaceManager::Get().GetWorkspace();
    if (workspace && workspace->IsRemote()) {
        return clSFTPManager::Get().AwaitWriteFile(content, fullpath, workspace->GetSshAccount());
    }
#endif

    wxFileName{fullpath}.Mkdir(wxS_DIR_DEFAULT, wxPATH_NATIVE);
    return FileUtils::WriteFileContent(fullpath, content, options.GetConv());
}

bool FileManager::WriteSettingsFileContent(const wxString& name, const wxString& content, const WriteOptions& options)
{
    wxString fullpath = GetSettingFileFullPath(name, options);
    return WriteContent(fullpath, content, true, options);
}
