//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : IWorkspace.h
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#pragma once

#include "AsyncProcess/asyncprocess.h"
#include "clResult.hpp"
#include "cl_command_event.h"
#include "codelite_exports.h"

#include <concepts>
#include <limits>
#include <list>
#include <map>
#include <optional>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>
#include <wx/event.h>
#include <wx/filename.h>
#include <wx/string.h>

class IEditor;
class clSFTPEvent;

/// Fired when `IWorkspace::LoadConfig()` completes. For a remote workspace the load is async, so use this event to
/// know when the configuration is ready. The event is sent to the workspace object and to the `EventNotifier`
/// GetEventObject() is the workspace. GetString() is empty on success, otherwise it contains the error message
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_WORKSPACE_CONFIG_LOADED, clCommandEvent);

/// Fired when `IWorkspace::SaveConfig()` completes. The event is sent to the workspace object and to the
/// `EventNotifier`
/// GetEventObject() is the workspace. GetString() is empty on success, otherwise it contains the error message
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_WORKSPACE_CONFIG_SAVED, clCommandEvent);

/// The value types that can be stored in the workspace configuration
template <typename T>
concept WorkspaceConfigType = std::same_as<T, int> || std::same_as<T, size_t> || std::same_as<T, wxString> ||
                              std::same_as<T, std::vector<wxString>> || std::same_as<T, double>;

/**
 * @class IWorkspace
 * @brief an interface representing the workspace concept of CodeLite
 * Each plugin that wants to implement a workspace must implement this interface
 */
class WXDLLIMPEXP_SDK IWorkspace : public wxEvtHandler
{
public:
    using ConfigValue = std::variant<int, size_t, wxString, std::vector<wxString>, double>;

protected:
    wxString m_workspaceType;

private:
    /// category -> (name -> value)
    std::map<wxString, std::map<wxString, ConfigValue>> m_config;
    /// path of the remote config file while an async load is in progress (empty otherwise)
    wxString m_pendingConfigPath;

    /// The state of the config file. `SaveConfig()` writes the whole in-memory config, so it is allowed only when the
    /// file content is known (`kLoaded`), otherwise values that are in the file could be lost
    enum class ConfigState {
        kNotLoaded, // `LoadConfig()` was not called
        kLoading,   // an async load is in progress
        kLoaded,    // loaded, or the file does not exist
        kFailed,    // the file could not be read, or it is not valid
    };
    ConfigState m_configState = ConfigState::kNotLoaded;

    const ConfigValue* FindConfigValue(const wxString& name, const wxString& category) const;
    void StoreConfigValue(const wxString& name, const wxString& category, ConfigValue value);
    /// parse the config file content. Values that were set while loading are not overridden.
    /// Return false if the content is not a valid config file
    bool ParseConfig(const wxString& content);
    /// queue `type` on this object and on the EventNotifier
    void NotifyConfigEvent(wxEventType type, const wxString& error);
    void OnConfigFileRead(clSFTPEvent& event);
    void OnConfigFileReadError(clSFTPEvent& event);

public:
    using List_t = std::list<IWorkspace*>;

public:
    IWorkspace();
    ~IWorkspace() override = default;

    /**
     * @brief return the workspace name
     */
    virtual wxString GetName() const = 0;

    /**
     * @brief return the workspace file full path
     */
    virtual wxString GetFileName() const = 0;

    /**
     * @brief return the workspace directory
     */
    virtual wxString GetDir() const = 0;

    /**
     * @brief set the workspace type
     * For example: "C++ Workspace", "File System Workspace" etc
     */
    void SetWorkspaceType(const wxString& type) { this->m_workspaceType = type; }

    /**
     * @brief return the workspace name
     */
    const wxString& GetWorkspaceType() const { return m_workspaceType; }

    /**
     * @brief return the currently selected debugger. i.e. if the user hit F7
     * this debugger will be activated
     */
    virtual wxString GetDebuggerName() const = 0;

    /**
     * @brief is this workspace support the build concept?
     */
    virtual bool IsBuildSupported() const = 0;

    /**
     * @brief is this workspace support the project concept?
     */
    virtual bool IsProjectSupported() const = 0;

    /**
     * @brief return the file masking for this workspace
     */
    virtual wxString GetFilesMask() const = 0;

    /**
     * @brief return the project name of a file.
     * If the workspace does not support projects, return an empty string
     * If we could not match a project for the given filename, return empty string
     */
    virtual wxString GetProjectFromFile(const wxFileName& filename) const = 0;

    /**
     * @brief return a list of files for this workspace
     * @param files [output] list of files in absolute path
     */
    virtual void GetWorkspaceFiles(wxArrayString& files) const = 0;
    /**
     * @brief return list of files belonged to the project. If the workspace does not support
     * projects, do not modify 'files'
     * @param projectName the project name
     * @param files [output] list of files in absolute path
     */
    virtual void GetProjectFiles(const wxString& projectName, wxArrayString& files) const = 0;

    /**
     * @brief return the active project name. For workspace that does not support
     * projects, return an empty string
     */
    virtual wxString GetActiveProjectName() const = 0;

    /**
     * @brief set the active project
     */
    virtual void SetProjectActive(const wxString& project) = 0;

    /**
     * @brief return the underlying file for a given project name
     */
    virtual wxFileName GetProjectFileName(const wxString& projectName) const = 0;

    /**
     * @brief return list of projects for this workspace
     */
    virtual wxArrayString GetWorkspaceProjects() const = 0;

    /**
     * @brief return true if this workspace is a remote one
     */
    virtual bool IsRemote() const { return false; }
    /**
     * @brief return the ssh account used by this workspace
     */
    virtual wxString GetSshAccount() const { return ""; }

    /**
     * @brief return the environment for the workspace
     */
    virtual clEnvList_t GetEnvironment() const { return {}; }

    /// Return the workspace specific indentation width
    virtual int GetIndentWidth() { return wxNOT_FOUND; }

    /// Open (or create if missing) `filepath` and load it into an editor.
    virtual IEditor* OpenFileInEditor(const wxString& filepath, bool createIfMissing = true) = 0;

    /// Return the command to run / debug.
    struct CommandResult {
        std::vector<wxString> argv;
        wxString working_directory;

        void Add(const wxString& arg) { argv.push_back(arg); }
        void Add(const wxArrayString& args) { argv.insert(argv.end(), args.begin(), args.end()); }
    };
    virtual std::optional<CommandResult> GetCommand(bool for_debug = false) const { return std::nullopt; }

    /// Workspace configuration
    /// -----------------------
    /// Per workspace settings that are NOT saved into the workspace file. They are stored in the file
    /// `WORKSPACE_DIR/.codelite/workspace-config.json`, for both local and remote workspaces.
    /// The values are kept in memory until `SaveConfig()` is called.

    /**
     * @brief set a configuration value (in memory). Call `SaveConfig()` to persist it
     * @param name the value name
     * @param category the category (group) of the value
     */
    template <WorkspaceConfigType T>
    void SetConfigValue(const wxString& name, const wxString& category, T value)
    {
        StoreConfigValue(name, category, ConfigValue{std::in_place_type<T>, std::move(value)});
    }

    /**
     * @brief get a configuration value
     * @param default_value returned if the value is missing, or its type does not match `T`.
     * Numeric values (`int`, `size_t`, `double`) can be read as any numeric type
     */
    template <WorkspaceConfigType T>
    T GetConfigValue(const wxString& name, const wxString& category, T default_value = T{}) const
    {
        const ConfigValue* value = FindConfigValue(name, category);
        if (value == nullptr) {
            return default_value;
        }

        if constexpr (std::is_arithmetic_v<T>) {
            return std::visit(
                [&default_value](const auto& v) -> T {
                    using U = std::decay_t<decltype(v)>;
                    if constexpr (std::is_arithmetic_v<U>) {
                        // return `default_value` if the value does not fit into `T`
                        if constexpr (std::is_floating_point_v<T>) {
                            return static_cast<T>(v);
                        } else if constexpr (std::is_floating_point_v<U>) {
                            // a NaN fails both comparisons
                            constexpr double lowest = static_cast<double>(std::numeric_limits<T>::lowest());
                            constexpr double highest = static_cast<double>(std::numeric_limits<T>::max());
                            return (v > lowest - 1.0 && v < highest + 1.0) ? static_cast<T>(v) : default_value;
                        } else {
                            return std::in_range<T>(v) ? static_cast<T>(v) : default_value;
                        }
                    } else {
                        return default_value;
                    }
                },
                *value);
        } else {
            const T* typed = std::get_if<T>(value);
            return typed ? *typed : default_value;
        }
    }

    /**
     * @brief discard the in-memory configuration and load it from the workspace `.codelite` folder.
     * A missing file results in an empty configuration (this is not an error). If the file could not be read, or it is
     * not a valid JSON, the event contains the error message and `SaveConfig()` fails until the next successful load,
     * so the file is not overwritten.
     * For a remote workspace the file is read asynchronously: this method returns immediately.
     * In both cases `wxEVT_WORKSPACE_CONFIG_LOADED` is fired (on this object and on the EventNotifier) when the
     * load is completed, and the values should be read only after it.
     * @event wxEVT_WORKSPACE_CONFIG_LOADED
     */
    virtual void LoadConfig();

    /**
     * @brief discard the in-memory configuration and cancel a pending load. Call it when the workspace is closed
     */
    void ClearConfig();

    /**
     * @brief write the configuration to the workspace `.codelite` folder.
     * Fails (and does not touch the file) if the configuration was not loaded yet, a load is in progress, or the load
     * failed
     * @event wxEVT_WORKSPACE_CONFIG_SAVED fired (on this object and on the EventNotifier) on success and on failure
     */
    virtual clStatus SaveConfig();
};
