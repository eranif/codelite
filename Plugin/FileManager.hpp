#pragma once

#include "codelite_exports.h"

#include <optional>
#include <wx/string.h>

struct WXDLLIMPEXP_SDK WriteOptions {
    wxMBConv* converter{nullptr};
    bool force_global{false};

    inline const wxMBConv& GetConv() const
    {
        if (converter == nullptr) {
            return wxConvUTF8;
        }
        return *converter;
    }
};

/// Helper class for managing the file system. The class is workspace aware. What does that mean? if a user requests to
/// create the file "main.cpp" (relative path) and a workspace is currently opened, the file will be created under the
/// workspace path: /path/to/workspace/main.cpp. This class also handles remote & local files.
class WXDLLIMPEXP_SDK FileManager
{
public:
    /// Given `path` return the fullpath to this file. If `path` relative, its absolute path is determined based on the
    /// workspace folder, otherwise, we use the current directory as the base folder for converting it into fullpath.
    /// If `path` is absolute, we do not modify it.
    static wxString GetFullPath(const wxString& path, const WriteOptions& options = {});

    /// Return the path to a settings file with a given name. e.g. "codelite-remote.json". If a workspace is loaded,
    /// the path returned is in the workspace private folder .codelite, otherwise, we return the global settings path:
    /// ~/.codelite/config/<NAME>
    static wxString GetSettingFileFullPath(const wxString& name, const WriteOptions& options = {});

    /// Create file with a given path. If the path is relative, it is converted to full path by using the method:
    /// `GetFullPath`.
    static bool Create(const wxString& filepath, const WriteOptions& options = {});

    /// Create a settings file with a given name. The path is calculated using the method: `GetSettingFileFullPath`.
    static bool CreateSettingsFile(const wxString& name, const WriteOptions& options = {});

    /// Check if a file exists.
    static bool FileExists(const wxString& filepath, const WriteOptions& options = {});

    static std::optional<wxString> ReadContent(const wxString& filepath, const WriteOptions& options = {});
    static std::optional<wxString> ReadSettingsFileContent(const wxString& name, const WriteOptions& options = {});

    /// Write file content. If the file already exists and "overwrite" is false, return false.
    static bool
    WriteContent(const wxString& filepath, const wxString& content, bool overwrite, const WriteOptions& options = {});

    /// Write a configuration file.
    static bool
    WriteSettingsFileContent(const wxString& name, const wxString& content, const WriteOptions& options = {});
};
