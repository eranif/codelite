#pragma once

#include "codelite_exports.h"

#include <optional>
#include <wx/string.h>

struct WXDLLIMPEXP_SDK WriteOptions {
    wxMBConv* converter{nullptr};
    bool ignore_workspace{false};

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
    /**
     * @brief Resolves a directory path to its full absolute path based on workspace context.
     *
     * This function handles both local and remote workspaces, converting relative paths to absolute
     * paths using the workspace directory as the base when appropriate. If no workspace is open or
     * the workspace should be ignored, the current working directory is used as the base for relative paths.
     *
     * @param path The directory path to resolve. Can be relative or absolute. If empty, returns the
     *             workspace directory or current working directory.
     * @param options A WriteOptions structure containing flags such as ignore_workspace, which forces
     *                the function to use the current working directory instead of the workspace directory.
     *
     * @return wxString The full absolute path of the directory. For empty input, returns either the
     *                  workspace directory (if available and not ignored) or the current working directory.
     *                  For relative paths, returns the path made absolute relative to the workspace
     *                  or current working directory. For absolute paths, returns the normalized path.
     *
     * @note This method is part of the FileManager class and depends on clWorkspaceManager to retrieve
     *       workspace information.
     *
     * @example
     * @code
     * FileManager fm;
     * WriteOptions opts;
     * opts.ignore_workspace = false;
     *
     * // Get workspace directory (or CWD if no workspace)
     * wxString dir = fm.GetDirectoryFullPath("", opts);
     *
     * // Resolve relative path
     * wxString fullPath = fm.GetDirectoryFullPath("src/include", opts);
     *
     * // Absolute path returned as-is (normalized)
     * wxString absPath = fm.GetDirectoryFullPath("/home/user/projects", opts);
     * @endcode
     *
     * @see clWorkspaceManager::Get()
     * @see WriteOptions
     * @see wxFileName
     */
    static wxString GetDirectoryFullPath(const wxString& path, const WriteOptions& options = {});

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
