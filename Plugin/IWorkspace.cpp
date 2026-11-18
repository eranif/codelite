#include "IWorkspace.h"

#include "WorkspaceHelper.hpp"
#include "event_notifier.h"
#include "file_logger.h"
#include "macros.h"
#include "ssh/clSFTPEvent.h"

#include <assistant/common/json.hpp> // <nlohmann/json.hpp>
#include <climits>

#if USE_SFTP
#include "clSFTPManager.hpp"
#endif

wxDEFINE_EVENT(wxEVT_WORKSPACE_CONFIG_LOADED, clCommandEvent);
wxDEFINE_EVENT(wxEVT_WORKSPACE_CONFIG_SAVED, clCommandEvent);

namespace
{
const wxString kConfigFileName = "workspace-config.json";

nlohmann::json ToJson(const IWorkspace::ConfigValue& value)
{
    return std::visit(
        [](const auto& v) -> nlohmann::json {
            using T = std::decay_t<decltype(v)>;
            if constexpr (std::is_same_v<T, wxString>) {
                return v.ToStdString(wxConvUTF8);
            } else if constexpr (std::is_same_v<T, std::vector<wxString>>) {
                nlohmann::json arr = nlohmann::json::array();
                for (const auto& s : v) {
                    arr.push_back(s.ToStdString(wxConvUTF8));
                }
                return arr;
            } else {
                // int, size_t, double
                return v;
            }
        },
        value);
}

/// Convert a JSON value to a config value. Return `std::nullopt` for unsupported types
std::optional<IWorkspace::ConfigValue> FromJson(const nlohmann::json& json)
{
    if (json.is_string()) {
        return wxString::FromUTF8(json.get<std::string>());
    } else if (json.is_array()) {
        std::vector<wxString> arr;
        for (const auto& item : json) {
            if (item.is_string()) {
                arr.push_back(wxString::FromUTF8(item.get<std::string>()));
            }
        }
        return arr;
    } else if (json.is_boolean()) {
        return json.get<bool>() ? 1 : 0;
    } else if (json.is_number_unsigned()) {
        const auto u = json.get<uint64_t>();
        if (u <= static_cast<uint64_t>(INT_MAX)) {
            return static_cast<int>(u);
        }
        return static_cast<size_t>(u);
    } else if (json.is_number_integer()) {
        const auto i = json.get<int64_t>();
        if (i >= INT_MIN && i <= INT_MAX) {
            return static_cast<int>(i);
        }
        return static_cast<double>(i);
    } else if (json.is_number_float()) {
        return json.get<double>();
    }
    return std::nullopt;
}
} // namespace

const IWorkspace::ConfigValue* IWorkspace::FindConfigValue(const wxString& name, const wxString& category) const
{
    auto category_iter = m_config.find(category);
    if (category_iter == m_config.end()) {
        return nullptr;
    }
    auto value_iter = category_iter->second.find(name);
    return value_iter == category_iter->second.end() ? nullptr : &value_iter->second;
}

void IWorkspace::StoreConfigValue(const wxString& name, const wxString& category, ConfigValue value)
{
    m_config[category].insert_or_assign(name, std::move(value));
}

IWorkspace::IWorkspace()
{
    Bind(wxEVT_SFTP_FILE_READ, &IWorkspace::OnConfigFileRead, this);
    Bind(wxEVT_SFTP_FILE_READ_ERROR, &IWorkspace::OnConfigFileReadError, this);
}

bool IWorkspace::ParseConfig(const wxString& content)
{
    const auto json = nlohmann::json::parse(content.ToStdString(wxConvUTF8), nullptr, false);
    if (json.is_discarded() || !json.is_object()) {
        clWARNING() << "Invalid workspace config file:" << kConfigFileName << endl;
        return false;
    }

    for (const auto& [category, values] : json.items()) {
        if (!values.is_object()) {
            continue;
        }
        for (const auto& [name, value] : values.items()) {
            auto config_value = FromJson(value);
            if (config_value.has_value()) {
                // do not override values that were set while loading
                m_config[wxString::FromUTF8(category)].try_emplace(wxString::FromUTF8(name), std::move(*config_value));
            }
        }
    }
    return true;
}

void IWorkspace::ClearConfig()
{
    m_config.clear();
    m_pendingConfigPath.clear();
    m_configState = ConfigState::kNotLoaded;
}

void IWorkspace::NotifyConfigEvent(wxEventType type, const wxString& error)
{
    clCommandEvent event{type};
    event.SetEventObject(this);
    event.SetString(error);
    QueueEvent(event.Clone());
    EventNotifier::Get()->QueueEvent(event.Clone());
}

void IWorkspace::LoadConfig()
{
    ClearConfig();

    WorkspaceHelper helper;
    const wxString config_path = helper.GetPrivateFilePath(this, kConfigFileName);
    if (IsRemote()) {
#if USE_SFTP
        // read the file asynchronously, the result is handled by OnConfigFileRead / OnConfigFileReadError
        m_configState = ConfigState::kLoading;
        m_pendingConfigPath = config_path;
        clSFTPManager::Get().AsyncReadFile(m_pendingConfigPath, GetSshAccount(), this);
#else
        m_configState = ConfigState::kFailed;
        NotifyConfigEvent(wxEVT_WORKSPACE_CONFIG_LOADED, _("SFTP is not supported in this build"));
#endif
        return;
    }

    // A missing file is not an error: the workspace never saved its config
    if (!wxFileName::FileExists(config_path)) {
        m_configState = ConfigState::kLoaded;
        NotifyConfigEvent(wxEVT_WORKSPACE_CONFIG_LOADED, wxEmptyString);
        return;
    }

    wxString content;
    if (!helper.ReadPrivateFile(this, kConfigFileName, &content)) {
        m_configState = ConfigState::kFailed;
        NotifyConfigEvent(wxEVT_WORKSPACE_CONFIG_LOADED, _("Failed to read file: ") + config_path);
    } else if (!ParseConfig(content)) {
        m_configState = ConfigState::kFailed;
        NotifyConfigEvent(wxEVT_WORKSPACE_CONFIG_LOADED, _("Invalid configuration file: ") + config_path);
    } else {
        m_configState = ConfigState::kLoaded;
        NotifyConfigEvent(wxEVT_WORKSPACE_CONFIG_LOADED, wxEmptyString);
    }
}

void IWorkspace::OnConfigFileRead(clSFTPEvent& event)
{
    if (m_pendingConfigPath.empty() || event.GetRemoteFile() != m_pendingConfigPath) {
        event.Skip();
        return;
    }

    const wxString config_path = m_pendingConfigPath;
    m_pendingConfigPath.clear();
    if (ParseConfig(event.GetContent())) {
        m_configState = ConfigState::kLoaded;
        NotifyConfigEvent(wxEVT_WORKSPACE_CONFIG_LOADED, wxEmptyString);
    } else {
        m_configState = ConfigState::kFailed;
        NotifyConfigEvent(wxEVT_WORKSPACE_CONFIG_LOADED, _("Invalid configuration file: ") + config_path);
    }
}

void IWorkspace::OnConfigFileReadError(clSFTPEvent& event)
{
    if (m_pendingConfigPath.empty() || event.GetRemoteFile() != m_pendingConfigPath) {
        event.Skip();
        return;
    }

    clDEBUG() << "Failed to read workspace config file:" << m_pendingConfigPath << "." << event.GetString() << endl;
    m_pendingConfigPath.clear();

    // A missing file is not an error (same as for a local workspace): the workspace never saved its config
    if (event.GetInt() == static_cast<int>(StatusCode::kNotFound)) {
        m_configState = ConfigState::kLoaded;
        NotifyConfigEvent(wxEVT_WORKSPACE_CONFIG_LOADED, wxEmptyString);
    } else {
        m_configState = ConfigState::kFailed;
        NotifyConfigEvent(wxEVT_WORKSPACE_CONFIG_LOADED, event.GetString());
    }
}

clStatus IWorkspace::SaveConfig()
{
    clStatus status;
    if (GetFileName().empty()) {
        status = StatusInvalidArgument("Workspace is not opened");
    } else if (m_configState == ConfigState::kLoading) {
        status = StatusResourceBusy("The workspace configuration is still loading");
    } else if (m_configState != ConfigState::kLoaded) {
        // saving now could delete values that exist in the file
        status = StatusInvalidArgument("The workspace configuration was not loaded");
    }

    if (!status.ok()) {
        clWARNING() << "Workspace config is not saved." << status.message() << endl;
        NotifyConfigEvent(wxEVT_WORKSPACE_CONFIG_SAVED, status.message());
        return status;
    }

    auto root = nlohmann::json::object();
    for (const auto& [category, values] : m_config) {
        auto& category_json = root[category.ToStdString(wxConvUTF8)];
        category_json = nlohmann::json::object();
        for (const auto& [name, value] : values) {
            category_json[name.ToStdString(wxConvUTF8)] = ToJson(value);
        }
    }

    if (!WorkspaceHelper{}.WritePrivateFile(this, kConfigFileName, wxString::FromUTF8(root.dump(2)))) {
        status = StatusIOError(wxString() << "Failed to write file: " << kConfigFileName);
        NotifyConfigEvent(wxEVT_WORKSPACE_CONFIG_SAVED, status.message());
        return status;
    }
    NotifyConfigEvent(wxEVT_WORKSPACE_CONFIG_SAVED, wxEmptyString);
    return StatusOk();
}
