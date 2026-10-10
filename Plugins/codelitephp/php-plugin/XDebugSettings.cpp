#include "XDebugSettings.h"

#include "FileSystemWorkspace/clFileSystemWorkspace.hpp"
#include "JSON.h"
#include "clWorkspaceManager.h"
#include "php_workspace.h"
#include "ssh_workspace_settings.h"

#include <memory>

const wxString XDebugSettings::DEBUGGER_NAME = "XDebug";

namespace
{
/// Replace one top level property of the settings file, and keep the others
void WriteProperty(const wxFileName& file, const wxString& name, const JSONItem& value)
{
    auto root = std::make_unique<JSON>(file);
    if (!root->isOk() || !root->toElement().isObject()) {
        root = std::make_unique<JSON>(JsonType::Object);
    }
    root->toElement().addProperty(name, value);
    file.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
    root->save(file);
}
} // namespace

bool XDebugSettings::IsActive()
{
    if (PHPWorkspace::Get()->IsOpen()) {
        return true;
    }
    return GetSettingsFile().IsOk() && clWorkspaceManager::Get().GetWorkspace()->GetDebuggerName() == DEBUGGER_NAME;
}

wxFileName XDebugSettings::GetSettingsFile()
{
    if (PHPWorkspace::Get()->IsOpen() || !clWorkspaceManager::Get().IsWorkspaceOpened()) {
        return {};
    }
    auto workspace = clWorkspaceManager::Get().GetWorkspace();
    if (workspace->IsRemote()) {
        return {};
    }
    wxFileName file(workspace->GetDir(), "xdebug.json");
    file.AppendDir(".codelite");
    return file;
}

XDebugBreakpoint::List_t XDebugSettings::LoadBreakpoints(const wxFileName& file)
{
    XDebugBreakpoint::List_t breakpoints;
    JSON root(file);
    JSONItem bpArr = root.toElement().namedObject("breakpoints");
    int count = bpArr.arraySize();
    for (int i = 0; i < count; ++i) {
        XDebugBreakpoint bp;
        bp.FromJSON(bpArr.arrayItem(i));
        breakpoints.push_back(bp);
    }
    return breakpoints;
}

void XDebugSettings::SaveBreakpoints(const wxFileName& file, const XDebugBreakpoint::List_t& breakpoints)
{
    // Don't add an Xdebug file to every workspace that clears its breakpoints
    if (breakpoints.empty() && !file.FileExists()) {
        return;
    }
    JSONItem bpArr = JSONItem::createArray();
    for (const auto& breakpoint : breakpoints) {
        bpArr.arrayAppend(breakpoint.ToJSON());
    }
    WriteProperty(file, "breakpoints", bpArr);
}

bool XDebugSettings::Load()
{
    m_ok = false;
    m_data = {};
    m_fileMapping.clear();
    m_projectName.clear();
    m_file.Clear();

    if (PHPWorkspace::Get()->IsOpen()) {
        PHPProject::Ptr_t project = PHPWorkspace::Get()->GetActiveProject();
        if (!project) {
            return false;
        }
        m_projectName = project->GetName();
        m_data = project->GetSettings();

        SSHWorkspaceSettings sftpSettings;
        sftpSettings.Load();
        if (!sftpSettings.GetRemoteFolder().IsEmpty() && sftpSettings.IsRemoteUploadEnabled()) {
            m_fileMapping.insert({PHPWorkspace::Get()->GetFilename().GetPath(), sftpSettings.GetRemoteFolder()});
        }

    } else {
        m_file = GetSettingsFile();
        if (!m_file.IsOk()) {
            return false;
        }
        JSON root(m_file);
        JSONItem settings = root.toElement().namedObject("settings");
        if (settings.isOk()) {
            m_data.FromJSON(settings);
        } else {
            m_data.SetWorkingDirectory(clWorkspaceManager::Get().GetWorkspace()->GetDir());
        }

        if (clFileSystemWorkspace::Get().IsOpen()) {
            auto config = clFileSystemWorkspace::Get().GetSettings().GetSelectedConfig();
            if (config && config->IsRemoteTargetEnabled() && !config->GetRemoteFolder().IsEmpty()) {
                m_fileMapping.insert({clFileSystemWorkspace::Get().GetDir(), config->GetRemoteFolder()});
            }
        }
    }

    // The user mapping wins over the remote folder of the workspace
    for (const auto& [local, remote] : m_data.GetFileMapping()) {
        m_fileMapping[local] = remote;
    }
    m_ok = true;
    return true;
}

void XDebugSettings::Save()
{
    if (!m_ok) {
        return;
    }

    if (!m_projectName.IsEmpty()) {
        PHPProject::Ptr_t project = PHPWorkspace::Get()->GetProject(m_projectName);
        if (project) {
            project->GetSettings() = m_data;
            project->Save();
        }
    } else {
        WriteProperty(m_file, "settings", m_data.ToJSON());
    }
}
